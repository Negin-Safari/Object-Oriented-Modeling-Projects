#include <systemc.h>
#include <iostream>
#include <vector>
using namespace std;



SC_MODULE(Decrementer) {
	sc_in<sc_lv<9>> in;
	sc_out<sc_lv<9>> out;

	SC_CTOR(Decrementer) {
		SC_METHOD(evl);
		sensitive << in;
	}
	void evl() {
		out = in->read().to_uint() - 1;
	}
};

SC_MODULE(UpDownCounter) {
	sc_in <sc_logic> clk, rst, upcnt, dncnt, init;
	sc_out <sc_lv<10>> Count;

	SC_CTOR(UpDownCounter) {
		SC_METHOD(evl);
		sensitive << clk << rst;
	}
	void evl() {
		if (rst->read() == SC_LOGIC_1)
			Count->write("0000000000");
		else if (clk.event() && (clk->read() == SC_LOGIC_1)) {
			if (init->read() == SC_LOGIC_1)
				Count->write("0000000000");
			else if (upcnt->read() == SC_LOGIC_1)
				Count = Count->read().to_uint() + 1;
			else if (dncnt->read() == SC_LOGIC_1)
				Count = Count->read().to_uint() - 1;
		}
	}
};

SC_MODULE(TIMER) {
	sc_in <sc_logic> clk, rst, cenWm, cenRm, init;
	sc_out<sc_logic> Co;

	sc_signal <sc_lv<3>> CNT;

	SC_CTOR(TIMER) {
		SC_METHOD(counting);
		sensitive << clk << rst;
		SC_METHOD(carrying);
		sensitive << CNT;
	}
	void counting() {
		if (rst->read() == SC_LOGIC_1)
			CNT = "000";
		else if (clk.event() && (clk->read() == SC_LOGIC_1)) {
			if ((init->read() == SC_LOGIC_1) || (Co->read() == SC_LOGIC_1))
				CNT = "011";
			else if ((cenWm->read() == SC_LOGIC_1) || (cenRm->read() == SC_LOGIC_1))
				CNT = CNT.read().to_uint() + 1;
		}
	}
	void carrying() {
		if (CNT.read() == "111")
			Co->write(SC_LOGIC_1);
		else
			Co->write(SC_LOGIC_0);
	}
};

SC_MODULE(STACK_HANDLER_datapath) {

	sc_in<sc_logic> clk, rst, upcen, dncen, init;
	sc_out<sc_lv<9>> tos;
	sc_out<sc_logic> full, empty;

	sc_in<sc_lv<16>> dataIN;
	sc_out<sc_lv<16>> dataOUT;
	sc_inout_rv<16> datamem;

	sc_in<sc_logic> enIN, enOUT;

	sc_signal<sc_lv<10>> ELNUM;
	sc_signal<sc_lv<9>> decrIn;

	UpDownCounter* count;
	Decrementer* decr;

	SC_CTOR(STACK_HANDLER_datapath) {
		count = new UpDownCounter("count");
		(*count)(clk, rst, upcen, dncen, init, ELNUM);
		decr = new Decrementer("decrement");
		(*decr)(decrIn, tos);

		SC_METHOD(assigns);
		sensitive << ELNUM << dataIN << datamem << enIN << enOUT;
	}
	void assigns() {
		decrIn = ELNUM.read().range(8, 0);
		full->write(ELNUM.read().bit(9));
		if (ELNUM.read() == "0000000000")
			empty->write(SC_LOGIC_1);
		else
			empty->write(SC_LOGIC_0);
		if (enIN->read() == SC_LOGIC_1)
			datamem = dataIN;
		else
			datamem->write("ZZZZZZZZZZZZZZZZ");
		if (enOUT->read() == SC_LOGIC_1)
			dataOUT = datamem;
		else
			dataOUT->write("ZZZZZZZZZZZZZZZZ");
	}
};

SC_MODULE(STACK_HANDLER_controller) {
	sc_in<sc_logic> clk, rst, pop, push, tos, full, empty, memReady;
	sc_out<sc_logic> ReadMem, WriteMem, init, upcen, dncen, initTimer, enIN, enOUT;

	sc_signal<sc_lv<3>> ps, ns;   //enum states { Starting, PUSH, WAIT, POP, TOS };

	SC_CTOR(STACK_HANDLER_controller) {
		SC_METHOD(Combs);
		sensitive << ps << pop << tos << push << memReady;
		SC_METHOD(Combo);
		sensitive << ps << memReady << push;
		SC_THREAD(sequencing);
		sensitive << clk << rst;

	}
	void Combs() {
		int n = ps.read().to_uint();
		switch (n) {
		case 0:
			if (push->read() == SC_LOGIC_1)
				ns = "001";
			else
				ns = "000";
			break;
		case 1:
			if (memReady->read() == SC_LOGIC_1)
				ns = "010";
			else
				ns = "001";
			break;
		case 2:
			if ((empty->read() == SC_LOGIC_0) && (pop->read() == SC_LOGIC_1))
				ns = "011";
			else if ((full->read() == SC_LOGIC_0) && (push->read() == SC_LOGIC_1))
				ns = "001";
			else if ((empty->read() == SC_LOGIC_0) && (tos->read() == SC_LOGIC_1))
				ns = "100";
			else
				ns = "010";
			break;
		case 3:
			if (memReady->read() == SC_LOGIC_1)
				ns = "010";
			else
				ns = "011";
			break;
		case 4:
			if (memReady->read() == SC_LOGIC_1)
				ns = "010";
			else
				ns = "100";
			break;
		default:
			ns = "000";
			break;

		}
	}
	void Combo() {
		int n = ps.read().to_uint();
		ReadMem = SC_LOGIC_0;
		WriteMem = SC_LOGIC_0;
		init = SC_LOGIC_0;
		initTimer = SC_LOGIC_0;
		upcen = SC_LOGIC_0;
		dncen = SC_LOGIC_0;
		enIN = SC_LOGIC_0;
		enOUT = SC_LOGIC_0;

		switch (n) {
		case 0:
			initTimer = SC_LOGIC_1;
			if (push->read() == SC_LOGIC_1) {
				upcen = SC_LOGIC_1;
				init = SC_LOGIC_0;
			}
			else
				init = SC_LOGIC_1;
			break;
		case 1:
			enIN = SC_LOGIC_1;
			WriteMem = SC_LOGIC_1;

			break;
		case 2:
			if ((push->read() == SC_LOGIC_1) && (full->read() == SC_LOGIC_0)) {
				upcen = SC_LOGIC_1;
			}
			break;
		case 3:
			ReadMem = SC_LOGIC_1;
			enOUT = SC_LOGIC_1;
			if (memReady == SC_LOGIC_1)
				dncen = SC_LOGIC_1;
			break;
		case 4:
			enOUT = SC_LOGIC_1;
			ReadMem = SC_LOGIC_1;
			break;
		}
	}
	void sequencing() {
		while (true) {
			if (rst->read() == SC_LOGIC_1)
				ps = "000";
			else if (clk->event() && (clk->read() == SC_LOGIC_1))
				ps = ns;
			wait();
		}
	}

};

SC_MODULE(STACK_HANDLER) {
	sc_in<sc_logic> clk, rst, push, pop, tos, memReady;
	sc_out<sc_lv<9>> tos_addr;
	sc_out<sc_logic> readMem, writeMem, full, empty, initTimer;
	sc_in<sc_lv<16>> dataIN;
	sc_out<sc_lv<16>> dataOUT;
	sc_inout_rv<16> datamem;

	sc_signal<sc_logic> upcen, dncen, initCnt, enIN, enOUT;

	STACK_HANDLER_datapath* DP;
	STACK_HANDLER_controller* CU;

	SC_CTOR(STACK_HANDLER) {

		DP = new STACK_HANDLER_datapath("dp");
		(*DP)(clk, rst, upcen, dncen, initCnt, tos_addr, full, empty, dataIN, dataOUT, datamem, enIN, enOUT);
		CU = new STACK_HANDLER_controller("cu");
		(*CU)(clk, rst, pop, push, tos, full, empty, memReady, readMem, writeMem, initCnt, upcen, dncen, initTimer, enIN, enOUT);
	}
};

SC_MODULE(MEMORY) {
	sc_in<sc_lv<9>> addr;
	sc_inout_rv<16> data;
	sc_in<sc_logic> clk, rst, writeMem, readMem;

	sc_uint <16> *mem;

	SC_HAS_PROCESS(MEMORY);

	MEMORY(sc_module_name) {

		mem = new sc_uint<16>[512];
		SC_METHOD(meminit);
		sensitive << clk << rst;
		SC_THREAD(memread);
		sensitive << readMem << clk;
		SC_THREAD(memwrite);
		sensitive << writeMem << clk;

	}


	void memread() {
		while (true) {
			sc_uint<9> ad;
			if (readMem->read() == SC_LOGIC_1) {
				for (int i = 0; i < 4; i++) {
					wait((clk->posedge_event()));
				}
				ad = addr;
				data = mem[ad];
			}
			else {
				data->write("ZZZZZZZZZZZZZZZZ");
			}
			wait();
		}


	}

	void meminit() {

		int i;
		sc_lv<16> dataz = 0;
		if (rst->read() == SC_LOGIC_1) {
			for (i = 0; i<512; i++) {
				mem[i] = dataz;
			}
		}
	}

	void memwrite() {
		while (true) {
			sc_uint<9> ad;
			if (writeMem->read() == SC_LOGIC_1) {
				for (int i = 0; i < 5; i++) {
					wait((clk->posedge_event()));
				}
				ad = addr;
				mem[ad] = data;
			}
			wait();
		}
	}



};

SC_MODULE(RTL) {
	sc_in<sc_logic> clk, rst, push, pop, tos;
	sc_in<sc_lv<16>> dataIN;
	sc_out<sc_logic>memReady, readMem, writeMem, full, empty;
	sc_out<sc_lv<16>> dataOUT;
	sc_signal<sc_lv<9>> tos_addr;
	sc_signal_rv<16> dataMem;
	sc_signal<sc_logic> initTimer;

	STACK_HANDLER* stackHandler;
	TIMER* timer;
	MEMORY* memory;

	SC_CTOR(RTL) {
		memory = new MEMORY("mem");
		(*memory)(tos_addr, dataMem, clk, rst, writeMem, readMem);
		timer = new TIMER("timer");
		(*timer)(clk, rst, readMem, writeMem, initTimer, memReady);
		stackHandler = new STACK_HANDLER("test");
		(*stackHandler)(clk, rst, push, pop, tos, memReady, tos_addr, readMem, writeMem, full, empty, initTimer, dataIN, dataOUT, dataMem);
	}
};

SC_MODULE(testbench_RTL) {
	sc_signal<sc_logic> clk, rst, push, pop, tos, memReady;
	sc_signal<sc_logic> readMem, writeMem, full, empty;
	sc_signal<sc_lv<16>> dataIN, dataOUT;

	RTL* rtl;

	SC_CTOR(testbench_RTL) {
		rtl = new RTL("rtl");
		(*rtl)(clk, rst, push, pop, tos, dataIN, memReady, readMem, writeMem, full, empty, dataOUT);
		SC_THREAD(clocking);
		SC_THREAD(reseting);
		SC_THREAD(signaling);
	}
	void clocking() {
		while (1) {
			clk = SC_LOGIC_0;
			wait(20, SC_NS);
			clk = SC_LOGIC_1;
			wait(20, SC_NS);
		}
	}
	void reseting() {
		while (1) {
			rst = SC_LOGIC_1;
			wait(11, SC_NS);
			rst = SC_LOGIC_0;
			wait();
		}
	}
	void signaling() {
		while (1) {
			
			wait(50, SC_NS);
			pop = SC_LOGIC_1;
			wait(50, SC_NS);
			pop = SC_LOGIC_0;
			push = SC_LOGIC_1;
			dataIN.write(11);
			wait(5, SC_NS);
			//tos = SC_LOGIC_1;
			wait(40, SC_NS);
			push = SC_LOGIC_0;
			wait(250, SC_NS);
			tos = SC_LOGIC_1;
			wait(40, SC_NS);
			tos = SC_LOGIC_0;
			wait(250, SC_NS);////////

			wait(250, SC_NS);

			pop = SC_LOGIC_1;
			wait(40, SC_NS);
			pop = SC_LOGIC_0;
			wait(250, SC_NS);

			dataIN.write(10);
			push = SC_LOGIC_1;
			wait(40, SC_NS);
			push = SC_LOGIC_0;
			wait(250, SC_NS);

			dataIN.write(9);
			push = SC_LOGIC_1;
			wait(40, SC_NS);
			push = SC_LOGIC_0;
			wait(250, SC_NS);

			dataIN.write(8);
			push = SC_LOGIC_1;
			wait(40, SC_NS);
			push = SC_LOGIC_0;
			wait(250, SC_NS);

			dataIN.write(7);
			push = SC_LOGIC_1;
			wait(40, SC_NS);
			push = SC_LOGIC_0;
			wait(250, SC_NS);

			pop = SC_LOGIC_1;
			wait(40, SC_NS);
			pop = SC_LOGIC_0;
			wait(250, SC_NS);

			pop = SC_LOGIC_1;
			wait(40, SC_NS);
			pop = SC_LOGIC_0;
			wait(250, SC_NS);

			pop = SC_LOGIC_1;
			wait(40, SC_NS);
			pop = SC_LOGIC_0;
			wait(250, SC_NS);

			dataIN.write(6);
			push = SC_LOGIC_1;
			wait(40, SC_NS);
			push = SC_LOGIC_0;
			wait(250, SC_NS);

			tos = SC_LOGIC_1;
			wait(40, SC_NS);
			tos = SC_LOGIC_0;
			

			/*wait(38, SC_NS);
			for (int i = 0; i < 514; i++) {

				dataIN.write(10);
				push = SC_LOGIC_1;
				wait(40, SC_NS);
				push = SC_LOGIC_0;
				wait(240, SC_NS);
			}*/

		}



		wait();
	}

};

