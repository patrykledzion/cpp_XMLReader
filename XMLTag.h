#pragma once
#include <iostream>
#include <map>
#include <vector>
#include "XMLProperty.h"

enum _XMLTAG_ {
	START,
	VIEW,
	TEXT,
};

class XMLTag
{
private:
	std::string tag;
	//Property properties[];
	std::vector<XMLProperty*>* properties;
	std::string innerXML;
	std::vector<XMLTag>* children;
	void AddProperty(std::string name, std::string value);
public:
	static bool isTagNameCharacter(char c);
	static bool isTagNameStartCharacter(char c);

	XMLTag(std::string tag);
	void SetProperty(std::string name, std::string value);
	void SetInnerXML(std::string value);
	void AddChild(XMLTag child);

	std::string GetInnerXML() const { return this->innerXML; }
	std::string GetTagName() const { return this->tag; }
	std::vector<XMLTag>* GetChildren() const { return this->children; }
	
	void PrintTag(int level);
	//Property* GetProperty(name);
	static std::map<_XMLTAG_, std::string> tags;
};

 
