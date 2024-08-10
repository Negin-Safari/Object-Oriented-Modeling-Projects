#include<systemc.h>


SC_MODULE(STACK_TIMER_BFM) {
	sc_in<sc_logic> clk, rst, push, pop, tosa;
	sc_in<sc_lv<16>> dataIN;
	sc_out<sc_logic> full, empty, memReady, Writemem, Readmem;
	sc_out<sc_lv<16>> dataOUT;
	sc_out<sc_lv<9>> Addr;
	sc_inout_rv<16> DataMEM;
	sc_signal<sc_lv<9>> tos;
	sc_lv<10> elem;

	SC_CTOR(STACK_TIMER_BFM) {

		SC_THREAD(operation);
		sensitive << clk << rst;
	}
	void operation() {
		while (true) {
			if (rst->read() == SC_LOGIC_1) {
				elem = "0000000001" - "0000000001";
				Addr = "111111111";
				full = SC_LOGIC_0;
				empty = SC_LOGIC_1;
				memReady = SC_LOGIC_0;
				Writemem = SC_LOGIC_0;
				Readmem = SC_LOGIC_0;
			}
			else if ((clk->event()) && (clk->read() == SC_LOGIC_1)) {
				memReady = SC_LOGIC_0;
				Writemem = SC_LOGIC_0;
				Readmem = SC_LOGIC_0;
				if ((push->read() == SC_LOGIC_1) && (full->read() == SC_LOGIC_0)) {
					tos = tos.read().to_uint() + 1;
					elem = elem.to_uint() + 1;
					empty = SC_LOGIC_0;
					Addr = Addr->read().to_uint() + 1;
					Writemem = SC_LOGIC_1;
					DataMEM = dataIN;
					for (int i = 0; i < 4; i++) {
						wait(clk->posedge_event());
					}
					memReady = SC_LOGIC_1;
					if (elem == 512) {
						full = SC_LOGIC_1;
					}
				}
				else if ((pop->read() == SC_LOGIC_1) && (empty->read() == SC_LOGIC_0)) {
					Readmem = SC_LOGIC_1;
					for (int i = 0; i < 4; i++) {
						wait(clk->posedge_event());
					}
					memReady = SC_LOGIC_1;
					dataOUT = DataMEM;
					DataMEM = "ZZZZZZZZZZZZZZZZ";
					Addr = Addr->read().to_uint() - 1;
					elem = elem.to_uint() - 1;
					if (elem == 0) {
						empty = SC_LOGIC_1;
					}
				}
				else if ((tosa->read() == SC_LOGIC_1) && (empty->read() == SC_LOGIC_0)) {
					Readmem = SC_LOGIC_1;
					for (int i = 0; i < 4; i++) {
						wait(clk->posedge_event());
					}
					memReady = SC_LOGIC_1;
					dataOUT = DataMEM;
					DataMEM = "ZZZZZZZZZZZZZZZZ";
				}
			}
			wait();
		}
	}
};

SC_MODULE(MEMORY_BFM) {
	sc_in<sc_lv<9>> addr;
	sc_inout_rv<16> data;
	sc_in<sc_logic> clk, rst, writeMem, readMem;

	sc_uint <16> *mem;

	SC_HAS_PROCESS(MEMORY_BFM);

	MEMORY_BFM(sc_module_name) {

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
				for (int i = 0; i < 5; i++) {
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

SC_MODULE(BFM) {
	sc_in<sc_logic> clk, rst, push, pop, tos;
	sc_out<sc_logic> readMem, writeMem, full, empty, memReady;
	sc_in<sc_lv<16>> dataIN;
	sc_out<sc_lv<16>> dataOUT;

	sc_signal_rv<16> dataMem;
	sc_signal<sc_lv<9>> tos_addr;


	STACK_TIMER_BFM* test;
	MEMORY_BFM* memory;

	SC_CTOR(BFM) {

		memory = new MEMORY_BFM("memory");
		(*memory)(tos_addr, dataMem, clk, rst, writeMem, readMem);
		test = new STACK_TIMER_BFM("test");
		(*test)(clk, rst, push, pop, tos, dataIN, full, empty, memReady, writeMem, readMem, dataOUT, tos_addr, dataMem);
	}
};

SC_MODULE(testbench_BFM) {
	sc_signal<sc_logic> clk, rst, push, pop, tos, memReady;
	sc_signal<sc_logic> readMem, writeMem, full, empty;
	sc_signal<sc_lv<16>> dataIN, dataOUT;

	BFM* bfm;

	SC_CTOR(testbench_BFM) {

		bfm = new BFM("bfm");
		(*bfm)(clk, rst, push, pop, tos, readMem, writeMem, full, empty, memReady, dataIN, dataOUT);
		SC_THREAD(clocking);
		SC_THREAD(resseting);
		SC_THREAD(signals);
	}
	void clocking() {
		while (1) {
			clk = SC_LOGIC_0;
			wait(20, SC_NS);
			clk = SC_LOGIC_1;
			wait(20, SC_NS);
		}
	}
	void resseting() {
		while (1) {
			rst = SC_LOGIC_1;
			wait(20, SC_NS);
			rst = SC_LOGIC_0;
			wait();
		}
	}
	void signals() {
		while (1) {
			wait(50, SC_NS);
			pop = SC_LOGIC_1;
			wait(50, SC_NS);
			pop = SC_LOGIC_0;
			push = SC_LOGIC_1;
			dataIN.write(12);
			wait(5, SC_NS);
			tos = SC_LOGIC_1;
			wait(40, SC_NS);
			push = SC_LOGIC_0;
			wait(250, SC_NS);
			tos = SC_LOGIC_1;
			wait(40, SC_NS);
			tos = SC_LOGIC_0;
			wait(250, SC_NS);

			dataIN.write(5);
			push = SC_LOGIC_1;
			wait(40, SC_NS);
			push = SC_LOGIC_0;
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

			dataIN.write(7);
			push = SC_LOGIC_1;
			wait(40, SC_NS);
			push = SC_LOGIC_0;
			wait(250, SC_NS);

			dataIN.write(8);
			push = SC_LOGIC_1;
			wait(40, SC_NS);
			push = SC_LOGIC_0;
			wait(250, SC_NS);

			pop = SC_LOGIC_1;
			wait(40, SC_NS);
			pop = SC_LOGIC_0;
			wait(250, SC_NS);

			dataIN.write(20);
			push = SC_LOGIC_1;
			wait(40, SC_NS);
			push = SC_LOGIC_0;
			wait(250, SC_NS);

			pop = SC_LOGIC_1;
			wait(40, SC_NS);
			pop = SC_LOGIC_0;
			wait(250, SC_NS);

			dataIN.write(40);
			push = SC_LOGIC_1;
			wait(40, SC_NS);
			push = SC_LOGIC_0;
			wait(250, SC_NS);

			tos = SC_LOGIC_1;
			wait(40, SC_NS);
			tos = SC_LOGIC_0;



			/*pop = SC_LOGIC_0;
			push = SC_LOGIC_0;
			tos = SC_LOGIC_0;
			wait(11, SC_NS);
			for (int i = 0; i < 513; i++) {

			push = SC_LOGIC_1;
			wait(10, SC_NS);
			push = SC_LOGIC_0;
			wait(50, SC_NS); */

			wait();

		}


	}

};

SC_MODULE(testbench_COMP) {
	sc_signal<sc_logic> clk, rst, push, pop, tos, memReady1, memReady2, Readmem1, Readmem2, Writemem1, Writemem2, full1, full2, empty1, empty2;
	sc_signal<sc_lv<16>> dataIN, dataOUT1, dataOUT2;

	RTL* rtl;
	BFM* bfm;

	SC_CTOR(testbench_COMP) {
		rtl = new RTL("RTL");
		(*rtl)(clk, rst, push, pop, tos, dataIN, memReady1, Readmem1, Writemem1, full1, empty1, dataOUT1);
		bfm = new BFM("BFM");
		(*bfm)(clk, rst, push, pop, tos, Readmem2, Writemem2, full2, empty2, memReady2, dataIN, dataOUT2);
		SC_THREAD(clocking);
		SC_THREAD(resseting);
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
	void resseting() {
		while (1) {
			rst = SC_LOGIC_1;
			wait(20, SC_NS);
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
			dataIN.write(12);
			wait(5, SC_NS);
			//tos = SC_LOGIC_1;
			wait(40, SC_NS);
			push = SC_LOGIC_0;
			wait(250, SC_NS);
			tos = SC_LOGIC_1;
			wait(40, SC_NS);
			tos = SC_LOGIC_0;
			wait(250, SC_NS);

			dataIN.write(5);
			push = SC_LOGIC_1;
			wait(40, SC_NS);
			push = SC_LOGIC_0;
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

			dataIN.write(7);
			push = SC_LOGIC_1;
			wait(40, SC_NS);
			push = SC_LOGIC_0;
			wait(250, SC_NS);

			dataIN.write(8);
			push = SC_LOGIC_1;
			wait(40, SC_NS);
			push = SC_LOGIC_0;
			wait(250, SC_NS);

			pop = SC_LOGIC_1;
			wait(40, SC_NS);
			pop = SC_LOGIC_0;
			wait(250, SC_NS);

			dataIN.write(20);
			push = SC_LOGIC_1;
			wait(40, SC_NS);
			push = SC_LOGIC_0;
			wait(250, SC_NS);

			pop = SC_LOGIC_1;
			wait(40, SC_NS);
			pop = SC_LOGIC_0;
			wait(250, SC_NS);

			dataIN.write(40);
			push = SC_LOGIC_1;
			wait(40, SC_NS);
			push = SC_LOGIC_0;
			wait(250, SC_NS);

			tos = SC_LOGIC_1;
			wait(40, SC_NS);
			tos = SC_LOGIC_0;


			/*pop = SC_LOGIC_0;
			push = SC_LOGIC_0;
			tos = SC_LOGIC_0;
			wait(11, SC_NS);
			for (int i = 0; i < 513; i++) {

			push = SC_LOGIC_1;
			wait(10, SC_NS);
			push = SC_LOGIC_0;
			wait(50, SC_NS); */
			wait();

		}
	}

};
