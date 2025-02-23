#include "Tokenizer_v2.h"


namespace nXMLReader {

	std::map<TokenizerError, std::string> Tokenizer2::str_err = {
		{TokenizerError::OK, "OK"},
		{TokenizerError::TAG_NOT_STARTED, "TAG_NOT_STARTED"},
		{TokenizerError::TAG_NOT_ENDED, "TAG_NOT_ENDED"},
		{TokenizerError::CHAR_NOT_EXPECTED, "CHAR_NOT_EXPECTED"},
		{TokenizerError::TAG_NOT_CLOSED, "TAG_NOT_CLOSED"},
		{TokenizerError::EXPECTED_QUOTE_MARK, "EXPECTED_QUOTE_MARK"},
		{TokenizerError::UNEXPECTED_TEXT, "UNEXPECTED_TEXT"},
		{TokenizerError::OTHER, "OTHER"},
		{TokenizerError::OK_EOF, "OK_EOF"},
		{TokenizerError::ERR_EOF, "ERR_EOF"},
	};

	TokenizerError Tokenizer2::PrintError(TokenizerError err)
	{
		this->err = err;
		int line = 0;
		int pos_in_line = 0;

		for (int i = 0; i < this->pos; i++)
		{
			pos_in_line++;
			if (isspace(this->content.at(i)) && pos_in_line == 1)pos_in_line = 0;
			if (this->content.at(i) == '\n')
			{
				line++;
				pos_in_line = 0;
			}
		}

		line++;
		pos_in_line++;
		std::cout << "Tokenizer error: " << Tokenizer2::str_err[err] << " in line " << line << " at pos " << pos_in_line << std::endl;
	
		return err;
	}

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

			if (ret != TokenizerError::OK && ret!= TokenizerError::OK_EOF)
			{
				this->PrintError(ret);
				return nullptr;
			}
		}
		this->err = ret;
		return ret==TokenizerError::OK || ret==TokenizerError::OK_EOF ? this->tokens : nullptr;
	}

	TokenizerError Tokenizer2::HandleOpeningBracket()
	{
		this->tokens->push_back(Token(TokenType::OPENING_BRACKET, "<"));

		//handle spaces
		while (isspace(this->content.at(++this->pos))) 
		{
			if (this->pos >= this->content.length() - 1)
			{
				return TokenizerError::ERR_EOF;
			}
		}


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
			if (this->pos >= this->content.length() - 1)return TokenizerError::ERR_EOF;
			builder += this->content.at(this->pos++);
		}

		this->tokens->push_back(Token(TokenType::TAG_NAME, std::move(builder)));

		//handle spaces
		while (isspace(this->content.at(this->pos)))
		{
			if (this->pos >= this->content.length() - 1)return TokenizerError::ERR_EOF;
			this->pos++;
		}

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
			if (this->pos >= this->content.length() - 1)return TokenizerError::ERR_EOF;
			builder += this->content.at(this->pos++);
		}

		this->tokens->push_back(Token(TokenType::PROPERTY_NAME, std::move(builder)));

		char c = this->content.at(this->pos);
		int x = 2137;
		//handle spaces
		while (isspace(this->content.at(this->pos)))
		{
			if (this->pos >= this->content.length() - 1)return TokenizerError::ERR_EOF;

			this->pos++;
		}
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
		do {
			if (this->pos >= this->content.length() - 1)return TokenizerError::ERR_EOF;
		} while (isspace(this->content.at(++this->pos)));


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
			if (this->pos >= this->content.length() - 1)return TokenizerError::ERR_EOF;
			builder += this->content.at(this->pos++);
		}

		this->tokens->push_back(Token(TokenType::PROPERTY_VALUE, std::move(builder)));

		//Handle spaces
		do {
			if (this->pos >= this->content.length() - 1)return TokenizerError::ERR_EOF;
		} while (isspace(this->content.at(++this->pos)));
		

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
			if (this->pos >= this->content.length() - 1)return TokenizerError::ERR_EOF;
			builder += this->content.at(this->pos++);
		}

		this->tokens->push_back(Token(TokenType::TEXT, std::move(builder)));

		return TokenizerError::OK;
	}

	TokenizerError Tokenizer2::HandleCloseSlash()
	{
		this->tokens->push_back(Token(TokenType::CLOSING_SLASH, "/"));

		//handle spaces
		do {
			if (this->pos >= this->content.length() - 1)return TokenizerError::ERR_EOF;
		}
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
