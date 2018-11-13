#include "ASTNode.h"


void ASTNode::addChild(ASTChild child)
{
	children.push_back(std::move(child));
}

std::string ASTNode::prettyPrint(unsigned tabDepth)
{
	std::string output;
	for (unsigned i = 0; i < tabDepth; ++i) {
		output += "\t";
	}

	output += "<" + expressionName + ">\n";
	for (auto& child : children) {
		output += child->prettyPrint(tabDepth + 1);
	}
	
	for (unsigned i = 0; i < tabDepth; ++i) {
		output += "\t";
	}

	output += "</" + expressionName + ">\n";
	return output;
}

ASTNode::ASTNode(const std::string & name)
{
	expressionName = name;
}

//ASTNode::ASTNode(const ASTNode & node) : children(std::move(node.children)), expressionName(node.expressionName)
//{
//	
//}
//
//ASTNode::ASTNode(ASTNode && node)
//{
//	expressionName = node.expressionName;
//	children = std::move(node.children);
//}

ASTNode::~ASTNode()
{
}
