#include <iostream>
#include "ISSV6.h"
#include "Memory.h"
#include "components.h"
#include <systemc.h>

template <int adrBit, int dataBit>
SC_MODULE (embsystem){
public:
	sc_in <sc_logic> clk;

	sc_signal <sc_logic> memReady, readMem, writeMem;
	sc_signal <sc_logic> CS;

	sc_signal <sc_logic> readIO, writeIO;
	sc_signal <sc_lv <16>> addrBus, DataFromSlave0, DataFromSlave1;
	sc_signal_rv<16> dataBus;
	sc_signal<sc_logic> startTimer, TimeOut, Enablereg0, Enablereg1;

	sayacInstruction<16, 4, 16, 3> *sayacInstructionModuleEmb;
	memory <16,16> *memoryModule;
	Display_IO *Display;
	IO_Decoder *Decoder;
	SINE_IO *S0;
	TRI_IO *S1;
	Timer_IO *Timer;


	SC_CTOR(embsystem) {
		sayacInstructionModuleEmb = new sayacInstruction<16, 4, 16, 3>("sayacInstructionSetModuleEmb");
		(*sayacInstructionModuleEmb)
		(
			clk, memReady, dataBus, readMem, writeMem, readIO, writeIO, addrBus
		);
		
		memoryModule = new memory <16, 16>("memoryModule");
		(*memoryModule)(clk, readMem, writeMem, CS, addrBus, dataBus, memReady, dataBus);

		S0 = new SINE_IO("S0");
		(*S0)(Enablereg0, DataFromSlave0);

		S1 = new TRI_IO("S1");
		(*S1)(Enablereg1, DataFromSlave1);

		Timer = new Timer_IO("T");
		(*Timer)(startTimer, TimeOut);

		Decoder = new IO_Decoder("Decoder");
		(*Decoder)(DataFromSlave0, DataFromSlave1, TimeOut, readIO, writeIO, addrBus, dataBus, startTimer,
			Enablereg0, Enablereg1);

		Display = new Display_IO("Disp");
		(*Display)(clk, writeIO, addrBus, dataBus);

		SC_METHOD (Modeling);
			sensitive << clk;
	}
	void Modeling();


};

template <int adrBit, int dataBit>
void embsystem<adrBit, dataBit>::Modeling()
{
	CS = SC_LOGIC_1;
}