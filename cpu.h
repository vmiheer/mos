/*
 * cpu.h
 *
 *  Created on: Jan 13, 2012
 *      Author: miheer
 */

#ifndef CPU_H_
#define CPU_H_
#include "memory.h"
#include"mos.h"
#include "mmu.h"
#include <vector>
#include <string>
using namespace std;
class MOS;
class Mmu;
class Cpu {
	short ip;	//instruction pointer
	int acc;	//accumulator
	char c;		//c flag
	int ir;
	int si,pi,ioi,ti;
	Memory *m;
	Mmu *pmu;
	long clk;
	int mar;
	int mbr;
	int mode;
	int instruction;
	Cpu();
public:
	Cpu(Memory *mm,MOS *os);
	void fetch();
	void decode();
	void execute();
	void set_mode(int mode,int pagetable_baseaddr);
	static const int gd=0;
	static const int pd=1;
	static const int lr=2;
	static const int sr=3;
	static const int cr=4;
	static const int bt=5;
	static const int h=6;
	static const int fe=7;
	static const int de=8;
	static const int real=0;
	static const int prot=1;
	static const int operand_fault=1;
	static const int operation_fault=2;
	static const int page_fault=4;	//FIXME:i want bit masking here and
									//Operation and operand error are possible
									//Simultaneously
	friend class MOS;
	virtual ~Cpu();
};

#endif /* CPU_H_ */
