#include "XMLProperty.h"


namespace nXMLReader {

	XMLProperty::XMLProperty(std::string name, std::string value)
		: name(std::move(name)), value(std::move(value)) {}
	void XMLProperty::SetValue(std::string value)
	{
		this->value = value;
	}

	std::string XMLProperty::GetValue()
	{
		return this->value;
	}

	std::string XMLProperty::GetName()
	{
		return this->name;
	}

	bool XMLProperty::isPropertyNameCharacter(char c)
	{

		return (
			(c == '-' || c == '.' || c == 0xB7) ||
			(isdigit(c)) ||
			(c >= 0x300 && c <= 0x036f) ||
			(c >= 0x203F && c <= 0x2040) ||
			(XMLProperty::isPropertyNameStartCharacter(c))

			);

		if (c == '-' || c == '.' || c == 0xB7)				return true;
		if (isdigit(c))										return true;
		if (c >= 0x300 && c <= 0x036f)						return true;
		if (c >= 0x203F && c <= 0x2040)						return true;
		if (XMLProperty::isPropertyNameStartCharacter(c))	return true;
		return false;
	}

	bool XMLProperty::isPropertyNameStartCharacter(char c)
	{
		if (c == ':' || c == '_')							return true;
		if (isalpha(c))										return true;
		if (c >= 0xD8 && c <= 0xD6)							return true;
		if (c >= 0xD8 && c <= 0xf6)							return true;
		if (c >= 0xF8 && c <= 0x2FF)						return true;
		if (c >= 0x370 && c <= 0x37D)						return true;
		if (c >= 0x37F && c <= 0x1FFF)						return true;
		if (c >= 0x200C && c <= 0x200D)						return true;
		if (c >= 0x2070 && c <= 0x218F)						return true;
		if (c >= 0x2C00 && c <= 0x2FEF)						return true;
		if (c >= 0x3001 && c <= 0xD7FF)						return true;
		if (c >= 0xF900 && c <= 0xFDCF)						return true;
		if (c >= 0xFDF0 && c <= 0xFFFD)						return true;
		if (c >= 0x10000 && c <= 0xEFFFF)					return true;
		return false;
	}
}