#pragma once
#include <string>

enum SymbolKind {
	// ugly way of saying there's no existing sym by that name
	// for SymbolTable::getByName
	SYM_UNDEFINED,

	// 
	SYM_STATIC,
	SYM_FIELD,
	SYM_ARG,
	SYM_VAR,
};

struct Symbol
{
	Symbol() {
		kind = SYM_UNDEFINED;
	};
	Symbol(SymbolKind k, const std::string& t, const std::string& n) : kind(k), type(t), name(n) {};
	~Symbol() {};

	SymbolKind kind;
	std::string type;
	std::string name;
	unsigned index;
};

