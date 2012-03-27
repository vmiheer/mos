/*
 * cardreader.cpp
 *
 *  Created on: Jan 13, 2012
 */

#include "cardreader.h"
CardReader::CardReader() {


}
CardReader::CardReader(const char* ifile){
	infilename=ifile;
	f=new ifstream(ifile);
}
CardReader::~CardReader() {
	if(f){
		f->close();
		delete f;
	}
}
int CardReader :: read(char* buff)
{
	if(f->eof())
	{
		return out_of_cards;
	}
	f->getline(buff,41,'\n');

	if((!f->eof()) && f->fail())
	{
		char tchar;
		f->get(tchar);
		while((!f->eof()) && tchar!='\n')
		{
			f->clear();
			f->get(tchar);
		}
		if(!f->eof())
			f->clear();
		else
		{
			f->clear();
			f->setstate(ifstream::eofbit);
		}
	}
	return out_of_cards+1;
}
