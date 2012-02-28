/*
 * mos.h
 *
 *  Created on: Jan 13, 2012
 *      Author: miheer
 */
#ifndef MOS_H_
#define MOS_H_
#define likely(x) __builtin_expect((x),1)
#define unlikely(x) __builtin_expect((x),0)
#include "lineprinter.h"
#include "cpu.h"
#include "cardreader.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <vector>
#include <string>
using namespace std;
class Cpu;
class MOS {
	char sys_ibuff[45],sys_obuff[45];
	Cpu *c;
	Memory *m;
	CardReader *cr;
	LinePrinter *pr;
	int curr_gid,curr_pid,iinstructions,ilines,itinstructions,itlines,basereg;
	MOS();
	int error[10];
	int NoOfErrors;
	std::vector<std::string> e;
public:
	int gd_service();//interrupt handlers for
	void pd_service();//si=1,2,3 respectively
	void al_service();//allocator
	void h_service();
	void check(Cpu *c);
	static const int amj_card=0;
	static const int dta_card=amj_card+1;
	static const int end_card=dta_card+1;
	static const int prog_card=end_card+1;
	MOS(LinePrinter *lnpr, CardReader *crd);
	virtual ~MOS();
};

#endif /* MOS_H_ */
