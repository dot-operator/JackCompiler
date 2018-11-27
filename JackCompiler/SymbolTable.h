#pragma once
#include "Symbol.h"
#include <vector>

class SymbolTable
{
private:
	unsigned varCount(SymbolKind type);

	// Class-scope
	std::vector<Symbol> staticSymbols;
	std::vector<Symbol> fieldSymbols;

	// Method-scope
	std::vector<Symbol> argSymbols;
	std::vector<Symbol> varSymbols;
public:
	SymbolTable();
	~SymbolTable();

	void startSubroutine();
	bool Define(Symbol sym);

	unsigned getNumSymbols(SymbolKind kind);
	
	// Replaces kindof/typeof
	Symbol getByName(const std::string& name);
	unsigned indexOf(const std::string& name);
};

