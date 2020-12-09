#pragma once
#include <exception>
class NodematException : public std::exception{
public:
	NodematException() = default;
	~NodematException() = default;
	NodematException(LPCSTR _message, int line, std::string file) : exception(_message), message(nullptr) {
		std::ostringstream oss;
		oss << _message << std::endl << "Line: " << line << std::endl << "File: " << file;
		message = oss.str().c_str();
	}

	const char* what(){
		return message;
	}

	NodematException(HRESULT hr, int line, std::string file) {
		std::ostringstream oss;
		oss << GetHrError(hr) << std::endl << "Line: " << line << std::endl << "File: " << file;
		message = oss.str().c_str();
	}
private:
	std::string GetHrError(HRESULT hr) const {
		const char* msgBuff = nullptr;
		DWORD msgLen = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPSTR>(&msgBuff), 0, nullptr);
		if (msgLen == 0) return "Unknown error";
		return msgBuff;
	}
	const char* message;
};

#define NMEX_HR(hr) if(FAILED(hr)) throw NodematException(hr,__LINE__,__FILE__) // Macro throw Nodemat Exception by hresult
#define NMEX(condition,message) if(condition) throw NodematException(message,__LINE__,__FILE__) // Macro throw Nodemat Exception by message
