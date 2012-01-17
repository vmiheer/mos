/*
 * cpu.cpp
 *
 *  Created on: Jan 13, 2012
 *      Author: miheer
 */

#include "cpu.h"

Cpu::Cpu() {
	// TODO Auto-generated constructor stub

}
Cpu::Cpu(Memory *mm,MOS *os)
{
	m=mm;
	this->os=os;
	ip=0;
	acc=0;
	c=0;
	ir=0;
	si=3;
	m=mm;
}
Cpu::~Cpu() {

}
void Cpu::fetch()
{
	ir=(*m).read(ip);				// base address of the program card must be known
	ip++;
}
void Cpu::execute(){
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
				int addr=((((char *)& ir)[2])-'0')*10;
				addr+=((((char *)& ir)[3])-'0');
				acc=(*m).read(addr);
			}
			break;
			case sr:
			{

				int addr=((((char *)& ir)[2])-'0')*10;
				addr+=((((char *)& ir)[3])-'0');
				(*m).write(addr,acc);
			}
			break;
			case cr:
			{
				int addr=((((char *)& ir)[2])-'0')*10;
				addr+=((((char *)& ir)[3])-'0');
				if((((*m).read(addr))==acc))
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
					int addr=((((char *)& ir)[2])-'0')*10;
					addr+=((((char *)& ir)[3])-'0');
					ip=addr;
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
