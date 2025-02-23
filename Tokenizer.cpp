#include "Tokenizer.h"

Tokenizer::Tokenizer(std::string content)
{
	this->content = content;
	this->tags = new std::vector<XMLTag*>();
	this->openTags = new std::vector<XMLTag*>();
}

std::vector<XMLTag*>* Tokenizer::Tokenize()
{
	TokenizerError ret;
	for (this->pos = 0; this->pos < this->content.length(); this->pos++)
	{
		const char c = this->content.at(this->pos);
		int cx = 2137;
		if (this->content.at(this->pos) == '<')
		{
			if (this->tag_started)
			{
				this->err = TokenizerError::TAG_NOT_ENDED;
				return nullptr;
			}
			ret = this->HandleOpenBracket();
			if (ret != TokenizerError::OK)
			{
				std::cout << "Error " << (int)ret << " at position " << this->pos << "("<< this->content.at(this->pos) << ")" << std::endl;
				return nullptr;
			}
			else {
				int x = 2137;
				for (int i = 0; i < this->tags->size(); i++)
				{
					this->tags->at(i)->PrintTag(0);
				}
			}

		}
		else
		{
		}
	}

	

	return this->tags;
}

TokenizerError Tokenizer::HandleOpenBracket()
{
	//After opening bracket, there must be tag name
	//Handle whitespaces
	while (isspace(this->content.at(++this->pos)));

	//Checking first character
	if (isalpha(this->content.at(this->pos)))
	{
		return this->HandleOpeningTag();
	}
	else if (this->content.at(this->pos) == '/')
	{
		return this->HandleClosingTag();
	}
	else {
		return TokenizerError::CHAR_NOT_EXPECTED;
	}

	return TokenizerError::OK;
}

TokenizerError Tokenizer::HandleCloseBracket()
{
	return TokenizerError::OK;
}

TokenizerError Tokenizer::HandleCloseSlash()
{
	return TokenizerError::OK;
}

TokenizerError Tokenizer::HandleOpeningTag()
{
	std::string name;
	while (isalpha(this->content.at(this->pos)))
	{
		name += this->content.at(this->pos++);
	}

	this->currTag = new XMLTag(name);

	//Handle spaces
	while (isspace(this->content.at(this->pos)))this->pos++;

	char c = this->content.at(this->pos);
	int x = 5;
	if (isalpha(this->content.at(this->pos)))
	{
		TokenizerError ret = this->HandlePropertyName();
		if (ret != TokenizerError::OK)
		{
			return ret;
		}
	}

	if (this->content.at(this->pos)  == '/')
	{
		if (this->content.at(this->pos + 1) == '>')
		{
			XMLTag* lastTag = this->currTag;
			if (this->openTags->size() > 0)
			{
				//Add to parent
				this->openTags->at(this->openTags->size() - 1)->AddChild(*lastTag); 
			}
			else {
				this->tags->push_back(lastTag); 
			}
			
			this->currTag = nullptr;
			return TokenizerError::OK;
		}
		else {
			return TokenizerError::CHAR_NOT_EXPECTED;
		}
	}
	else if (this->content.at(this->pos) == '>')
	{
		this->openTags->push_back(this->currTag);
		this->currTag = nullptr;

		//Handle spaces
		//while (isspace(this->content.at(++this->pos)));
		//this->pos--;
		char ctext = this->content.at(this->pos);
		std::cout << name << std::endl;
		//if (ctext != '<' && ctext != '>')return this->HandleText();

		return TokenizerError::OK;
	}
	
	else {
		c = this->content.at(this->pos);
		return TokenizerError::CHAR_NOT_EXPECTED;
	}

	return TokenizerError::OK;
}

TokenizerError Tokenizer::HandleClosingTag()
{
	this->pos++;
	//Handle spaces
	while (isspace(this->content.at(this->pos)))this->pos++;

	std::string name;
	while (isalpha(this->content.at(this->pos)))
	{
		name += this->content.at(this->pos++);
	}

	//Handle spaces
	while (isspace(this->content.at(this->pos)))this->pos++;

	const char c = this->content.at(this->pos);
	if (c == '>')
	{
		//Find opened tag (should be last one)
		XMLTag* lastTag = this->openTags->at(this->openTags->size() - 1);
		bool test = lastTag->GetTagName() == name;
		int x = 12;
		if (lastTag->GetTagName() == name)
		{
			if (this->openTags->size() > 1)
			{
				//Add to parent
				this->openTags->at(this->openTags->size() - 2)->AddChild(*lastTag);
				this->openTags->pop_back();
			}
			else {
				this->tags->push_back(lastTag);
				this->openTags->pop_back();
			}
			int y = 17;
			return TokenizerError::OK;
		}
		else {
			return TokenizerError::TAG_NOT_CLOSED;
		}

	}
	else {
		return TokenizerError::CHAR_NOT_EXPECTED;
	}
	int x = 4;

	return TokenizerError::OK;
}


TokenizerError Tokenizer::HandlePropertyName()
{
	std::string name;
	while (isalpha(this->content.at(this->pos)))
	{
		name += this->content.at(this->pos++);
	}

	//Handle spaces
	while (isspace(this->content.at(this->pos)))this->pos++;

	char c = this->content.at(this->pos);
	int x = 2137;

	if (this->content.at(this->pos) == '=')
	{
		this->HandlePropertyValue(name);
		return TokenizerError::OK;
	}
	if (isalpha(this->content.at(this->pos)))
	{
		this->currTag->SetProperty(name, "true");
		this->HandlePropertyName();
		return TokenizerError::OK;
	}
	
	this->currTag->SetProperty(name, "true");
	return TokenizerError::OK;
}

TokenizerError Tokenizer::HandlePropertyValue(std::string propertyName)
{
	//Handle spaces
	while (isspace(this->content.at(++this->pos)));

	if (this->content.at(this->pos) == '\"')
	{
		this->pos++;
		std::string value;
		while (this->content.at(this->pos) != '\"')
		{
			value += this->content.at(this->pos++);
		}
		this->currTag->SetProperty(propertyName, value);

		//Handle spaces
		while (isspace(this->content.at(++this->pos)));

		if (isalpha(this->content.at(this->pos)))return this->HandlePropertyName();

		return TokenizerError::OK;
	}
	else {
		return TokenizerError::EXPECTED_QUOTE_MARK;
	}

	char c = this->content.at(this->pos);
	int x = 2137;

	return TokenizerError::OK;
}

TokenizerError Tokenizer::HandleText()
{
	if (this->openTags->size() > 0 && (this->openTags->at(this->openTags->size() - 1)->GetTagName() != "Text" && !isspace(this->content.at(this->pos))))return TokenizerError::UNEXPECTED_TEXT;
	this->pos++;
	std::cout << this->content.at(this->pos);
	char c = this->content.at(this->pos);
	while (this->content.at(this->pos) != '<' && this->content.at(this->pos) != '>')
	{
		//this->pos++;
		std::cout << this->content.at(this->pos++);
	}
	this->pos--;
	c = this->content.at(this->pos);

	return TokenizerError::OK;
}
