#pragma once
#include "Token.h"
#include <string>

enum ErrorStage {
	ERR_File,
	ERR_Token,
	ERR_Parse
};

enum ErrorType {
	ErrorType_Info,
	ErrorType_Warning,
	ErrorType_Error
};

class Tokenizer
{
private:
	unsigned uLineNumber = 0, uLinePosition = 0;

	std::string strFileContents;
	std::string::iterator itFilePos;
	char nextChar();
	const std::string symbols = "{}()[].,;+-*/&|<>=~";
public:
	Tokenizer(const std::string &path);
	~Tokenizer();

	void loadFile(const std::string &path);

	bool hasMoreTokens();
	Token advance();

	void Error(const std::string& errMsg, ErrorType type, ErrorStage stage = ERR_Token);
};

