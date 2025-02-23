#pragma once
#include <vector>
#include <iostream>
#include "XMLTag.h"
#include "XMLProperty.h"

enum class TokenizerError {
	OK,
	TAG_NOT_STARTED,
	TAG_NOT_ENDED,
	CHAR_NOT_EXPECTED,
	TAG_NOT_CLOSED,
	EXPECTED_QUOTE_MARK,
	UNEXPECTED_TEXT,
	OTHER,
};

class Tokenizer
{
private:
	std::string content;
	size_t pos = 0;
	bool tag_started = false;
	XMLTag* currTag = nullptr;
	std::vector<XMLTag*>* openTags;
	std::vector<XMLTag*>* tags;
public:
	TokenizerError err = TokenizerError::OK;

	Tokenizer(std::string content);
	std::vector<XMLTag*>* Tokenize();
	TokenizerError HandleOpenBracket(); // <
	TokenizerError HandleCloseBracket(); // >
	TokenizerError HandleCloseSlash(); // /
	TokenizerError HandleOpeningTag();	// characters after <
	TokenizerError HandleClosingTag();	// characters after </
	TokenizerError HandlePropertyName(); //characters after tag name
	TokenizerError HandlePropertyValue(std::string propertyName); //characters after property name and ="
	TokenizerError HandleText(); //Characters after <Text>

	size_t GetCursorPosition() { return this->pos; }
};

