#include "stdafx.h"
#include "HttpManager.h"
#include "ErrorHandler.h"
#include "ScopedHandle.h"
#include <boost/format.hpp>

using namespace FTPC;
using namespace FTPC::ErrorHandler;

// ----------------------------------------------------------------------------------------
// CHttpManager 클래스 
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

	DWORD rec_timeout = 60 * 1000 * 2;	// 120초
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
	// 응답코드 얻기 (원격지 파일존재유무 검사), 
	// 파일이 존재하지 않는 경우 CONTENT LENGTH가 엉뚱한 값 (에러 문자열)으로 대체 되기 때문이다.
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
	// 원격지(http)의 파일 사이즈 얻기
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
	ErrorHandler::ErrorHandlerFormat(_T("[%s - 파일수신시작] %s"), _T(__FUNCTION__), lpszRemoteFilePath);

	// ---------------------------------------------------
	// 파일명으로 뮤텍스 생성(사용중 상태로 만들기 위해서)
	// ---------------------------------------------------
	std::wstring prefix = L"Global\\";
	MutexScopedHandle hMutex(CreateMutex(NULL, TRUE, prefix.append(File::GetFileName(lpszLocalFilePath)).c_str()));
	if (GetLastError() == ERROR_ALREADY_EXISTS){
		//해당 파일이 사용중이므로 대기한다.
		ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"WaitForSingleObject", _T("파일사용 대기 시작"));
		DWORD ret = WaitForSingleObject(hMutex, INFINITE);
		ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"WaitForSingleObject", ret);
	}

	// ---------------------------------------------------
	// 원격지파일과 로컬파일의 사이즈를 비교한다.
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

		ErrorHandler::ErrorHandlerFormat(_T("[%s - 파일수신취소] %s"), _T(__FUNCTION__), lpszRemoteFilePath);
		return NO_ERROR;
	}

	// ---------------------------------------------------
	// 원격지 파일을 다운로드 한다.
	// ---------------------------------------------------
	errorCode = DownloadFileSub(lpszRemoteFilePath, nRemoteFileSize, lpszLocalFilePath, nTotalWrittenSize);
	
	// ---------------------------------------------------
	// 원격지파일과 로컬파일의 사이즈를 비교한다.
	// ---------------------------------------------------
	if (nTotalWrittenSize == nRemoteFileSize && NO_ERROR == errorCode) {
		ErrorHandler::ErrorHandlerFormat(_T("[%s - 파일수신완료] %s(%I64d byte)"), _T(__FUNCTION__), lpszRemoteFilePath, nTotalWrittenSize);
	}else{
		ErrorHandler::ErrorHandlerFormat(_T("[%s - 파일수신실패] %s(%I64d byte)"), _T(__FUNCTION__), lpszRemoteFilePath, nTotalWrittenSize);
		DeleteFile(lpszLocalFilePath);
	}

	return errorCode;
}

DWORD CHttpManager::DownloadFileSub(LPCTSTR lpszRemoteFilePath, INT64 nRemoteFileSize, LPCTSTR lpszLocalFilePath, INT64& nTotalWrittenSize)
{
	DWORD errorCode = NO_ERROR;
	nTotalWrittenSize = 0;

	// ---------------------------------------------------
	// 원격지 파일을 연다.
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
	// 파일 생성
	// --------------------------------------------------------
	FileScopedHandle hLocalFile(File::FCreateFile(lpszLocalFilePath));
	if (INVALID_HANDLE_VALUE == hLocalFile) {
		errorCode = ERROR_INVALID_HANDLE;
		ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"FCreateFile", errorCode);
		return errorCode;
	}

	// --------------------------------------------------------
	// 버퍼 할당
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
	// 파일 다운로드
	// --------------------------------------------------------
	DWORD receiveBufferLength = 0, writtenBufferLength = 0, index = 0;
	do 
	{
		// 원격 파일에서 데이터를 읽어온다
		if (!InternetReadFile(hRemoteFile, receiveBuffer, dwAvailableBytes, &receiveBufferLength)){
			errorCode = GetLastError();
			ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"InternetReadFile", errorCode);
			break;
		}

		// 로컬 파일에 데이터를 저장한다.
		if (!WriteFile(hLocalFile, receiveBuffer, receiveBufferLength, &writtenBufferLength, NULL)){
			errorCode = GetLastError();
			ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"WriteFile", errorCode);
			break;
		}

		// 전체 크기를 누적한다.
		nTotalWrittenSize += writtenBufferLength;

		// 콜백을 호출한다.
		if(percent == 0){
			if (m_fpProgress)
				m_fpProgress(m_lpThis, nTotalWrittenSize, nRemoteFileSize, lpszLocalFilePath);
		}else if (index++ % percent == 0) {
			if (m_fpProgress)
				m_fpProgress(m_lpThis, nTotalWrittenSize, nRemoteFileSize, lpszLocalFilePath);
		}

		// 정지명령이 들어왔는지 확인한다.
		if (CANCEL_DOWNLOAD == m_command || FINISHED == m_command) {
			errorCode = ERROR_CANCELLED;
			SetLastError(ERROR_CANCELLED);
			ErrorHandler::ErrorHandler(_T(__FUNCTION__), errorCode);
			break;
		} 

	} while (receiveBufferLength != 0);

	// 마지막 콜백을 호출한다.
	if (m_fpProgress)
		m_fpProgress(m_lpThis, nTotalWrittenSize, nRemoteFileSize, lpszLocalFilePath);

	return errorCode;
}