#include "Tokenizer.h"
#include <fstream>
#include <iostream>

using std::string;

const string Tokenizer::symbols = "{}()[].,;+-*/&|<>=~";

char Tokenizer::nextChar()
{
	if (itFilePos != strFileContents.end()) {
		// Keep track of the line number and offset in the file.
		if ((*itFilePos) == '\n') {
			uLineNumber++;
			uLinePosition = 0;
		}
		else ++uLinePosition;

		return *itFilePos++;
	}
	else return '\0';
}

char Tokenizer::curChar()
{
	if (itFilePos != strFileContents.end()) {
		
		return *itFilePos;
	}
	else return '\0';
}

Token Tokenizer::makeInt()
{
	Token token;
	string str;
	for (char c = nextChar(); isdigit(c); c = nextChar()) {
		str += c;

		if (!isdigit(curChar()))
			break;
	}
	unsigned result = (unsigned)std::stoi(str);

	if (result > 32767) {
		Error("Integer constant " + str + " is greater than the maximum size 32767.", ErrorType_Error);
	}

	token.setInt(result);
	return token;
}

Token Tokenizer::makeString()
{
	Token token;
	string str;
	nextChar(); // move past the quote
	for (char c = nextChar(); c != '"'; c = nextChar()) {
		str += c;
	}
	//nextChar(); // move past the quote

	token.setString(str);
	return token;
}

Token Tokenizer::makeidentifier()
{
	Token token;
	string str;

	for (char c = nextChar(); isIdentifierChar(c); c = nextChar()) {
		str += c;

		if (!isIdentifierChar(curChar())) {
			break;
		}
	}

	if (keywords.find(str) != keywords.end()) {
		token.setKeyword(keywords.at(str));
	}
	else token.setIdentifier(str);

	return token;
}

Token Tokenizer::makeSymbol()
{
	Token token;
	token.setSymbol(nextChar());
	return token;
}

bool Tokenizer::isIdentifierChar(char c)
{
	if ((symbols.find(c) != symbols.npos)
		|| iswspace(c) || c == '\0') {
		return false;
	}
	return true;
}

Tokenizer::Tokenizer(const string &path)
{
	loadFile(path);
}


Tokenizer::~Tokenizer()
{
}

void Tokenizer::loadFile(const string & path)
{
	std::ifstream file(path);
	if (file.is_open()) {
		string line;
		while (std::getline(file, line)) {
			strFileContents += line + "\n";
		}
		itFilePos = strFileContents.begin();
		file.close();
	}
	else {
		Error("Couldn't open file " + path, ErrorType_Error/*, ERR_File*/);
	}
}

bool Tokenizer::hasMoreTokens()
{
	if (itFilePos != strFileContents.end()) {
		return true;
	}
	return false;
}

Token Tokenizer::advance()
{
	Token token;
	char value = curChar();
	// Strip whitespace
	while (iswspace(value)) {
		nextChar();
		value = curChar();
	}
	if (value == '\0') {
		// Return EOF token.
		token = Token();
	}
	// Int constant
	else if (isdigit(value)) {
		token = makeInt();
	}
	// Symbol
	else if (symbols.find(value) != symbols.npos) {
		token = makeSymbol();
	}
	// String constant
	else if (value == '"') {
		token = makeString();
	}
	// identifier or keyword
	else{
		token = makeidentifier();
	}

	curToken = token;
	return token;
}

void Tokenizer::Error(const string & errMsg, ErrorType type/*, ErrorStage stage*/)
{
	string msg = " on line " + std::to_string(uLineNumber) + ", " + std::to_string(uLinePosition);
	switch (type) {
	case ErrorType_Warning:
		msg = "Warning: " + errMsg + msg;
		break;
	case ErrorType_Info:
		msg = errMsg + msg;
		break;
	case ErrorType_Error:
		msg = "ERROR: " + errMsg + msg;
		std::cerr << msg << std::endl;
		exit(1);
	}
	std::cout << msg << std::endl;
}
