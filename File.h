#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
class File
{
private:
	std::string content;
	std::string filename;
	std::fstream* file;
	size_t filesize;

	void CheckFileSize();
	void ReadFile();

public:
	File(std::string filename);
	std::string GetContent() { return this->content; }
	std::string GetFilename() { return this->filename; }
	size_t GetSize() { return this->filesize; }
};

