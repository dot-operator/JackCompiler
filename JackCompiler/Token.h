#pragma once
#include <variant>
#include <string>

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
	TOK_CONST_INT,
	TOK_EOF
};

typedef std::variant<int, char, Keyword, std::string> TokenValue;

class Token
{
private:
	TokenType type;
	TokenValue value;
public:
	Token();
	~Token();
	
	inline void setInt(int val) {
		type = TOK_CONST_INT;
		value = val;
	}
	inline void setString(TokenType newType, const std::string& val) {
		type = newType;
		value = val;
	}
	inline void setSymbol(char val) {
		type = TOK_SYMBOL;
		value = val;
	}
	inline void setKeyword(Keyword kwd) {
		value = kwd;
	}

	inline TokenType getType() {
		return type;
	}
	inline TokenValue getValue() {
		return value;
	}
};

