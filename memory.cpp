#include "memory.h"
#include<string.h>
Memory::Memory() {
	memset(mm,0,300*sizeof(int));
}
int Memory::read(int address)
{
	if(address<0 || address>300)
	 {
		 //write code to throw exception corresponding
		 //to page fault
	 }
	 return mm[address];
}
void Memory::write(int address,int data)
{
	if(address<0 || address>300)
	{
		 //write code to throw exception corresponding
		 //to page fault
	}
	mm[address]=data;
}
Memory::~Memory() {

}
