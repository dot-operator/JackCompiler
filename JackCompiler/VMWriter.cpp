#include "VMWriter.h"
#include <iostream>

using std::unordered_map;
using std::string;

const unordered_map<Segment, string> VMWriter::segmentNames = {
	{SEG_ARG, "arg "},
	{SEG_CONST, "const "},
	{SEG_LOCAL, "local "},
	{SEG_POINTER, "pointer "},
	{SEG_STATIC, "static "},
	{SEG_TEMP, "temp "},
	{SEG_THAT, "that "},
	{SEG_THIS, "this "},
	{SEG_NONE, ""}
};

const unordered_map<Math, string> VMWriter::mathNames = {
	{MATH_ADD, "add"},
	{MATH_SUB, "sub"},
	{MATH_AND, "and"},
	{MATH_EQ, "eq"},
	{MATH_GT, "gt"},
	{MATH_LT, "lt"},
	{MATH_NEG, "neg"},
	{MATH_NOT, "not"},
	{MATH_OR, "or"}
};

VMWriter::VMWriter()
{
}

VMWriter::~VMWriter()
{
	close();
}

void VMWriter::writeSubroutine(ASTNode * node)
{
	if (node->getChildren().size() < 3) {
		std::cerr << "Parser broke: Need more nodes in the subroutine.\n";
		exit(1);
	}

	// messy; could use a dynamic_cast and do error checking
	TerminalASTNode *subName = static_cast<TerminalASTNode*>(node->getChildren()[2].get());
	
	// Function definition
	writeFunction(className + "." + subName->token.ValueToString(), symbols.getNumSymbols(SYM_VAR));
	writePush(SEG_ARG, 0);
	writePop(SEG_POINTER, 0);

	// Find the node with the statements.
	ASTNode* body = node->getChild("subroutineBody");
	if (!body)
		return;
	ASTNode* statements = body->getChild("statements");

	// Write each statement.
	for (ASTNode* statement = statements->firstChild(); statement; statement = statements->nextChild()) {
		writeStatement(statement);
	}
}

void VMWriter::setClassName(ASTNode * identifier)
{
	auto* id = static_cast<TerminalASTNode*>(identifier);
	if (!id)
		return;

	className = id->token.ValueToString();
}

void VMWriter::writeStatement(ASTNode * node)
{
	// Do, Let, While, If, Return
	if (node->getName() == "letStatement") {
		writeLet(node);
	}
	else if (node->getName() == "doStatement") {
		writeDo(node);
	}
	else if (node->getName() == "ifStatement") {
		writeIf(node);
	}
	else if (node->getName() == "WhileStatement") {

	}
	else if (node->getName() == "ReturnStatement") {
		writeReturn(node);
	}
}

void VMWriter::writeLet(ASTNode * node)
{
	// Push the expression.
	ASTNode* expr = node->getChild("expression");
	if (!expr)
		return;
	writeExpression(expr);


	// Pop onto the variable...

	// Get the identifier as that's what we're assigning to
	// and match it to a symbol in the table
	TerminalASTNode* symNode = static_cast<TerminalASTNode*>(node->getChild("identifier"));
	if (!symNode)
		return;

	Symbol sym = symbols.getByName(symNode->token.ValueToString());
	popSymbol(sym);
}

void VMWriter::writeDo(ASTNode * node)
{
	// Call a function and discard the result.
}

void VMWriter::writeIf(ASTNode * node)
{
	// Write an expression and an if-goto
}

void VMWriter::writeWhile(ASTNode * node)
{
	// an if statement with a goto back to the top in the body
}

void VMWriter::writeExpression(ASTNode * expr)
{
	// add every term and operator
	for (ASTNode *c = expr->firstChild(); c; c = expr->nextChild()) {
		if (c->getName() == "term") {
			writeTerm(c);
		}
		else if (c->getName() == "symbol") {
			TerminalASTNode* s = static_cast<TerminalASTNode*>(c);
			if (!c)
				return;

			switch (s->token.ValueToString()[0]) {
			case '+':
				writeArithetic(MATH_ADD);
				break;
			case '-':
				writeArithetic(MATH_SUB);
				break;
			case '*':
				writeCall("multiply", 2);
				break;
			case '/':
				writeCall("divide", 2);
				break;
			case '&':
				writeArithetic(MATH_AND);
				break;
			case '|':
				writeArithetic(MATH_OR);
				break;
			case '=':
				writeArithetic(MATH_EQ);
				break;
			case '<':
				writeArithetic(MATH_LT);
				break;
			case '>':
				writeArithetic(MATH_GT);
				break;
			case '!':
			case '~':
				writeArithetic(MATH_NOT);
				break;
			}
		}
	}
}

void VMWriter::writePush(Segment segment, int index)
{
	file << "push " << segmentNames.at(segment) << std::to_string(index) << '\n';
}

void VMWriter::writePop(Segment segment, int index)
{
	file << "pop " << segmentNames.at(segment) << std::to_string(index) << '\n';
}

void VMWriter::writeArithetic(Math type)
{
	file << mathNames.at(type) << '\n';
}

void VMWriter::writeLabel(const std::string & label)
{
	file << "label " << label << "\n";
}

void VMWriter::writeGoto(const std::string & label)
{
	file << "goto " << label << "\n";
}

void VMWriter::writeIf(const std::string & label)
{
	file << "if-goto " << label << "\n";
}

void VMWriter::writeCall(const std::string & name, unsigned nArgs)
{
	file << "call " << name << ' ' << std::to_string(nArgs) << '\n';
}

void VMWriter::writeFunction(const std::string & name, unsigned nLocals)
{
	file << "function " << name << ' ' << std::to_string(nLocals) << '\n';
}

void VMWriter::writeReturn(ASTNode* node)
{
	// If there's an expression, push it
	ASTNode* expr = node->getChild("expression");
	if (!expr)
		return;
	writeExpression(expr);

	file << "return\n";
}

void VMWriter::pushSymbol(Symbol sym)
{
	switch (sym.kind) {
	default:
	case SYM_UNDEFINED:
		std::cerr << "VM Writer Error: Variable " << sym.name << " is undefined.\n";
		exit(1);
	case SYM_ARG:
		writePush(SEG_ARG, sym.index);
		break;
	case SYM_FIELD:
		writePush(SEG_THIS, sym.index);
		break;
	case SYM_STATIC:
		writePush(SEG_STATIC, sym.index);
		break;
	case SYM_VAR:
		writePush(SEG_NONE, sym.index);
		break;
	}
}

void VMWriter::popSymbol(Symbol sym)
{
	switch (sym.kind) {
	default:
	case SYM_UNDEFINED:
		std::cerr << "VM Writer Error: Variable " << sym.name << " is undefined.\n";
		exit(1);
	case SYM_ARG:
		writePop(SEG_ARG, sym.index);
		break;
	case SYM_FIELD:
		writePop(SEG_THIS, sym.index);
		break;
	case SYM_STATIC:
		writePop(SEG_STATIC, sym.index);
		break;
	case SYM_VAR:
		writePop(SEG_NONE, sym.index);
		break;
	}
}

void VMWriter::writeTerm(ASTNode * node)
{
	TerminalASTNode* t = static_cast<TerminalASTNode*>(node->firstChild());
	if (!t)
		return;

	string name = t->token.ValueToString();
	if (t->getName() == "keyword") {
		if (keywords.find(name) != keywords.end()) {
			switch (keywords.at(name)) {
			default:
			case KWD_THIS:
				writePush(SEG_POINTER, 0);
				break;
			case KWD_TRUE:
				writePush(SEG_CONST, 0);
				writeArithetic(MATH_NOT);
				break;
			case KWD_FALSE:
				writePush(SEG_CONST, 0);
			}
		}
		return;
	}
	else if (t->getName() == "integerConstant") {
		writePush(SEG_CONST, std::stoi(name));
		return;
	}
	else if(t->getName() == "identifier"){
		Symbol sym = symbols.getByName(name);
		pushSymbol(sym);
	}
}

void VMWriter::open(const std::string & outFile)
{
	if (file.is_open()) {
		file.close();
	}
	file.open(outFile);
}

void VMWriter::close()
{
	file.close();
}
