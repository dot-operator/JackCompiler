#include "SymbolTable.h"



unsigned SymbolTable::varCount(SymbolKind type)
{
	switch (type) {
	default:
	case SYM_STATIC:
		return staticSymbols.size();
	case SYM_FIELD:
		return fieldSymbols.size();
	case SYM_ARG:
		return argSymbols.size();
	case SYM_VAR:
		return varSymbols.size();
	}
}

SymbolTable::SymbolTable()
{
	// Should always have `this` in the symbol table
	// at arg 0
	// kind of a weird way to do it
	Symbol symThis;
	symThis.kind = SYM_ARG;
	symThis.name = "this";
	argSymbols.push_back(symThis);
}

SymbolTable::~SymbolTable()
{
}

void SymbolTable::startSubroutine()
{
	argSymbols.clear();
	varSymbols.clear();
}

bool SymbolTable::Define(Symbol sym)
{
	/*if (getByName(sym.name).kind != SYM_UNDEFINED)
		return false;*/

	switch (sym.kind) {
	default:
	case SYM_STATIC:
		staticSymbols.push_back(sym);
		staticSymbols.back().index = staticSymbols.size() - 1;
		break;
	case SYM_FIELD:
		fieldSymbols.push_back(sym);
		fieldSymbols.back().index = fieldSymbols.size() - 1;
		break;
	case SYM_ARG:
		argSymbols.push_back(sym);
		argSymbols.back().index = argSymbols.size() - 1;
		break;
	case SYM_VAR:
		varSymbols.push_back(sym);
		varSymbols.back().index = varSymbols.size() - 1;
		break;
	}
	return true;
}

unsigned SymbolTable::getNumSymbols(SymbolKind kind)
{
	switch (kind) {
	default:
	case SYM_ARG:
		return argSymbols.size();
	case SYM_VAR:
		return varSymbols.size();
	case SYM_STATIC:
		return staticSymbols.size();
	case SYM_FIELD:
		return fieldSymbols.size();
	}
}

Symbol SymbolTable::getByName(const std::string & name)
{
	// Check to see if it exists in each 
	// start with the most recently-declared first
	// so that local variables override class ones
	// so: Var, Arg, and then class-scope variables
	for (auto& sym : varSymbols) {
		if (sym.name == name)
			return sym;
	}
	for (auto& sym : argSymbols) {
		if (sym.name == name)
			return sym;
	}

	for (auto& sym : staticSymbols) {
		if (sym.name == name)
			return sym;
	}

	for (auto& sym : fieldSymbols) {
		if (sym.name == name)
			return sym;
	}

	// Must be undefined.
	return Symbol();
}

unsigned SymbolTable::indexOf(const std::string & name)
{
	// Check to see if it exists in each 
	for (int i = 0; i < staticSymbols.size(); ++i) {
		if (staticSymbols[i].name == name)
			return i;
	}

	for (int i = 0; i < fieldSymbols.size(); ++i) {
		if (fieldSymbols[i].name == name)
			return i;
	}

	for (int i = 0; i < argSymbols.size(); ++i) {
		if (argSymbols[i].name == name)
			return i;
	}

	for (int i = 0; i < varSymbols.size(); ++i) {
		if (varSymbols[i].name == name)
			return i;
	}

	// Must be undefined.
	return 0;
}
