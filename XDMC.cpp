// XDMC.cpp : Defines the entry point for the DLL application.
//


#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <comdef.h>
#include <tchar.h>
#include "TCONVERT.H"
#include "XDMC.h"
#include "CXDMC.H"
#include "LOG.H"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

#define XCOMMAND_SIZE   255
#define XRESPONSE_SIZE  255

class log Applog;
char path [256];


//int CXDMC::m_sObjCount = 0; 

int ShowError (LONG lError, LPCTSTR lptszMessage)
{
	// Generate a message text
	TCHAR tszMessage[256];
	wsprintf(tszMessage,_T("%s\n(error code %d)"), lptszMessage, lError);

	// Display message-box and return with an error-code
	::MessageBox( 0 , tszMessage ,_T("XDMC - DLL"), MB_ICONSTOP|MB_OK);
	return 1;
}

extern "C" DllExport LONG WINAPI XDMC_GetDMCHandle(void * objptr)
{
  try
 {
	LONG    ldllErr;
//	LONG    RC;

	if (NULL == objptr)
	{
	   ldllErr = 10;
	   throw (ldllErr);	 
	}
	
	CXDMC *ptrController = (CXDMC*)objptr;
    /*
	LONG err = ptrController->GetLastError(); 
	 ::ShowError ( err, _T("This last error"));
      */
	LONG hand = ptrController->GetHandle();
		 ::ShowError (hand, _T("This is the galil handle"));
    
    return hand;
 } 
 
    catch (LONG ErrorCode)
	{
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : Close Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	  }
	    return 1;
	} 

	catch (...)
    {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : Close Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		  return 1;
    }

}


extern "C" DllExport void * WINAPI XDMC_Init(void * objptr,HWND hWnd,USHORT usController,BSTR bstrDMCFile,BSTR bstrCommand)
{	
   try
  { 
	LONG    ldllErr;
	LONG    RC; 
    char *ptr;

	if (objptr != NULL )
	{
	   ldllErr = 20;
	   throw (ldllErr);	 
	}
	
	if (objptr == NULL)
	{
	  
	  CXDMC *ptrController  = new CXDMC;
	
	  //CDMCWinRegistry DMCWinRegistry;
	  GALILREGISTRY galilregistry;
		
      // Is the controller registered?
      if (ptrController->GetGalilRegistryInfo(usController , &galilregistry) == DMCERROR_CONTROLLER)
	  {
	       delete ptrController;
		   ldllErr = 21;
	       throw (ldllErr);	
	  }

	  ptrController->Open(usController); 
      
	  
	  WCHAR wPath[256]={0};
	  char * tmp;

      GetModuleFileName((HINSTANCE) hWnd, (LPWSTR)wPath ,sizeof(wPath));
      //::MessageBox( 0 ,(LPCWSTR) wPath ,_T("Init XDMC - DLL"), MB_ICONSTOP|MB_OK);
	  
	  class _tochar cPath(wPath);
	  tmp = cPath;
	  sprintf(path,"%s",tmp);

	  ptr = strrchr(path ,'\\');
     
	  if (ptr != NULL)
	   *(ptr + 1) = 0;
      else
	    path[0] = 0;
      
	  
      Applog.OpenLog(path,"Init XDMC.dll") ;

      if (ptrController->IsConnected())
	  {
        //Download Dmc File to Controller
		if (bstrDMCFile != NULL)
        { 
			_bstr_t bstrDMC (bstrDMCFile,true);
			char * dmcFile = bstrDMC;
			//::MessageBox (0, (wchar_t*) bstrDMC , _T("XDMC DLL"), 
            //     MB_ICONEXCLAMATION | MB_OK); 
			RC = ptrController->DownloadFile(dmcFile,NULL);
			
			if (RC != DMCNOERROR)
			{
              char szMessage[128];
			  
			  ldllErr = 23;
	          throw (ldllErr);	
			  
			  ptrController->GetErrorText(RC,szMessage,sizeof(szMessage));
			  //_bstr_t bstrErrText(szMessage);
			  //::MessageBox (0, (wchar_t*) bstrErrText, _T("XDMC DLL : Init Exception"),MB_ICONEXCLAMATION | MB_OK);
              
			  //delete ptrController;
              //ptrController = NULL;
			  //return NULL; 
			}
		}
        
		      memset(ptrController->m_sCommand,0, XCOMMAND_SIZE);
 	   memset(ptrController->m_sResponse,0, XRESPONSE_SIZE);

		ptrController->Command ("CW,1",ptrController->m_sResponse,sizeof(ptrController->m_sResponse));
        ptrController->Clear();

		if (bstrCommand != NULL)
		{
			_bstr_t bstrCmd (bstrCommand, true);
			char * command = bstrCmd;
			//char response[4096]={0};
			
			RC = ptrController->Command(command,ptrController->m_sResponse,sizeof(ptrController->m_sResponse));
		  
			if (RC != DMCNOERROR)
			{
			  ldllErr = 24;
	          throw (ldllErr);		  
			}
		}
		  ptrController->Clear();
		  Applog.LogEntry("Init XDMC.dll- Finished Succefully") ;
	      //ptrController->DiagnosticsOn("c:\\galil.txt",FALSE); 	  
		  return ptrController;

	  }
      else
	  {
        //delete ptrController;
        //ptrController = NULL;

		ldllErr = 22;
	    throw (ldllErr);	
	  }

	  return NULL; 
	}	 
   }
   catch (LONG ErrorCode)
	{
      switch(ErrorCode)
	  {
        case 20: ::MessageBox (0, _T("Galil Card already Initialized (Open)"), _T("XDMC DLL : Init Exception"),MB_ICONEXCLAMATION | MB_OK);break;//Unable to open COM-port
		case 21: ::MessageBox (0, _T("Could not find controller in the Windows registry") , _T("XDMC DLL : Init Exception"),MB_ICONEXCLAMATION | MB_OK);break;
		case 22: ::MessageBox (0, _T("Could not connect to controller"), _T("XDMC DLL : Init Exception"),MB_ICONEXCLAMATION | MB_OK); break;
        case 23: ::MessageBox (0, _T("Downloading Motion File Failed"), _T("XDMC DLL : Init Exception"),MB_ICONEXCLAMATION | MB_OK); return NULL;break;
	    case 24: ::MessageBox (0, _T("Executing XQ#AUTO Failed"), _T("XDMC DLL : Init Exception"),MB_ICONEXCLAMATION | MB_OK); return NULL;break;

	  }
	  return NULL;
	}  

	catch (...)
    {
        ::MessageBox (0, _T("Unknown XDMC Galil Init Error"), _T("XDMC DLL Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		  return NULL;
    }
}



extern "C" DllExport BSTR WINAPI XDMC_SendCmd (void * objptr, BSTR bstrCommand)
{
  try
  {
     LONG    ldllErr;

	if ( NULL == objptr)
	{
	   ldllErr = 10;
	   throw (ldllErr);	 
	}
		
	 if (objptr != NULL)
	{
	  CXDMC * ptrController = (CXDMC*)objptr;

	  if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE) && (SysStringLen(bstrCommand) != NULL))
	  {
          _bstr_t bstrCmd(bstrCommand,true);
		  char * ptrCommand  = bstrCmd;
		  
		  //if (bResponseRequired == TRUE)
		  //{
		    char response[4096]={0};
			ptrController->Command(ptrCommand,response,sizeof(response));
		  
		  //char *ptrResponse = response;
		    char *ptrValue;

		    ptrValue = strchr(response,':');
		    if ((ptrValue != NULL) && (*ptrValue == ':'))
			{
              *ptrValue = '\0';
		      ++ptrValue;
            
			//_bstr_t bstrResponse ( response);
            //::MessageBox (0, (wchar_t*) bstrResponse , _T("XDMC DLL"), 
              //       MB_ICONEXCLAMATION | MB_OK); 
            
			}
		    else if (ptrValue = strchr(response,'?'))
			{
             
              ldllErr = 30;
	          throw (ldllErr);	
			
			 //  _bstr_t bstrResponse ( ptrValue);
            //::MessageBox (0, (wchar_t*) bstrResponse , _T("XDMC DLL Exception"), 
            //         MB_ICONEXCLAMATION | MB_OK); 
			}
		  /*
		  char * pch;       
          pch=strchr(response,':');
		  char *temp = response[pch];
          */

		   _bstr_t bstrResponse ( response);
		   ptrController->m_CmdResponse = bstrResponse;
          
		  //TCHAR tszMessage[256];
	      //wsprintf(tszMessage,_T("%s)"), bstrResponse);
          
		  //::MessageBox (0, (wchar_t*) ptrController->m_CmdResponse , _T("XDMC DLL Exception"), 
            //         MB_ICONEXCLAMATION | MB_OK); 

		   //::ShowError ((LONG)objptr, _T("This is the pointer from VB"));
		   
		   /*
		   LONG hand = ptrController->GetHandle();
		 ::ShowError (hand, _T("This is the galil handle"));
      
	      //hand =ptrController->GetKeepHandle();
	      //::ShowError(hand ,_T("This XDMC.dll handle"));
	        ::ShowError((LONG) objptr,_T("This object pointer fron VB"));
           */
		   
		   
		   return ptrController->m_CmdResponse;
	/*	 }
		 else
		 {

		   ptrController->FastCommand (ptrCommand);

		   ptrController->m_CmdResponse = _T("0");
		   return 0;//ptrController->m_CmdResponse ;
		 }
    */
	  }
	  else if (! ptrController->IsConnected()) 
      {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
	  }
	  else if (! ptrController->m_bEnabled )
	  {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

	  }
	  else if ( bstrCommand == NULL )
	  {
		//  ::ShowError (0,_T("No Command supplied"));

		//  ptrController->m_CmdResponse = _T("Handled Exception");
		 // return ptrController->m_CmdResponse ;
        ldllErr = 34;
	    throw (ldllErr);
	  }
	}
	else
	{
      //::ShowError (0,_T("Empty pointer"));

	  //ptrController->m_CmdResponse = _T("0");
	 // _bstr_t bstrResponse (_T("E ")) ;
     
		ldllErr = 31;
	    throw (ldllErr);	
	}
  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : SendCmd Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 30: ::MessageBox (0, _T("Check Command Syntax"), _T("XDMC DLL : SendCmd Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 31: ::MessageBox (0, _T("The function was passed an Empty pointer"), _T("XDMC DLL : SendCmd Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : SendCmd Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : SendCmd Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 34: ::MessageBox (0, _T("No Command supplied"), _T("XDMC DLL : SendCmd Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port

	  }
	  _bstr_t bstrException (_T("Handled Exception")) ;
	  return bstrException;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown XDMC Send Command Error"), _T("XDMC DLL : SendCmd Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		 _bstr_t bstrException (_T("Handled Exception")) ;
    	  return bstrException;
  }
}



extern "C" DllExport LONG WINAPI XDMC_GetMotorBusy(void * objptr, int iAxisNo )//, BSTR bstrVarName )
{
 try
 {
   LONG    ldllErr;

   if(NULL == objptr)
   {
     ldllErr =10;
	 throw(ldllErr);
   }
 

   CXDMC *ptrController = (CXDMC*)objptr;
   
   //::MessageBox (0, _T("Before testing connection"), _T("XDMC DLL : GetMotorBusy"), 
    //                  MB_ICONEXCLAMATION | MB_OK);

   if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
   {

        char response1[4096]={0};
        char command[100] ={0};

        //_bstr_t bstrCmd(bstrVarName,true);
		//long lLen= (long)SysStringLen(bstrVarName);

		//::MessageBox (0, _T("Before testing VarName"), _T("XDMC DLL : GetMotorBusy"), 
                      //MB_ICONEXCLAMATION | MB_OK);

		//if (SysStringLen(bstrVarName) == NULL)//bstrVarName == NULL)
		//{
          char cAxis;
	      int  iMotionStatus;

	      switch(iAxisNo)
		  {
	        case 0: cAxis='A'; break;
	        case 1: cAxis='B'; break;
	        case 2: cAxis='C'; break;
	        case 3: cAxis='D'; break;
            case 4: cAxis='E'; break;
            case 5: cAxis='F'; break;
	        case 6: cAxis='G'; break;
	        case 7: cAxis='H'; break;
		  }

	       sprintf(command,"TS%c", cAxis );
		  // _bstr_t bstrComm ( command);
           // ::MessageBox (0, (wchar_t*) bstrComm , _T("XDMC DLL"), 
           //        MB_ICONEXCLAMATION | MB_OK); 
		   
		   //ptrController->Clear() ;
	       ptrController->Command(command,response1,sizeof(response1)); 

		  // _bstr_t bstrResponse ( response1);
           // ::MessageBox (0, (wchar_t*) bstrResponse , _T("XDMC DLL"), 
           //        MB_ICONEXCLAMATION | MB_OK); 

		   char *ptrValue;

		    ptrValue = strchr(response1,':');
		    if ((ptrValue != NULL) && (*ptrValue == ':'))
			{
              *ptrValue = '\0';
		      ++ptrValue;
            
			
            
			}

	       iMotionStatus= atoi(response1);

           //::ShowError ((long)iMotionStatus, _T("iMotionStatus"));

	      if( (iMotionStatus & 0x80))
		     return 1;
	      else 
          {
	         //::MessageBox (0, _T("shouldn't come here"), _T("XDMC DLL : GetMotorBusy"), 
             //         MB_ICONEXCLAMATION | MB_OK);
			 sprintf(command,"YBUSY%c=?",cAxis);
	         
			 char response2[4096]={0};
             ptrController->Command(command,response2,sizeof(response2)); 

			// _bstr_t bstrResponse ( response2);
			 //::MessageBox (0, (wchar_t*) bstrResponse , _T("XDMC DLL"), 
               //   MB_ICONEXCLAMATION | MB_OK); 

		     /*
			 ptrValue = strchr(response2,':');
		    if ((ptrValue != NULL) && (*ptrValue == ':'))
			{
              *ptrValue = '\0';
		      ++ptrValue;
            
			
            
			}
             int iValue =atoi(response2);

			 ::ShowError ((long)iValue, _T("iMotionStatus"));
	         if( iValue != NULL)
			{
			  ::MessageBox (0, _T("Return true") , _T("XDMC DLL"), 
                  MB_ICONEXCLAMATION | MB_OK);	
				 return 1;
			 }
			 else
			{	 
	          ::MessageBox (0, _T("Return false") , _T("XDMC DLL"), 
                  MB_ICONEXCLAMATION | MB_OK);
				 return 0;
			 }
			 
			 ptrValue = strchr(response2,'1');
			 if ((ptrValue != NULL) && (*ptrValue == '1'))
			 {
			  ::MessageBox (0, _T("Return true") , _T("XDMC DLL"), 
                  MB_ICONEXCLAMATION | MB_OK);	 
	           return TRUE;
			 }



             ptrValue = strchr(response2,'0');
			 if ((ptrValue != NULL) && (*ptrValue == '0'))
			 {	 
	          ::MessageBox (0, _T("Return flase") , _T("XDMC DLL"), 
                  MB_ICONEXCLAMATION | MB_OK);
				 return FALSE;
			 }
			 */

			 if (strchr(response2,'1'))
				 return 1;
			 else if(strchr(response2,'0'))
			//	{	 
	         // ::MessageBox (0, _T("Return false") , _T("XDMC DLL"), 
               //   MB_ICONEXCLAMATION | MB_OK);
				 return 0;
			 //}
			 
			 
		  }
   }
   else if (! ptrController->IsConnected()) 
   {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
   }
   else if (! ptrController->m_bEnabled )
   {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

   }
   //return TRUE;
 }

  catch (LONG ErrorCode)
	{
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : GetMotorBusy Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : GetMotorBusy Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : GetMotorBusy Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port

			//case 40
	  }
	  return 1;
	} 

	catch (...)
    {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : GetMotorBusy Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
          return 1;
    }
}


extern "C" DllExport LONG WINAPI XDMC_GetAllMotorsBusy(void * objptr, int iNoOfAxis )//, BSTR bstrVarName )
{
 try
 {
   LONG    ldllErr;

   if(NULL == objptr)
   {
     ldllErr =10;
	 throw(ldllErr);
   }
 

   CXDMC *ptrController = (CXDMC*)objptr;
   
   //::MessageBox (0, _T("Before testing connection"), _T("XDMC DLL : GetMotorBusy"), 
    //                  MB_ICONEXCLAMATION | MB_OK);

   if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
   {

        //char response1[4096]={0};
        char * command;
//		BYTE BinaryCommand[32];
  //      ULONG BinaryCommandLength;

		int  iMotionStatus[10]={0};
        int  iAllMotionStatus = 0x80;

        //ptrController->Clear() ;
        memset(ptrController->m_sResponse,0,80);
	    ptrController->Command("TS",ptrController->m_sResponse,sizeof(ptrController->m_sResponse)); 

		  // _bstr_t bstrResponse ( response1);
           // ::MessageBox (0, (wchar_t*) bstrResponse , _T("XDMC DLL"), 
            //       MB_ICONEXCLAMATION | MB_OK); 

		   char *ptrValue;

		    ptrValue = strchr(ptrController->m_sResponse,':');
		    if ((ptrValue != NULL) && (*ptrValue == ':'))
			{
              *ptrValue = '\0';
		      ++ptrValue;
              
			  char *ptr_First = ptrController->m_sResponse;
			  char *ptr_Last;

			  // _bstr_t bstrResponse10 ( ptr_First);
            //::MessageBox (0, (wchar_t*) bstrResponse10 , _T("XDMC DLL"), 
              //     MB_ICONEXCLAMATION | MB_OK); 

			  
			  for (int i=0 ; i < iNoOfAxis; i++)
			  {
                ptr_Last = strchr(ptr_First,','); 
			    if ((ptr_Last != NULL) && (*ptr_Last == ','))
				{
			     *ptr_Last = '\0';
		         ++ptr_Last;
			     iMotionStatus[i]= atoi(ptr_First);
				 ptr_First = ptr_Last ;
				 
				 //_bstr_t bstrResponse11 ( ptr_First);
                //::MessageBox (0, (wchar_t*) bstrResponse11 , _T("XDMC DLL"), 
                  // MB_ICONEXCLAMATION | MB_OK); 
                 
				iAllMotionStatus = iAllMotionStatus & iMotionStatus[i];
				 //::ShowError ((long)i, _T("Inside for : iMotionStatus[i]"));
				}
				else if (i == iNoOfAxis -1)//if (*ptr_Last != NULL)
				{
				 //_bstr_t bstrResponse12 ( ptr_First);
                //::MessageBox (0, (wchar_t*) bstrResponse12 , _T("XDMC DLL"), 
                  // MB_ICONEXCLAMATION | MB_OK); 
                 
                  iMotionStatus[i]= atoi(ptr_First);
				  iAllMotionStatus = iAllMotionStatus & iMotionStatus[i];
				 // ::ShowError ((long)iMotionStatus[i], _T("Inside for else part : iMotionStatus[i]"));

				}
			  }

            
			}
        

	      if( iAllMotionStatus & 0x80)
          {
		     //::ShowError ((long)iAllMotionStatus, _T("iAllMotionStatus -> give TRUE"));
			  return 1;
		  }
		  else 
          {
	         //::MessageBox (0, _T("shouldn't come here"), _T("XDMC DLL : GetMotorBusy"), 
             //         MB_ICONEXCLAMATION | MB_OK);
			 //sprintf(command,"YBUSY%c=?",cAxis);
	         //char response2[4096]={0};
			 char cAxis;
			 iAllMotionStatus = 1;

			 for (int i=0 ; i< iNoOfAxis ; i++)
             {     
	           switch(i)
			   {
	            case 0: cAxis='A'; break;
	            case 1: cAxis='B'; break;
	            case 2: cAxis='C'; break;
	            case 3: cAxis='D'; break;
                case 4: cAxis='E'; break;
                case 5: cAxis='F'; break;
	            case 6: cAxis='G'; break;
	            case 7: cAxis='H'; break;
			   }
			  
              
			   sprintf(command,"YBUSY%c=?",cAxis);
			   memset(ptrController->m_sResponse,0,80);
			   ptrController->Command(command,ptrController->m_sResponse,sizeof(ptrController->m_sResponse)); 


                //_bstr_t bstrResponse13 ( response2);
                //::MessageBox (0, (wchar_t*) bstrResponse13 , _T("XDMC DLL"), 
                  // MB_ICONEXCLAMATION | MB_OK); 

			   if (strchr(ptrController->m_sResponse,'1'))
				 iAllMotionStatus = iAllMotionStatus & 1;
			   else if(strchr(ptrController->m_sResponse,'0'))
			//	{	 
	         // ::MessageBox (0, _T("Return false") , _T("XDMC DLL"), 
               //   MB_ICONEXCLAMATION | MB_OK);
				 iAllMotionStatus = iAllMotionStatus & 0;

			   //::ShowError ((long)iAllMotionStatus, _T("iAllMotionStatus values"));
			   //::ShowError ((long)i, _T("Inside for : iMotionStatus[i]"));
			 }
			 
			  //::ShowError ((long)iAllMotionStatus, _T("iAllMotionStatus (else part)"));
			  return (LONG) iAllMotionStatus;
		  }
   }
   else if (! ptrController->IsConnected()) 
   {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
   }
   else if (! ptrController->m_bEnabled )
   {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

   }
   //return TRUE;
 }

  catch (LONG ErrorCode)
	{
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : GetAllMotorsBusy Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : GetAllMotorsBusy Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : GetAllMotorsBusy Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port

			//case 50
	  }
      return 1;
	} 

	catch (...)
    {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : GetAllMotorsBusy Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		return 1;
    }
}


extern "C" DllExport long WINAPI XDMC_MotorGetPos(void * objptr, int iAxisNo )
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
   

   //::MessageBox (0, _T("Before testing connection"), _T("XDMC DLL : GetMotorBusy"), 
    //                  MB_ICONEXCLAMATION | MB_OK);

   if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
   {
        
	   //char response[4096]={0};
       char * command;
	   long lmotorPos = 0;
       LONG RC;
	   
	         memset(ptrController->m_sCommand,0, XCOMMAND_SIZE);
	   memset(ptrController->m_sResponse,0, XRESPONSE_SIZE);

	   switch(iAxisNo)
	   {
	       case 0: command="TPA"; break;
	       case 1: command="TPB"; break;
	       case 2: command="TPC"; break;
	       case 3: command="TPD"; break;
           case 4: command="TPE"; break;
           case 5: command="TPF"; break;
	       case 6: command="TPG"; break;
	       case 7: command="TPH"; break;
	   }

       RC= ptrController->Command(command,ptrController->m_sResponse,sizeof(ptrController->m_sResponse)); 
       
	   if (RC != DMCNOERROR)
	   {
           char szMessage[128];
		   ptrController->GetErrorText(RC,szMessage,sizeof(szMessage));
		   
		    memset(ptrController->m_sLine,0,300);
			sprintf(ptrController->m_sLine,"Get App Flag Error:Command %s  - Error : %d  -Error :%s",command,RC ,szMessage);
            Applog.LogEntry(ptrController->m_sLine);
			
		   class _totchar cl(szMessage);
		   
		   ShowError(RC, cl);
	       return 1;	 
	   }

		char *ptrValue;

		ptrValue = strchr(ptrController->m_sResponse,':');
		if ((ptrValue != NULL) && (*ptrValue == ':'))
		{
           *ptrValue = '\0';
		   ++ptrValue;
        }
        lmotorPos = atol(ptrController->m_sResponse);


		//LONG hand = ptrController->GetHandle();
		// ::ShowError (hand, _T("This is the galil handle"));
      
	 //hand =ptrController->GetKeepHandle();
	 //::ShowError(hand ,_T("This XDMC.dll handle"));
	 //::ShowError((LONG) objptr,_T("This object pointer fron VB"));

		return lmotorPos;
        
   }
   else if (! ptrController->IsConnected()) 
   {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
   }
   else if (! ptrController->m_bEnabled )
   {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

   }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : MotorGetPos Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : MotorGetPosy Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : MotorGetPos Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
			// case 60:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : MotorGetPos Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		return 1;
  }
}

extern "C" DllExport long WINAPI XDMC_MotorInit(void * objptr, int iAxisNo )
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
    {
	 char * command;
	 char response [4096]={0};
	 char cAxis;

	 switch(iAxisNo)
	 {
	       case 0: command="XQ#REINITA"; cAxis='A'; break;
	       case 1: command="XQ#REINITB"; cAxis='B'; break;
	       case 2: command="XQ#REINITC"; cAxis='C'; break;
	       case 3: command="XQ#REINITD"; cAxis='D'; break;
           case 4: command="XQ#REINITE"; cAxis='E'; break;
           case 5: command="XQ#REINITF"; cAxis='F'; break;
	       case 6: command="XQ#REINITG"; cAxis='G'; break;
	       case 7: command="XQ#REINITH"; cAxis='H'; break;
           case 10: command="XQ#REINIT"; cAxis='Z'; break;  
	 }

     ptrController->Command(command,response,sizeof(response));
     
     if(strchr(response,'?')) 
	   return 1; //error
     else
       return 0;
   
	 /*

	     ::ShowError (ptrController->GetLastError() ,_T("Error from motor init"));
	 if (ldllErr == NULL)
		 return TRUE;
	 else
	     throw(ldllErr);
	 
	 ::MessageBox (0, _T("after throw error") , _T("XDMC DLL"), 
                    MB_ICONEXCLAMATION | MB_OK); 
	 //ptrController->Command(command,response,sizeof(response));
	 
	 sprintf(TestCmd,"YBUSY%c=?",cAxis);
	
     ptrController->Command(TestCmd,response,sizeof(response)); 

			 _bstr_t bstrResponse ( response);
            ::MessageBox (0, (wchar_t*) bstrResponse , _T("XDMC DLL"), 
                    MB_ICONEXCLAMATION | MB_OK); 
     
	 if(strchr(response,':')) 
	   return TRUE;
     else
       return FALSE;*/
     //if (strchr(response,'0')) 
	 //  return FALSE;
	}
    else if (! ptrController->IsConnected()) 
    {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
    }
    else if (! ptrController->m_bEnabled )
    {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

    }

  }
  
  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : MotorInit Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : MotorInit Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : MotorInit Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 70:
	  }
       return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : MotorInit Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		return 1;
  }
}



extern "C" DllExport long WINAPI XDMC_MotorEnable(void * objptr, int iAxisNo, bool EnableDisable, bool bAllAxis)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
     {
	   
       char response [4096]={0};
       char cAxis;

	   if (bAllAxis)
       {
		 char * command ;
		 if (EnableDisable)
           command="SH";
	     else
           command="MO";
		 
		 ptrController->Command(command,response,sizeof(response));
	   }
	   else
       {
         char command[100] ={0};
	     switch(iAxisNo)
		 {
	        case 0: cAxis='A'; break;
	        case 1: cAxis='B'; break;
	        case 2: cAxis='C'; break;
	        case 3: cAxis='D'; break;
            case 4: cAxis='E'; break;
            case 5: cAxis='F'; break;
	        case 6: cAxis='G'; break;
	        case 7: cAxis='H'; break;
		 }
	   
	     if (EnableDisable)
           sprintf(command,"SH%c", cAxis);
			 //sprintf(command,"SH%c;DP%c=_TP%c", cAxis,cAxis,cAxis );
	     else
          sprintf(command,"MO%c", cAxis ); 
		 
		 ptrController->Command(command,response,sizeof(response));
	   }
       
     
       if(strchr(response,'?')) 
	     return 1; //error
       else
         return 0;

	 }
	 else if (! ptrController->IsConnected()) 
    {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
    }
    else if (! ptrController->m_bEnabled )
    {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

    }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : MotorEnable Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : MotorEnable Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : MotorEnable Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 80:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : MotorEnable Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
        return 1;
  }
}

extern "C" DllExport long WINAPI XDMC_MotorSetSpd(void * objptr, int iAxisNo, long lMotorSpeed)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
     {
	   char command[100] ={0};
       char response [4096]={0};
       char cAxis;

	   switch(iAxisNo)
	   {
	        case 0: cAxis='A'; break;
	        case 1: cAxis='B'; break;
	        case 2: cAxis='C'; break;
	        case 3: cAxis='D'; break;
            case 4: cAxis='E'; break;
            case 5: cAxis='F'; break;
	        case 6: cAxis='G'; break;
	        case 7: cAxis='H'; break;
	   }

	   sprintf(command,"YSTRSPD%c=%d",cAxis,lMotorSpeed);
	
       ptrController->Command(command,response,sizeof(response)); 
       
	   if(strchr(response,'?')) 
	     return 1; //error
       else
         return 0;
	 }
	 else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : MotorSetSpd Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : MotorSetSpd Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : MotorSetSpd Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 90:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : MotorSetSpd Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		return 1;
  }

}



extern "C" DllExport long WINAPI XDMC_MotorSetAC(void * objptr, int iAxisNo, long lMotorAcc)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
     {
	   char command[100] ={0};
       char response [4096]={0};
       char cAxis;

	   switch(iAxisNo)
	   {
	        case 0: cAxis='A'; break;
	        case 1: cAxis='B'; break;
	        case 2: cAxis='C'; break;
	        case 3: cAxis='D'; break;
            case 4: cAxis='E'; break;
            case 5: cAxis='F'; break;
	        case 6: cAxis='G'; break;
	        case 7: cAxis='H'; break;
	   }

	   sprintf(command,"YSTRAC%c=%d",cAxis,lMotorAcc);
	
       ptrController->Command(command,response,sizeof(response)); 
       
	   if(strchr(response,'?')) 
	     return 1; //error
       else
         return 0;
	 }
	 else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : MotorSetAC Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : MotorSetAC Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : MotorSetAC Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 100:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : MotorSetAC Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		return 1;
  }

}


extern "C" DllExport long WINAPI XDMC_MotorSetDC(void * objptr, int iAxisNo, long lMotorDec)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
     {
	   char command[100] ={0};
       char response [4096]={0};
       char cAxis;

	   switch(iAxisNo)
	   {
	        case 0: cAxis='A'; break;
	        case 1: cAxis='B'; break;
	        case 2: cAxis='C'; break;
	        case 3: cAxis='D'; break;
            case 4: cAxis='E'; break;
            case 5: cAxis='F'; break;
	        case 6: cAxis='G'; break;
	        case 7: cAxis='H'; break;
	   }

	   sprintf(command,"YSTRDC%c=%d",cAxis,lMotorDec);
	
       ptrController->Command(command,response,sizeof(response)); 
       
	   if(strchr(response,'?')) 
	     return 1; //error
       else
         return 0;
	 }
	 else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }
  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : MotorSetDC Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : MotorSetDC Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : MotorSetDC Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 110:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : MotorSetDC Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		 return 1;
  }

}

extern "C" DllExport long WINAPI XDMC_MotorSetStartPos(void * objptr, int iAxisNo,double lMotorPos)
{
 try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
     {
          
	   char command[100] ={0}; 
       char response [4096]={0};

	   char cAxis;

	   switch(iAxisNo)
	   {
	        case 0: cAxis='A'; break;
	        case 1: cAxis='B'; break;
	        case 2: cAxis='C'; break;
	        case 3: cAxis='D'; break;
            case 4: cAxis='E'; break;
            case 5: cAxis='F'; break;
	        case 6: cAxis='G'; break;
	        case 7: cAxis='H'; break;
	   }
       
	    sprintf(command,"YSTRPOS%c=%.3f",cAxis,lMotorPos); 

	   
        //_bstr_t bstrCommand1 ( command);
         //   ::MessageBox (0, (wchar_t*) bstrCommand1 , _T("XDMC DLL"), 
          //         MB_ICONEXCLAMATION | MB_OK); 

        ptrController->Command(command,response,sizeof(response));
		
		 //_bstr_t bstrCommand ( response);
         //   ::MessageBox (0, (wchar_t*) bstrCommand , _T("XDMC DLL"), 
           //        MB_ICONEXCLAMATION | MB_OK); 
       
	    if(strchr(response,'?')) 
	     return 1; //error
        else
         return 0;
	 }
	 else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

 }
 catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : MotorSetStartPos Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : MotorSetStartPos Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : MotorSetStartPos Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 120:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : MotorSetStartPos Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		return 1;
  }
}

extern "C" DllExport long WINAPI XDMC_MotorSetStartPosArray(void * objptr, int iAxisNo,double *lMotorPosArray, long lArraySize, long iRecipeCount)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
     {
	   char command[1500] ={0}; 
	   char temp[200]={0};
       char response [4096]={0};

	   char cAxis;

	   switch(iAxisNo)
	   {
	        case 0: cAxis='A'; break;
	        case 1: cAxis='B'; break;
	        case 2: cAxis='C'; break;
	        case 3: cAxis='D'; break;
            case 4: cAxis='E'; break;
            case 5: cAxis='F'; break;
	        case 6: cAxis='G'; break;
	        case 7: cAxis='H'; break;
	   }
       
	   sprintf(command,"YSTRPOS%c[%d]=%.3f",cAxis,1,lMotorPosArray[1]); 

	   for (int i = 2; i <= iRecipeCount  ;i++ )
	   {
         sprintf(temp,";YSTRPOS%c[%d]=%.3f",cAxis,i,lMotorPosArray[i]); 
		 strcat(command,temp);
	   }
	   sprintf(temp,";Tcounts=%d",iRecipeCount);
	   strcat(command,temp);

        //_bstr_t bstrCommand ( command);
        //    ::MessageBox (0, (wchar_t*) bstrCommand , _T("XDMC DLL"), 
         //          MB_ICONEXCLAMATION | MB_OK); 

       ptrController->Command(command,response,sizeof(response)); 
       
	   if(strchr(response,'?')) 
	     return 1; //error
       else
         return 0;
	 }
	 else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : MotorSetStartPosArray Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : MotorSetStartPosArray Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : MotorSetStartPosArray Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 130:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : MotorSetStartPosArray Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		  return 1;
  }

}



extern "C" DllExport long WINAPI XDMC_MotorSetStandbyPos(void * objptr, int iAxisNo, long lStandbyPos)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
     {
	   char command[100] ={0};
       char response [4096]={0};
       char cAxis;

	   switch(iAxisNo)
	   {
	        case 0: cAxis='A'; break;
	        case 1: cAxis='B'; break;
	        case 2: cAxis='C'; break;
	        case 3: cAxis='D'; break;
            case 4: cAxis='E'; break;
            case 5: cAxis='F'; break;
	        case 6: cAxis='G'; break;
	        case 7: cAxis='H'; break;
	   }

	   sprintf(command,"YSTDY%c=%d",cAxis,lStandbyPos);
	   
        //_bstr_t bstrCommand ( command);
        //    ::MessageBox (0, (wchar_t*) bstrCommand , _T("XDMC DLL"), 
         //           MB_ICONEXCLAMATION | MB_OK); 

       ptrController->Command(command,response,sizeof(response)); 
       
	   if(strchr(response,'?')) 
	     return 1; //error
       else
         return 0;
	 }
	 else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }
  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : MotorSetStandbyPos Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : MotorSetStandbyPos Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : MotorSetStandbyPos Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 140:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : MotorSetStandbyPos Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		  return 1;
  }

}



extern "C" DllExport long WINAPI XDMC_MotorSetStandbyMotion(void * objptr, int iAxisNo, long lStandbyAcc, long lStandbyDec, long lStandbySpd)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
     {
	   char command[100] ={0};
       char response [4096]={0};
       char cAxis;

	   switch(iAxisNo)
	   {
	        case 0: cAxis='A'; break;
	        case 1: cAxis='B'; break;
	        case 2: cAxis='C'; break;
	        case 3: cAxis='D'; break;
            case 4: cAxis='E'; break;
            case 5: cAxis='F'; break;
	        case 6: cAxis='G'; break;
	        case 7: cAxis='H'; break;
	   }

	   sprintf(command,"YSTDYAC=%d;YSTDYDC=%d;YSTDYSPD=%d",lStandbyAcc,lStandbyDec,lStandbySpd);
	   
        //_bstr_t bstrCommand ( command);
        //    ::MessageBox (0, (wchar_t*) bstrCommand , _T("XDMC DLL"), 
         //           MB_ICONEXCLAMATION | MB_OK); 

       ptrController->Command(command,response,sizeof(response)); 
       
	   if(strchr(response,'?')) 
	     return 1; //error
       else
         return 0;
	 }
	 else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }
  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : MotorSetStandbyMotion Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : MotorSetStandbyMotion Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : MotorSetStandbyMotion Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 150:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : MotorSetStandbyMotion Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		  return 1;
  }

}


extern "C" DllExport long WINAPI XDMC_MotorSetStartMotion(void * objptr, int iAxisNo ,long lStartAcc, long lStartDec, long lStartSpd,bool bFlagCut)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
     {
	   char command[100] ={0};
       char response [4096]={0};
       char cAxis;

	   switch(iAxisNo)
	   {
	        case 0: cAxis='A'; break;
	        case 1: cAxis='B'; break;
	        case 2: cAxis='C'; break;
	        case 3: cAxis='D'; break;
            case 4: cAxis='E'; break;
            case 5: cAxis='F'; break;
	        case 6: cAxis='G'; break;
	        case 7: cAxis='H'; break;
	   }

	   if (bFlagCut)
	      sprintf(command,"YCUTAC%c=%d;YCUTDC%c=%d;YCUTSPD%c=%d",cAxis,lStartAcc,cAxis,lStartDec,cAxis,lStartSpd);
	   else
	      sprintf(command,"YSTRAC%c=%d;YSTRDC%c=%d;YSTRSPD%c=%d",cAxis,lStartAcc,cAxis,lStartDec,cAxis,lStartSpd);
	   
        //_bstr_t bstrCommand ( command);
          //  ::MessageBox (0, (wchar_t*) bstrCommand , _T("XDMC DLL"), 
            //        MB_ICONEXCLAMATION | MB_OK); 

       ptrController->Command(command,response,sizeof(response)); 
       
	   if(strchr(response,'?')) 
	     return 1; //error
       else
         return 0;
	 }
	 else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }
  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : MotorSetStartMotion Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : MotorSetStartMotion Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : MotorSetStartMotion Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 160:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : MotorSetStartMotion Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		  return 1;
  }

}

extern "C" DllExport long WINAPI XDMC_STOP(void * objptr, int iAxisNo, bool bAllAxis)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
     {
	   char *command;
       char response [4096]={0};

	   if (bAllAxis)
          command ="ST";
       else
       {
	     switch(iAxisNo)
		 {
	        case 0: command="STA;HX0"; break;
	        case 1: command="STB;HX2"; break;
	        case 2: command="STC"; break;
	        case 3: command="STD"; break;
            case 4: command="STE"; break;
            case 5: command="STF"; break;
	        case 6: command="STG"; break;
	        case 7: command="STH"; break;
		 }
	   }

       ptrController->Command(command,response,sizeof(response)); 
       
	   if(strchr(response,'?')) 
	     return 1; //error
       else
         return 0;
	 }
	 else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : STOP Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : STOP Exception Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : STOP Exception Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 170:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : STOP Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		  return 1;
  }

}





extern "C" DllExport long WINAPI XDMC_MoveStandby(void * objptr, int iAxisNo, int iDmcThread)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
     {
	   
       char command[100]={0};	 
       char response [4096]={0};
       char cAxis;

	   switch(iAxisNo)
	   {
	        case 0: cAxis='A'; break;
	        case 1: cAxis='B'; break;
	        case 2: cAxis='C'; break;
	        case 3: cAxis='D'; break;
            case 4: cAxis='E'; break;
            case 5: cAxis='F'; break;
	        case 6: cAxis='G'; break;
	        case 7: cAxis='H'; break;
	   }

	   if (iDmcThread)
		  sprintf(command,"XQ#STDBY%c,%d",cAxis,iDmcThread);
	   else if (iDmcThread == 0) 
		  sprintf(command,"XQ#STDBY%c",cAxis);
	   
        
 	   //::ShowError (iDmcThread, _T("Controller Not Enabled"));

	   //_bstr_t bstrCommand ( command);
         //  ::MessageBox (0, (wchar_t*) bstrCommand , _T("XDMC DLL"), 
           //       MB_ICONEXCLAMATION | MB_OK); 
       ptrController->Clear(); 
	   ptrController->Command(command,response,sizeof(response)); 
       
	   if(strchr(response,'?')) 
	     return 1; //error
       else
         return 0;
	 }
     else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : MoveStandby Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : MoveStandby Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : MoveStandby Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 190:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : MoveStandby Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		return 1;
  }

}

extern "C" DllExport long WINAPI XDMC_MoveSTARDBY(void * objptr)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
     {
	   
       char *command = "XQ#STARDBY";	 
       char response [4096]={0};
       
 	   //::ShowError (iDmcThread, _T("Controller Not Enabled"));

	   //_bstr_t bstrCommand ( command);
         //  ::MessageBox (0, (wchar_t*) bstrCommand , _T("XDMC DLL"), 
           //       MB_ICONEXCLAMATION | MB_OK); 
       ptrController->Clear(); 
	   ptrController->Command(command,response,sizeof(response)); 
       
	   if(strchr(response,'?')) 
	     return 1; //error
       else
         return 0;
	 }
     else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : MoveSTARDBY Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : MoveSTARDBY Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : MoveSTARDBY Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 190:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : MoveSTARDBY Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		return 1;
  }

}


extern "C" DllExport long WINAPI XDMC_SetMotorCycleCount(void * objptr,long lCount)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
     
     CXDMC *ptrController = (CXDMC*)objptr;

   if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
   {

        char response[4096]={0};
        char *command ;
		char *ptrValue ; 
		  // _bstr_t bstrComm ( command);
           // ::MessageBox (0, (wchar_t*) bstrComm , _T("XDMC DLL"), 
           //        MB_ICONEXCLAMATION | MB_OK); 
		
		sprintf(command,"count=%d", lCount);
		//ptrController->Clear() ;
	    ptrController->Command(command,response,sizeof(response)); 
		
		ptrValue = strchr(response,':');
		if ((ptrValue != NULL) && (*ptrValue == ':'))
		{   
			  return 0;
			//_bstr_t bstrResponse ( response);
            //::MessageBox (0, (wchar_t*) bstrResponse , _T("XDMC DLL"), 
              //       MB_ICONEXCLAMATION | MB_OK); 
            
		}
		else if (ptrValue = strchr(response,'?'))
		{
             
              ldllErr = 30;
	          throw (ldllErr);	

			  return 1;
			
			 //  _bstr_t bstrResponse ( ptrValue);
            //::MessageBox (0, (wchar_t*) bstrResponse , _T("XDMC DLL Exception"), 
            //         MB_ICONEXCLAMATION | MB_OK); 
		}
		 
	 }	
     else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : SetMotorCycleCount Exception"),MB_ICONEXCLAMATION | MB_OK); return 1; break;
	    case 30: ::MessageBox (0, _T("Check Command Syntax"), _T("XDMC DLL : SetMotorCycleCount Exception"),MB_ICONEXCLAMATION | MB_OK); return 1; break;
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : SetMotorCycleCount Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : SetMotorCycleCount Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 200:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : SetMotorCycleCount Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		return 1;
  }

}

extern "C" DllExport long WINAPI XDMC_GetMotorCycleCount(void * objptr)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
     
     CXDMC *ptrController = (CXDMC*)objptr;

   if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
   {

        char response[4096]={0};
        char *command = "count=?";
        char *ptrValue ; 
		  // _bstr_t bstrComm ( command);
           // ::MessageBox (0, (wchar_t*) bstrComm , _T("XDMC DLL"), 
           //        MB_ICONEXCLAMATION | MB_OK); 
		   
		//ptrController->Clear() ;
	    ptrController->Command(command,response,sizeof(response)); 
		
		ptrValue = strchr(response,':');
		if ((ptrValue != NULL) && (*ptrValue == ':'))
		{
              *ptrValue = '\0';
		      ++ptrValue;
            
			  return atol(response);
			//_bstr_t bstrResponse ( response);
            //::MessageBox (0, (wchar_t*) bstrResponse , _T("XDMC DLL"), 
              //       MB_ICONEXCLAMATION | MB_OK); 
            
		}
		else if (ptrValue = strchr(response,'?'))
		{
             
              ldllErr = 30;
	          throw (ldllErr);	
			
			 //  _bstr_t bstrResponse ( ptrValue);
            //::MessageBox (0, (wchar_t*) bstrResponse , _T("XDMC DLL Exception"), 
            //         MB_ICONEXCLAMATION | MB_OK); 
		}
		 
	 }	
     else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : GetMotorCycleCount Exception"),MB_ICONEXCLAMATION | MB_OK); break;
	    case 30: ::MessageBox (0, _T("Check Command Syntax"), _T("XDMC DLL : GetMotorCycleCount Exception"),MB_ICONEXCLAMATION | MB_OK); break;
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : GetMotorCycleCount Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : GetMotorCycleCount Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 210:
	  }
	  return 1;
  }

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : GetMotorCycleCount Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		return 1;
  }

}

extern "C" DllExport long WINAPI XDMC_GetModLive(void * objptr)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
     
     CXDMC *ptrController = (CXDMC*)objptr;
     char response[4096]={0};

	 if(ptrController->IsConnected())
	 {
        ptrController->Command("MODLIVE = ?",response,sizeof(response)); 

		if(strchr(response,'1') && (ptrController->m_bEnabled == TRUE)) 
	      return 1;
        else if (strchr(response,'0') && (ptrController->m_bEnabled == FALSE)) 
	      return 0;
		 
	 }	
     else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : GetModLive Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : GetModLive Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : GetModLive Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 220:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : GetModLive Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		return 1;
  }

}



extern "C" DllExport long WINAPI XDMC_SetModLive(void * objptr, bool OnOff)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 if(ptrController->IsConnected())
     {
       /*
       	LONG hand;
		hand = ptrController->GetHandle();
		 ::ShowError (hand, _T("This is the galil handle"));
         */
       char response[4096]={0};

	   if (OnOff) //== TRUE)
	   { 
		 ptrController->m_bEnabled = TRUE; 
	     //ptrController->FastCommand("MODLIVE=1");
		 
		 ptrController->Command("MODLIVE=1",response,sizeof(response)); 
       
         //_bstr_t bstrResponse ( response);
           // ::MessageBox (0, (wchar_t*) bstrResponse , _T("XDMC DLL"), 
             //      MB_ICONEXCLAMATION | MB_OK); 

	     //hand = ptrController->GetHandle();
		 //::ShowError (hand, _T("This is the galil handle"));

        /*LONG hand = ptrController->GetHandle();
		 ::ShowError (hand, _T("This is the galil handle"));
      
	 hand =ptrController->GetKeepHandle();
	 ::ShowError(hand ,_T("This XDMC.dll handle"));
	 ::ShowError((LONG) objptr,_T("This object pointer fron VB"));
      */

		 if(strchr(response,'?')) 
	       return 1; //error
         else
           return 0;
	   }
	   else
	   { 
		 ptrController->m_bEnabled = FALSE;
	     //ptrController->FastCommand("MODLIVE=0"); 
		 
		 ptrController->Command("MODLIVE=0",response,sizeof(response)); 
       
	     if(strchr(response,'?')) 
	       return 1; //error
         else
           return 0;
	   
	   }
       
	 }
	 else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : SetModLive Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : SetModLive Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : SetModLive Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 230:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : SetModLive Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		return 1;
  }

}



extern "C" DllExport long WINAPI XDMC_DownloadSystemFile(void * objptr, BSTR bstrDMCFile)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
     {
	   	
		 _bstr_t bstrDMC (bstrDMCFile,true);
		char * dmcFile = bstrDMC;
		
		::MessageBox (0, (wchar_t*) bstrDMC , _T("XDMC DLL"), 
                 MB_ICONEXCLAMATION | MB_OK); 
		
		ptrController->DownloadFile(dmcFile,NULL);

	     return 0;
	 }
	 else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : DownloadSystemFile Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : DownloadSystemFile Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : DownloadSystemFile Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 240:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : DownloadSystemFile Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		return 1;
  }

}



extern "C" DllExport long WINAPI XDMC_ModReset(void * objptr)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	
	 ptrController->Reset(); 

	return 0;
  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : ModReset Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : ModReset Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
  }

}

extern "C" DllExport long WINAPI XDMC_Close(void * objptr)
{
 try
 {
    LONG    ldllErr;
	LONG    RC;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
    
	 CXDMC * ptrController = (CXDMC*) objptr ;
    
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
	 {
        
	  ptrController->Command("ST",ptrController->m_sResponse,sizeof(ptrController->m_sResponse));
      //ptrController->DiagnosticsOff (); 
	   LONG hand = ptrController->GetHandle();
		 //::ShowError (hand, _T("Rel...This is the galil handle"));
      
	   //hand =ptrController->GetKeepHandle();
	   //::ShowError(hand ,_T("Rel...This XDMC.dll handle"));
	   //::ShowError((LONG) objptr,_T("Rel...This object pointer fron VB"));
	   //::ShowError((LONG) &ptrController,_T("This object pointer"));

	   RC = ptrController->Close();
	   if (RC != DMCNOERROR)
	   { 
              //char szMessage[128];
			  //ptrController->GetErrorText(RC,szMessage,sizeof(szMessage));
			  //_bstr_t bstrErrText(szMessage);
			  //::MessageBox (0, (wchar_t*) bstrErrText, _T("XDMC DLL : Init Exception"),MB_ICONEXCLAMATION | MB_OK);
              
	   }
	   delete ptrController;
       
	   ptrController =NULL;
	   return 0;
	 }
	 else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

   
 }
 
  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : ModReset Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : DownloadSystemFile Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : DownloadSystemFile Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 250:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : ModReset Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		return 1;
  }

}

//*****************************************************************************************************
//---------------------------------DSP 620 Application specific Functions-------------------------------*
//                                             Galil IO is used
//*****************************************************************************************************

extern "C" DllExport long WINAPI XDSP_GetAppFlags(void * objptr,long lFlagNo )
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
     
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 char response[4096]={0};
	 char *command = 0;

	 //long  lFlagNo = 0;
	 long resultFlag = 0;

	 if(ptrController->IsConnected())
	 {  

        //_bstr_t bstrCmd(bstrVarName,true);
		//long lLen= (long)SysStringLen(bstrVarName);

		//::MessageBox (0, _T("Before testing VarName"), _T("XDMC DLL : GetMotorBusy"), 
                      //MB_ICONEXCLAMATION | MB_OK);
      
	  /*
	  for (lFlagNo=1;lFlagNo < 9 ;lFlagNo++) 
	  {
*/	      switch(lFlagNo)
		  {
	        case 1:  command ="DxCm=?"; break;
	        case 2:  command ="DxHm=?"; break;
	        case 3:  command ="TpCl=?"; break;
	        case 4:  command ="FfCl=?"; break;
            case 5:  command ="RfCl=?"; break;
            case 6:  command ="PnCm=?"; break;
	        case 7:  command ="PnDw=?"; break;
	        case 8:  command ="PnUp=?"; break;
            case 9:  command ="CtDx=?"; break;
  
		  }	 
				 
		 ptrController->Command(command,response,sizeof(response)); 

		if(strchr(response,'1') ) 
	      resultFlag = 1;
 
		//_bstr_t bstrCommand ( command);
          //  ::MessageBox (0, (wchar_t*) bstrCommand , _T("XDMC DLL"), 
            //       MB_ICONEXCLAMATION | MB_OK); 

        //::ShowError(resultFlag,_T("Flag result"));
        //response[0]='\0';
	 // }
      return resultFlag;
	  
	 }	
     else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : GetAppFlags Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : GetAppFlags Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : GetAppFlags Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 220:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : GetAppFlags Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		return 1;
  }

}


extern "C" DllExport long WINAPI XDSP_GetInputFlag(void * objptr,long lFlagNo)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
     
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 char response[4096]={0};
	 char *command = 0;
     char *ptrValue;
	 
	 long resultFlag = 0;

	 if(ptrController->IsConnected())
	 {  

        //_bstr_t bstrCmd(bstrVarName,true);
		//long lLen= (long)SysStringLen(bstrVarName);

		//::MessageBox (0, _T("Before testing VarName"), _T("XDMC DLL : GetMotorBusy"), 
                      //MB_ICONEXCLAMATION | MB_OK);
      
	      switch(lFlagNo)
		  {
	        case 0: command ="TI0"; break;
	        case 2: command ="TI2"; break;
	        case 3: command ="TI3=?"; break;
	        
		  }	 
				 
 		 ptrController->Command(command,response,sizeof(response)); 

		 ptrValue = strchr(response,':');
		 if ((ptrValue != NULL) && (*ptrValue == ':'))
		 {
           *ptrValue = '\0';
		   ++ptrValue;
         }
         
		 return atol(response);

	 //::ShowError(hand ,_T("This XDMC.dll handle"));
	 //::ShowError((LONG) objptr,_T("This object pointer fron VB"));

	 }	
     else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : GetInputFlag Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : GetInputFlag Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : GetInputFlag Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 220:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : GetInputFlag Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		return 1;
  }

}

extern "C" DllExport long WINAPI XDSP_StartIndexer(void * objptr, int iAxisNo,bool bFlagCut, long iDmcThread)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
     {
	   char command[100]={0};
       char response [4096]={0};
	   //char * Entry;
	   //char *time;
	   //BYTE BinaryCommand[32];
       //ULONG BinaryCommandLength;
	   char cAxis;

	   
         switch(iAxisNo)
		 {
	        case 0: cAxis='A'; break;
	        case 1: cAxis='B'; break;
	        case 2: cAxis='C'; break;
	        case 3: cAxis='D'; break;
            case 4: cAxis='E'; break;
            case 5: cAxis='F'; break;
	        case 6: cAxis='G'; break;
	        case 7: cAxis='H'; break;
		 }	   
	 
		   
	   if (bFlagCut)
           sprintf(command,"XQ#AFRCUTA");
	   else if (iDmcThread)
	        //sprintf(command,"XQ#MANMODE,%d",cAxis,iDmcThread);
           sprintf(command,"XQ#MANMODE,%d",iDmcThread);
	   else
            sprintf(command,"XQ#START%c",cAxis);

 
		   
	   //_bstr_t bstrCommand ( command);
         //   ::MessageBox (0, (wchar_t*) bstrCommand , _T("XDMC DLL"), 
           //        MB_ICONEXCLAMATION | MB_OK); 
       //Entry = "Start Indexer";
	   //ptrController->TimeStamp(time);
	   //strncat(Entry,time,110);
	   //ptrController->TroubleShoot (Entry);
	   //Need further investigation
	   //ptrController->Clear() ;
   
		// Convert a command from ASCII to binary
        //ptrController->AsciiCommandToBinaryCommand(command, strlen(command), BinaryCommand,sizeof(BinaryCommand), &BinaryCommandLength);

		// Send the binary command to the controller and get the response
        //ptrController->BinaryCommand(BinaryCommand, BinaryCommandLength, response,sizeof(response));

	
	   ptrController->Command(command,response,sizeof(response)); 
       
	   if(strchr(response,'?')) 
	     return 1; //error
       else
         return 0;
	 }
	 else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : XDSP_StartIndexer Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : XDSP_StartIndexer Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : XDSP_StartIndexer Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 180:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : XDSP_StartIndexer Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		  return 1;
  }

}

extern "C" DllExport long WINAPI XDSP_GetMotionStatus(void * objptr, int iAxisNo )//, BSTR bstrVarName )
{
 try
 {
   LONG    ldllErr;

   if(NULL == objptr)
   {
     ldllErr =10;
	 throw(ldllErr);
   }
 

   CXDMC *ptrController = (CXDMC*)objptr;
   
   //::MessageBox (0, _T("Before testing connection"), _T("XDMC DLL : GetMotorBusy"), 
    //                  MB_ICONEXCLAMATION | MB_OK);

   if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
   {

        char response1[4096]={0};
        char command[100] ={0};

        //_bstr_t bstrCmd(bstrVarName,true);
		//long lLen= (long)SysStringLen(bstrVarName);

		//::MessageBox (0, _T("Before testing VarName"), _T("XDMC DLL : GetMotorBusy"), 
                      //MB_ICONEXCLAMATION | MB_OK);

		//if (SysStringLen(bstrVarName) == NULL)//bstrVarName == NULL)
		//{
          char cAxis;
	      int  iMotionStatus;

	      switch(iAxisNo)
		  {
	        case 0: cAxis='A'; break;
	        case 1: cAxis='B'; break;
	        case 2: cAxis='C'; break;
	        case 3: cAxis='D'; break;
            case 4: cAxis='E'; break;
            case 5: cAxis='F'; break;
	        case 6: cAxis='G'; break;
	        case 7: cAxis='H'; break;
		  }

	       sprintf(command,"TS%c", cAxis );
		  // _bstr_t bstrComm ( command);
           // ::MessageBox (0, (wchar_t*) bstrComm , _T("XDMC DLL"), 
           //        MB_ICONEXCLAMATION | MB_OK); 
		   
		   //ptrController->Clear() ;
	       ptrController->Command(command,response1,sizeof(response1)); 

		  // _bstr_t bstrResponse ( response1);
           // ::MessageBox (0, (wchar_t*) bstrResponse , _T("XDMC DLL"), 
           //        MB_ICONEXCLAMATION | MB_OK); 

		   char *ptrValue;

		    ptrValue = strchr(response1,':');
		    if ((ptrValue != NULL) && (*ptrValue == ':'))
			{
              *ptrValue = '\0';
		      ++ptrValue;
            
			}

	       iMotionStatus= atoi(response1);

           //::ShowError ((long)iMotionStatus, _T("iMotionStatus"));

	      if( (iMotionStatus & 0x80))
		     return 1;
	      else
		     return 0;
/*          {
	         //::MessageBox (0, _T("shouldn't come here"), _T("XDMC DLL : GetMotorBusy"), 
             //         MB_ICONEXCLAMATION | MB_OK);
			 sprintf(command,"YBUSY%c=?",cAxis);
	         
			 char response2[4096]={0};
             ptrController->Command(command,response2,sizeof(response2)); 

			// _bstr_t bstrResponse ( response2);
			 //::MessageBox (0, (wchar_t*) bstrResponse , _T("XDMC DLL"), 
               //   MB_ICONEXCLAMATION | MB_OK); 

		     /*
			 ptrValue = strchr(response2,':');
		    if ((ptrValue != NULL) && (*ptrValue == ':'))
			{
              *ptrValue = '\0';
		      ++ptrValue;
            
			
            
			}
             int iValue =atoi(response2);

			 ::ShowError ((long)iValue, _T("iMotionStatus"));
	         if( iValue != NULL)
			{
			  ::MessageBox (0, _T("Return true") , _T("XDMC DLL"), 
                  MB_ICONEXCLAMATION | MB_OK);	
				 return 1;
			 }
			 else
			{	 
	          ::MessageBox (0, _T("Return false") , _T("XDMC DLL"), 
                  MB_ICONEXCLAMATION | MB_OK);
				 return 0;
			 }
			 
			 ptrValue = strchr(response2,'1');
			 if ((ptrValue != NULL) && (*ptrValue == '1'))
			 {
			  ::MessageBox (0, _T("Return true") , _T("XDMC DLL"), 
                  MB_ICONEXCLAMATION | MB_OK);	 
	           return TRUE;
			 }



             ptrValue = strchr(response2,'0');
			 if ((ptrValue != NULL) && (*ptrValue == '0'))
			 {	 
	          ::MessageBox (0, _T("Return flase") , _T("XDMC DLL"), 
                  MB_ICONEXCLAMATION | MB_OK);
				 return FALSE;
			 }
			 */
/*
			 if (strchr(response2,'1'))
				 return 1;
			 else if(strchr(response2,'0'))
			//	{	 
	         // ::MessageBox (0, _T("Return false") , _T("XDMC DLL"), 
               //   MB_ICONEXCLAMATION | MB_OK);
				 return 0;
			 //}
			 
			
		  } */
   }
   else if (! ptrController->IsConnected()) 
   {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
   }
   else if (! ptrController->m_bEnabled )
   {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

   }
   //return TRUE;
 }

  catch (LONG ErrorCode)
	{
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : GetMotionStatus Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : GetMotionStatus Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : GetMotionStatus Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port

			//case 40
	  }
	  return 1;
	} 

	catch (...)
    {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : GetMotionStatus Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
          return 1;
    }
}

extern "C" DllExport long WINAPI XDMC_GetMotionStatus(void * objptr, int iAxisNo)
{
 try
 {
   LONG    ldllErr;

   if(NULL == objptr)
   {
     ldllErr =10;
	 throw(ldllErr);
   }
 

   CXDMC *ptrController = (CXDMC*)objptr;
   
   //::MessageBox (0, _T("Before testing connection"), _T("XDMC DLL : GetMotorBusy"), 
    //                  MB_ICONEXCLAMATION | MB_OK);

   if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
   {

        char response[4096]={0};
        char command[100] ={0};

        //_bstr_t bstrCmd(bstrVarName,true);
		//long lLen= (long)SysStringLen(bstrVarName);

		//::MessageBox (0, _T("Before testing VarName"), _T("XDMC DLL : GetMotorBusy"), 
                      //MB_ICONEXCLAMATION | MB_OK);

		//if (SysStringLen(bstrVarName) == NULL)//bstrVarName == NULL)
		//{
           
	         //::MessageBox (0, _T("shouldn't come here"), _T("XDMC DLL : GetMotorBusy"), 
             //         MB_ICONEXCLAMATION | MB_OK);
			 
   		char cAxis;

	    switch(iAxisNo)
		{
	        case 0: cAxis='A'; break;
	        case 1: cAxis='B'; break;
	        case 2: cAxis='C'; break;
	        case 3: cAxis='D'; break;
            case 4: cAxis='E'; break;
            case 5: cAxis='F'; break;
	        case 6: cAxis='G'; break;
	        case 7: cAxis='H'; break;
		}

		
		sprintf(command,"YBUSY%c=?",cAxis);
	         
		ptrController->Command(command,response,sizeof(response)); 

			// _bstr_t bstrResponse ( response2);
			 //::MessageBox (0, (wchar_t*) bstrResponse , _T("XDMC DLL"), 
               //   MB_ICONEXCLAMATION | MB_OK); 

		     /*
			 ptrValue = strchr(response2,':');
		    if ((ptrValue != NULL) && (*ptrValue == ':'))
			{
              *ptrValue = '\0';
		      ++ptrValue;
            
			
            
			}
             int iValue =atoi(response2);

			 ::ShowError ((long)iValue, _T("iMotionStatus"));
	         if( iValue != NULL)
			{
			  ::MessageBox (0, _T("Return true") , _T("XDMC DLL"), 
                  MB_ICONEXCLAMATION | MB_OK);	
				 return 1;
			 }
			 else
			{	 
	          ::MessageBox (0, _T("Return false") , _T("XDMC DLL"), 
                  MB_ICONEXCLAMATION | MB_OK);
				 return 0;
			 }
			 
			 ptrValue = strchr(response2,'1');
			 if ((ptrValue != NULL) && (*ptrValue == '1'))
			 {
			  ::MessageBox (0, _T("Return true") , _T("XDMC DLL"), 
                  MB_ICONEXCLAMATION | MB_OK);	 
	           return TRUE;
			 }



             ptrValue = strchr(response2,'0');
			 if ((ptrValue != NULL) && (*ptrValue == '0'))
			 {	 
	          ::MessageBox (0, _T("Return flase") , _T("XDMC DLL"), 
                  MB_ICONEXCLAMATION | MB_OK);
				 return FALSE;
			 }
			 */

			 if (strchr(response,'1'))
				 return 1;
			 else if(strchr(response,'0'))
			//	{	 
	         // ::MessageBox (0, _T("Return false") , _T("XDMC DLL"), 
               //   MB_ICONEXCLAMATION | MB_OK);
				 return 0;
			 //}
			 
			
		  
   }
   else if (! ptrController->IsConnected()) 
   {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
   }
   else if (! ptrController->m_bEnabled )
   {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

   }
   //return TRUE;
 }

  catch (LONG ErrorCode)
	{
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : GetMotionStatus Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : GetMotionStatus Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : GetMotionStatus Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port

			//case 40
	  }
	  return 1;
	} 

	catch (...)
    {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : GetMotionStatus Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
          return 1;
    }
}

extern "C" DllExport long WINAPI XDSP_SetAppFlag(void * objptr,long lFlagNo )
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
     
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 char response[4096]={0};
	 char *command = 0;

	 //long  lFlagNo = 0;
	 long resultFlag = 0;

	 if(ptrController->IsConnected())
	 {  

        //_bstr_t bstrCmd(bstrVarName,true);
		//long lLen= (long)SysStringLen(bstrVarName);

		//::MessageBox (0, _T("Before testing VarName"), _T("XDMC DLL : GetMotorBusy"), 
                      //MB_ICONEXCLAMATION | MB_OK);
      
	      switch(lFlagNo)
		  {
	        case 1:  command ="EdRy=1"; break;
	        case 2:  command ="CtCm=1"; break;
	        case 3:  command ="PgCm=1"; break;
		  }	 
				 
		 ptrController->Command(command,response,sizeof(response)); 

		if(strchr(response,'?')) 
	     return 1; //error
       else
         return 0;
	  
	 }	
     else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : SetAppFlag Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : SetAppFlag Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : SetAppFlag Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 220:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : SetAppFlag Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		return 1;
  }

}


extern "C" DllExport long WINAPI XDSP_SetOutput(void * objptr,long lModuleNo, long lONOFF )
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
     
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 char response[4096]={0};
	 char *command = 0;

	 if(ptrController->IsConnected())
	 {  

        //_bstr_t bstrCmd(bstrVarName,true);
		//long lLen= (long)SysStringLen(bstrVarName);

		//::MessageBox (0, _T("Before testing VarName"), _T("XDMC DLL : GetMotorBusy"), 
                      //MB_ICONEXCLAMATION | MB_OK);
      
	      switch(lModuleNo)
		  {
	        case 1:  
				if (lONOFF == 1)
					command ="XQ#TCLON";
                else
                    command ="XQ#TCLOFF";
				break;
	        case 2:  
				if (lONOFF == 1)
					command ="XQ#FSCLON";
                else
                    command ="XQ#FSCLOFF";
				break;
			
			case 3:  
				if (lONOFF == 1)
					command ="XQ#RSCLON";
                else
                    command ="XQ#RSCLOFF";
				break;
			
            case 4:  
				if (lONOFF == 1)
					command ="XQ#SCLON";
                else
                    command ="XQ#SCLOFF";
				break;
			
		  }	 
				 
		 ptrController->Command(command,response,sizeof(response)); 

		if(strchr(response,'?')) 
	     return 1; //error
       else
         return 0;
	  
	 }	
     else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : XDSP_SetOutput Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : XDSP_SetOutput Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : XDSP_SetOutput Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 220:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : XDSP_SetOutput Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		return 1;
  }

}


extern "C" DllExport long WINAPI XDSP_SetPunchArray(void * objptr,long *lPunchRecipe, int iRecipeCount)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
     {
	   char command[50] ={0}; 
	   char response [4096]={0};

	   
	   for (int i = 1; i < iRecipeCount +1  ;i++ )
	   {
         sprintf(command,";PnRc[%d]=%d",i,lPunchRecipe[i]); 
	     ptrController->Command(command,response,sizeof(response));	 
	     
		 if(strchr(response,'?')) 	     
		   break; //error
   	 
		 //strcat(command,temp);
	   }
	   //sprintf(temp,";Tcounts=%d",iRecipeCount);
	   //strcat(command,temp);

        //_bstr_t bstrCommand ( command);
          //  ::MessageBox (0, (wchar_t*) bstrCommand , _T("XDMC DLL"), 
            //       MB_ICONEXCLAMATION | MB_OK); 

       //ptrController->Command(command,response,sizeof(response)); 
       
	   if(i == iRecipeCount) 
		  return 1; //error
       else
          return 0;
	 }
	 else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : SetPunchArray Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : SetPunchArray Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : SetPunchArray Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 130:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : SetPunchArray Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		  return 1;
  }

}

extern "C" DllExport long WINAPI XDSP_SetCutterCount(void * objptr,long lCutCount)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
     {
	   char command[100] ={0}; 
	   char response [4096]={0};

	 
       sprintf(command,"CtCn = %d",lCutCount); 
	   ptrController->Command(command,response,sizeof(response));	 
	     
	   
        //_bstr_t bstrCommand ( command);
          //  ::MessageBox (0, (wchar_t*) bstrCommand , _T("XDMC DLL"), 
            //       MB_ICONEXCLAMATION | MB_OK); 

       
       if(strchr(response,'?')) 	     
		  return 1; //error
       else
          return 0;
	   

	 }
	 else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : SetPunchArray Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : SetPunchArray Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : SetPunchArray Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 130:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : SetPunchArray Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		  return 1;
  }

}

extern "C" DllExport long WINAPI XDSP_ManualMove(void * objptr, int iAxisNo, long iDmcThread)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
     {
		 char command[100]={0};
       char response [4096]={0};
	   //BYTE BinaryCommand[32];
       //ULONG BinaryCommandLength;
	   char cAxis;

	   
         switch(iAxisNo)
		 {
	        case 0: cAxis='A'; break;
	        case 1: cAxis='B'; break;
	        case 2: cAxis='C'; break;
	        case 3: cAxis='D'; break;
            case 4: cAxis='E'; break;
            case 5: cAxis='F'; break;
	        case 6: cAxis='G'; break;
	        case 7: cAxis='H'; break;
		 }	   
	 
		   
	   if (iDmcThread)
	        sprintf(command,"XQ#MANUAL%c,%d",cAxis,iDmcThread);
	   else
            sprintf(command,"XQ#MANUAL%c",cAxis);

 
		   
	   //_bstr_t bstrCommand ( command);
         //   ::MessageBox (0, (wchar_t*) bstrCommand , _T("XDMC DLL"), 
           //        MB_ICONEXCLAMATION | MB_OK); 
        //ptrController->Clear() ;
   
		// Convert a command from ASCII to binary
        //ptrController->AsciiCommandToBinaryCommand(command, strlen(command), BinaryCommand,sizeof(BinaryCommand), &BinaryCommandLength);

		// Send the binary command to the controller and get the response
        //ptrController->BinaryCommand(BinaryCommand, BinaryCommandLength, response,sizeof(response));

	
	   ptrController->Command(command,response,sizeof(response)); 
       
	   if(strchr(response,'?')) 
	     return 1; //error
       else
         return 0;
	 }
	 else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : ManualMove Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : ManualMove Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : ManualMove Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 180:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : ManualMove Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		  return 1;
  }
}


extern "C" DllExport long WINAPI XDMC_JOG(void * objptr,long iAxisNo, double dSpeed)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
     {
	   
		 char command[1000] ={0};	 
         char response [4096]={0};
         char cAxis;
 	   //::ShowError (iDmcThread, _T("Controller Not Enabled"));

	   
	   switch(iAxisNo)
	   {
	        case 0: cAxis='A'; break;
	        case 1: cAxis='B'; break;
	        case 2: cAxis='C'; break;
	        case 3: cAxis='D'; break;
            case 4: cAxis='E'; break;
            case 5: cAxis='F'; break;
	        case 6: cAxis='G'; break;
            case 7: cAxis='H'; break;
	   }
       
	   sprintf(command,"SH%c;JG%c=%f;BG%c",cAxis,cAxis,dSpeed,cAxis);
	    //_bstr_t bstrCommand ( command);
          // ::MessageBox (0, (wchar_t*) bstrCommand , _T("XDMC DLL"), 
            //      MB_ICONEXCLAMATION | MB_OK); 
      
	   ptrController->Clear(); 
	   ptrController->Command(command,response,sizeof(response)); 
       
	   if(strchr(response,'?')) 
	     return 1; //error
       else
         return 0;
	 }
     else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : JOG Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : JOG Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : JOG Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 190:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : JOG Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		return 1;
  }

}
//*****************************************************************************************************
//---------------------------------NWC Application specific Functions-------------------------------*
//                                             Galil IO is used
//*****************************************************************************************************


extern "C" DllExport long WINAPI XNWC_Start(void * objptr, int iFnRef, long lDmcThread)
{
  try
  {
    LONG    ldllErr;
	LONG    RC;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
     {
	   //char command [100] ={0};
       char *temp = 0;
	   //char response [4096]={0};
	   
	         memset(ptrController->m_sCommand,0, XCOMMAND_SIZE);
	   memset(ptrController->m_sResponse,0, XRESPONSE_SIZE);

         switch(iFnRef)
		 {
	        case 1:  temp ="AUTO";       break;
	        case 2:  temp ="REINIT";     break;
	        case 3:  temp ="INDEXER";    break;
	        case 4:  temp ="MAIN";       break;
			case 5:  temp ="MCAMERA";    break;
            case 6:  temp ="FCAMERA";    break;	
            case 7:  temp ="PLCSCN";     break;   
		 	case 8:  temp ="MINDEX";     break; 
			case 9:  temp ="XYHOME";     break;	
			case 10: temp ="VACON";      break;
			case 11: temp ="VACOFF";     break;
            case 12: temp ="TRGCAM";     break;
            case 13: temp ="MCURING";    break;
			case 14: temp ="AHOME"; 	 break;
            case 15: temp ="BHOME";      break;
		    case 16: temp ="CURGXY";     break;
		    case 17: temp ="STARTXY";    break;
		    case 18: temp ="RECOIL";    break;
		 }
		 if (lDmcThread)
	        sprintf(ptrController->m_sCommand,"XQ#%s,%d",temp,lDmcThread);
	     else
            sprintf(ptrController->m_sCommand,"XQ#%s",temp);
		//_bstr_t bstrCommand ( command);
          //  ::MessageBox (0, (wchar_t*) bstrCommand , _T("XDMC DLL"), 
            //       MB_ICONEXCLAMATION | MB_OK); 
	   //ptrController->Clear() ;
	   RC = ptrController->Command(ptrController->m_sCommand,ptrController->m_sResponse,sizeof(ptrController->m_sResponse)); 
       
	   if (RC != DMCNOERROR)
	   {
           char szMessage[128];
		   ptrController->GetErrorText(RC,szMessage,sizeof(szMessage));
		   
		   memset(ptrController->m_sLine,0,300);
			sprintf(ptrController->m_sLine,"Start Error:Command %s  - Error : %d  -Error :%s",ptrController->m_sCommand,RC ,szMessage);
            Applog.LogEntry(ptrController->m_sLine);
			
		   class _totchar cl(szMessage);
		   
		   ShowError(RC, cl);
	       return 1;	 
	   }

	   if(strchr(ptrController->m_sResponse,'?')) 
	     return 1; //error
       else
         return 0;
	 }
	 else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : XDSP_Start Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : XDSP_Start Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : XDSP_Start Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 180:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : XDSP_Start Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		  return 1;
  }

}

extern "C" DllExport long WINAPI XNWC_SetData(void * objptr,int iDataRef, double dValue )
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
     
     CXDMC *ptrController = (CXDMC*)objptr;
     

	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
	 {  

	 	 //char response[4096]={0};
		 char tmpStr2[100]={0};
		 char *tmpStr1=0;
		 LONG RC ;
		 //char command[100] ={0};

        //_bstr_t bstrCmd(bstrVarName,true);
		//long lLen= (long)SysStringLen(bstrVarName);

		//::MessageBox (0, _T("Switch case"), _T("XDMC DLL : GetMotorBusy"), 
          //            MB_ICONEXCLAMATION | MB_OK);
          //::ShowError(dValue, _T("Controller Not Connected"));
	 
	       memset(ptrController->m_sCommand,0, XCOMMAND_SIZE);
    	   memset(ptrController->m_sResponse,0,XRESPONSE_SIZE);

		 switch(iDataRef)
		  {
	       
		    case 1:  tmpStr1 ="RchpCn";      break;
	        case 2:  tmpStr1 ="IndxLn1";     break;
	        case 3:  tmpStr1 ="IndxCmp";     break;
            case 4:  tmpStr1 ="TchpCn";      break;
	        case 5:  tmpStr1 ="ANMAX";       break;
            case 6:  tmpStr1 ="ANMIN";       break;
            case 7:  tmpStr1 ="IndxMIN";     break;
            case 8:  tmpStr1 ="XREFPOS";     break;
			case 9:	 tmpStr1 ="YREFPOS";     break;
            case 10: tmpStr1 ="IndxCn";      break;
            case 11: tmpStr1 ="ChpCn";       break;
            case 12: tmpStr1 ="XCmOFF";      break;
            case 13: tmpStr1 ="YCmOFF";      break;
            case 14: tmpStr1 ="ZCmOFF";      break;
            case 15: tmpStr1 ="AXCmOFF";     break;
	        case 16: tmpStr1 ="AYCmOFF";     break;
            case 17: tmpStr1 ="Ecode";       break;
            case 18: tmpStr1 ="Curgstp";     break;
            case 19: tmpStr1 ="TindxCn";     break;
            case 20: tmpStr1 ="TcycCn";      break;
            case 21: tmpStr1 ="IndxDur";     break;
			case 22: tmpStr1 ="CTimOut";	 break;
            case 23: tmpStr1 ="GTimOut";     break;
            case 24: tmpStr1 ="CurgOut";     break;
         
	 }	 
		  
		  //::MessageBox (0, _T("after Switch case"), _T("XDMC DLL : GetMotorBusy"), 
            //          MB_ICONEXCLAMATION | MB_OK);

      
		 sprintf(tmpStr2,"=%.3f",dValue);
         sprintf(ptrController->m_sCommand,"%s",tmpStr1);

       //  _bstr_t bstrResponse2 (tmpStr2);
         //   ::MessageBox (0, (wchar_t*) bstrResponse2 , _T("XDMC DLL"), 
           //        MB_ICONEXCLAMATION | MB_OK);
		 strcat(ptrController->m_sCommand,tmpStr2);
         
		 //_bstr_t bstrResponse (command);
           // ::MessageBox (0, (wchar_t*) bstrResponse , _T("XDMC DLL"), 
             //      MB_ICONEXCLAMATION | MB_OK); 
		 
         //ptrController->Clear(); 
		 RC = ptrController->Command(ptrController->m_sCommand,ptrController->m_sResponse,sizeof(ptrController->m_sResponse)); 
         
		 if (RC != DMCNOERROR)
		  {
           char szMessage[128];
		   ptrController->GetErrorText(RC,szMessage,sizeof(szMessage));
		   
		   memset(ptrController->m_sLine,0,300);
			sprintf(ptrController->m_sLine,"Set Data Error:Command %s  - Error : %d  -Error :%s",ptrController->m_sCommand,RC ,szMessage);
            Applog.LogEntry(ptrController->m_sLine);
			
		   class _totchar cl(szMessage);
		   
		   ShowError(RC, cl);
	       return 1;	 
		  }

		 //_bstr_t bstrResponse1 (response);
           // ::MessageBox (0, (wchar_t*) bstrResponse1 , _T("XDMC DLL"), 
             //      MB_ICONEXCLAMATION | MB_OK); 

		if(strchr(ptrController->m_sResponse,'?')) 
	     return 1; //error
       else
         return 0;
	  
	 }	
     else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : XIPT_SetData Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : XIPT_SetData Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : XIPT_SetData Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 220:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : XIPT_SetData Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		return 1;
  }

}

extern "C" DllExport double WINAPI XNWC_GetData(void * objptr, int iDataRef )//, BSTR bstrVarName )
{
 try
 {
   LONG    ldllErr;

   if(NULL == objptr)
   {
     ldllErr =10;
	 throw(ldllErr);
   }
 

   CXDMC *ptrController = (CXDMC*)objptr;
   
   //::MessageBox (0, _T("Before testing connection"), _T("XDMC DLL : GetMotorBusy"), 
    //                  MB_ICONEXCLAMATION | MB_OK);

   if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
   {

        //char response[4096]={0};
        char *command = 0;
        char *ptrValue;
		LONG RC;
        //_bstr_t bstrCmd(bstrVarName,true);
		//long lLen= (long)SysStringLen(bstrVarName);

		//::MessageBox (0, _T("Before testing VarName"), _T("XDMC DLL : GetMotorBusy"), 
        //MB_ICONEXCLAMATION | MB_OK);
	    
		   memset(ptrController->m_sCommand,0, XCOMMAND_SIZE);
    	   memset(ptrController->m_sResponse,0, XRESPONSE_SIZE);

		switch(iDataRef)
		  {
	        case 1: command="ChpCn=?";    break;
	        case 2: command="XCmOFF=?"; break;
	        case 3: command="YCmOFF=?"; break;
	        case 4: command="IndxCn=?";      break;
            case 5: command="PInput=?";      break;
			case 6: command="TI"; break;
			case 7: command="TI2"; break;
			case 8: command="TS"; break;
            case 9: command="CmTrgFg=?"; break;
			case 10: command="Adata=?"; break;
			case 11: command="Rcount[8]=?"; break;
			case 12: command="Rcount[9]=?"; break;
			case 13: command="Rcount[10]=?"; break;
            case 14: command="Rcount[11]=?"; break;
            case 15: command="MG CyTme[1]- CyTme[0]"; break;
	        case 16: command="MG CyTme[2]- CyTme[1]"; break;
	        case 17: command="MG CyTme[3]- CyTme[2]"; break;
            case 18: command="MG CyTme[4]- CyTme[3]"; break;
		    
			case 19: command="Ecode=?"  ;      break;
			case 20: command="Tcode=?"  ;      break;
			case 21: command="Tcode1=?"  ;     break;
			case 22: command="Tcode2=?"  ;     break;
			case 23: command="Tcode3=?"  ;     break;
			case 24: command="Tcode4=?"  ;     break;
			case 25: command="Tcode5=?"  ;     break;
			case 26: command="Tcode6=?"  ;     break;
            
            case 27: command="TindxCn=?";      break;
			case 28: command="TcycCn=?";	   break;
            case 29: command="TI3";            break;
            case 30: command="CYCFg=?";       break;
            case 31: command="Rcode=?";       break;
		    //case 32: command="YCmOFF=?";       break;
		}

		   //_bstr_t bstrComm ( command);
            //::MessageBox (0, (wchar_t*) bstrComm , _T("XDMC DLL"), 
              //     MB_ICONEXCLAMATION | MB_OK); 
		   
		
	       RC = ptrController->Command(command,ptrController->m_sResponse,sizeof(ptrController->m_sResponse)); 
           
		    if (RC != DMCNOERROR)
			{
              char szMessage[128];
		      ptrController->GetErrorText(RC,szMessage,sizeof(szMessage));
		      
			  memset(ptrController->m_sLine,0,300);
			  sprintf(ptrController->m_sLine,"Get Data Flag Error:Command %s  - Error : %d  -Error :%s",command,RC ,szMessage);
              Applog.LogEntry(ptrController->m_sLine);
			
		      class _totchar cl(szMessage);
		   
		     ShowError(RC, cl);
	         return 1;	 
			}

		   //_bstr_t bstrResponse ( response);
            //::MessageBox (0, (wchar_t*) bstrResponse , _T("XDMC DLL"), 
              //     MB_ICONEXCLAMATION | MB_OK); 

		    ptrValue = strchr(ptrController->m_sResponse,':');
		    if ((ptrValue != NULL) && (*ptrValue == ':'))
			{
              *ptrValue = '\0';
		      ++ptrValue;
            
			}
  

	       return atof(ptrController->m_sResponse);

           //::ShowError ((long)iMotionStatus, _T("iMotionStatus"));

	      
   }
   else if (! ptrController->IsConnected()) 
   {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
   }
   else if (! ptrController->m_bEnabled )
   {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

   }
   //return TRUE;
 }

  catch (LONG ErrorCode)
	{
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : GetData Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : GetData Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : GetData Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port

			//case 40
	  }
	  return 1;
	} 

	catch (...)
    {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : GetData Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
          return 1;
    }
}

extern "C" DllExport long WINAPI XNWC_GetAppFlags(void * objptr,int iFlagNo )
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
     
     CXDMC *ptrController = (CXDMC*)objptr;
     

    if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
    {  

  	  //char response[4096]={0};
	  char *command = 0;
	  long resultFlag = 0;
      LONG RC;  
          //_bstr_t bstrCmd(bstrVarName,true);
		//long lLen= (long)SysStringLen(bstrVarName);

		//::MessageBox (0, _T("Before testing VarName"), _T("XDMC DLL : GetMotorBusy"), 
                      //MB_ICONEXCLAMATION | MB_OK);
          memset(ptrController->m_sResponse,0, XRESPONSE_SIZE);
	      switch(iFlagNo)
		  {
	        case 1:  command ="YMBUSY=?";    break;
	        case 2:  command ="YSBUSY=?";    break;
	        case 3:  command ="CHPRDY=?";    break;
	        case 4:  command ="CAMBSY=?";    break;
            case 5:  command ="XYCMPST=?";   break;
			case 6:  command ="MODLIVE=?";   break;
			case 7:  command ="IndxON=?";    break;
	        case 8:  command ="CmTrgFg=?";   break;
            case 9:  command ="TrgZ=?";      break;
            case 10: command ="WATZ=?";      break;
			case 11: command ="XYREF=?";     break;
			case 12: command ="InitRq=?";    break;
			case 13: command ="ZUPRg=?";     break;
			case 14: command ="ZDWRq=?";     break;
			case 15: command ="PLCSRFg=?";   break;
			case 16: command ="ZSECFg=?";    break;
			case 17: command ="ZDWFg=?";     break;
            case 18: command ="InFg=?";      break;
            case 19: command ="SQSRTFg=?";   break;
            case 20: command ="RstFg=?";     break;
            case 21: command ="PLCSPFg=?";   break;
            case 22: command ="McamFg=?";    break;
			case 23: command ="FCamFg=?";	 break;
            case 24: command ="StopFg=?";    break;
            case 25: command ="StopCf=?";    break;
            case 26: command ="StopAsm=?";   break;
            case 27: command ="YRBUSY=?";    break;
            case 28: command ="CurgBSY=?";    break;
            case 29: command ="CstpCm=?";    break;
            case 30: command ="CYCFg=?";     break;
		  }	 

		 ptrController->Clear();
		 //memset(ptrController->m_sResponse,0, 80);
		 RC = ptrController->Command(command,ptrController->m_sResponse,sizeof(ptrController->m_sResponse)); 
         
		  if (RC != DMCNOERROR)
		  {
           char szMessage[128];
		   ptrController->GetErrorText(RC,szMessage,sizeof(szMessage));
		   
		    memset(ptrController->m_sLine,0,300);
			sprintf(ptrController->m_sLine,"Get App Flag Error:Command %s  - Error : %d  -Error :%s",command,RC ,szMessage);
            Applog.LogEntry(ptrController->m_sLine);
			
		   class _totchar cl(szMessage);
			ShowError(RC, cl);
	       return 1;	 
		  }

		if(strchr(ptrController->m_sResponse,'1') ) 
	      resultFlag = 1;
 
		//_bstr_t bstrCommand ( command);
          //  ::MessageBox (0, (wchar_t*) bstrCommand , _T("XDMC DLL"), 
            //       MB_ICONEXCLAMATION | MB_OK); 
               
        // _bstr_t bstrCommand1 ( response);
          //  ::MessageBox (0, (wchar_t*) bstrCommand1 , _T("XDMC DLL"), 
            //       MB_ICONEXCLAMATION | MB_OK); 

        //::ShowError(resultFlag,_T("Flag result"));
        //response[0]='\0';
	 // }
      return resultFlag;
	  
	 }	
     else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : XIPT_GetAppFlags Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : XIPT_GetAppFlags Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : XIPT_GetAppFlags Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 220:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : XIPT_GetAppFlags Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		return 1;
  }

}

extern "C" DllExport long WINAPI XNWC_SetAppFlag(void * objptr,int iFlagNo, long iValue )
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
     
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
	 {  
      
		 //char response[4096]={0};
    	 char *tcom = 0;
		 char temp[20]={0};
         LONG RC;

        //_bstr_t bstrCmd(bstrVarName,true);
		//long lLen= (long)SysStringLen(bstrVarName);

		//::MessageBox (0, _T("Before testing VarName"), _T("XDMC DLL : GetMotorBusy"), 
                      //MB_ICONEXCLAMATION | MB_OK);


	       memset(ptrController->m_sCommand,0, XCOMMAND_SIZE);
    	   memset(ptrController->m_sResponse,0, XRESPONSE_SIZE);

		 switch(iFlagNo)
		  {
	        
		    case 1:  tcom ="PCSRFg";    break;
	        case 2:  tcom ="SNGFg";     break;
			case 3:  tcom ="FcmTgFg";   break;
            case 4:  tcom ="McmTgFg";   break;
            case 5:  tcom ="MnMODFg";   break;
			case 6:  tcom ="McmBy";     break;	
			case 7:  tcom ="ANGBy";     break;
            case 8:  tcom ="ScvBy";      break;
			
			case 9:  tcom ="CurBy";     break;
            case 10: tcom ="FcmBy";     break;
			
			case 11: tcom ="IndxBy";    break;
            
			case 12: tcom ="RCBy";    break;
            case 13: tcom ="GerBy";    break;
			case 14: tcom ="StopFg";    break; 
			case 15: tcom ="InFg";      break;	
			case 16: tcom ="CstpFg";    break;
		    case 17: tcom ="CYCFg";     break;
		    case 18: tcom ="ZRdyFg";     break;
		    case 19: tcom ="CURNOW";     break;
			
            	
		  }	 
		 
		 sprintf(temp," = %d",iValue);
		 sprintf(ptrController->m_sCommand,"%s",tcom);
         strcat(ptrController->m_sCommand,temp);
		 
		 

		 RC = ptrController->Command(ptrController->m_sCommand,ptrController->m_sResponse,sizeof(ptrController->m_sResponse)); 
         
		 //memset(ptrController->m_sLine,0,300);
		 //sprintf(ptrController->m_sLine,"Set App Flag Error:Command %s -t: %s",ptrController->m_sCommand,temp);
         //Applog.LogEntry(ptrController->m_sLine);

		  if (RC != DMCNOERROR)
		  {
           char szMessage[128];
		   ptrController->GetErrorText(RC,szMessage,sizeof(szMessage));
		   
		    memset(ptrController->m_sLine,0,300);
			sprintf(ptrController->m_sLine,"Set App Flag Error:Command %s  - Error : %d  -Error :%s",ptrController->m_sCommand,RC ,szMessage);
            Applog.LogEntry(ptrController->m_sLine);
			
		   class _totchar cl(szMessage);
		   
		   ShowError(RC, cl);
	       return 1;	 
		  }

        //memset(temp,0,20);
		if(strchr(ptrController->m_sResponse,'?')) 
	     return 1; //error
       else
         return 0;
	  
	 }	
     else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : XIPT_SetAppFlag Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : XIPT_SetAppFlag Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : XIPT_SetAppFlag Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 220:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : XIPT_SetAppFlag Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		return 1;
  }

}

extern "C" DllExport long WINAPI XNWC_SetMotion(void * objptr,long lRef,long lAcc, long lDec,long lSpd)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
     {
	   //char command[100] ={0};
       //char response [4096]={0};
       LONG RC;
  
	   memset(ptrController->m_sCommand,0, XCOMMAND_SIZE);
  	   memset(ptrController->m_sResponse,0, XRESPONSE_SIZE);



	     switch(lRef)
		 {
	        /* 
			1- INDEXER SPEED 
             ZIDAC=140000;ZIDDC=140000;ZIDSP=10000
            
			2- X-AXIS REFRENCE POSITION SPEED
             XRPAC=1000000;XRPDC=6000000;XRPSP=400000

             3-Y-AXIS REFRENCE POSITION SPEED
             YRPAC=1000000;YRPDC=6000000;YRPSP=400000
             
             4-'HOME X-AXIS
              XHMAC=20000;XHMDC=3000000;XHMSP=100000

             5-'HOME Y-AXIS
              YHMAC=20000;YHMDC=3000000;YHMSP=100000

             6-'X-Axis Camera Compensation
              XCmOFAC=3000000;XCmOFDC=5560000;XCmOFSP=500000

             7-'Y-Axis Camera Compensation
              YCmOFAC=3000000;YCmOFDC=5560000;YCmOFSP=500000

			 */
		    case 1:  
			  sprintf(ptrController->m_sCommand,"ZIDAC=%d;ZIDDC=%d;ZIDSP=%d",lAcc,lDec,lSpd);
			  break;

	        case 2:  
			  sprintf(ptrController->m_sCommand,"XRPAC=%d;XRPDC=%d;XRPSP=%d",lAcc,lDec,lSpd);
			  break;

	        case 3:  
   	   		  sprintf(ptrController->m_sCommand,"YRPAC=%d;YRPDC=%d;YRPSP=%d",lAcc,lDec,lSpd);
			  break;

            case 4:  
			  sprintf(ptrController->m_sCommand,"XHMAC=%d;XHMDC=%d;XHMSP=%d",lAcc,lDec,lSpd);
			  break;

	        case 5:  
			  sprintf(ptrController->m_sCommand,"YHMAC=%d;YHMDC=%d;YHMSP=%d",lAcc,lDec,lSpd);
			  break;
            
			case 6: 
			  sprintf(ptrController->m_sCommand,"XCmOFAC=%d;XCmOFDC=%d;XCmOFSP=%d",lAcc,lDec,lSpd);
			  break;

            case 7: 
			  sprintf(ptrController->m_sCommand,"YCmOFAC=%d;YCmOFDC=%d;YCmOFSP=%d",lAcc,lDec,lSpd);
			  break;
            
			case 8: 
			  sprintf(ptrController->m_sCommand,"CstpAC=%d;CstpDC=%d;CstpSP=%d",lAcc,lDec,lSpd);
			  break;
  
		  }	 
		  		  
        //_bstr_t bstrCommand ( command);
          //  ::MessageBox (0, (wchar_t*) bstrCommand , _T("XDMC DLL"), 
            //        MB_ICONEXCLAMATION | MB_OK); 
 
       
	   RC = ptrController->Command(ptrController->m_sCommand,ptrController->m_sResponse,sizeof(ptrController->m_sResponse)); 
       
	    if (RC != DMCNOERROR)
		{
           char szMessage[128];
		   ptrController->GetErrorText(RC,szMessage,sizeof(szMessage));
		   
		   memset(ptrController->m_sLine,0,300);
			sprintf(ptrController->m_sLine,"Set Motion Flag Error:Command %s  - Error : %d  -Error :%s",ptrController->m_sCommand,RC ,szMessage);
            Applog.LogEntry(ptrController->m_sLine);
			
		   class _totchar cl(szMessage);
		   
		   ShowError(RC, cl);
	       return 1;	 
		}

	   if(strchr(ptrController->m_sResponse,'?')) 
	     return 1; //error
       else
         return 0;
	 }
	 else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }
  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : XIPT_SetMotion Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : XIPT_SetMotion Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : XIPT_SetMotion Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 160:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : XIPT_SetMotion Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		  return 1;
  }

}

extern "C" DllExport double WINAPI XNWC_TroubleShoot (void * objptr,long lDataRef)
{
  try
  {
     LONG    ldllErr;

	if ( NULL == objptr)
	{
	   ldllErr = 10;
	   throw (ldllErr);	 
	}
		
	 if (objptr != NULL)
	{
	  CXDMC * ptrController = (CXDMC*)objptr;

	  if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE)) //&& (SysStringLen(bstrCommand) != NULL))
	  {
          
		  
        //char response[4096]={0};
        char *command = 0;
        char *ptrValue;
		LONG RC ;
        //_bstr_t bstrCmd(bstrVarName,true);
		//long lLen= (long)SysStringLen(bstrVarName);

		//::MessageBox (0, _T("Before testing VarName"), _T("XDMC DLL : GetMotorBusy"), 
                      //MB_ICONEXCLAMATION | MB_OK);
              memset(ptrController->m_sCommand,0, XCOMMAND_SIZE);
	     memset(ptrController->m_sResponse,0, XRESPONSE_SIZE);

	   
		switch(lDataRef)
		  {
	        case 1:  command="TC";        break;
	        case 2:  command="SCA";       break;
	        case 3:  command="SCB";       break;
	        case 4:  command="SCC";       break;
	        case 5:  command="SCD";       break;
	        //case 6:  command="SCE";       break;
	        //case 7:  command="SCF";       break;
	        //case 8:  command="SCG";       break;
            case 6:  command="TSA";       break;
            case 7:  command="TSB";       break;
	        case 8:  command="TSC";       break;
	        case 9:  command="TSD";       break;
	        //case 13:  command="TSE";       break;
	        //case 14:  command="TSF";       break;
	        //case 15:  command="TSG";       break;				
		    case 10:  command="MG _XQ0";   break;
			case 11:  command="MG _XQ1";   break;	
            case 12: command="MG _XQ2";  break;
		    case 13: command="MG _XQ3";  break;
		    case 14: command="MG _XQ4";  break;
			case 15: command="MG _XQ5";  break;
			case 16: command="MG _XQ6";  break;
			case 17: command="MG _XQ7";  break;
            case 18: command="MG @AN[1]";break; 

		  }

		   //_bstr_t bstrComm ( command);
            //::MessageBox (0, (wchar_t*) bstrComm , _T("XDMC DLL"), 
              //     MB_ICONEXCLAMATION | MB_OK); 
		      
		   //ptrController->Clear() ;
	       RC = ptrController->Command(command,ptrController->m_sResponse,sizeof(ptrController->m_sResponse)); 
           
           memset(ptrController->m_sLine,0,300);
		   sprintf(ptrController->m_sLine,"troubleshoot Error:Command %s - R:%s",command,ptrController->m_sResponse);
           Applog.LogEntry(ptrController->m_sLine);
		   
		   if (RC != DMCNOERROR)
			{
             char szMessage[128];
		     ptrController->GetErrorText(RC,szMessage,sizeof(szMessage));
		     
			 memset(ptrController->m_sLine,0,300);
			 sprintf(ptrController->m_sLine,"Troubleshoot Error:Command %s  - Error : %d  -Error :%s",command,RC ,szMessage);
             Applog.LogEntry(ptrController->m_sLine);
			
		     class _totchar cl(szMessage);
		   
		     ShowError(RC, cl);
	         
			 return 0;	 
			}

		  //char *ptrResponse = response;

		    ptrValue = strchr(ptrController->m_sResponse,':');
		    if ((ptrValue != NULL) && (*ptrValue == ':'))
			{
              *ptrValue = '\0';
		      ++ptrValue;
            
			//_bstr_t bstrResponse1 ( response);
            //::MessageBox (0, (wchar_t*) bstrResponse1 , _T("XDMC DLL"), 
              //       MB_ICONEXCLAMATION | MB_OK); 
            
			}
		    else if (ptrValue = strchr(ptrController->m_sResponse,'?'))
			{
             
              ldllErr = 30;
	          throw (ldllErr);	
			
			 //  _bstr_t bstrResponse ( ptrValue);
            //::MessageBox (0, (wchar_t*) bstrResponse , _T("XDMC DLL Exception"), 
            //         MB_ICONEXCLAMATION | MB_OK); 
			}
		  /*
		  char * pch;       
          pch=strchr(response,':');
		  char *temp = response[pch];
          */

		   //_bstr_t bstrResponse ( response);
		   //ptrController->m_CmdResponse = bstrResponse;
          
		  //TCHAR tszMessage[256];
	      //wsprintf(tszMessage,_T("%s)"), bstrResponse);
          
		  //::MessageBox (0, (wchar_t*) ptrController->m_CmdResponse , _T("XDMC DLL Exception"), 
            //         MB_ICONEXCLAMATION | MB_OK); 

		   //::ShowError ((LONG)objptr, _T("This is the pointer from VB"));
		   
		   /*
		   LONG hand = ptrController->GetHandle();
		 ::ShowError (hand, _T("This is the galil handle"));
      
	      //hand =ptrController->GetKeepHandle();
	      //::ShowError(hand ,_T("This XDMC.dll handle"));
	        ::ShowError((LONG) objptr,_T("This object pointer fron VB"));
           */
		   
		   
		   return atof(ptrController->m_sResponse) ; //ptrController->m_CmdResponse;
	/*	 }
		 else
		 {

		   ptrController->FastCommand (ptrCommand);

		   ptrController->m_CmdResponse = _T("0");
		   return 0;//ptrController->m_CmdResponse ;
		 }
    */
	  }
	  else if (! ptrController->IsConnected()) 
      {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
	  }
	  else if (! ptrController->m_bEnabled )
	  {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

	  }
	  
	}
	else
	{
      //::ShowError (0,_T("Empty pointer"));

	  //ptrController->m_CmdResponse = _T("0");
	 // _bstr_t bstrResponse (_T("E ")) ;
     
		ldllErr = 31;
	    throw (ldllErr);	
	}
  }

  catch (LONG ErrorCode)
	{
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : XNWC_TroubleShoot Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : XNWC_TroubleShoot Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : XNWC_TroubleShoot Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port

			//case 40
	  }
	  return 1;
	} 

	catch (...)
    {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : XNWC_TroubleShoot Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
          return 1;
    }
}

extern "C" DllExport long WINAPI XNWC_SetArray(void * objptr,double *buffer, long lLength,long lDataRef)
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
 
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
     {
	    char part1[100]={0};
        char part2[100]={0};
        //char part3[100]={0};
		//char Resp[255]={0};
        char *command = 0;
        //char *ptrValue;
		LONG RC ;

		memset(ptrController->m_sCommand,0, XCOMMAND_SIZE);
	   memset(ptrController->m_sResponse,0,XRESPONSE_SIZE);

	   switch(lDataRef)
	   {
	        case 0: command="IndxPos["; break;
	        case 1: command="YPthPos["; break;
	   }
       

	   for (int i=0; i <= lLength ;i++)
	   {
           sprintf(part1,"%d]=%.3f",i,buffer[i]);
		   if ( (i+1) <= lLength) 
		     sprintf(part2,"%s%s;",command,part1); 
		   else
             sprintf(part2,"%s%s",command,part1); 
		   
		   strcat(ptrController->m_sCommand,part2);
	   }
		
	    

		 // _bstr_t bstrComm (ptrController->m_sCommand);
           // ::MessageBox (0, (wchar_t*) bstrComm , _T("XDMC DLL"),MB_ICONEXCLAMATION | MB_OK); 
		   
		     
		   //ptrController->Clear() ;
	       RC = ptrController->Command(ptrController->m_sCommand,ptrController->m_sResponse,sizeof(ptrController->m_sResponse)); 
           
           //memset(ptrController->m_sLine,0,300);
		   //sprintf(ptrController->m_sLine,"Set Array Error:Command %s - R:%s",command,ptrController->m_sResponse);
           //Applog.LogEntry(ptrController->m_sLine);
		   
		   if (RC != DMCNOERROR)
			{
             char szMessage[128];
		     ptrController->GetErrorText(RC,szMessage,sizeof(szMessage));
		     
			 memset(ptrController->m_sLine,0,300);
			 sprintf(ptrController->m_sLine,"Aet Array Error:Command %s  - Error : %d  -Error :%s",command,RC ,szMessage);
             Applog.LogEntry(ptrController->m_sLine);
			
		     class _totchar cl(szMessage);
		   
		     ShowError(RC, cl);
	         
			 return 0;	 
			}
		  


        //_bstr_t bstrCommand ( command);
        //    ::MessageBox (0, (wchar_t*) bstrCommand , _T("XDMC DLL"), 
         //          MB_ICONEXCLAMATION | MB_OK); 

     	if(strchr(ptrController->m_sResponse,'?')) 
	     return 1; //error
       else
         return 0;
	 }
	 else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : MotorSetStartPosArray Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : MotorSetStartPosArray Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : MotorSetStartPosArray Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 130:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : MotorSetStartPosArray Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		  return 1;
  }

}


extern "C" DllExport long   WINAPI XNWC_GetArray (void * objptr,char *buffer,short start,short length,long lDataRef)
{
  try
  {
     LONG    ldllErr;

	if ( NULL == objptr)
	{
	   ldllErr = 10;
	   throw (ldllErr);	 
	}
		
	 if (objptr != NULL)
	{
	  CXDMC * ptrController = (CXDMC*)objptr;

	  if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE)) //&& (SysStringLen(bstrCommand) != NULL))
	  {
          
		  
        char part1[200]={0};
        char part2[200]={0};
        char part3[255]={0};
		char Resp[255]={0};
        char *command = 0;
        char *ptrValue=0;
		LONG RC ;
        //_bstr_t bstrCmd(bstrVarName,true);
		//long lLen= (long)SysStringLen(bstrVarName);

		//::MessageBox (0, _T("Before testing VarName"), _T("XDMC DLL : GetMotorBusy"), 
                      //MB_ICONEXCLAMATION | MB_OK);

		memset(ptrController->m_sCommand,0, XCOMMAND_SIZE);
    	 memset(ptrController->m_sResponse,0, XRESPONSE_SIZE);

		switch(lDataRef)
		  {
	        case 1: command="CyTme[";      break;
	        case 2: command="S1Tme[ ";     break;
	        case 3: command="S2Tme[";      break;
			case 4: command="SC";          break;
			case 5: command="TS";         break;
            case 6: command="TP";         break;

		  }
          	 
		 ptrValue = strchr(command,'[');
         if ((ptrValue != NULL) && (*ptrValue == '['))
		 {
		  for (int i=  start; i < (length + start) ;i++)
		  {
           sprintf(part1,"%d]",i);
		   if ( (i+1) < (length + start)) 
		     sprintf(part2,"%s%s,",command,part1); 
		   else
             sprintf(part2,"%s%s",command,part1); 
		   
		   strcat(part3,part2);
		  }
		  
		  
		  sprintf(ptrController->m_sCommand,"MG %s",part3);
		 }
		 else
		 {
          sprintf(ptrController->m_sCommand,"%s",command);
		 }

		  //_bstr_t bstrComm (ptrController->m_sCommand);
            //::MessageBox (0, (wchar_t*) bstrComm , _T("XDMC DLL"), 
              //     MB_ICONEXCLAMATION | MB_OK); 
		   
		     
		   //ptrController->Clear() ;
	       RC = ptrController->Command(ptrController->m_sCommand,ptrController->m_sResponse,sizeof(ptrController->m_sResponse)); 
           
           //memset(ptrController->m_sLine,0,300);
		   //sprintf(ptrController->m_sLine,"Get Array Error:Command %s - R:%s",command,ptrController->m_sResponse);
           //Applog.LogEntry(ptrController->m_sLine);
		   
		   if (RC != DMCNOERROR)
			{
             char szMessage[128];
		     ptrController->GetErrorText(RC,szMessage,sizeof(szMessage));
		     
			 memset(ptrController->m_sLine,0,300);
			 sprintf(ptrController->m_sLine,"Get Array Error:Command %s  - Error : %d  -Error :%s",command,RC ,szMessage);
             Applog.LogEntry(ptrController->m_sLine);
			
		     class _totchar cl(szMessage);
		   
		     ShowError(RC, cl);
	         
			 return 0;	 
			}

		  //char *ptrResponse = response;

		    ptrValue = strchr(ptrController->m_sResponse,':');
		    if ((ptrValue != NULL) && (*ptrValue == ':'))
			{
              *ptrValue = '\0';
		      ++ptrValue;
             
			 sprintf(buffer,"%s",ptrController->m_sResponse );
		     //buffer = Resp;
		     //_bstr_t bstrResponse1 ( buffer);
            //::MessageBox (0, (wchar_t*) bstrResponse1 , _T("XDMC DLL Exception"), 
              //       MB_ICONEXCLAMATION | MB_OK); 
			//_bstr_t bstrResponse1 (ptrController->m_sResponse );
            //::MessageBox (0, (wchar_t*) bstrResponse1 , _T("XDMC DLL"), 
               //      MB_ICONEXCLAMATION | MB_OK); 
            
			}
		    else if (ptrValue = strchr(ptrController->m_sResponse,'?'))
			{
             
              ldllErr = 30;
	          throw (ldllErr);	
			
			 //  _bstr_t bstrResponse ( ptrValue);
            //::MessageBox (0, (wchar_t*) bstrResponse , _T("XDMC DLL Exception"), 
            //         MB_ICONEXCLAMATION | MB_OK); 
			}
		  /*
		  char * pch;       
          pch=strchr(response,':');
		  char *temp = response[pch];
          */

		   //_bstr_t bstrResponse ( response);
		   //ptrController->m_CmdResponse = bstrResponse;
          
		  //TCHAR tszMessage[256];
	      //wsprintf(tszMessage,_T("%s)"), bstrResponse);
          
		  //::MessageBox (0, (wchar_t*) ptrController->m_CmdResponse , _T("XDMC DLL Exception"), 
            //         MB_ICONEXCLAMATION | MB_OK); 

		   //::ShowError ((LONG)objptr, _T("This is the pointer from VB"));
		   
		   /*
		   LONG hand = ptrController->GetHandle();
		 ::ShowError (hand, _T("This is the galil handle"));
      
	      //hand =ptrController->GetKeepHandle();
	      //::ShowError(hand ,_T("This XDMC.dll handle"));
	        ::ShowError((LONG) objptr,_T("This object pointer fron VB"));
           */
		   
		   
		   return 0 ; //ptrController->m_CmdResponse;
	/*	 }
		 else
		 {

		   ptrController->FastCommand (ptrCommand);

		   ptrController->m_CmdResponse = _T("0");
		   return 0;//ptrController->m_CmdResponse ;
		 }
    */
	  }
	  else if (! ptrController->IsConnected()) 
      {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
	  }
	  else if (! ptrController->m_bEnabled )
	  {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

	  }
	  
	}
	else
	{
      //::ShowError (0,_T("Empty pointer"));

	  //ptrController->m_CmdResponse = _T("0");
	 // _bstr_t bstrResponse (_T("E ")) ;
     
		ldllErr = 31;
	    throw (ldllErr);	
	}
  }

  catch (LONG ErrorCode)
	{
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : XNWC_GetArray Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : XNWC_GetArray Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XXDMC DLL : XNWC_GetArray Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port

			//case 40
	  }
	  return 1;
	} 

	catch (...)
    {
        ::MessageBox (0, _T("Unknown"), _T("XNWC DLL : GetArray Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
          return 1;
    }
}

extern "C" DllExport long WINAPI XNWC_SetOutput(void * objptr,long lModuleNo, long lONOFF )
{
  try
  {
    LONG    ldllErr;

    if(NULL == objptr)
    {
     ldllErr =10;
	 throw(ldllErr);
    }
     
     CXDMC *ptrController = (CXDMC*)objptr;
     
	 
	 if((ptrController->IsConnected()) && (ptrController->m_bEnabled == TRUE))
	 {  
      
		 //char response[4096]={0};
    	 char *tcom = 0;
		 char temp[20]={0};
         LONG RC;

        //_bstr_t bstrCmd(bstrVarName,true);
		//long lLen= (long)SysStringLen(bstrVarName);

		//::MessageBox (0, _T("Before testing VarName"), _T("XDMC DLL : GetMotorBusy"), 
                      //MB_ICONEXCLAMATION | MB_OK);

	      
		  if (lONOFF == 1)
				tcom="SB";
           else
                tcom="CB";
			

		 sprintf(temp,"%d",lModuleNo);
		 sprintf(ptrController->m_sCommand,"%s",tcom);
         strcat(ptrController->m_sCommand,temp);
		 
		 //ptrController->Clear();
		 memset(ptrController->m_sResponse,0, XRESPONSE_SIZE);
		 RC = ptrController->Command(ptrController->m_sCommand,ptrController->m_sResponse,sizeof(ptrController->m_sResponse)); 
         
		 //memset(ptrController->m_sLine,0,300);
		 //sprintf(ptrController->m_sLine,"Set App Flag Error:Command %s -t: %s",ptrController->m_sCommand,temp);
         //Applog.LogEntry(ptrController->m_sLine);

		  if (RC != DMCNOERROR)
		  {
           char szMessage[128];
		   ptrController->GetErrorText(RC,szMessage,sizeof(szMessage));
		   
		    memset(ptrController->m_sLine,0,300);
			sprintf(ptrController->m_sLine,"Set Motion Error:Command %s  - Error : %d  -Error :%s",ptrController->m_sCommand,RC ,szMessage);
            Applog.LogEntry(ptrController->m_sLine);
			
		   class _totchar cl(szMessage);
		   
		   ShowError(RC, cl);
	       return 1;	 
		  }

        //memset(temp,0,20);
		if(strchr(ptrController->m_sResponse,'?')) 
	     return 1; //error
       else
         return 0;
	  
	 }	
     else if (! ptrController->IsConnected()) 
     {
        //::ShowError(0, _T("Controller Not Connected"));

        //ptrController->m_CmdResponse = _T("Handled Exception");
		//return ptrController->m_CmdResponse ;
		  
		ldllErr = 32;
	    throw (ldllErr);
     }
     else if (! ptrController->m_bEnabled )
     {
		  //::ShowError (0, _T("Controller Not Enabled"));

		  //ptrController->m_CmdResponse = _T("Handled Exception");
		  //return ptrController->m_CmdResponse ;

        ldllErr = 33;
	    throw (ldllErr);

     }

  }

  catch (LONG ErrorCode)
  {
      switch(ErrorCode)
	  {
        case 10: ::MessageBox (0, _T("Galil Card Not Initialized (Open)"), _T("XDMC DLL : XNWC_SetAppFlag Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
	    case 32: ::MessageBox (0, _T("Controller Not Connected"), _T("XDMC DLL : XNWC_SetAppFlag Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        case 33: ::MessageBox (0, _T("Controller Not Enabled"), _T("XDMC DLL : XNWC_SetAppFlag Exception"),MB_ICONEXCLAMATION | MB_OK); break;//Unable to open COM-port
        // case 220:
	  }
	  return 1;
  } 

  catch (...)
  {
        ::MessageBox (0, _T("Unknown"), _T("XDMC DLL : XNWC_SetAppFlag Exception"), 
                      MB_ICONEXCLAMATION | MB_OK);
		return 1;
  }

}

