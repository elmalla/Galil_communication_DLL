

#include <windows.h>
#include "CXDMC.h"

CXDMC::CXDMC()
{
	m_bEnabled=FALSE;
	m_usController = 0;
	m_hWnd=0;
	m_bConnected = FALSE;
	m_hDmc = -1;
	m_lLastError = 0;
}


CXDMC::~CXDMC()
{
	if (m_hDmc != -1)
		Close();
}


LONG CXDMC::Open()
{
	m_lLastError = 0;
	
	if (m_bConnected)
		return m_lLastError;
	
	if (m_usController > 0 && m_usController < 17)
	{
		m_lLastError = DMCOpen(m_usController, m_hWnd, &m_hDmc);

	    if (m_lLastError == 0)
			m_bConnected = TRUE;
		else
			m_hDmc = -1;
	}
	else
		m_lLastError = DMCERROR_CONTROLLER; 
		
	return m_lLastError;		
}

LONG CXDMC::Open(USHORT usController)
{
	m_usController = usController;
	m_hWnd = NULL;

	return Open();
}

HANDLEDMC CXDMC::GetHandle()
{
	if (m_hDmc != -1)
		return m_hDmc;
		
	if (m_usController > 0 && m_usController < 17)
	{	
		m_lLastError = DMCGetHandle(m_usController, &m_hDmc);		

		if (m_lLastError == 0)
			return m_hDmc;
	}		
	
	return -1;			
}

LONG CXDMC::Close()
{
	m_lLastError = 0;

	if (m_hDmc != -1)	
	{
		m_lLastError = DMCClose(m_hDmc);
	}

	m_hDmc = -1;
	m_bConnected = FALSE;

	return m_lLastError;
}

LONG CXDMC::Command(const PSZ pszCommand, PCHAR pchResponse, ULONG cbResponse)
{
	if (m_bConnected)
	{
		m_lLastError = DMCCommand(m_hDmc, (PSZ)pszCommand, pchResponse, cbResponse);
	}
	else
		m_lLastError = DMCERROR_HANDLE;		
	
	return m_lLastError;
}

LONG CXDMC::DiagnosticsOn(const PSZ pszFileName, BOOL bAppend)
{
	m_lLastError = DMCDiagnosticsOn(m_hDmc, (PSZ)pszFileName, bAppend);
	
	return m_lLastError;
}

LONG CXDMC::DiagnosticsOff()
{
	m_lLastError = DMCDiagnosticsOff(m_hDmc);
	
	return m_lLastError;
}



LONG CXDMC::FastCommand(const PSZ pszCommand)
{
	if (m_bConnected)
	{
		m_lLastError = DMCFastCommand(m_hDmc, (PSZ)pszCommand);
	}
	else
		m_lLastError = DMCERROR_HANDLE;		
	
	return m_lLastError;
}

LONG CXDMC::BinaryCommand(PBYTE pbCommand, ULONG ulCommandLength, PCHAR pchResponse, ULONG cbResponse)
{
	if (m_bConnected)
	{
		m_lLastError = DMCBinaryCommand(m_hDmc, pbCommand, ulCommandLength, pchResponse, cbResponse);
	}
	else
		m_lLastError = DMCERROR_HANDLE;		
	
	return m_lLastError;
}

LONG CXDMC::GetUnsolicitedResponse(PCHAR pchResponse, ULONG cbResponse)
{
	if (m_bConnected)
	{
		m_lLastError = DMCGetUnsolicitedResponse(m_hDmc, pchResponse, cbResponse);
	}
	else
		m_lLastError = DMCERROR_HANDLE;		
	
	return m_lLastError;
}

LONG CXDMC::GetAdditionalResponseLen()
{
	LONG	lLength;

	if (m_bConnected)
	{
		m_lLastError = DMCGetAdditionalResponseLen(m_hDmc, (PULONG)&lLength);
	}
	else
		m_lLastError = DMCERROR_HANDLE;		
	
	if (m_lLastError != 0)
		lLength = 0;
		
	return lLength;		
}

LONG CXDMC::GetAdditionalResponse(PCHAR pchResponse, ULONG cbResponse)
{
	if (m_bConnected)
	{
		m_lLastError = DMCGetAdditionalResponse(m_hDmc, pchResponse, cbResponse);
	}
	else
		m_lLastError = DMCERROR_HANDLE;		
	
	return m_lLastError;
}

LONG CXDMC::GetErrorText(LONG lError, PCHAR pchMessage, ULONG cbMessage)
{
	m_lLastError = DMCError(m_hDmc, lError, pchMessage, cbMessage);
	
	return m_lLastError;
}

LONG CXDMC::Clear()
{
	if (m_bConnected)
	{
		m_lLastError = DMCClear(m_hDmc);
	}

	return m_lLastError;
}

LONG CXDMC::Reset()
{
	if (m_bConnected)
	{
		m_lLastError = DMCReset(m_hDmc);
	}

	return m_lLastError;
}

LONG CXDMC::DownloadFile(const PSZ pszFileName, const PSZ pszLabel)
{
	if (m_bConnected)
	{
		m_lLastError = DMCDownloadFile(m_hDmc, (PSZ)pszFileName, (PSZ)pszLabel);
	}
	
	return m_lLastError;
}

LONG CXDMC::DownloadFromBuffer(const PSZ pszBuffer, const PSZ pszLabel)
{
	if (m_bConnected)
	{
		m_lLastError = DMCDownloadFromBuffer(m_hDmc, (PSZ)pszBuffer, (PSZ)pszLabel);
	}
	
	return m_lLastError;
}

LONG CXDMC::RefreshDataRecord(ULONG ulLength)
{
	if (m_bConnected)
	{
		m_lLastError = DMCRefreshDataRecord(m_hDmc, ulLength);
	}
	
	return m_lLastError;
}

LONG CXDMC::GetDataRecord(USHORT usGeneralOffset, USHORT usAxisInfoOffset, PUSHORT pusDataType, PLONG plData)
{
	if (pusDataType)
		*pusDataType = 0;

	if (m_bConnected)
	{
		m_lLastError = DMCGetDataRecord(m_hDmc, usGeneralOffset, usAxisInfoOffset, pusDataType, plData);
	}
	
	return m_lLastError;
}

LONG CXDMC::GetDelay()
{
	LONG	lDelay;

	m_lLastError = DMCGetDelay(m_hDmc, &lDelay);
	
	if (m_lLastError != 0)
		lDelay = 0;
		
	return lDelay;		
}

void CXDMC::SetDelay(LONG lDelay)
{
	m_lLastError = DMCSetDelay(m_hDmc, lDelay);	
}

LONG CXDMC::GetGalilRegistryInfo(USHORT usController, PGALILREGISTRY pgalilregistry)
{

	m_lLastError = DMCGetGalilRegistryInfo(usController, pgalilregistry);

	return m_lLastError;
}

