//============================================================================
// Name        : mos.cpp
// Author      :
// Version     :
// Copyright   : GPL v3
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
using namespace std;
#include "mos.h"
#include "memory.h"
#include "cpu.h"
#include "lineprinter.h"
#include "cardreader.h"
int main() {
	Memory m;
	LinePrinter lnpr("outfile");
	CardReader crd("infilename");
	MOS os(&lnpr,&crd);
	Cpu c(&m,&os);
	while(1){
		c.fetch();
		c.decode();
		c.execute();
		os.check(&c);
	}
	return 0;
}
