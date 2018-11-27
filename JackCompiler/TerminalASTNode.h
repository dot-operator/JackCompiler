#pragma once
#include "ASTNode.h"
#include "Token.h"

class TerminalASTNode :
	public ASTNode
{
private:
public:
	Token token;
	//void addChild(Token child);

	std::string prettyPrint(unsigned tabDepth = 0) override;

	TerminalASTNode(Token tok, const std::string &name);
	TerminalASTNode(Token tok);
	TerminalASTNode(const TerminalASTNode& node) = delete;
	TerminalASTNode(TerminalASTNode&& n) = default;
	~TerminalASTNode();
};

