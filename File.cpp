#include "File.h"

void File::CheckFileSize()
{
	if (this->file == nullptr)
	{
		this->filesize = 0;
		return;
	}

	this->file->seekp(0, std::ios::end);
	this->filesize = this->file->tellp();
	this->file->seekp(0, std::ios::beg);
}

void File::ReadFile()
{
	if (this->file == nullptr || this->filesize == 0)return;
	std::stringstream buffer;
	buffer << this->file->rdbuf();
	this->content = std::move(buffer.str());
}

File::File(std::string filename) : filename(std::move(filename))
{
	this->filesize = 0;

	this->file = new std::fstream(this->filename, std::ios::in | std::ios::binary);
	if (!this->file->is_open())
	{
		this->file = nullptr;
		return;
	}

	this->CheckFileSize();
	this->ReadFile();

	this->file->close();
	int x = 5;
}
 