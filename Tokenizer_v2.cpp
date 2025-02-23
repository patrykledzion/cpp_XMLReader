#include "Tokenizer_v2.h"


namespace nXMLReader {
	Tokenizer2::Tokenizer2(std::string content)
	{
		this->content = content;
		this->tokens = new std::vector<Token>();
	}

	std::vector<Token>* Tokenizer2::Tokenize()
	{
		if (this->tokens == nullptr)
		{
			this->err = TokenizerError::OTHER;
			return nullptr;
		}

		TokenizerError ret = TokenizerError::OK;

		while (this->pos < this->content.length() && ret != TokenizerError::OK_EOF)
		{
			while (isspace(this->content.at(this->pos)))
			{
				if (this->pos >= this->content.length() - 1)
				{
					ret = TokenizerError::OK_EOF;
					break;
				}
				this->pos++;
			}

			if (ret == TokenizerError::OK_EOF)break;
			
			if (this->content.at(this->pos) == '<')
			{
				ret = this->HandleOpeningBracket();
			}
			else {
				ret = this->HandleText();
			}

			if (ret != TokenizerError::OK)
			{
				int x = 2137;
			}
		}
		this->err = ret;
		return ret==TokenizerError::OK || ret==TokenizerError::OK_EOF ? this->tokens : nullptr;
	}

	TokenizerError Tokenizer2::HandleOpeningBracket()
	{
		this->tokens->push_back(Token(TokenType::OPENING_BRACKET, "<"));

		//handle spaces
		while (isspace(this->content.at(++this->pos)));

		if (XMLTag::isTagNameStartCharacter(this->content.at(this->pos)))
		{
			return this->HandleTagName();
		}
		else if (this->content.at(this->pos) == '/')
		{
			return this->HandleCloseSlash();
		}
		else {
			return TokenizerError::CHAR_NOT_EXPECTED;
		}

		return TokenizerError::OK;
	}

	TokenizerError Tokenizer2::HandleTagName()
	{
		std::string builder;

		while (XMLTag::isTagNameCharacter(this->content.at(this->pos)))
		{
			builder += this->content.at(this->pos++);
		}

		this->tokens->push_back(Token(TokenType::TAG_NAME, std::move(builder)));

		//handle spaces
		while (isspace(this->content.at(this->pos)))this->pos++;

		if (this->content.at(this->pos) == '>')
		{
			return this->HandleClosingBracket();
		}
		else if (this->content.at(this->pos) == '/')
		{
			return this->HandleCloseSlash();
		}
		else if (XMLProperty::isPropertyNameStartCharacter(this->content.at(this->pos)))
		{
			return this->HandlePropertyName();
		}
		else {
			return TokenizerError::CHAR_NOT_EXPECTED;
		}

		return TokenizerError::OK;
	}

	TokenizerError Tokenizer2::HandleClosingBracket()
	{
		this->tokens->push_back(Token(TokenType::CLOSING_BRACKET, ">"));
		if (this->IsLastChar(this->pos) || this->IsLastChar(this->pos + 1))
		{
			return TokenizerError::OK_EOF;
		}
		//Handle Spaces
		while (isspace(this->content.at(++this->pos)))
		{
			if (this->IsLastChar(this->pos))return TokenizerError::OK;
		};

		return TokenizerError::OK;
	}

	TokenizerError Tokenizer2::HandlePropertyName()
	{
		std::string builder;
		while (XMLProperty::isPropertyNameCharacter(this->content.at(this->pos)))
		{
			builder += this->content.at(this->pos++);
		}

		this->tokens->push_back(Token(TokenType::PROPERTY_NAME, std::move(builder)));

		char c = this->content.at(this->pos);
		int x = 2137;
		//handle spaces
		while (isspace(this->content.at(this->pos)))this->pos++;

		c = this->content.at(this->pos);
		x = 2137;

		if (this->content.at(this->pos) == '>')
		{
			return this->HandleClosingBracket();
		}
		else if (this->content.at(this->pos) == '=')
		{
			return this->HandleEqualSign();
		}
		else if (this->content.at(this->pos) == '/')
		{
			return this->HandleCloseSlash();
		}
		else if (XMLProperty::isPropertyNameStartCharacter(this->content.at(this->pos)))
		{
			return this->HandlePropertyName();
		}
		else {
			return TokenizerError::CHAR_NOT_EXPECTED;
		}


		return TokenizerError::OK;
	}

	TokenizerError Tokenizer2::HandleEqualSign()
	{
		//this->tokens->push_back(Token(TokenType::EQUAL_SIGN, "="));

		//Handle spaces
		while (isspace(this->content.at(++this->pos)));

		if (this->content.at(this->pos) == '\"')
		{
			return this->HandleQuotationMark();
		}
		else {
			return TokenizerError::EXPECTED_QUOTE_MARK;
		}

		return TokenizerError::OK;
	}

	TokenizerError Tokenizer2::HandleQuotationMark()
	{
		//this->tokens->push_back(Token(TokenType::QUOTATION_MARK, "\""));
		return this->HandlePropertyValue();
	}

	TokenizerError Tokenizer2::HandlePropertyValue()
	{
		std::string builder;
		this->pos++;
		while (this->content.at(this->pos) != '\"')
		{
			builder += this->content.at(this->pos++);
		}

		this->tokens->push_back(Token(TokenType::PROPERTY_VALUE, std::move(builder)));

		//Handle spaces
		while (isspace(this->content.at(++this->pos)));

		if (this->content.at(this->pos) == '>')
		{
			return this->HandleClosingBracket();
		}
		else if (this->content.at(this->pos) == '/')
		{
			return this->HandleCloseSlash();
		}
		else if (XMLProperty::isPropertyNameStartCharacter(this->content.at(this->pos)))
		{
			return this->HandlePropertyName();
		}
		else {
			return TokenizerError::CHAR_NOT_EXPECTED;
		}


		return TokenizerError::OK;
	}

	TokenizerError Tokenizer2::HandleText()
	{
		std::string builder;

		while (this->content.at(this->pos) != '<')
		{
			builder += this->content.at(this->pos++);
		}

		this->tokens->push_back(Token(TokenType::TEXT, std::move(builder)));

		return TokenizerError::OK;
	}

	TokenizerError Tokenizer2::HandleCloseSlash()
	{
		this->tokens->push_back(Token(TokenType::CLOSING_SLASH, "/"));

		//handle spaces
		while (isspace(this->content.at(++this->pos)));

		if (XMLTag::isTagNameStartCharacter(this->content.at(this->pos)))
		{
			return this->HandleTagName();
		}
		else if (this->content.at(this->pos) == '>')
		{
			return this->HandleClosingBracket();
		}
		else {
			return TokenizerError::CHAR_NOT_EXPECTED;
		}

		char c = this->content.at(this->pos);
		int x = 69;

		return TokenizerError::OK;
	}

	bool Tokenizer2::IsLastChar(int pos)
	{
		return pos == this->content.length() - 1;
	}
}
