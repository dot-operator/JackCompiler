#pragma once
#include "Tokenizer.h"
#include "ASTNode.h"

class Parser
{
private:
	Tokenizer lexer;

	ASTChild parseClassVarDec();
	ASTChild parseSubroutine();
	ASTChild parseParameterList();
	ASTChild parseVarDec();
	ASTChild parseDo();
	ASTChild parseLet();
	ASTChild parseWhile();
	ASTChild parseReturn();
	ASTChild parseIf();
	ASTChild parseExpression();
	ASTChild parseTerm();
	ASTChild parseExpressionList();

	ASTChild parseSubroutineCall();

	static bool isKeywordType(Keyword kw);
	static bool isKeywordSubDec(Keyword kw);
	static bool isKeywordStatement(Keyword kw);
	static bool isSymbolOperator(char sym);
	static bool isSymbolUnaryOp(char sym);

	Token expect(Token token, bool usevalue);
public:
	// Top-level parse: everything is done from here.
	void parseClass();

	Parser(const std::string &path);
	~Parser();
};

