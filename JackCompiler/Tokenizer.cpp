#include "Tokenizer.h"
#include <fstream>
#include <iostream>


char Tokenizer::nextChar()
{
	if (itFilePos != strFileContents.end()) {
		++uLinePosition;
		return *itFilePos++;
	}
	else return '\0';
}

Tokenizer::Tokenizer(const std::string &path)
{
	loadFile(path);
}


Tokenizer::~Tokenizer()
{
}

void Tokenizer::loadFile(const std::string & path)
{
	std::ifstream file(path);
	if (file.is_open()) {
		file >> strFileContents;
		itFilePos = strFileContents.begin();
		file.close();
	}
	else {
		Error("Couldn't open file " + path, ErrorType_Error, ERR_File);
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

	for (char value = nextChar(); value != '\0'; value = nextChar()) {
		// Whitespace
		if (value == '\n') {
			++uLineNumber;
			uLinePosition = 0;
		}
		else if (iswspace(value)) {

		}
		// Int constant
		else if (isdigit(value)) {

		}
		// Symbol
		else if (symbols.find(value) != symbols.npos) {

		}
		// String constant
		else if (value == '"') {

		}
	}
	
	return Token();
}

void Tokenizer::Error(const std::string & errMsg, ErrorType type, ErrorStage stage)
{
	std::string msg = " on line " + std::to_string(uLineNumber) + ", " + std::to_string(uLinePosition);
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
