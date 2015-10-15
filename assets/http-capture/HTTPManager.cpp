#include "stdafx.h"
#include "HttpManager.h"
#include "ErrorHandler.h"
#include "ScopedHandle.h"
#include <boost/format.hpp>

using namespace FTPC;
using namespace FTPC::ErrorHandler;

// ----------------------------------------------------------------------------------------
// CHttpManager Ŭ���� 
// ----------------------------------------------------------------------------------------
CHttpManager::CHttpManager(LPCWSTR szServerAddr, 
	DWORD dwPort, 
	CONST map<wstring, wstring>& mFiles, 
	CONST INT nRetryCount, 
	FTPC_CALLBACK_PROGRESS fpProgress, 
	LPVOID lpThis 
	)
	: CFileTransfer(mFiles, nRetryCount),
	m_strServerAddr(szServerAddr),
	m_dwServerPort(dwPort), 	
	m_fpProgress(fpProgress), 
	m_lpThis(lpThis) 
{
}

CHttpManager::~CHttpManager()
{
	Release();
}

DWORD CHttpManager::Init()
{
	DWORD errorCode = NO_ERROR;
	m_session = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL);
	if (NULL == m_session) {
		errorCode = GetLastError();
	}
	ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"InternetOpen", errorCode);

	DWORD rec_timeout = 60 * 1000 * 2;	// 120��
	InternetSetOption(m_session, INTERNET_OPTION_RECEIVE_TIMEOUT, &rec_timeout, sizeof(rec_timeout));

	return errorCode;
}

DWORD CHttpManager::Connect()
{
	DWORD errorCode = NO_ERROR;
	errorCode = Init();
	if (errorCode != NO_ERROR)
		return errorCode;

	m_http = InternetConnect(m_session, 
		m_strServerAddr.c_str(), 
		static_cast<INTERNET_PORT>(m_dwServerPort), 
		_T(""), 
		_T(""), 
		INTERNET_SERVICE_HTTP, 
		0, 
		NULL);
	if (NULL == m_http){
		errorCode = GetLastError();
	}

	std::wstring addr = (boost::wformat(L"%s:%d") % m_strServerAddr % m_dwServerPort).str();
	ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"ServerAddress", addr.c_str());
	ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"InternetConnect", errorCode);
	return errorCode;
}

VOID CHttpManager::Disconnect()
{
	Release();
	ErrorHandler::ErrorHandler(_T(__FUNCTION__), (DWORD)NO_ERROR);
}

VOID CHttpManager::Release()
{
	if (NULL != m_http)
		InternetCloseHandle(m_http);
	if (NULL != m_session)
		InternetCloseHandle(m_session);

	m_http = NULL;
	m_session = NULL;
}

DWORD CHttpManager::GetRemoteFileSize(LPCTSTR lpszRemoteFilePath, INT64& nRemoteFileSize)
{
	DWORD errorCode = NO_ERROR;
	nRemoteFileSize = 0;

	LPCTSTR lpszAcceptTypes[2]; ZeroMemory(lpszAcceptTypes, sizeof(lpszAcceptTypes)); lpszAcceptTypes[0] = _T("*/*");
	InternetScopedHandle hRemoteFile(HttpOpenRequest(m_http, _T("GET"), lpszRemoteFilePath, NULL, NULL, lpszAcceptTypes,
		INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_KEEP_CONNECTION, NULL));
	if (NULL == hRemoteFile){
		errorCode = GetLastError();
		ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"HttpOpenRequest", errorCode);
		return errorCode;
	}
	if (!HttpSendRequest(hRemoteFile, NULL, 0, NULL, 0)) {
		errorCode = GetLastError();
		ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"HttpSendRequest", errorCode);
		return errorCode;
	}

	// -------------------------------------------
	// �����ڵ� ��� (������ ������������ �˻�), 
	// ������ �������� �ʴ� ��� CONTENT LENGTH�� ������ �� (���� ���ڿ�)���� ��ü �Ǳ� �����̴�.
	// -------------------------------------------
	BYTE info[32]; ZeroMemory(info, sizeof(info));
	DWORD dwInfoSize = sizeof(info);
	if (HttpQueryInfo(hRemoteFile, HTTP_QUERY_STATUS_CODE, info, &dwInfoSize, NULL)){
		INT64 statusCode = static_cast<INT64>(_ttol((PWCHAR)info));
		if (200 != statusCode){
			errorCode = ERROR_FILE_NOT_FOUND;
			ErrorHandler::ErrorHandlerFormat(_T("[%s - %s] StatusCode : %d"), _T(__FUNCTION__), L"HttpQueryStatusCode", statusCode);
			return errorCode;
		}
	}

	// -------------------------------------------
	// ������(http)�� ���� ������ ���
	// -------------------------------------------
	ZeroMemory(info, sizeof(info));
	dwInfoSize = sizeof(info);
	if (HttpQueryInfo(hRemoteFile, HTTP_QUERY_CONTENT_LENGTH, info, &dwInfoSize, NULL)){
		nRemoteFileSize = static_cast<DWORD>(_ttol((PWCHAR)info));
		if (0 == nRemoteFileSize){
			errorCode = ERROR_FILE_NOT_FOUND;
			ErrorHandler::ErrorHandlerFormat(_T("[%s - %s] ContentLength : %d"), _T(__FUNCTION__), L"HttpQueryContentLength", nRemoteFileSize);
		}
	}

	return NO_ERROR;
}

DWORD CHttpManager::DownloadFile(LPCTSTR lpszRemoteFilePath, LPCTSTR lpszLocalFilePath)
{
	DWORD errorCode = NO_ERROR;
	ErrorHandler::ErrorHandlerFormat(_T("[%s - ���ϼ��Ž���] %s"), _T(__FUNCTION__), lpszRemoteFilePath);

	// ---------------------------------------------------
	// ���ϸ����� ���ؽ� ����(����� ���·� ����� ���ؼ�)
	// ---------------------------------------------------
	std::wstring prefix = L"Global\\";
	MutexScopedHandle hMutex(CreateMutex(NULL, TRUE, prefix.append(File::GetFileName(lpszLocalFilePath)).c_str()));
	if (GetLastError() == ERROR_ALREADY_EXISTS){
		//�ش� ������ ������̹Ƿ� ����Ѵ�.
		ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"WaitForSingleObject", _T("���ϻ�� ��� ����"));
		DWORD ret = WaitForSingleObject(hMutex, INFINITE);
		ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"WaitForSingleObject", ret);
	}

	// ---------------------------------------------------
	// ���������ϰ� ���������� ����� ���Ѵ�.
	// ---------------------------------------------------
	INT64 nRemoteFileSize = 0, nLocalFileSize = 0, nTotalWrittenSize = 0;
	errorCode = GetRemoteFileSize(lpszRemoteFilePath, nRemoteFileSize);
	if (errorCode != NO_ERROR){
		return errorCode;
	}
	BOOL bRet = File::GetFileSize(lpszLocalFilePath, nLocalFileSize);
	if (bRet && nLocalFileSize == nRemoteFileSize){
		
		if (m_fpProgress)
			m_fpProgress(m_lpThis, nRemoteFileSize, nRemoteFileSize, lpszLocalFilePath);

		ErrorHandler::ErrorHandlerFormat(_T("[%s - ���ϼ������] %s"), _T(__FUNCTION__), lpszRemoteFilePath);
		return NO_ERROR;
	}

	// ---------------------------------------------------
	// ������ ������ �ٿ�ε� �Ѵ�.
	// ---------------------------------------------------
	errorCode = DownloadFileSub(lpszRemoteFilePath, nRemoteFileSize, lpszLocalFilePath, nTotalWrittenSize);
	
	// ---------------------------------------------------
	// ���������ϰ� ���������� ����� ���Ѵ�.
	// ---------------------------------------------------
	if (nTotalWrittenSize == nRemoteFileSize && NO_ERROR == errorCode) {
		ErrorHandler::ErrorHandlerFormat(_T("[%s - ���ϼ��ſϷ�] %s(%I64d byte)"), _T(__FUNCTION__), lpszRemoteFilePath, nTotalWrittenSize);
	}else{
		ErrorHandler::ErrorHandlerFormat(_T("[%s - ���ϼ��Ž���] %s(%I64d byte)"), _T(__FUNCTION__), lpszRemoteFilePath, nTotalWrittenSize);
		DeleteFile(lpszLocalFilePath);
	}

	return errorCode;
}

DWORD CHttpManager::DownloadFileSub(LPCTSTR lpszRemoteFilePath, INT64 nRemoteFileSize, LPCTSTR lpszLocalFilePath, INT64& nTotalWrittenSize)
{
	DWORD errorCode = NO_ERROR;
	nTotalWrittenSize = 0;

	// ---------------------------------------------------
	// ������ ������ ����.
	// ---------------------------------------------------
	LPCTSTR lpszAcceptTypes[2]; ZeroMemory(lpszAcceptTypes, sizeof(lpszAcceptTypes)); lpszAcceptTypes[0] = _T("*/*");
	InternetScopedHandle hRemoteFile(HttpOpenRequest(m_http, _T("GET"), lpszRemoteFilePath, NULL, NULL, lpszAcceptTypes, 
		INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_KEEP_CONNECTION, NULL));
	if (NULL == hRemoteFile){

		errorCode = GetLastError();
		ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"HttpOpenRequest", errorCode);
		return errorCode;
	}
	if (!HttpSendRequest(hRemoteFile, NULL, 0, NULL, 0)) {
		errorCode = GetLastError();
		ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"HttpSendRequest", errorCode);
		return errorCode;
	}

	// --------------------------------------------------------
	// ���� ����
	// --------------------------------------------------------
	FileScopedHandle hLocalFile(File::FCreateFile(lpszLocalFilePath));
	if (INVALID_HANDLE_VALUE == hLocalFile) {
		errorCode = ERROR_INVALID_HANDLE;
		ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"FCreateFile", errorCode);
		return errorCode;
	}

	// --------------------------------------------------------
	// ���� �Ҵ�
	// --------------------------------------------------------
	DWORD dwAvailableBytes = 0;
	if (!InternetQueryDataAvailable(hRemoteFile, &dwAvailableBytes, 0, 0)) {
		errorCode = GetLastError();
		ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"InternetQueryDataAvailable", errorCode);
		return errorCode;
	}
	GlobalAllocScopedHandle hAlloc(GlobalAlloc(GMEM_FIXED, dwAvailableBytes));
	if (hAlloc == NULL) return GetLastError();
	BYTE* receiveBuffer = (PBYTE)GlobalLock(hAlloc);
	if (receiveBuffer == NULL) return GetLastError();

	INT64 percent = nRemoteFileSize / (25 * dwAvailableBytes);

	// --------------------------------------------------------
	// ���� �ٿ�ε�
	// --------------------------------------------------------
	DWORD receiveBufferLength = 0, writtenBufferLength = 0, index = 0;
	do 
	{
		// ���� ���Ͽ��� �����͸� �о�´�
		if (!InternetReadFile(hRemoteFile, receiveBuffer, dwAvailableBytes, &receiveBufferLength)){
			errorCode = GetLastError();
			ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"InternetReadFile", errorCode);
			break;
		}

		// ���� ���Ͽ� �����͸� �����Ѵ�.
		if (!WriteFile(hLocalFile, receiveBuffer, receiveBufferLength, &writtenBufferLength, NULL)){
			errorCode = GetLastError();
			ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"WriteFile", errorCode);
			break;
		}

		// ��ü ũ�⸦ �����Ѵ�.
		nTotalWrittenSize += writtenBufferLength;

		// �ݹ��� ȣ���Ѵ�.
		if(percent == 0){
			if (m_fpProgress)
				m_fpProgress(m_lpThis, nTotalWrittenSize, nRemoteFileSize, lpszLocalFilePath);
		}else if (index++ % percent == 0) {
			if (m_fpProgress)
				m_fpProgress(m_lpThis, nTotalWrittenSize, nRemoteFileSize, lpszLocalFilePath);
		}

		// ��������� ���Դ��� Ȯ���Ѵ�.
		if (CANCEL_DOWNLOAD == m_command || FINISHED == m_command) {
			errorCode = ERROR_CANCELLED;
			SetLastError(ERROR_CANCELLED);
			ErrorHandler::ErrorHandler(_T(__FUNCTION__), errorCode);
			break;
		} 

	} while (receiveBufferLength != 0);

	// ������ �ݹ��� ȣ���Ѵ�.
	if (m_fpProgress)
		m_fpProgress(m_lpThis, nTotalWrittenSize, nRemoteFileSize, lpszLocalFilePath);

	return errorCode;
}