/* Log File Defination */
/* File name: LOG.H */
/* Date:
   Author:
   Dexcription:



*/

#ifndef LOG_H
#define LOG_H

/* Global Definations */
#include <stdio.h>

/* Class Defination */

#define LOG_FILE_NAME "XDMC.log"

class log
{
  private :
	  // Data Structures 

	  static char log_file_name [256];
	  static FILE *log_file;
	  static CRITICAL_SECTION log_access;
      static BOOL bEnableLog;

	  //Functions

	  void TimeStamp (char*);
	  BOOL CopyLog(char*);

  public :
  
	  BOOL OpenLog(char *, char *);
	  BOOL CloseLog (char*);
	  BOOL LogEntry (char *);
      BOOL LogErrorEntry(SYSTEMTIME *,unsigned short);
	  BOOL FileName (char *);
	  BOOL CopyLogFile (char *);
	  void SetLogStatus(BOOL );
      char ELine [1000]; 
};
#endif



   


