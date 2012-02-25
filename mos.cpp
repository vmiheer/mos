/*
 * mos.cpp
 *
 *  Created on: Jan 13, 2012
 */
#include "mos.h"

MOS::MOS() {
}
MOS::~MOS() {

}
void MOS::al_service()
{

}
void MOS::check(Cpu *c)
{
	if(unlikely(!(this->c)))
	{
		this->c=c;
		this->m=c->m;
		if(likely(c->mode==Cpu::real)){
			basereg=rand()%30;
			//pick random address for setting pagetable
			int init=0;
			((char*)&init)[0]='-';
			((char*)&init)[1]='0';
			((char*)&init)[2]='0';
			((char*)&init)[3]='1';
			for (int i=0;i<10;i++)
				c->m->write(basereg+i,init);
			c->set_mode(Cpu::prot,basereg);
		}
	}
	iinstructions++;
	if(iinstructions>itinstructions)
	{
		// print 2 blank lines to the output file
		h_service();
	}
	if(!c->si)
	{
		return;
	}
	//TODO:Same switch case for ti pi...
	//Call al_service from it
	switch (c->si)
	{
	case 1 :gd_service();
			break;
	case 2 :pd_service();
			break;
	case 3 :h_service();
			break;
	}
}
int MOS::gd_service(){
//	int temp=(c->si);
//	if((temp & 1))
//		(c->si)=(temp)^1;
	(c->si)=0;
	if((cr->read(sys_ibuff))==CardReader::out_of_cards)
	{
		pr->~LinePrinter();
		cr->~CardReader();
		exit(0);
	}
	//((char *)(&c->ir))[4]='0'; how the hell i have written 4
	((char *)(&c->ir))[3]='0';
	if(sys_ibuff[0] == '$' && sys_ibuff[1] == 'A' && sys_ibuff[2] == 'M' && sys_ibuff[3] == 'J'){
		// this code should be in halt service.
		curr_gid=(sys_ibuff[4]-'0')*10+(sys_ibuff[5]-'0');
		curr_pid=(sys_ibuff[6]-'0')*10+(sys_ibuff[7]-'0');
		itinstructions=(sys_ibuff[8]-'0')*1000+(sys_ibuff[9]-'0')*100+(sys_ibuff[10]-'0')*10+(sys_ibuff[11]-'0');
		itlines=(sys_ibuff[12]-'0')*1000+(sys_ibuff[13]-'0')*100+(sys_ibuff[14]-'0')*10+(sys_ibuff[15]-'0');
		return amj_card;
	}
	else if(sys_ibuff[0] == '$' && sys_ibuff[1] == 'D' && sys_ibuff[2] == 'T' && sys_ibuff[3] == 'A')
	{
		return dta_card;
	}
	else if(sys_ibuff[0] == '$' && sys_ibuff[1] == 'E' && sys_ibuff[2] == 'N' && sys_ibuff[3] == 'D')
	{
		int itgid=(sys_ibuff[4]-'0')*10+(sys_ibuff[5]-'0');
		int itpid=(sys_ibuff[6]-'0')*10+(sys_ibuff[7]-'0');
		if(itgid==curr_gid && itpid==curr_pid){
			//this code has no right to call halt_service let that be decide by halt_service
			//h_service(c);
		}
		return end_card;
	}
	else{
		//a mistake again!!!
		//int base=((((char *)(&(c->ir)))[3])-'0')*10 + (((((char *)(&c->ir))[4]))-'0');
		int base=((((char *)(&(c->ir)))[2])-'0')*10 + (((((char *)(&c->ir))[3]))-'0');
		for(int i=0;i<10;i++)
		{
			m->write((base + i),((int*)(sys_ibuff))[i]);
		}
		return prog_card;
	}
}
void MOS::pd_service()
{
	(c->si)=0;
	ilines++;
	if(ilines>itlines)
		h_service();
//	if((c->si) & 2)
//		(c->si)^=2;
	//& again
	int base=((((char *)(&(c->ir)))[2])-'0')*10 + (((((char *)(&c->ir))[3]))-'0');
	int i=0;
	for(int x=0;x<10;x++)
	{
		int tempdata=(m->read(base + x));
		sys_obuff[i] = ((char*)(&tempdata))[0];
		i++;
		sys_obuff[i] = ((char*)(&tempdata))[1];
		i++;
		sys_obuff[i] = ((char*)(&tempdata))[2];
		i++;
		sys_obuff[i] = ((char*)(&tempdata))[3];
		i++;
	}
	pr->print(sys_obuff);
}
MOS::MOS(LinePrinter *lnpr,CardReader *crd)
{
	c=0;
	cr = crd;
	pr = lnpr;
}
void MOS::h_service()
{
	c->si=c->ti=c->ioi=c->pi=0;
	if(((char*)(&c->ir))[0]!=0)
	{
		//if h_service wasn't called first time
		//verify condition as it fails in case of less no of data cards
		//cpu has encountered atleast 1 halt instruction now we are ignoring
		//remaining data cards
		sys_obuff[0]=0;
		pr->print(sys_obuff);
		sys_obuff[0]=0;
		pr->print(sys_obuff);
		while(gd_service()!=end_card);
	}
	//next card is either amj or our os may power off
	int card;
	while((card=gd_service())!=amj_card); //this will cause power off
	int i=0;
	((char*)&c->ir)[2]=i+'0';
	((char*)&c->ir)[3]=0+'0';

	//Reinitialize page table for each new process
	//FIXME:We can acctually shouldn't do this.
	//This will break multiprocessing...
	int init=0;
	((char*)&init)[0]='-';
	((char*)&init)[1]='0';
	((char*)&init)[2]='0';
	((char*)&init)[3]='1';
	for (int i=0;i<10;i++)
		c->m->write(basereg+i,init);
//	c->set_mode(Cpu::prot,basereg);

	while((card=gd_service())!=dta_card){
		i++;
		((char*)&c->ir)[2]=i+'0';
		((char*)&c->ir)[3]=0+'0';
	}
	if(card==dta_card)
	{
		//we have forgotten remaining initialization
		c->ir=0;//sould we clean memory again?
		c->ip=0;
		c->acc=0;
		c->c=0;
		c->si=0;
		ilines=0;
		iinstructions=0; //initialize the count
		return;
	}
	// read next card.... if it is the '$end' card,,, halt
	// else if its an AMJ card,,, load the next instructions in the memory unless you get the $DTA card.
	// when you get the $DTA card stop ,,,,, and return
}
