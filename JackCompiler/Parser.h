#pragma once
#include "Tokenizer.h"
#include "VMWriter.h"
#include "ASTNode.h"

class Parser
{
private:
	Tokenizer lexer;
	VMWriter writer;

	ASTChild parseClassVarDec();
	ASTChild parseSubroutine();
	ASTChild parseVarDec();
	ASTChild parseDo();
	ASTChild parseLet();
	ASTChild parseWhile();
	ASTChild parseReturn();
	ASTChild parseIf();
	ASTChild parseExpression();
	ASTChild parseTerm();
	ASTChild parseExpressionList();

	ASTChild parseStatements();

	Token expect(Token token, bool usevalue);
	Token expectNow(Token token, bool usevalue);
	std::string outputXML;

	ASTChild root;
	// Traverse AST and emit VM code.
	void writeVMOutput(const std::string& outFile);
public:
	// Top-level parse: everything is done from here.
	void parseClass();
	inline std::string getOutput() {
		return outputXML;
	}

	Parser(const std::vector<std::string>& files);
	~Parser();
};

