#pragma once
#include "Token.h"
#include <string>
//#include <map>

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

	const static std::string symbols;
	//const static std::map<std::string, Keyword> keywords;

	std::string strFileContents;
	std::string::iterator itFilePos;
	char nextChar();
	char curChar();

	Token makeInt();
	Token makeString();
	Token makeidentifier();
	Token makeSymbol();

	bool isIdentifierChar(char c);

	Token curToken;
public:
	Tokenizer(const std::string &path);
	~Tokenizer();

	void loadFile(const std::string &path);

	bool hasMoreTokens();
	Token advance();
	inline Token current() {
		return curToken;
	}

	void Error(const std::string& errMsg, ErrorType type = ErrorType_Error/*, ErrorStage stage = ERR_Token*/);
};

