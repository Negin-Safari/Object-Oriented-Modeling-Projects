#include "components.h"
#include "BFMparts.h"

int sc_main(int argv, char** argc) {
	testbench_RTL* testRTL = new testbench_RTL("testbench");
	sc_trace_file* outputfile;
	outputfile = sc_create_vcd_trace_file("outputfile_RTL");
	////clk, rst, push, pop, tos, memReady, tos_addr, readMem, writeMem, full, empty, initTimer

	sc_trace(outputfile, testRTL->clk, "clk");
	sc_trace(outputfile, testRTL->rst, "rst");

	sc_trace(outputfile, testRTL->push, "push");
	sc_trace(outputfile, testRTL->pop, "pop");
	sc_trace(outputfile, testRTL->tos, "tos");

	sc_trace(outputfile, testRTL->dataIN, "dataIN");
	sc_trace(outputfile, testRTL->dataOUT, "dataOUT");


	sc_trace(outputfile, testRTL->memReady, "memReady");
	sc_trace(outputfile, testRTL->readMem, "readMem");
	sc_trace(outputfile, testRTL->writeMem, "writeMem");
	sc_trace(outputfile, testRTL->full, "full");
	sc_trace(outputfile, testRTL->empty, "empty");
	sc_trace(outputfile, testRTL->rtl->stackHandler->DP->tos, "tosAddress");
	//////////////////////////////////////////////////////////////////////////////

	testbench_BFM* testBFM = new testbench_BFM("testbench_BFM");
	sc_trace_file* outputfile1;
	outputfile1 = sc_create_vcd_trace_file("outputfile_BFM");

	sc_trace(outputfile1, testBFM->clk, "clk");
	sc_trace(outputfile1, testBFM->rst, "rst");

	sc_trace(outputfile1, testBFM->push, "push");
	sc_trace(outputfile1, testBFM->pop, "pop");
	sc_trace(outputfile1, testBFM->tos, "tos");

	sc_trace(outputfile1, testBFM->dataIN, "dataIN");
	sc_trace(outputfile1, testBFM->dataOUT, "dataOUT");

	sc_trace(outputfile1, testBFM->memReady, "memReady");
	sc_trace(outputfile1, testBFM->readMem, "readMem");
	sc_trace(outputfile1, testBFM->writeMem, "writeMem");
	sc_trace(outputfile1, testBFM->full, "full");
	sc_trace(outputfile1, testBFM->empty, "empty");
	sc_trace(outputfile1, testBFM->bfm->tos_addr, "tosAddress");
	/////////////////////////////////////////////////////////////

	testbench_COMP* testCompare = new testbench_COMP("testbench_BOTH");
	sc_trace_file* outputfile2;
	outputfile2 = sc_create_vcd_trace_file("outputfile_BOTH");

	sc_trace(outputfile2, testCompare->clk, "clk");
	sc_trace(outputfile2, testCompare->rst, "rst");

	sc_trace(outputfile2, testCompare->push, "push");
	sc_trace(outputfile2, testCompare->pop, "pop");
	sc_trace(outputfile2, testCompare->tos, "tos");

	sc_trace(outputfile2, testCompare->dataIN, "dataIN");
	sc_trace(outputfile2, testCompare->dataOUT1, "dataOUT1");
	sc_trace(outputfile2, testCompare->dataOUT2, "dataOUT2");

	sc_trace(outputfile2, testCompare->memReady1, "memReady1");
	sc_trace(outputfile2, testCompare->Readmem1, "readMem1");
	sc_trace(outputfile2, testCompare->Writemem1, "writeMem1");
	sc_trace(outputfile2, testCompare->full1, "full1");
	sc_trace(outputfile2, testCompare->empty1, "empty1");
	sc_trace(outputfile2, testCompare->rtl->stackHandler->DP->tos, "tosAddress1");

	sc_trace(outputfile2, testCompare->memReady2, "memReady2");
	sc_trace(outputfile2, testCompare->Readmem2, "readMem2");
	sc_trace(outputfile2, testCompare->Writemem2, "writeMem2");
	sc_trace(outputfile2, testCompare->full2, "full2");
	sc_trace(outputfile2, testCompare->empty2, "empty2");
	sc_trace(outputfile2, testCompare->bfm->tos_addr, "tosAddress2");

	sc_start(4500, SC_NS); //155000 for showing full
	getchar();
	return 0;
}
