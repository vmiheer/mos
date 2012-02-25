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
	ioi=pi=ti=0;

	m=mm;
	mode=real;
}
void Cpu::set_mode(int mode,int pagetable_baseaddr)
{
	pmu->set_mode(mode);
	pmu->set_base(pagetable_baseaddr);
	this->mode=mode;
}

Cpu::~Cpu() {
	delete pmu;
}
void Cpu::fetch()
{

	mar=pmu->to_physical(ip);
	if(mar<0){
		pi|=page_fault;		//loading is done by os someone is trying to fetching
						//from unallocated (other's) page kill it...
		return;
	}
	ir=(*m).read(mar);				// base address of the program card must be known
	ip++;
}
void Cpu::decode()
{
	if(si||pi)	return;
	char *t=(char*)&ir;
	//TODO:CHeck instruction format and set pi
	//if(isalpha(t[0]) && isalpha(t[1]))

	mar=pmu->to_physical(ir);
	if(mar<0)
	{
		pi|=page_fault;
	}
	if(((char*)&ir)[0] =='G' && ((char*)&ir)[1] =='D')
	{
		if(mar<0)	pi|=page_fault;
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
		pi|=operation_fault;
		//raise exception for invalid instuction
		//generate a fault
	}

}
void Cpu::execute()
{
	if(si || 0)
	{
		ip--;
		return;
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

			acc=(*m).read(mar);
		}
		break;
		case sr:
		{
			(*m).write(mar,acc);
		}
		break;
		case cr:
		{
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
