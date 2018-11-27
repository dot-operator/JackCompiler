#pragma once
#include "Segment.h"
#include <string>
#include <fstream>
#include <unordered_map>
#include "SymbolTable.h"
#include "TerminalASTNode.h"

class VMWriter
{
private:
	std::ofstream file;
	const static std::unordered_map<Math, std::string> mathNames;
	const static std::unordered_map<Segment, std::string> segmentNames;
	std::string className;

	void writePush(Segment segment, int index);
	void writePop(Segment segment, int index);

	void writeArithetic(Math type);

	void writeLabel(const std::string& label);
	void writeGoto(const std::string& label);
	void writeIf(const std::string& label);

	void writeCall(const std::string& name, unsigned nArgs);
	void writeFunction(const std::string& name, unsigned nLocals);

	void writeReturn(ASTNode* node);

	void pushSymbol(Symbol sym);
	void popSymbol(Symbol sym);


	// A better way of doing this
	// would be to have more subclasses of ASTNode
	// that each override a WriteVM method.
	void writeTerm(ASTNode* node);
	void writeStatement(ASTNode* node);
	void writeLet(ASTNode* node);
	void writeDo(ASTNode* node);
	void writeIf(ASTNode* node);
	void writeWhile(ASTNode* node);
	void writeExpression(ASTNode* expr);

public:
	VMWriter();
	~VMWriter();

	SymbolTable symbols;

	// These write methods are a little bit weird
	// because of how the tree was built
	// in the last project.
	void writeSubroutine(ASTNode* node);

	void setClassName(ASTNode* identifier);

	void open(const std::string& outFile);
	void close();
};

