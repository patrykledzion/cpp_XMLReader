#pragma once
#include <iostream>

enum class TokenType {
	OPENING_BRACKET,
	CLOSING_BRACKET,
	TAG_NAME,
	PROPERTY_NAME,
	PROPERTY_VALUE,
	TEXT,
	CLOSING_SLASH

};

class Token
{
public:
	TokenType type;
	std::string value;
	Token(TokenType type, std::string value) : type(type), value(std::move(value)) {}
};

