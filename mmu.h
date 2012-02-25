/*
 * mmu.h
 *
 *  Created on: Feb 14, 2012
 *      Author: miheer
 */

#ifndef MMU_H_
#define MMU_H_
#include"memory.h"
#include "cpu.h"
class Mmu {
	int mode;
	int base; 						//address of base of page table
	Mmu();
	Memory *m;
public:
	Mmu(Memory *m);
	int to_physical(short ip);		//decode instruction pointer
	int to_physical(int ir);		//whole instruction reg just 2 lsb are tacken in consideration
	int set_mode(int mode);			//Mode can be Cpu::real or ::prot
	int get_mode(void);
	int set_base(int base);
	int get_base(void);
	virtual ~Mmu();
};

#endif /* MMU_H_ */
