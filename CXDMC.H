// CXDMC Class Library


#include <stdio.h>
#include <tchar.h>
#include <comdef.h>
#include "Dmccom.h"

class CXDMC 
{
public:
	// Contructors

	// Default constructor
	CXDMC();

    ~CXDMC ();
	void SetController(USHORT usController) {m_usController = usController;};
	// Set the window handle to be used by Open
	// Can be used to change the window handle dynamically
	LONG Open();
	
	LONG Open(USHORT usController);
	// Is there an active connection?
	//   returns TRUE or FALSE
	BOOL IsConnected() {return m_bConnected;};
	// Get the value of the controller handle
	//   returns the value of the controller handle 
	HANDLEDMC GetHandle();
	// Close a connection with the controller
	//   returns error code
	LONG Close();
	// Send an ASCII command to the controller
	//   returns error code
	LONG Command(const PSZ pszCommand, PCHAR pchResponse = NULL, ULONG cbResponse = 0);
	// Send an ASCII command to the controller without waiting for a response
	//   returns error code
	// Diagnostics functions for all controllers

    LONG DiagnosticsOn(const PSZ pszFileName, BOOL bAppend);
    // Turn on diagnostics.

    // hdmc              Handle to the Galil controller.
    // pszFileName       File name for the diagnostic file.
    // fAppend           TRUE if the file will open for append, otherwise FALSE.

    LONG DiagnosticsOff();
    // Turn off diagnostics.

    // hdmc              Handle to the Galil controller.

    // Configuration functions for all controllers

	
	LONG FastCommand(const PSZ pszCommand);
	// Send a binary command to the controller
	//   returns error code
	LONG BinaryCommand(PBYTE pbCommand, ULONG ulCommandLength, PCHAR pchResponse, ULONG cbResponse);
	// Get the unsolicited responses from the controller
	//   returns error code
	LONG GetUnsolicitedResponse(PCHAR pchResponse, ULONG cbResponse);
	// Get the additional response length
	//   returns the value of the additional response length 
	//   returns error code
	LONG GetAdditionalResponseLen();
	// Get the additional response
	//   returns error code
	LONG GetAdditionalResponse(PCHAR pchResponse, ULONG cbResponse);
	// Clear the controller (FIFO)
	//   returns error code
	LONG Clear();
	// Reset the controller
	//   returns error code
	LONG Reset();
	// Master reset the controller
	//   returns error code
	LONG DownloadFile(const PSZ pszFileName, const PSZ pszLabel = NULL);
	// Download a DMC program to the controller from a buffer
	//   returns error code
	LONG DownloadFromBuffer(const PSZ pszBuffer, const PSZ pszLabel = NULL);
	// Upload a DMC program from the controller to a file
	//   returns error code
	LONG SendFile(const PSZ pszFileName);
	// Send a file to the controller - reads and sends binary commands to the controller
	// line-by-line
	//   returns error code
	LONG RefreshDataRecord(ULONG ulLength = 0);
	// Get data record item using an offset for data record access
	//   returns error code
	LONG GetDataRecord(USHORT usGeneralOffset, USHORT usAxisInfoOffset, PUSHORT pusDataType, PLONG plData);
	// Get data record item using an item Id for data record access
	//   returns error code
	LONG GetDataRecordByItemId(USHORT usItemId, USHORT usAxisId, PUSHORT pusDataType, PLONG plData);
	// Get the size of the data record for data record access
	//   returns error code
	LONG GetTimeout();
	// Set the timeout for the controller
	//   returns void
	void SetTimeout(LONG lTimeout);
	// Get the current delay value for the controller
	//   returns the value for delay
	LONG GetDelay();
	// Set the delay for the controller
	//   returns void
	void SetDelay(LONG lDelay);
	// Wait for motion complete
	//   returns error code
	LONG WaitForMotionComplete(const PSZ pszAxes, SHORT fDispatchMsgs = TRUE);
	// Get the error text for an error code
	//   returns error code
	LONG GetErrorText(LONG lError, PCHAR pchMessage, ULONG cbMessage);
	
	LONG GetLastError() {return m_lLastError;};
	

    //Registry Info
	LONG GetGalilRegistryInfo(USHORT usController, PGALILREGISTRY pgalilregistry);

	// Attributes
public:
	bool      m_bEnabled;
	char      m_sResponse[255];
    char      m_sCommand[255];
	char      m_sLine[300];
   _bstr_t    m_CmdResponse;


protected:
	USHORT		    m_usController;	// Controller number
	HANDLEDMC	    m_hDmc;				// Handle to the controller
	BOOL			m_bConnected;		// Is there an active connection?
	LONG			m_lLastError;		// Last return code from a DMCWIN library function	
    HWND                m_hWnd;
};