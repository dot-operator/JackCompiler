#pragma once
#include <vector>
#include <memory>

class ASTNode;
typedef std::unique_ptr<ASTNode> ASTChild;

class ASTNode
{
protected:
	std::vector<ASTChild> children;
	std::string expressionName;
public:
	//void addChild(ASTNode* pChild);
	void addChild(ASTChild child);
	virtual std::string prettyPrint(unsigned tabDepth = 0);

	ASTNode(const std::string &name);

	ASTNode(const ASTNode &node) = delete;
	ASTNode(ASTNode&& node) = default;
	ASTNode() {};
	~ASTNode();
};

