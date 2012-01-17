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
class MOS;
class Cpu {
	short ip;	//instruction pointer
	int acc;	//accumulator
	char c;		//c flag
	int ir;
	int si;
	MOS *os;
	Memory *m;
	long clk;
	Cpu();
public:

	Cpu(Memory *mm,MOS *os);

	void fetch();
	void decode();
	void execute();
	static const int gd=0;
	static const int pd=1;
	static const int lr=2;
	static const int sr=3;
	static const int cr=4;
	static const int bt=5;
	static const int h=6;
	friend class MOS;
	virtual ~Cpu();
};

#endif /* CPU_H_ */
