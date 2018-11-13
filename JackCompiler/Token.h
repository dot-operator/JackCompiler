#pragma once
#include <variant>
#include <string>
#include <map>

enum Keyword {
	KWD_CLASS,
	KWD_METHOD,
	KWD_FUNCTION,
	KWD_CONSTRUCTOR,
	KWD_INT,
	KWD_BOOL,
	KWD_CHAR,
	KWD_VOID,
	KWD_VAR,
	KWD_STATIC,
	KWD_FIELD,
	KWD_LET,
	KWD_DO,
	KWD_IF,
	KWD_ELSE,
	KWD_WHILE,
	KWD_RETURN,
	KWD_TRUE,
	KWD_FALSE,
	KWD_NULL,
	KWD_THIS
};

enum TokenType {
	TOK_KEYWORD,
	TOK_SYMBOL,
	TOK_IDENTIFIER,
	TOK_CONST_INT,
	TOK_CONST_STRING,
	TOK_EOF
};

static std::string TokenToString(TokenType type) {
	switch (type) {
	default:
	case TOK_EOF:
		return "EOF";
	case TOK_IDENTIFIER:
		return "identifier";
	case TOK_KEYWORD:
		return "keyword";
	case TOK_SYMBOL:
		return "symbol";
	case TOK_CONST_INT:
		return "integer constant";
	case TOK_CONST_STRING:
		return "string constant";
	}
};

const std::map<std::string, Keyword> keywords = {
		{"class", KWD_CLASS},
		{"constructor", KWD_CONSTRUCTOR},
		{"function", KWD_FUNCTION},
		{"method", KWD_METHOD},
		{"field", KWD_FIELD},
		{"static", KWD_STATIC},
		{"var", KWD_VAR},
		{"int", KWD_INT},
		{"char", KWD_CHAR},
		{"boolean", KWD_BOOL},
		{"bool", KWD_BOOL}, // !! - idk why but "boolean" annoys me so here's an alternative
		{"void", KWD_VOID},
		{"true", KWD_TRUE},
		{"false", KWD_FALSE},
		{"null", KWD_NULL},
		{"this", KWD_THIS},
		{"let", KWD_LET},
		{"do", KWD_DO},
		{"if", KWD_IF},
		{"else", KWD_ELSE},
		{"while", KWD_WHILE},
		{"return", KWD_RETURN}
};

typedef std::variant<unsigned, char, Keyword, std::string> TokenValue;

class Token
{
private:
	TokenType type;
	TokenValue value;
public:
	Token() {
		type = TOK_EOF;
	}
	~Token() {}
	
	inline void setInt(unsigned val) {
		type = TOK_CONST_INT;
		value = val;
	}
	inline void setString(const std::string& val) {
		type = TOK_CONST_STRING;
		value = val;
	}
	inline void setSymbol(char val) {
		type = TOK_SYMBOL;
		value = val;
	}
	inline void setKeyword(Keyword kwd) {
		type = TOK_KEYWORD;
		value = kwd;
	}
	inline void setIdentifier(const std::string& val) {
		type = TOK_IDENTIFIER;
		value = val;
	}

	inline TokenType getType() {
		return type;
	}
	inline TokenValue getValue() {
		return value;
	}

	inline std::string ValueToString() {
		switch (type) {
		default:
			return "";
		case TOK_CONST_INT:
			return std::to_string(std::get<unsigned>(value));
		case TOK_CONST_STRING:
			return std::get<std::string>(value);
		case TOK_SYMBOL:
			return std::string() + std::get<char>(value);
		case TOK_KEYWORD:
			for (auto& entry : keywords) {
				if (entry.second == std::get<Keyword>(value)) {
					return entry.first;
				}
			}
			return "error-keyword";
		case TOK_IDENTIFIER:
			return std::get<std::string>(value);
		}
	}

};

