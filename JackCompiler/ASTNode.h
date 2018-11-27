#pragma once
#include <vector>
#include <memory>

class ASTNode;
typedef std::unique_ptr<ASTNode> ASTChild;

class ASTNode
{
protected:
	std::vector<ASTChild>::iterator childIt;

	std::vector<ASTChild> children;
	std::string expressionName;
public:
	//void addChild(ASTNode* pChild);
	void addChild(ASTChild child);
	virtual std::string prettyPrint(unsigned tabDepth = 0);

	inline const std::string& getName() {
		return expressionName;
	}

	// iterating
	ASTNode* firstChild();
	ASTNode* nextChild();
	inline bool hasMoreChildren() {
		return childIt != children.end();
	}
	// ugly quick fix
	const inline std::vector<ASTChild>& getChildren() {
		return children;
	};

	// We just want to grab a specific child node
	ASTNode* getChild(const std::string& name);


	ASTNode(const std::string &name);

	ASTNode(const ASTNode &node) = delete;
	ASTNode(ASTNode&& node) = default;
	ASTNode() {};
	~ASTNode();
};

