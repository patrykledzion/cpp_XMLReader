#pragma once
#include <iostream>
#include <map>
#include <vector>
#include "XMLProperty.h"

namespace nXMLReader {

	class XMLTag
	{
	private:
		std::string tag;
		//Property properties[];
		std::unique_ptr<std::vector<std::shared_ptr<XMLProperty>>> properties;
		std::string innerXML;
		std::shared_ptr<std::vector<XMLTag>> children;
		void AddProperty(std::string name, std::string value);
	public:
		static bool isTagNameCharacter(char c);
		static bool isTagNameStartCharacter(char c);

		XMLTag(std::string tag);
		~XMLTag() = default;
		XMLTag(const XMLTag& other);
		void SetProperty(std::string name, std::string value);
		void SetInnerXML(std::string value);
		void AddChild(XMLTag& child);

		std::string GetInnerXML() const { return this->innerXML; }
		std::string GetTagName() const { return this->tag; }
		std::weak_ptr<std::vector<XMLTag>> GetChildren() const { return this->children; }

		void PrintTag(int level); 
	};


}