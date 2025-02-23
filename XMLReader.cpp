#include "XMLReader.h"

namespace nXMLReader {

	std::map < XMLReaderError, std::string> XMLReader::err_str = {
			{XMLReaderError::OK, "OK"},
			{XMLReaderError::OTHER_PROBLEM, "OTHER_PROBLEM"},
			{XMLReaderError::TAG_NOT_ENDED, "TAG_NOT_ENDED"},
			{XMLReaderError::TAG_NOT_STARTED, "TAG_NOT_STARTED"},
			{XMLReaderError::UNEXPECTED_TOKEN, "UNEXPECTED_TOKEN"},
	};

	XMLReader::XMLReader(std::string filename)
	{
		this->file = new File(std::move(filename));
		this->tags = new std::vector<XMLTag>();
		this->tokens = nullptr;
		this->currTags = new std::vector<XMLTag>();
		this->openedTag = nullptr;
	}

	XMLReaderError XMLReader::Read()
	{
		XMLReaderError ret = this->Tokenize();
		if (this->tokens == nullptr)
			return ret;
		ret = this->InterpretTokens();
		return ret;
	}

	XMLReaderError XMLReader::Tokenize()
	{
		if (this->file == nullptr || this->file->GetSize() < 1)return XMLReaderError::OTHER_PROBLEM;

		Tokenizer2 tokenizer(this->file->GetContent());
		this->tokens = tokenizer.Tokenize();

		if (tokenizer.err != TokenizerError::OK)return XMLReaderError::OTHER_PROBLEM;
		return XMLReaderError::OK;
	}

	XMLReaderError XMLReader::InterpretTokens()
	{
		XMLReaderError ret = XMLReaderError::OK;

		while (this->pos < this->tokens->size())
		{
			if (this->GetCurrentToken() == nullptr)return XMLReaderError::OTHER_PROBLEM;

			switch (this->GetCurrentToken()->type)
			{
			case TokenType::OPENING_BRACKET: 
				ret = this->HandleOpeningBracket();
				break;
			case TokenType::TEXT:
				ret = this->HandleText();
				break;
			default: 
				ret = XMLReaderError::UNEXPECTED_TOKEN;
			}

			if (ret != XMLReaderError::OK)return this->PrintError(ret);
		}

		if (this->currTags->size() > 0 || this->openedTag!=nullptr)return this->PrintError(XMLReaderError::TAG_NOT_ENDED);

		return XMLReaderError::OK;
	}
	XMLReaderError XMLReader::HandleOpeningBracket()
	{
		//handle bracket
		this->bracketOpened = true;
		
		this->pos++;
		if (this->GetCurrentToken() == nullptr)return XMLReaderError::OTHER_PROBLEM;

		switch (this->GetCurrentToken()->type)
		{
		case TokenType::TAG_NAME: 
			return this->HandleTagName();
		case TokenType::CLOSING_SLASH:
			return this->HandleClosingSlash();
		default: 
			return XMLReaderError::UNEXPECTED_TOKEN;
		}

		return XMLReaderError::OK;
	}
	XMLReaderError XMLReader::HandleTagName()
	{
		//handle name
		if (this->tokens->at(this->pos - 1).type == TokenType::OPENING_BRACKET)
		{
			if (!this->bracketOpened || this->openedTag != nullptr)
				return XMLReaderError::UNEXPECTED_TOKEN;
			this->openedTag = new XMLTag(this->GetCurrentToken()->value);

			this->pos++;

			switch (this->GetCurrentToken()->type)
			{
			case TokenType::CLOSING_SLASH:
				return this->HandleClosingSlash();
			case TokenType::CLOSING_BRACKET:
				return this->HandleClosingBracket();
			case TokenType::PROPERTY_NAME:
				return this->HandlePropertyName();
			default:
				return XMLReaderError::UNEXPECTED_TOKEN;
			}
		}
		else if (this->tokens->at(this->pos - 1).type == TokenType::CLOSING_SLASH)
		{
			if (!this->bracketOpened)
				return XMLReaderError::UNEXPECTED_TOKEN;

			//handle name
			if (this->GetCurrentToken()->value != this->GetLastCurrTag()->GetTagName())return XMLReaderError::TAG_NOT_ENDED;
			if (this->currTags->size() > 1)
			{
				XMLTag ct = *GetLastCurrTag();
				this->currTags->at(this->currTags->size() - 2).AddChild(ct);
				this->currTags->pop_back();

			}
			else {
				this->tags->push_back(*this->GetLastCurrTag());
				this->currTags->pop_back();
			}
			
			this->pos++; // ">"
			this->pos++; //Next token
			return XMLReaderError::OK;
		}
		

		return XMLReaderError::OK;
	}

	XMLReaderError XMLReader::HandleClosingSlash()
	{
		//handle slash
		//next: ">"; prev: "?", np: <Img src="dwadada"/>, <br/>
		//next: "tagname"; prev: "<", nextnext: ">"
		this->pos++;
		switch (this->GetCurrentToken()->type)
		{
		case TokenType::CLOSING_BRACKET:
			return this->HandleClosingBracket();
		case TokenType::TAG_NAME:
			return this->HandleTagName();
		default: 
			return XMLReaderError::UNEXPECTED_TOKEN;
		}

		return XMLReaderError::OK;
	}
	XMLReaderError XMLReader::HandlePropertyName()
	{
		if (this->openedTag == nullptr)return XMLReaderError::UNEXPECTED_TOKEN;

		this->openedTag->SetProperty(this->GetCurrentToken()->value, "true");

		this->pos++;

		switch(this->GetCurrentToken()->type)
		{
		case TokenType::PROPERTY_VALUE:
			return this->HandlePropertyValue();
		case TokenType::PROPERTY_NAME:
			return this->HandlePropertyName();
		case TokenType::CLOSING_SLASH:
			return this->HandleClosingSlash();
		case TokenType::CLOSING_BRACKET:
			return this->HandleClosingBracket();
		default:
			return XMLReaderError::UNEXPECTED_TOKEN;
		}
		
		return XMLReaderError::OK;
	}
	XMLReaderError XMLReader::HandlePropertyValue()
	{
		if (this->tokens->at(this->pos - 1).type != TokenType::PROPERTY_NAME)return XMLReaderError::UNEXPECTED_TOKEN;
		this->openedTag->SetProperty(this->tokens->at(this->pos - 1).value, this->GetCurrentToken()->value);

		this->pos++;
		switch (this->GetCurrentToken()->type)
		{
		case TokenType::CLOSING_SLASH:
			return this->HandleClosingSlash();
		case TokenType::CLOSING_BRACKET:
			return this->HandleClosingBracket();
		case TokenType::PROPERTY_NAME:
			return this->HandlePropertyName();
		default: 
			return XMLReaderError::UNEXPECTED_TOKEN;
		}

		return XMLReaderError::OK;
	}
	XMLReaderError XMLReader::HandleClosingBracket()
	{
		if (this->tokens->at(this->pos - 1).type != TokenType::CLOSING_SLASH)
		{

			this->currTags->push_back(*this->openedTag);

			this->openedTag = nullptr;
			this->pos++;
		}
		else if (this->tokens->at(this->pos - 1).type == TokenType::CLOSING_SLASH)
		{
			if (this->openedTag == nullptr)return XMLReaderError::OTHER_PROBLEM;

			if (this->currTags->size() > 0)
			{
				XMLTag ct = *this->openedTag;
				this->currTags->at(this->currTags->size() - 1).AddChild(ct);
				this->openedTag = nullptr;
			}
			else {
				XMLTag ct = *this->openedTag;
				this->tags->push_back(ct);
				this->openedTag = nullptr;
			}

			this->pos++;
		}
		
		this->bracketOpened = false;
		return XMLReaderError::OK;
	}
	XMLReaderError XMLReader::HandleText()
	{
		if (this->currTags->size() < 1)return XMLReaderError::UNEXPECTED_TOKEN;

		this->GetLastCurrTag()->SetInnerXML(this->GetCurrentToken()->value);
		pos++;
		 
		return XMLReaderError::OK;
	}
	XMLReaderError XMLReader::PrintError(XMLReaderError err)
	{
		if(this->currTags->size() > 0)
			std::cout << "Interpreter error: " << XMLReader::err_str[err] << " (" << this->GetLastCurrTag()->GetTagName() << ")" << std::endl;
		else if(this->openedTag!=nullptr)
			std::cout << "Interpreter error: " << XMLReader::err_str[err] << " (" << this->openedTag->GetTagName() << ")" << std::endl;

		return err;
	}
}