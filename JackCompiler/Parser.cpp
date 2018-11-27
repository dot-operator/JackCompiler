#include "Parser.h"
#include "TerminalASTNode.h"
#include <iostream>

using std::string;

#define MakeChild(x) std::make_unique<TerminalASTNode>(x)
#define getKeyword(x) std::get<Keyword>(x)

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

Token Parser::expectNow(Token token, bool usevalue)
{
	Token tok = lexer.current();
	if (tok.getType() != token.getType()) {
		lexer.Error("Expected type " + TokenToString(token.getType()) + " but got " + TokenToString(tok.getType()) + " instead.");
	}
	if (usevalue && (token.getValue() != tok.getValue())) {
		lexer.Error("Expected value " + token.ValueToString() + " but got " + tok.ValueToString() + " instead.");
	}
	return tok;
}

void Parser::writeVMOutput(const string & outFile)
{
	for (ASTNode* node = root->firstChild(); node; node = root->nextChild()) {
		// A top-level identifier can only be the class name
		// (assuming we parsed correctly :)
		if (node->getName() == "identifier") {
			writer.setClassName(node);
		}
		// Subroutines.
		else if (node->getName() == "subroutineDec") {
			writer.writeSubroutine(node);
		}
	}
}

void Parser::parseClass()
{
	root = ASTChild(new ASTNode("class"));

	Token t, current;
	t.setKeyword(KWD_CLASS);
	root->addChild(MakeChild(TerminalASTNode(expect(t, true))) );

	t.setIdentifier("");
	current = expect(t, false);
	root->addChild(MakeChild( TerminalASTNode(current) ));

	t.setSymbol('{');
	root->addChild(MakeChild( TerminalASTNode(expect(t, true))) );

	current = lexer.advance();

	// Parse all of the Class Variable Declarations.
	while (lexer.current().getType() == TOK_KEYWORD &&
		(std::get<Keyword>(lexer.current().getValue()) == KWD_STATIC ||
		std::get<Keyword>(lexer.current().getValue()) == KWD_FIELD)) 
	{
		root->addChild(parseClassVarDec());
	}

	// Parse all of the Subroutine dec/definitions.
	while (lexer.current().getType() == TOK_KEYWORD &&
		isKeywordSubDec( getKeyword(lexer.current().getValue()) ))
	{
		root->addChild(parseSubroutine());
	}

	if (lexer.current().getType() == TOK_SYMBOL && std::get<char>(lexer.current().getValue()) == '}') {
		root->addChild(MakeChild(TerminalASTNode(lexer.current())));
		std::cout << root->prettyPrint() << "\n";
	}
	else lexer.Error("Expected a closing } at the end of class definition.");

	outputXML = root->prettyPrint();
}

Parser::Parser(const std::vector<string>& files)
{
	for (auto& fileName : files) {
		// Reset the lexer and symbol table.
		lexer.loadFile(fileName);
		writer.symbols = SymbolTable();

		parseClass();

		// Emit VM code.
		string outName;
		if (fileName.find('.') != fileName.npos) {
			outName = fileName.substr(0, fileName.find('.'));
		}
		outName += ".vm";
		writer.open(outName);
		writeVMOutput(outName);
		writer.close();
	}
}


Parser::~Parser()
{
}

ASTChild Parser::parseClassVarDec()
{
	ASTChild dec(new ASTNode("classVarDec"));

	Token current = lexer.current();
	Keyword keyword = getKeyword(current.getValue());
	Symbol sym;
	switch (keyword) {
	default:
		lexer.Error("Expected 'static' or 'field' keyword, got " + current.ValueToString());
	case KWD_STATIC:
		sym.kind = SymbolKind::SYM_STATIC;
		dec->addChild(MakeChild(TerminalASTNode(current)));
		break;
	case KWD_FIELD:
		sym.kind = SymbolKind::SYM_FIELD;
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
		sym.type = current.ValueToString();
		dec->addChild(MakeChild(TerminalASTNode(current)));
		break;
	}

	t.setIdentifier("");
	current = expect(t, false);
	sym.name = current.ValueToString();
	dec->addChild(MakeChild(TerminalASTNode(current)));
	
	// Add to symbol table.
	if (!writer.symbols.Define(sym)) {
		lexer.Error("Class variable declaration " + sym.name + " has already been defined.");
	}

	current = lexer.advance();
	while (current.getType() == TOK_SYMBOL && (std::get<char>(current.getValue()) == ',')) {
		dec->addChild(MakeChild(TerminalASTNode(expect(t, false))));
		
		// Add to symbol table.
		sym.name = expectNow(t, false).ValueToString();
		if (!writer.symbols.Define(sym)) {
			lexer.Error("Class variable declaration " + sym.name + " has already been defined.");
		}

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
	subroutine->addChild(MakeChild(TerminalASTNode(expectNow(t, true))));

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
	body->addChild(parseStatements());

	// closing brace
	t.setSymbol('}');
	body->addChild(MakeChild(TerminalASTNode(expect(t, true))));

	subroutine->addChild(std::move(body));
	return subroutine;
}

ASTChild Parser::parseVarDec()
{
	ASTChild dec(new ASTNode("varDec"));

	// We already know there's a var because that's how we got into this routine
	dec->addChild(MakeChild(TerminalASTNode(lexer.current())));

	// Prepare the symbol to add to the table.
	Symbol sym;
	sym.kind = SYM_VAR;

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
	sym.type = lexer.current().ValueToString();

	// identifier
	t.setIdentifier("");
	dec->addChild(MakeChild(TerminalASTNode(expect(t, false))));
	sym.name = lexer.current().ValueToString();

	// Try adding it to the symbol table.
	writer.symbols.Define(sym);

	// additional declarations
	cur = lexer.advance();
	while (cur.getType() == TOK_SYMBOL && (std::get<char>(cur.getValue()) == ',')) {
		dec->addChild(MakeChild(TerminalASTNode(expect(t, false))));

		sym.name = lexer.current().ValueToString();
		writer.symbols.Define(sym);

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
		// Now we do need the paren
		t.setSymbol('(');
		expect(t, true);
	}

	if (std::get<char>(cur.getValue()) == '(') {
		statement->addChild(MakeChild(TerminalASTNode(lexer.current())));
		statement->addChild(parseExpressionList());
		statement->addChild(MakeChild(TerminalASTNode(lexer.current())));
	}

	return statement;
}

ASTChild Parser::parseLet()
{
	ASTChild let(new ASTNode("letStatement"));
	// let keyword
	let->addChild(MakeChild(TerminalASTNode(lexer.current())));

	// identifier
	Token t, cur;
	t.setIdentifier("");
	let->addChild(MakeChild(TerminalASTNode(expect(t, false))));

	cur = lexer.advance();
	// Array operator
	if (cur.getType() == TOK_SYMBOL &&
		std::get<char>(lexer.current().getValue()) == '[') 
	{
		let->addChild(MakeChild(TerminalASTNode(cur)));
		let->addChild(parseExpression());
		let->addChild(MakeChild(TerminalASTNode(cur)));
	}

	// = operator
	t.setSymbol('=');
	let->addChild(MakeChild(TerminalASTNode(expectNow(t, true))));

	lexer.advance();

	let->addChild(parseExpression());

	// semicolon
	t.setSymbol(';');
	let->addChild(MakeChild(TerminalASTNode(expectNow(t, true))));

	lexer.advance();

	return let;
}

ASTChild Parser::parseWhile()
{
	ASTChild statement(new ASTNode("whileStatement"));
	// add the keyword
	statement->addChild(MakeChild(TerminalASTNode(lexer.current())));

	// open paren
	Token t;
	t.setSymbol('(');
	statement->addChild(MakeChild(TerminalASTNode(expect(t, true))));

	// expr
	statement->addChild(parseExpression());

	// close paren, opening brace
	t.setSymbol(')');
	statement->addChild(MakeChild(TerminalASTNode(expect(t, true))));
	t.setSymbol('{');
	statement->addChild(MakeChild(TerminalASTNode(expect(t, true))));

	// statements
	statement->addChild(parseStatements());

	t.setSymbol('}');
	statement->addChild(MakeChild(TerminalASTNode(expect(t, true))));

	return ASTChild();
}

ASTChild Parser::parseReturn()
{
	ASTChild ret(new ASTNode("ReturnStatement"));
	// add the return keyword
	ret->addChild(MakeChild(TerminalASTNode(lexer.current())));

	// Optionally, add any expression.
	if (!(lexer.advance().getType() == TOK_SYMBOL && std::get<char>(lexer.current().getValue()) == ';')) {
			ret->addChild(parseExpression());
	}
	return ret;
}

ASTChild Parser::parseIf()
{
	ASTChild statement(new ASTNode("ifStatement"));

	// add keyword
	statement->addChild(MakeChild(TerminalASTNode(lexer.current())));

	// opening paren
	Token t;
	t.setSymbol('(');
	statement->addChild(MakeChild(TerminalASTNode(expect(t, true))));

	// expr
	statement->addChild(parseExpression());

	// close paren
	t.setSymbol(')');
	statement->addChild(MakeChild(TerminalASTNode(expect(t, true))));

	// open brace
	t.setSymbol('{');
	statement->addChild(MakeChild(TerminalASTNode(expect(t, true))));

	// statements
	statement->addChild(parseStatements());

	// close brace
	t.setSymbol('}');
	statement->addChild(MakeChild(TerminalASTNode(expect(t, true))));

	// else ?
	if (lexer.advance().getType() == TOK_KEYWORD && getKeyword(lexer.current().getValue()) == KWD_ELSE) {
		statement->addChild(MakeChild(TerminalASTNode(lexer.current())));

		// open brace
		t.setSymbol('{');
		statement->addChild(MakeChild(TerminalASTNode(expect(t, true))));

		// statements
		statement->addChild(parseStatements());

		// close brace
		t.setSymbol('}');
		statement->addChild(MakeChild(TerminalASTNode(expect(t, true))));
	}

	return statement;
}

ASTChild Parser::parseTerm()
{
	// parse the term
	ASTChild term(new ASTNode("term"));

	Token cur = lexer.current();
	switch (cur.getType()) {
	default:
		lexer.Error("Expected an expression.");
	case TOK_CONST_INT:
	case TOK_CONST_STRING:
		term->addChild(MakeChild(TerminalASTNode(cur)));
		break;

	case TOK_SYMBOL:
		if (isSymbolUnaryOp(std::get<char>(cur.getValue()))) {
			term->addChild(MakeChild(TerminalASTNode(cur)));
		}
		else if (std::get<char>(cur.getValue()) == '(') {
			term->addChild(MakeChild(TerminalASTNode(cur)));
			term->addChild(parseExpression());
			term->addChild(MakeChild(TerminalASTNode(cur)));
		}
		else lexer.Error("Expected a unary operator or parenthesis in expression term.");
		break;

	case TOK_KEYWORD:
		switch (getKeyword(cur.getValue())) {
		default:
			lexer.Error("Expected a constant.");
		case KWD_TRUE:
		case KWD_FALSE:
		case KWD_NULL:
		case KWD_THIS:
			term->addChild(MakeChild(TerminalASTNode(cur)));
		}
		break;

		// Identifiers might have an array operator.
		// Honestly, this aught to be done with some operator precedence parsing.
	case TOK_IDENTIFIER:
		term->addChild(MakeChild(TerminalASTNode(cur)));
		if (lexer.advance().getType() == TOK_SYMBOL &&
			std::get<char>(lexer.current().getValue()) == '[') {
			term->addChild(MakeChild(TerminalASTNode(cur)));
			term->addChild(parseExpression());
			term->addChild(MakeChild(TerminalASTNode(cur)));
		}
	}

	lexer.advance();
	return term;
}

ASTChild Parser::parseExpressionList()
{
	ASTChild list(new ASTNode("expressionList"));

	Token cur = lexer.advance();
	// Close parenthesis. No expressions in this list.
	if (cur.getType() == TOK_SYMBOL && std::get<char>(cur.getValue()) == ')') {
		return list;
	}

	list->addChild(parseExpression());

	while (lexer.current().getType() == TOK_SYMBOL && std::get<char>(lexer.current().getValue()) == ',') {
		list->addChild(MakeChild(TerminalASTNode(lexer.current())));
		list->addChild(parseExpression());
	}

	return list;
}

ASTChild Parser::parseStatements()
{
	ASTChild statements(new ASTNode("statements"));

	Token cur = lexer.current();
	// statements
	while (cur.getType() == TOK_KEYWORD &&
		isKeywordStatement(getKeyword(cur.getValue())))
	{
		switch (getKeyword(cur.getValue())) {
		case KWD_DO:
			statements->addChild(parseDo());
			break;
		case KWD_IF:
			statements->addChild(parseIf());
			break;
		case KWD_LET:
			statements->addChild(parseLet());
			break;
		case KWD_WHILE:
			statements->addChild(parseWhile());
			break;
		case KWD_RETURN:
			statements->addChild(parseReturn());
		}
		cur = lexer.current();
	}

	return statements;
}

ASTChild Parser::parseExpression()
{
	ASTChild exp(new ASTNode("expression"));

	// Here, I'd want to build up a list of terms and operators
	// and use the shunting yard algorithm or something
	// but...
	exp->addChild(parseTerm());

	// parse the op/term pairs
	//lexer.advance();
	while (lexer.current().getType() == TOK_SYMBOL && isSymbolOperator(std::get<char>(lexer.current().getValue()))) {
		exp->addChild(MakeChild(TerminalASTNode(lexer.current())));
		lexer.advance();
		exp->addChild(parseTerm());
	}

	return exp;
}
