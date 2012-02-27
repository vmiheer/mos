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
	if(si||pi||ti)
		return;
//	instruction=fe;
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
	if(si||pi||ti)
	{
		return;
	}
	instruction=de;
	char *t=(char*)&ir;
	if(!(isalpha(t[0]) && isalpha(t[1])))
	{
		//This is optional additional checint k
		pi|=operation_fault;
		return;
	}
	if(!((isdigit(t[2]) && isdigit(t[3])) || (t[0]=='H')))
	{
		pi|=operand_fault;
		return;
	}
	mar=pmu->to_physical(ir);
	if(mar<0)
	{
		pi|=page_fault;
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
		pi|=operation_fault;
		//raise exception for invalid instuction
		//generate a fault
	}

}
void Cpu::execute()
{
	if(si|| pi||ti)
	{
		if(pi)
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
				ip=(((char*)&ir)[2]-'0')*10+(((char*)&ir)[3]-'0');
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
