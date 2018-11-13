#include "Parser.h"
#include "TerminalASTNode.h"
#include <iostream>

#define MakeChild(x) std::make_unique<TerminalASTNode>(x)
#define getKeyword(x) std::get<Keyword>(x)

bool Parser::isKeywordType(Keyword kw)
{
	const static Keyword types[] = {
		KWD_BOOL,
		KWD_CHAR,
		KWD_INT,
	};
	for (auto& k : types) {
		if (kw == k)
			return true;
	}
	return false;
}

bool Parser::isKeywordSubDec(Keyword kw)
{
	const static Keyword subs[] = {
		KWD_CONSTRUCTOR,
		KWD_FUNCTION,
		KWD_METHOD,
	};
	for (auto& k : subs) {
		if (kw == k)
			return true;
	}
	return false;
}

bool Parser::isKeywordStatement(Keyword kw)
{
	const static Keyword statements[] = {
		KWD_LET,
		KWD_IF,
		KWD_WHILE,
		KWD_DO,
		KWD_RETURN
	};
	for (auto& k : statements) {
		if (kw == k)
			return true;
	}
	return false;
}

bool Parser::isSymbolOperator(char sym)
{
	const static std::string operators = "+-*/&|<>=";
	if (operators.find(sym) != operators.npos)
		return true;
	return false;
}

bool Parser::isSymbolUnaryOp(char sym)
{
	const static std::string unaryops = "-~";
	if (unaryops.find(sym) != unaryops.npos)
		return true;
	return false;
}

Token Parser::expect(Token token, bool usevalue)
{
	Token tok = lexer.advance();
	if (tok.getType() != token.getType()) {
		lexer.Error("Expected type " + TokenToString(token.getType()) + " but got " + TokenToString(tok.getType()) + " instead.");
	}
	if (usevalue && (token.getValue() != tok.getValue())) {
		lexer.Error("Expected value " + token.ValueToString() + " but got " + tok.ValueToString() + " instead.");
	}
	return tok;
}

void Parser::parseClass()
{
	ASTNode root("class");

	Token t, current;
	t.setKeyword(KWD_CLASS);
	root.addChild(MakeChild(TerminalASTNode(expect(t, true))) );

	t.setIdentifier("");
	current = expect(t, false);
	root.addChild(MakeChild( TerminalASTNode(current) ));

	t.setSymbol('{');
	root.addChild(MakeChild( TerminalASTNode(expect(t, true))) );

	current = lexer.advance();

	// Parse all of the Class Variable Declarations.
	while (lexer.current().getType() == TOK_KEYWORD &&
		(std::get<Keyword>(lexer.current().getValue()) == KWD_STATIC ||
		std::get<Keyword>(lexer.current().getValue()) == KWD_FIELD)) 
	{
		root.addChild(parseClassVarDec());
	}

	// Parse all of the Subroutine dec/definitions.
	while (lexer.current().getType() == TOK_KEYWORD &&
		isKeywordSubDec( getKeyword(lexer.current().getValue()) ))
	{
		root.addChild(parseSubroutine());
	}

	if (lexer.current().getType() == TOK_SYMBOL && std::get<char>(lexer.current().getValue()) == '}') {
		root.addChild(MakeChild(TerminalASTNode(lexer.current())));
		std::cout << root.prettyPrint() << "\n";
	}
	else lexer.Error("Expected a closing } at the end of class definition.");
}

Parser::Parser(const std::string &path) : lexer(path)
{
	parseClass();
}


Parser::~Parser()
{
}

ASTChild Parser::parseClassVarDec()
{
	ASTChild dec(new ASTNode("classVarDec"));

	Token current = lexer.current();
	Keyword keyword = getKeyword(current.getValue());
	switch (keyword) {
	default:
		lexer.Error("Expected 'static' or 'field' keyword, got " + current.ValueToString());
	case KWD_STATIC:
	case KWD_FIELD:
		dec->addChild(MakeChild(TerminalASTNode(current)));
		break;
	}

	Token t;
	t.setKeyword(KWD_BOOL);
	current = expect(t, false);
	keyword = getKeyword(current.getValue());
	switch (keyword) {
	default:
		lexer.Error("Expected a type keyword, got " + current.ValueToString());
	case KWD_BOOL:
	case KWD_CHAR:
	case KWD_INT:
		dec->addChild(MakeChild(TerminalASTNode(current)));
		break;
	}

	t.setIdentifier("");
	current = expect(t, false);
	dec->addChild(MakeChild(TerminalASTNode(current)));

	current = lexer.advance();
	while (current.getType() == TOK_SYMBOL && (std::get<char>(current.getValue()) == ',')) {
		dec->addChild(MakeChild(TerminalASTNode(expect(t, false))));
		current = lexer.advance(); // move to the next comma.
	}

	// Adding the semicolon to the AST is completely and absolutely redundant.
	// Are we generating code from the semicolon?
	// What purpose is its presence in the tree supposed to fulfill?
	t.setSymbol(';');
	if (current.getType() == TOK_SYMBOL && std::get<char>(current.getValue()) == ';') {
		dec->addChild(MakeChild(TerminalASTNode(current)));
		lexer.advance();
	}
	else lexer.Error("Expected a semicolon at the end of variable declarations.");

	return dec;
}

ASTChild Parser::parseSubroutine()
{
	ASTChild subroutine(new ASTNode("subroutineDec"));

	// method/function/etc
	Token t, cur;
	cur = lexer.current();
	if (isKeywordSubDec(getKeyword(cur.getValue()))) {
		subroutine->addChild(MakeChild(TerminalASTNode(cur)));
	}
	else lexer.Error("Expected a subroutine declaration.");

	// return type
	cur = lexer.advance();
	if (cur.getType() == TOK_KEYWORD){
		if (isKeywordType(getKeyword(cur.getValue()))) {
			subroutine->addChild(MakeChild(TerminalASTNode(cur)));
		}
		else lexer.Error("Expected a type.");
	}
	// Can also be a class name.
	else if (cur.getType() == TOK_IDENTIFIER) {
		subroutine->addChild(MakeChild(TerminalASTNode(cur)));
	}
	else {
		lexer.Error("Expected a type.");
	}

	// subroutine name
	t.setIdentifier("");
	subroutine->addChild(MakeChild(TerminalASTNode(expect(t, false))));

	// open paren
	t.setSymbol('(');
	subroutine->addChild(MakeChild(TerminalASTNode(expect(t, true))));

	// parameter list
	subroutine->addChild(parseExpressionList());

	// closing paren
	t.setSymbol(')');
	subroutine->addChild(MakeChild(TerminalASTNode(expect(t, true))));

	// Subroutine body:
	ASTChild body(new ASTNode("subroutineBody"));
	// opening brace
	t.setSymbol('{');
	body->addChild(MakeChild(TerminalASTNode(expect(t, true))));

	// variable declarations
	cur = lexer.advance();
	while (lexer.current().getType() == TOK_KEYWORD &&
		(getKeyword(lexer.current().getValue()) == KWD_VAR))
	{
		body->addChild(parseVarDec());
	}

	// statements
	while (lexer.current().getType() == TOK_KEYWORD &&
		isKeywordStatement(getKeyword(lexer.current().getValue())))
	{
		switch (getKeyword(lexer.current().getValue())) {
		case KWD_DO:
			body->addChild(parseDo());
			break;
		case KWD_IF:
			body->addChild(parseIf());
			break;
		case KWD_LET:
			body->addChild(parseLet());
			break;
		case KWD_WHILE:
			body->addChild(parseWhile());
			break;
		case KWD_RETURN:
			body->addChild(parseReturn());
		}
	}

	// closing brace
	if (lexer.current().getType() == TOK_SYMBOL && std::get<char>(lexer.current().getValue()) == '}') {
		body->addChild(MakeChild(TerminalASTNode(lexer.current())));
	}

	subroutine->addChild(std::move(body));
	return subroutine;
}

ASTChild Parser::parseVarDec()
{
	ASTChild dec(new ASTNode("varDec"));

	// We already know there's a var because that's how we got into this routine
	dec->addChild(MakeChild(TerminalASTNode(lexer.current())));

	Token t, cur;
	t.setKeyword(KWD_VOID);
	cur = lexer.advance();
	if (cur.getType() == TOK_KEYWORD && isKeywordType(getKeyword(cur.getValue())) ) {
		dec->addChild(MakeChild(TerminalASTNode(lexer.current())));
	}
	else if (cur.getType() == TOK_IDENTIFIER) {
		dec->addChild(MakeChild(TerminalASTNode(lexer.current())));
	}
	else lexer.Error("Expected a type.");

	// identifier
	t.setIdentifier("");
	dec->addChild(MakeChild(TerminalASTNode(expect(t, false))));

	// additional declarations
	cur = lexer.advance();
	while (cur.getType() == TOK_SYMBOL && (std::get<char>(cur.getValue()) == ',')) {
		dec->addChild(MakeChild(TerminalASTNode(expect(t, false))));
		cur = lexer.advance(); // move to the next comma.
	}

	t.setSymbol(';');
	if (cur.getType() == TOK_SYMBOL && std::get<char>(cur.getValue()) == ';') {
		dec->addChild(MakeChild(TerminalASTNode(cur)));
		lexer.advance();
	}
	else lexer.Error("Expected a semicolon at the end of variable declarations.");

	return dec;
}

ASTChild Parser::parseDo()
{
	ASTChild statement(new ASTNode("doStatement"));
	statement->addChild(MakeChild(TerminalASTNode(lexer.current())));

	// parse subroutine call
	Token t, cur;

	// identifier
	t.setIdentifier("");
	statement->addChild(MakeChild(TerminalASTNode(expect(t, false))));

	// paren
	t.setSymbol('.');
	cur = expect(t, false);
	if (std::get<char>(cur.getValue()) == '.') {
		// Need another identifier
		t.setIdentifier("");
		statement->addChild(MakeChild(TerminalASTNode(expect(t, false))));
	}
	else if (std::get<char>(cur.getValue()) == '(') {
		statement->addChild(MakeChild(TerminalASTNode(lexer.current())));
		statement->addChild(parseExpressionList());
	}

	return statement;
}

ASTChild Parser::parseReturn()
{
	ASTChild ret(new ASTNode("ReturnStatement"));
	// add the return keyword
	ret->addChild(MakeChild(TerminalASTNode(lexer.current())));

	// Optionally, add any expression.
	if (lexer.advance().getType() == TOK_SYMBOL && std::get<char>(lexer.current().getValue()) == ';') {
			ret->addChild(parseExpression());
	}
	return ret;
}

ASTChild Parser::parseTerm()
{
	// parse the term
	ASTChild term(new ASTNode("term"));
	switch (lexer.current().getType()) {
	default:
		lexer.Error("Expected an expression.");
	case TOK_CONST_INT:
	case TOK_CONST_STRING:
		term->addChild(MakeChild(TerminalASTNode(lexer.current())));
		break;

	case TOK_SYMBOL:
		if (isSymbolUnaryOp(std::get<char>(lexer.current().getValue()))) {
			term->addChild(MakeChild(TerminalASTNode(lexer.current())));
		}
		else if (std::get<char>(lexer.current().getValue()) == '(') {
			term->addChild(MakeChild(TerminalASTNode(lexer.current())));
			term->addChild(parseExpression());
			term->addChild(MakeChild(TerminalASTNode(lexer.current())));
		}
		else lexer.Error("Expected a unary operator or parenthesis in expression term.");
		break;

	case TOK_KEYWORD:
		switch (getKeyword(lexer.current().getValue())) {
		default:
			lexer.Error("Expected a constant.");
		case KWD_TRUE:
		case KWD_FALSE:
		case KWD_NULL:
		case KWD_THIS:
			term->addChild(MakeChild(TerminalASTNode(lexer.current())));
		}
		break;

		// Identifiers might have an array operator.
		// Honestly, this aught to be done with some operator precedence parsing.
	case TOK_IDENTIFIER:
		term->addChild(MakeChild(TerminalASTNode(lexer.current())));
		if (lexer.advance().getType() == TOK_SYMBOL &&
			std::get<char>(lexer.current().getValue()) == '[') {
			term->addChild(MakeChild(TerminalASTNode(lexer.current())));
			term->addChild(parseExpression());
			term->addChild(MakeChild(TerminalASTNode(lexer.current())));
		}
	}

	lexer.advance();
	return term;
}

ASTChild Parser::parseExpression()
{
	ASTChild exp(new ASTNode("expression"));

	// Here, we'd usually build up a list of terms and operators
	// and use the shunting yard algorithm or something
	// but...
	exp->addChild(parseTerm());

	// parse the op/term pairs
	//lexer.advance();
	while (lexer.current().getType() == TOK_SYMBOL && isSymbolOperator(std::get<char>(lexer.current().getValue()))) {
		exp->addChild(MakeChild(TerminalASTNode(lexer.current())));
		exp->addChild(parseTerm());
	}

	return exp;
}
