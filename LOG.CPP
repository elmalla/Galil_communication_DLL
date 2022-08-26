/* Log File Implmentations */
/* File name:
   Date :
   Author:
   Description:

*/

/* Global Definations */
#include <windows.h>
#include <stdio.h>

/* Class Definations */
#include "log.h"

char log::log_file_name [256];
FILE * log::log_file = NULL;
CRITICAL_SECTION log::log_access ;
BOOL log::bEnableLog = FALSE;
/* Private Functions */

/* Date-Time Stamp */
/* Description:




*/

void log::TimeStamp (char * line)
{
 SYSTEMTIME st;
	
 GetLocalTime (&st);

 sprintf(line,"%d/%d/%d %d:%02d:%02d ",st.wMonth,st.wDay,st.wYear,st.wHour,st.wMinute,st.wSecond); 
}

/* Copy log File */
/* Description:





*/
BOOL log::CopyLog (char * copy_file_name)
{
  FILE * copy_log_file;
  BOOL rtn = FALSE;

  char line[80];

  if (log_file != NULL)
  {
   EnterCriticalSection(&log_access);
   if ((copy_log_file = fopen (copy_file_name,"w")) != NULL)
   {
    fseek(log_file,0,SEEK_SET);
	while(fgets(line,sizeof(line) ,log_file) != NULL )
		fputs(line,copy_log_file);

	fclose(copy_log_file);
	rtn = TRUE;
   }
   LeaveCriticalSection (&log_access);
  }
  return (rtn); 
}


/* Public Functions */

/* Open Log File */
/* Description :
   
   Passed parameters:


   Returned value:
*/

BOOL log::OpenLog (char *path, char * entry)
{
  //Initialize the public Line Array
  ELine[0]= '\0';

  char open_line[132];  
  BOOL rtn ;

  strcpy(log_file_name,path );
  strcat(log_file_name,LOG_FILE_NAME);

  if((log_file = fopen(log_file_name,"w+c")) != NULL )
  {
   InitializeCriticalSection(&log_access);
   EnterCriticalSection(&log_access);
   TimeStamp (open_line);
   strncat( open_line, entry ,110);
   fprintf(log_file,"%s \n",open_line);
   fflush(log_file);
   LeaveCriticalSection(&log_access);
   rtn = TRUE;
  }
  else
	  rtn = FALSE;

  return (rtn);
}

/* Close Log File */
/* Description:

   Passed Parameters:
   
   Return value:
   

*/

BOOL log::CloseLog(char * entry)
{
 char close_line [132];
 BOOL rtn;

 if (log_file != NULL)
 {
  EnterCriticalSection(&log_access);
  TimeStamp(close_line);
  strncat(close_line,entry,110);
  fprintf(log_file,"%s \n",close_line);
  fclose(log_file);
  log_file = NULL;
  LeaveCriticalSection(&log_access);
  DeleteCriticalSection(&log_access);
  rtn = TRUE;
 }
 else
	 rtn = FALSE;
 
 return (rtn);
}


/* Make a log entry */
/* Description :

   Passed Paramters:

   Return value:
*/

BOOL log::LogEntry (char * entry)
{
  if (bEnableLog == TRUE )
  {
   char entry_line [132];
   BOOL rtn;
   
   if (log_file != NULL)
   {
    TimeStamp (entry_line);
	strncat(entry_line,entry,110);
	EnterCriticalSection(&log_access);
    fprintf(log_file,"%s \n",entry_line);
    fflush(log_file);
	rtn = TRUE ;
    LeaveCriticalSection (&log_access);
   }
   else
	   rtn = FALSE;

   return (rtn);
  }
  else
     return (bEnableLog);
}

/* Log Error Entry */
/* Description:
   
   Passed Parameters:

   Return value:
*/

BOOL  log::LogErrorEntry(SYSTEMTIME *et , unsigned short err_no)
{
	char entry_line [132];
	BOOL rtn;

	if (log_file != NULL)
	{
     sprintf(entry_line,"%d /%d %d %d:%02d:%02d   Error: %d",et->wMonth,et->wDay,et->wYear,et->wHour,et->wMinute ,et->wSecond ,err_no     );
	 EnterCriticalSection(&log_access);
	 fprintf(log_file,entry_line);
	 rtn = TRUE;
	 LeaveCriticalSection(&log_access);
	}
	else
		rtn = FALSE;

	return (rtn);
}

/* Log File Name */
/* Description:

   Passed Parameters:

   Return Value:

*/

BOOL log::FileName(char * filename)
{
 BOOL rtn = FALSE;
	
 if (log_file != NULL)
 {
  strcpy(filename,log_file_name);
  rtn = TRUE;
 }
 else
	 rtn = FALSE;

 return (rtn);
}

/* Copy Log File Name */
/* Description:
   
   Passed parameters:

   Return value:

*/

BOOL log::CopyLogFile(char *filename)
{
 BOOL rtn = FALSE;
 char copy_file_name[256], *ptr;

 filename[0] = 0;
 if (log_file != NULL)
 {
  strcpy(copy_file_name,log_file_name);
  ptr = strrchr (copy_file_name, '\\');
  if (ptr != NULL)
	  *(ptr + 1 ) = 0;
  else
	  copy_file_name[0] =0;
  strcat(copy_file_name,"copyof");
  strcat(copy_file_name,LOG_FILE_NAME);
  if (CopyLog(copy_file_name) == TRUE)
  {
    strcat(filename,copy_file_name);
	rtn = TRUE ;
  }
 }
 return (rtn);

}

void log::SetLogStatus (BOOL status)
{
  bEnableLog = status;
}