#include "TerminalASTNode.h"

//void TerminalASTNode::addChild(Token child)
//{
//	children.push_back(ASTChild(new TerminalASTNode(child)));
//}

std::string TerminalASTNode::prettyPrint(unsigned tabDepth)
{
	std::string output;
	for (unsigned i = 0; i < tabDepth; ++i) {
		output += "\t";
	}

	output += "<" + expressionName + ">" + token.ValueToString() + "</" + expressionName + ">\n";
	return output;
}

TerminalASTNode::TerminalASTNode(Token tok, const std::string& name)
{
	expressionName = name;
	token = tok;
}

TerminalASTNode::TerminalASTNode(Token tok)
{
	token = tok;
	expressionName = TokenToString(tok.getType());
}

//TerminalASTNode::TerminalASTNode()
//{
//}


TerminalASTNode::~TerminalASTNode()
{
}
