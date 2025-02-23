//XMLReader beta v1.0
//By Patryk Ledzion

#include <iostream>
#include "XMLReader.h"
int main()
{
	nXMLReader::XMLReader* reader = new nXMLReader::XMLReader("testfile.xml");
	nXMLReader::XMLReaderError ret = reader->Read();

	for (int i = 0; i < reader->GetTags()->size(); i++)
	{
		reader->GetTags()->at(i).PrintTag(0);
	}

	int x = 2137;

	return 0;
}