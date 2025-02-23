#pragma once
#include <vector>
#include <iostream>
#include "XMLTag.h"
#include "XMLProperty.h"
#include "Token.h"

namespace nXMLReader {

enum class TokenizerError {
	OK,
	TAG_NOT_STARTED,
	TAG_NOT_ENDED,
	CHAR_NOT_EXPECTED,
	TAG_NOT_CLOSED,
	EXPECTED_QUOTE_MARK,
	UNEXPECTED_TEXT,
	OTHER,
	OK_EOF
};

class Tokenizer2
{
private:
	std::string content;
	size_t pos = 0;
	std::vector<Token>* tokens;
	
public:
	TokenizerError err = TokenizerError::OK;

	Tokenizer2(std::string content);
	std::vector<Token>* Tokenize();
	TokenizerError HandleOpeningBracket();		// <
	TokenizerError HandleTagName();
	TokenizerError HandleClosingBracket();		// >
	TokenizerError HandlePropertyName();		// Property name
	TokenizerError HandleEqualSign();			// =
	TokenizerError HandleQuotationMark();		// "
	TokenizerError HandlePropertyValue();		// Property value
	TokenizerError HandleText();				// Text
	TokenizerError HandleCloseSlash();			// /
	
	bool IsLastChar(int pos); 

	size_t GetCursorPosition() { return this->pos; }
};
}