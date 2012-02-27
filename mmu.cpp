
/*
 * mmu.cpp
 *
 *  Created on: Feb 14, 2012
 *      Author: miheer
 */

#include "mmu.h"
Mmu::Mmu() {
	mode=Cpu::real;
	base=0;
}
Mmu::Mmu(Memory *m){
	this->m=m;
	mode=Cpu::real;
	base=0;

}
int Mmu::to_physical(short ic)
{
	if(mode==Cpu::real){
		//we are in real mode just return flat addr
		return ic;
	}
	else{
		int addr=m->read((base+(ic/10)));	//get the line in page table
		int retvalue;
		if(((char*)&addr)[0]=='-')			//the 0th index '-' sign indicates free page
			return -1;
		else
		{
			char *t=((char*)&addr);
			retvalue=(t[2]-'0')*100;
			retvalue+=(t[3]-'0')*10;
			retvalue+=ic%10;
			return retvalue;
		}
	}
	return -1;
}
int Mmu::to_physical(int ir)
{//The ir register is sent as parameter here.
	if(mode==Cpu::real){
		//we are in real mode just return flat addr
		return ((((((char*)&ir)[2])-'0')*10)+((((char*)&ir)[3])-'0'));
	}
	else{
		char *t=(char*)&ir;
		int addr=m->read(base+(t[2]-'0'));	//get the line in page table
		int retvalue;
		if(((char*)&addr)[0]=='-')			//the 0th index '-' sign indicates free page
			return -1;
		else
		{
			t=((char*)&addr);
			retvalue=(t[2]-'0')*100;
			retvalue+=(t[3]-'0')*10;
			retvalue+=(((char*)&ir)[3])-'0';
			return retvalue;
		}
	}
	return -1;
}
int Mmu::set_mode(int mode)
{
	this->mode=mode;
	return mode;
}

int Mmu::get_mode(void)
{
	return mode;
}

Mmu::~Mmu() {

}
int Mmu::get_base(void)
{
	return base;
}

int Mmu::set_base(int base)
{
	this->base=base;
	return base;
}
