////////////////////// Negin Safari 810197525
#include "systemTest.h"

int sc_main(int argc, char* argv[]) {

	sc_core::sc_set_time_resolution(1.0, sc_core::SC_NS);
	
	//Bracketing

	
    EmbededBracketing tb1("TB1");
	sc_trace_file* atf1 = sc_create_vcd_trace_file("trace1");
	sc_trace(atf1, tb1.clk, "clk");
	sc_trace(atf1, tb1.readIO, "readIO");
	sc_trace(atf1, tb1.writeIO, "writeIO");
	sc_trace(atf1, tb1.S0->Register_0->regIn, "regIn0");
	sc_trace(atf1, tb1.S1->Register_1->regIn, "regIn1");
	sc_trace(atf1, tb1.addrBus, "AddrBus");
	sc_trace(atf1, tb1.dataBus, "DataBus");
	sc_trace(atf1, tb1.dataFromSlave0, "DataFrom0");
	sc_trace(atf1, tb1.dataFromSlave1, "DataFrom1");
	sc_trace(atf1, tb1.enableReg0, "Enablereg0");
	sc_trace(atf1, tb1.enableReg1, "Enablereg1");
	sc_trace(atf1, tb1.startTimer, "startTimer");
	sc_trace(atf1, tb1.timeOut, "timeOut");
	

	//ISS
	
	systemTest tb2("TB");
	sc_trace_file* atf2 = sc_create_vcd_trace_file("trace2");
	sc_trace(atf2, tb2.clk, "clk");
	sc_trace(atf2, tb2.systemModule->readIO, "readIO");
	sc_trace(atf2, tb2.systemModule->writeIO, "writeIO");
	sc_trace(atf2, tb2.systemModule->addrBus, "AddrBus");
	sc_trace(atf2, tb2.systemModule->dataBus, "DataBus");
	sc_trace(atf2, tb2.systemModule->sayacInstructionModuleEmb->IRregister, "instruction");
	sc_trace(atf2, tb2.systemModule->DataFromSlave0, "DataFrom0");
	sc_trace(atf2, tb2.systemModule->DataFromSlave1, "DataFrom1");
	sc_trace(atf2, tb2.systemModule->Enablereg0, "Enablereg0");
	sc_trace(atf2, tb2.systemModule->Enablereg1, "Enablereg1");
	sc_trace(atf2, tb2.systemModule->S0->Register_0->regIn, "regIn0");
	sc_trace(atf2, tb2.systemModule->S1->Register_1->regIn, "regIn1");
	sc_trace(atf2, tb2.systemModule->memReady, "memReady");
	sc_trace(atf2, tb2.systemModule->readMem, "readmem");
	sc_trace(atf2, tb2.systemModule->writeMem, "writemem"); 
	sc_trace(atf2, tb2.systemModule->startTimer, "startTimer");
	sc_trace(atf2, tb2.systemModule->TimeOut, "timeOut");
	

	cout << "                  *** simulation started ***" << endl;
	sc_start(1.001, SC_MS);
	cout << "                   *** simulation ended ***" << endl;
	getchar();
	return 0;
}