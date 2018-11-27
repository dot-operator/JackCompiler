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

ASTNode * ASTNode::firstChild()
{
	childIt = children.begin();
	return childIt->get();
}

ASTNode * ASTNode::nextChild()
{
	if (hasMoreChildren()) {
		return (childIt++)->get();
	}
	return nullptr;
}

ASTNode * ASTNode::getChild(const std::string & name)
{
	for (auto& child : children) {
		if (child->getName() == name)
			return child.get();
	}
	return nullptr;
}

ASTNode::ASTNode(const std::string & name)
{
	expressionName = name;
	childIt = children.begin();
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
