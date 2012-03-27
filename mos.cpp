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
	int flag=0;
	int page_no;
	while(flag != 1)
	{
		page_no = rand() % 30;	// This will generate a random number between 0 to 29
		int Index,tpgno;
		int bse = c->pmu->get_base();
		for(int i = 0 ;i < 10;i++)
		{
			Index = bse + i;
			tpgno = m->read(Index);
			if(((((char*)(&tpgno))[2] - '0') * 10 + (((char*)(&tpgno))[3] - '0')) == page_no || page_no==basereg/10)
			{
				flag=1;
				break;
			}
		}
		if(flag == 1)
		{
			flag = 0;
			continue;
		}
		else
		{
			break;
		}
	}
	// Put the entry of this page in the page table
	int temp = 0;
	((char*)(&temp))[0] = '0';
	((char*)(&temp))[1] = '0';
	((char*)(&temp))[2] = (page_no/10)+'0';
	((char*)(&temp))[3] = (page_no%10)+'0';
	m->write((c->pmu->get_base() + (((char*)(&c->ir))[2]-'0')) , temp );
	if(c->pi & Cpu::page_fault){
		c->pi^=Cpu::page_fault;
	}
}
void MOS::check(Cpu *c)
{
	if(unlikely(!(this->c)))
	{
		this->c=c;
		this->m=c->m;
		if(likely(c->mode==Cpu::real)){
			basereg=rand()%30;
			basereg=basereg*10;
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
		NoOfErrors++;
		error[NoOfErrors]=3;
		h_service();
	}
	if(!((bool)(c->si || c->pi || c->ti)))
	{
		return;
	}


	if(c->ti == 0 && c->si == 1)
	{
		//The program may run out of data cards
		if(gd_service()==end_card){
			NoOfErrors++;
			error[NoOfErrors]=1;
			h_service();
		}
	}
	else if(c->ti == 0 && c->si == 2)
	{
		pd_service();
	}
	else if(c->ti == 0 && c->si == 3)
	{
		NoOfErrors++;
		error[NoOfErrors] = 0;
		h_service();
	}
	else if(c->ti == 2 && c->si == 1)
	{
		NoOfErrors++;
		error[NoOfErrors] = 3;
		h_service();
	}
	else if(c->ti == 2 && c->si == 2)
	{
		pd_service();
		NoOfErrors++;
		error[NoOfErrors] = 3;
		h_service();
	}
	else if(c->ti == 2 && c->si == 3)
	{
		h_service();
	}
	else if(c->ti == 0 && c->pi == Cpu::operation_fault)
	{
		NoOfErrors++;
		error[NoOfErrors] = 4;
		h_service();
	}
	else if(c->ti == 0 && c->pi == Cpu::operand_fault)
	{
		NoOfErrors++;
		error[NoOfErrors] = 5;
		h_service();
	}
	else if(c->ti == 0 && c->pi == Cpu::page_fault)
	{
		if(c->instruction==Cpu::gd || c->instruction==Cpu::sr)
		{
			al_service();
		}
		else
		{
			NoOfErrors++;
			error[NoOfErrors] = 6;
			h_service();
		}

	}
	else if(c->ti == 2 && c->pi == 1)
	{
		NoOfErrors++;
		error[NoOfErrors] = 3;
		NoOfErrors++;
		error[NoOfErrors] = 4;
		h_service();
	}
	else if(c->ti == 2 && c->pi == 2)
	{
		NoOfErrors++;
		error[NoOfErrors] = 3;
		NoOfErrors++;
		error[NoOfErrors] = 5;
		h_service();
	}
	else if(c->ti == 2 && c->pi == 3)
	{
		NoOfErrors++;
		error[NoOfErrors] = 3;
		h_service();
	}
}
int MOS::gd_service(){
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
		while((c->mar=c->pmu->to_physical(c->ir))==-1)
		{
			al_service();
		}
		for(int i=0;i<10;i++)
		{
			m->write(c->mar+i,((int*)(sys_ibuff))[i]);
		}
		return prog_card;
	}
}
void MOS::pd_service()
{
	(c->si)=0;
	ilines++;
	if(ilines>itlines)
	{
		NoOfErrors++;
		error[NoOfErrors]=2;
		h_service();
	}

	c->mar=c->pmu->to_physical(c->ir);
	int i=0;
	for(int x=0;x<10;x++)
	{
		int tempdata=(m->read(c->mar + x));
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
	NoOfErrors = -1;
	e.push_back("ERROR CODE:1  No error\n");
	e.push_back("ERROR CODE:2  Out Of Data\n");
	e.push_back("ERROR CODE:3  Line Limit Exceeded\n");
	e.push_back("ERROR CODE:4  Time Limit Exceeded\n");
	e.push_back("ERROR CODE:5  Operation Code Error\n");
	e.push_back("ERROR CODE:6  Operand Error\n");
	e.push_back("ERROR CODE:7  Invalid Page Fault\n");
}
void MOS::h_service()
{
	c->si=c->ti=c->ioi=c->pi=0;

	if(((char*)(&c->ir))[0]!=0)
	{
		char tmp[40]={0};
		char mp[5]={0};
		char *t=(char*)&c->ir;
		sprintf(mp,"%c%c%c%c",t[0],t[1],t[2],t[3]);
		sprintf(tmp,"ip:%d\tir:%s\t%d\t%d\n",c->ip,mp,iinstructions,ilines);
		strcpy(sys_obuff,tmp);
		pr->print(sys_obuff);
		int tempCount=NoOfErrors;
		bool outOffData=false;
		while(tempCount>=0)
		{
			if(error[tempCount]==1)
				outOffData=true;
			strcpy(sys_obuff,e[error[tempCount--]].c_str());
			pr->print(sys_obuff);
		}
		//if h_service wasn't called first time
		//verify condition as it fails in case of less no of data cards
		//cpu has encountered atleast 1 halt instruction now we are ignoring
		//remaining data cards
		sys_obuff[0]=0;
		pr->print(sys_obuff);
		sys_obuff[0]=0;
		pr->print(sys_obuff);
		if(!outOffData)
			while(gd_service()!=end_card);
	}

	//Reinitialize page table for each new process
	int init=0;
	((char*)&init)[0]='-';
	((char*)&init)[1]='0';
	((char*)&init)[2]='0';
	((char*)&init)[3]='1';
	for (int i=0;i<10;i++)
		c->m->write(basereg+i,init);
	c->set_mode(Cpu::prot,basereg);

	//next card is either amj or our os may power off
	int card;
	while((card=gd_service())!=amj_card); //this will cause power off


	//__block__	don't seperate
	int i=0;
	((char*)&c->ir)[2]=i+'0';
	((char*)&c->ir)[3]=0+'0';
	while((card=gd_service())!=dta_card){
		i++;
		((char*)&c->ir)[2]=i+'0';
		((char*)&c->ir)[3]=0+'0';
	}
	//__end__block__	
	
	
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
		NoOfErrors=-1;
		return;
	}
	// read next card.... if it is the '$end' card,,, halt
	// else if its an AMJ card,,, load the next instructions in the memory unless you get the $DTA card.
	// when you get the $DTA card stop ,,,,, and return
}
