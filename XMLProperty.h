#pragma once
#include <iostream>
#include <map>

namespace nXMLReader {
	enum _XMLPROPERTY_ {
		ID,
		CLASS,
	};

	class XMLProperty
	{
	private:
		std::string name;
		std::string value;
	public:
		XMLProperty(std::string name, std::string value);
		void SetValue(std::string value);
		std::string GetValue();
		std::string GetName();
		static bool isPropertyNameCharacter(char c);
		static bool isPropertyNameStartCharacter(char c);
		static std::map<_XMLPROPERTY_, std::string> properties;

	};
}


 