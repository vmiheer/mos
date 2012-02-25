/*
 * cpu.cpp
 *
 *  Created on: Jan 13, 2012
 *      Author: miheer
 */

#include "cpu.h"

Cpu::Cpu() {

}
Cpu::Cpu(Memory *mm,MOS *os)
{
	m=mm;
	pmu=new Mmu(mm);
	ip=0;
	acc=0;
	c=0;
	ir=0;
	si=3;
	m=mm;
	mode=real;
}
void Cpu::set_mode(int mode,int pagetable_baseaddr)
{
	pmu->set_mode(mode);
	this->mode=mode;
}

Cpu::~Cpu() {
	delete pmu;
}
void Cpu::fetch()
{

	mar=pmu->to_physical(ip);
	ir=(*m).read(mar);				// base address of the program card must be known
	ip++;
}
void Cpu::decode()
{
	if(si||0)	return;
	mar=pmu->to_physical(ir);
}
void Cpu::execute()
{
	int instruction;
	if(si || 0)
	{
		ip--;
		return;
	}
	if(((char*)&ir)[0] =='G' && ((char*)&ir)[1] =='D')
	{
		instruction=gd;
	}
	else if(((char*)&ir)[0] =='P' && ((char*)&ir)[1] =='D')
	{
		instruction=pd;
	}
	else if(((char*)&ir)[0] =='L' && ((char*)&ir)[1] =='R')
	{
		instruction=lr;
	}
	else if(((char*)&ir)[0] =='S' && ((char*)&ir)[1] =='R')
	{
		instruction=sr;
	}
	else if(((char*)&ir)[0] =='C' && ((char*)&ir)[1] =='R')
	{
		instruction=cr;
	}
	else if(((char*)&ir)[0] =='B' && ((char*)&ir)[1] =='T')
	{
		instruction=bt;
	}
	else if(((char*)&ir)[0] =='H')
	{
		instruction=h;
	}
	else
	{
		//raise exception for invalid instuction
		//generate a fault
	}
	switch(instruction)
	{
		case gd:
		{
			si=1;
		}
		break;
		case pd:
		{
			si=2;
		}
		break;
		case lr:
		{
//			int addr=((((char *)& ir)[2])-'0')*10;
//			addr+=((((char *)& ir)[3])-'0');
			acc=(*m).read(mar);
		}
		break;
		case sr:
		{

//			int addr=((((char *)& ir)[2])-'0')*10;
//			addr+=((((char *)& ir)[3])-'0');
			(*m).write(mar,acc);
		}
		break;
		case cr:
		{
//			int addr=((((char *)& ir)[2])-'0')*10;
//			addr+=((((char *)& ir)[3])-'0');
			if((((*m).read(mar))==acc))
			{
				c=1;
			}
			else
				c=0;
		}
		break;
		case bt:
		{
			if(c)
			{
//				int addr=((((char *)& ir)[2])-'0')*10;
//				addr+=((((char *)& ir)[3])-'0');
				ip=mar;
			}
		}
		break;
		case h:
		{
			si=3;
		}
		break;
	}
}
