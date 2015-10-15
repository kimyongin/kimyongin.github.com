#include "stdafx.h"
#include "HttpManager2.h"
#include "ErrorHandler.h"
#include "ScopedHandle.h"
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <regex>
#include <iostream>

using namespace FTPC;
using namespace FTPC::ErrorHandler;

// ----------------------------------------------------------------------------------------
// CHttpManager2 Ŭ���� 
// ----------------------------------------------------------------------------------------
CHttpManager2::CHttpManager2(LPCWSTR szServerAddr,
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
	m_lpThis(lpThis),
	m_socket(m_io_service)
{

}

CHttpManager2::~CHttpManager2()
{

}

DWORD CHttpManager2::Init()
{
	DWORD errorCode = NO_ERROR;
	std::string ansiServerAddr, ansiServerPort;
	Common::StringEx::ToString(m_strServerAddr, ansiServerAddr);
	ansiServerPort = (boost::format("%d") % m_dwServerPort).str();

	boost::system::error_code ec;
	tcp::resolver resolver(m_io_service);
	tcp::resolver::query query(tcp::v4(), ansiServerAddr, ansiServerPort);
	m_endpoint_iterator = resolver.resolve(query, ec);
	if (ec.value()){
		errorCode = ec.value();
	}
	ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"Resolve", errorCode);
	return errorCode;
}	

DWORD CHttpManager2::Connect()
{
	DWORD errorCode = NO_ERROR;
	errorCode = Init();
	if (errorCode != NO_ERROR)
		return errorCode;

	boost::system::error_code ec;
	boost::asio::connect(m_socket, m_endpoint_iterator, ec);
	string msg = ec.message();
	if (ec.value()){
		errorCode = ec.value();
	}
	std::wstring addr = (boost::wformat(L"%s:%d") % m_strServerAddr % m_dwServerPort).str();
	ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"ServerAddress", addr.c_str());
	ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"Connect", errorCode);
	return errorCode;
}
VOID CHttpManager2::Disconnect()
{
	m_socket.close();
	ErrorHandler::ErrorHandler(_T(__FUNCTION__), (DWORD)NO_ERROR);
}

DWORD CHttpManager2::ParseHeaderData(CONST std::string header, INT& majorVersion, INT& minorVersion, INT& statusCode, INT64& contentSize)
{
	// Get Status Line
	std::regex rgxStatus(".*HTTP/(\\d).(\\d) (\\d{3}) (\\w+).*");
	std::smatch matchStatus;
	if (!std::regex_search(header.cbegin(), header.cend(), matchStatus, rgxStatus)){
		ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"Can Not Parsing Header Status-Line");
		return ERROR_INVALID_DATA;
	}

	// Get Content Length
	std::regex rgxContentLength(".*Content-Length: (\\w+).*");
	std::smatch matchContentLength;
	if (!std::regex_search(header.cbegin(), header.cend(), matchContentLength, rgxContentLength)){
		ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"Can Not Parsing Header Content-Length");
		return ERROR_INVALID_DATA;
	}

	try{
		majorVersion = boost::lexical_cast<int>(matchStatus[1]);
		minorVersion = boost::lexical_cast<int>(matchStatus[2]);
		statusCode = boost::lexical_cast<int>(matchStatus[3]);
		contentSize = boost::lexical_cast<int>(matchContentLength[1]);
	}catch (boost::bad_lexical_cast ec){
		ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"Can Not Cast Header Data");
		return ERROR_INVALID_DATA;
	}

	return NO_ERROR;
}

DWORD CHttpManager2::DownloadFile(LPCTSTR lpszRemoteFilePath, LPCTSTR lpszLocalFilePath)
{
	DWORD errorCode = NO_ERROR;
	boost::system::error_code ec;
	ErrorHandler::ErrorHandlerFormat(_T("[%s - ���ϼ��Ž���] %s"), _T(__FUNCTION__), lpszRemoteFilePath);

	std::string lpszRemoteFilePathAnsi, ansiServerAddr;
	Common::StringEx::ToString(lpszRemoteFilePath, lpszRemoteFilePathAnsi);
	Common::StringEx::ToString(m_strServerAddr, ansiServerAddr);

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
	// ������ ��û�Ѵ�.
	// ---------------------------------------------------
	boost::asio::streambuf request;
	std::ostream request_stream(&request);
	request_stream << "GET /" << lpszRemoteFilePathAnsi << " HTTP/1.1\r\n";
	request_stream << "Host: " << ansiServerAddr.c_str() << "\r\n";
	request_stream << "Accept: */*\r\n";
	request_stream << "Connection: keep-alive\r\n\r\n";
	boost::asio::write(m_socket, request);

	// ---------------------------------------------------
	// HTTP RESPONSE ����� ���Ź޴´�.
	// ---------------------------------------------------
	size_t headerSize = 0, currentSize = 0, readSize = 0;
	boost::asio::streambuf readUntilData;
	headerSize = boost::asio::read_until(m_socket, readUntilData, "\r\n\r\n", ec);
	if (ec){
		errorCode = ec.value();
		ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"read_until", ec.value());
		return errorCode;
	}

	currentSize = readUntilData.size();
	std::string responseHeader;
	responseHeader.assign(
		boost::asio::buffers_begin(readUntilData.data()),
		boost::asio::buffers_begin(readUntilData.data()) + currentSize
		);

	// ---------------------------------------------------
	// HTTP RESPONSE ����� �Ľ��ؼ� ���������� ����� ���Ѵ�.
	// ---------------------------------------------------
	INT majorVersion, minorVersion, statusCode; INT64 nRemoteFileSize;
	ParseHeaderData(responseHeader, majorVersion, minorVersion, statusCode, nRemoteFileSize);
	if (nRemoteFileSize <= 0){
		// ������ �������� �ʴ´�.
		ErrorHandler::ErrorHandlerFormat(_T("[%s - ����ִ�����] %s"), _T(__FUNCTION__), lpszRemoteFilePath);
		return ERROR_NO_MORE_FILES;
	}

	// ---------------------------------------------------
	// ���������ϰ� ���������� ����� ���Ѵ�.
	// ---------------------------------------------------
	INT64 nLocalFileSize = 0, nTotalWrittenSize = 0;
	BOOL bRet = File::GetFileSize(lpszLocalFilePath, nLocalFileSize);
	if (bRet && nLocalFileSize == nRemoteFileSize){

		if (m_fpProgress)
			m_fpProgress(m_lpThis, nRemoteFileSize, nRemoteFileSize, lpszLocalFilePath);
		ErrorHandler::ErrorHandlerFormat(_T("[%s - ���ϼ������] %s"), _T(__FUNCTION__), lpszRemoteFilePath);
		return NO_ERROR;
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

	// ---------------------------------------------------
	// HTTP RESPONSE �ٵ� ���Ź޴´�.
	// ---------------------------------------------------
	DWORD writtenBufferLength = 0;
	if (nRemoteFileSize > 0)
	{
		// �������Ͽ� �����͸� �����Ѵ�.(��� �����Ҷ� �ٵ� �Ϻ� ���ŵȴ�.)
		if (!WriteFile(hLocalFile, &responseHeader[headerSize], currentSize - headerSize, &writtenBufferLength, NULL)){
			errorCode = GetLastError();
			ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"WriteFile", errorCode);
			return errorCode;
		}

		// ��ü ũ�⸦ �����Ѵ�.
		nTotalWrittenSize += writtenBufferLength;
	}

	// �� �޾ƾ��� �ٵ� ����� ����Ѵ�.
	unsigned int remainSize = headerSize + nRemoteFileSize - currentSize;
	if (remainSize > 0)
	{
		const unsigned int bufferSize = 10240;
		char buffer[bufferSize];
		int index = 0;
		INT64 percent = nRemoteFileSize / (25 * bufferSize);
		do
		{
			// ������ �����͸� �о�´�.
			memset(buffer, 0, bufferSize);
			readSize = m_socket.read_some(boost::asio::buffer(buffer, bufferSize), ec);
			std::string msg = ec.message();
			if (ec.value() != 2/*End Of File*/ && ec.value() != 0){
				errorCode = ec.value();
				ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"read", ec.value());
				return errorCode;
			}

			// �������Ͽ� �����͸� �����Ѵ�.
			if (!WriteFile(hLocalFile, buffer, readSize, &writtenBufferLength, NULL)){
				errorCode = GetLastError();
				ErrorHandler::ErrorHandler(_T(__FUNCTION__), L"WriteFile", errorCode);
				return errorCode;
			}

			// ��ü ũ�⸦ �����Ѵ�.
			nTotalWrittenSize += writtenBufferLength;

			// �ݹ��� ȣ���Ѵ�.
			if (percent == 0){
				if (m_fpProgress)
					m_fpProgress(m_lpThis, nTotalWrittenSize, nRemoteFileSize, lpszLocalFilePath);
			}
			else if (index++ % percent == 0) {
				if (m_fpProgress)
					m_fpProgress(m_lpThis, nTotalWrittenSize, nRemoteFileSize, lpszLocalFilePath);
			}

		} while (readSize > 0);
	}

	// ������ �ݹ��� ȣ���Ѵ�.
	if (m_fpProgress)
		m_fpProgress(m_lpThis, nTotalWrittenSize, nRemoteFileSize, lpszLocalFilePath);

	// ---------------------------------------------------
	// ���������ϰ� ���������� ����� ���Ѵ�.
	// ---------------------------------------------------
	if (nTotalWrittenSize == nRemoteFileSize && NO_ERROR == errorCode) {
		ErrorHandler::ErrorHandlerFormat(_T("[%s - ���ϼ��ſϷ�] %s(%I64d byte)"), _T(__FUNCTION__), lpszRemoteFilePath, nTotalWrittenSize);
	}
	else{
		ErrorHandler::ErrorHandlerFormat(_T("[%s - ���ϼ��Ž���] %s(%I64d byte)"), _T(__FUNCTION__), lpszRemoteFilePath, nTotalWrittenSize);
		DeleteFile(lpszLocalFilePath);
	}

	return errorCode;
}