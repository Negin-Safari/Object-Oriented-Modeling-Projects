#include "systemc-ams.h"
#include "systemc.h"

#include <cmath>

SC_MODULE(SQWave_100KHz) {
	sc_out <double> out;

	SC_CTOR(SQWave_100KHz) {
		SC_THREAD(wave);
	}
	void wave() {
		while (true)
		{
			out = 0;
			wait(5, SC_US);
			out = 1;
			wait(5, SC_US);
		}

	}
};

SC_MODULE(Sensor1) {
	sc_in <double> in;
	sca_tdf::sca_out <double> out;
	sca_eln::sca_node a, b;
	sca_eln::sca_node_ref gnd;

	sca_eln::sca_r r1;
	sca_eln::sca_c c1;

	sca_eln::sca_de_vsource Vin;
	sca_eln::sca_tdf_vsink Vout;

	Sensor1(sc_module_name, double rVal = 400, double cVal = (1e-9)) :   // time constant is 400*(10^-9) = 0.4 Us
		r1("r1", rVal), c1("c1", cVal), a("a"), b("b"), gnd("gnd"),
		Vin("Vin", 1), Vout("Vout", 1) {

		Vin.set_timestep(100, SC_NS);                                 //          a----^^^^----b
		Vin.inp(in);                                                  //          |            |--------
		Vin.p(a);                                                     //       -> Vin         ---       Vout ->
		Vin.n(gnd);                                                   //          |           ---       
		                                                              //          |            |--------
		r1.p(a);                                                      //          ------------gnd
		r1.n(b);

		c1.p(b);
		c1.n(gnd);

		Vout.p(b);
		Vout.n(gnd);
		Vout.outp(out);
	}
};

SCA_TDF_MODULE(ADC) {

	sca_tdf::sca_in <double> in;
	sca_tdf::sca_out <sc_dt::sc_int<16>> out;

	sca_core::sca_time timeStep;
	double Vmax;

	ADC(sc_module_name, sca_core::sca_time T = sca_time(100, SC_NS), double Vm = 0.6)
		: timeStep(T), Vmax(Vm), out("outp"), in("inp") {
		sc_assert((2 <= 16) && (16 <= 64));
		sc_assert(Vmax > 0.0);
	}

	void set_attributes() {
		in.set_timestep(timeStep);
	}

	void processing() {
		double vin = in.read();
		if (vin >= Vmax) {
			out.write(Vmax);
		}
		else if (vin <= -Vmax) {
			out.write(-Vmax);
		}
		else {
			out.write(lround((vin / Vmax) * (pow(2, 15) - 1)));
		}
	}

};

SCA_TDF_MODULE(TDF2DE) {
	sca_tdf::sca_in <sc_dt::sc_int<16>> in;
	sca_tdf::sca_de::sca_out <sc_lv<16>> out;

	sca_core::sca_time Time;
	TDF2DE(sc_module_name, sca_core::sca_time T = sca_time(100, SC_NS))
		:Time(T), out("outp"), in("inp") {}

	void set_attributes() {
		in.set_timestep(Time);
	}


	void processing() {
		out = static_cast<sc_lv<16>>(in);  // converting int to lv
	}

};

template<int halfT>
SC_MODULE(Register) {
	sc_in <sc_logic> OutEnable;
	sc_in <sc_lv<16>> regIn;
	sc_out <sc_lv<16>> Out;
	sc_signal<sc_logic> clk;

	sc_signal<sc_lv<16>> regOut;

	SC_CTOR(Register) {
		SC_THREAD(clocking);
		SC_THREAD(Reging);
		sensitive << clk;
		SC_THREAD(enabling);
		sensitive << OutEnable << regOut;
	}

	void clocking() {
		while (true) {
			clk = SC_LOGIC_0;
			wait(halfT, SC_NS);
			clk = SC_LOGIC_1;
			wait(halfT, SC_NS);
		}
	}

	void Reging() {
		while (true) {
			if ((clk.event()) && (clk == SC_LOGIC_1)) {
				regOut = regIn;
			}
			wait();
		}
	}
	void enabling() {
		while (true) {
			if (OutEnable == SC_LOGIC_1) {
				Out = regOut;
			}
			else {
				Out = "ZZZZZZZZZZZZZZZZ";
			}
			wait();
		}
	}

};


SC_MODULE(sensor1_frontEnd) {

	sc_in <double> in;
	sc_out<sc_lv<16>> out;

	Sensor1* sen;
	ADC* adc;
	TDF2DE* tdf2de;
	sca_tdf::sca_signal<double> Sine_sig;
	sca_tdf::sca_signal<sc_dt::sc_int<16>> adc_sine;


	sensor1_frontEnd(sc_module_name) : in("in"), out("out"),
		Sine_sig("sign"), adc_sine("ads")
	{
		sen = new Sensor1("sen");
		sen->in(in);
		sen->out(Sine_sig);

		adc = new ADC("adc");
		adc->in(Sine_sig);
		adc->out(adc_sine);

		tdf2de = new TDF2DE("tdf2de");
		tdf2de->in(adc_sine);
		tdf2de->out(out);

	}

};

SC_MODULE(SINE_IO) {
	sc_in<sc_logic> Enable;
	sc_out <sc_lv<16>> dataFromSineIO;

	SQWave_100KHz* SW;
	sensor1_frontEnd* S1FE;
	Register<500>* Register_0;  // frequency = 1 MHz

	sc_signal<double> SQ1_sig;
	sc_signal<sc_lv<16>> sine_LV;

	SC_CTOR(SINE_IO) {
		SW = new SQWave_100KHz("SW");
		SW->out(SQ1_sig);

		S1FE = new sensor1_frontEnd("S1FE");
		S1FE->in(SQ1_sig);
		S1FE->out(sine_LV);

		Register_0 = new Register<500>("reg0");
		Register_0->OutEnable(Enable);
		Register_0->regIn(sine_LV);
		Register_0->Out(dataFromSineIO);
	}
};

////////////////////////////////////////////////////////
//////////////////////// Sensor 2 /////////////////////
//////////////////////////////////////////////////////

SC_MODULE(SQWave_200KHz) {
	sc_out <double> out;

	SC_CTOR(SQWave_200KHz) {
		SC_THREAD(wave);
	}
	void wave() {
		while (1) {
			out = 300000;  // in order to have a triangular waveform that passes through the ADC
			wait(2.5, SC_US);
			out = -300000;
			wait(2.5, SC_US);
		}

	}
};

SCA_TDF_MODULE(Sensor2) {
	sca_tdf::sca_de::sca_in<double> in;
	sca_tdf::sca_out<double> out;

	sca_core::sca_time TimeStep;
	sca_tdf::sca_ltf_nd Integrator;
	sca_util::sca_vector <double> num, den;
	Sensor2(sc_module_name, sca_core::sca_time T = sca_time(1, SC_US)) :TimeStep(T), out("outp"), in("inp") {}

	void set_attribute() {
		set_timestep(TimeStep);
	}

	void initialize() {
		num(0) = 1;
		den(0) = 0;  //// H(S) = 1/S
		den(1) = 1;
	}

	void processing() {
		out.write(Integrator(num, den, in.read(), 1));
	}
};

SC_MODULE(sensor2_frontEnd) {
	sc_in <double> in;
	sc_out<sc_lv<16>> out;

	Sensor2* sen;
	ADC* adc;
	TDF2DE* tdf2de;
	sca_tdf::sca_signal<double> Trig_sig;
	sca_tdf::sca_signal<sc_dt::sc_int<16>> adc_Trig;

	sensor2_frontEnd(sc_module_name) : in("in"), out("out"),
		Trig_sig("sign"), adc_Trig("ads") {
		sen = new Sensor2("sen2");
		sen->in(in);
		sen->out(Trig_sig);

		adc = new ADC("adc2", sca_time(0.5, SC_US), 0.6); // time step = 500 NS (due to the frequency of SQ wave (200KHz)), Vmax = 0.6
		adc->in(Trig_sig);
		adc->out(adc_Trig);

		tdf2de = new TDF2DE("tdf2de2", sca_time(0.5, SC_US));
		tdf2de->in(adc_Trig);
		tdf2de->out(out);

	}

};

SC_MODULE(TRI_IO) {
	sc_in <sc_logic> Enable;
	sc_out <sc_lv<16>> dataFromTriIO;

	SQWave_200KHz* SW;
	sensor2_frontEnd* S2FE;
	Register<250>* Register_1;

	sc_signal<double> SQ2_sig;
	sc_signal<sc_lv<16>> Trig_LV;

	SC_CTOR(TRI_IO) {
		SW = new SQWave_200KHz("SW");
		SW->out(SQ2_sig);

		S2FE = new sensor2_frontEnd("S1FE");
		S2FE->in(SQ2_sig);
		S2FE->out(Trig_LV);

		Register_1 = new Register<250>("reg1"); // 2 MHz
		Register_1->OutEnable(Enable);
		Register_1->regIn(Trig_LV);
		Register_1->Out(dataFromTriIO);
	}

};

/////////////////////////////////////////////////////////////////
//////////////////////////Timer/////////////////////////////////
///////////////////////////////////////////////////////////////

SC_MODULE(Timer_IO) {
	sc_in<sc_logic> startTimer;
	sc_out<sc_logic> timeOut;

	SC_CTOR(Timer_IO) {
		SC_THREAD(evl);
		sensitive << startTimer.pos();
	}
	void evl() {
		while (true) {
			timeOut = SC_LOGIC_0;
			wait(1, SC_MS);
			timeOut = SC_LOGIC_1;
			wait();
		}
	}
};

/////////////////////////////////////////////////////////////////
//////////////////////////Display///////////////////////////////
///////////////////////////////////////////////////////////////

SC_MODULE(Display_IO) {
	sc_in<sc_logic> clk;
	sc_in<sc_logic>  writeIO;
	sc_in<sc_lv<16>> AddrBus;
	sc_inout_rv<16> DataBus;

	sc_lv<16> sineDataPenaulty, triDataPenaulty;
	SC_CTOR(Display_IO) {
		SC_THREAD(evl);
		sensitive << clk.pos();

	}
	void evl() {
		while (true) {
			DataBus = "ZZZZZZZZZZZZZZZZ";
			if ((writeIO == SC_LOGIC_1) && (AddrBus.read().range(3, 0) == "1100")) { // decoding the address
				sineDataPenaulty = DataBus;
				if ((sineDataPenaulty == 0) || (sineDataPenaulty.bit(15) == '1')) {
					cout << "Sensor1 FAILED" << ", |Sensor1Data - Sensor1Threshhold| =" << -(sineDataPenaulty.to_int()) << endl;
				}
				else {
					cout << "Sensor1 PASSED" << ", |Sensor1Data - Sensor1Threshhold| =" << (sineDataPenaulty.to_int()) << endl;
				}
			}
			else if ((writeIO == SC_LOGIC_1) && (AddrBus.read().range(3, 0) == "1101")) {
				triDataPenaulty = DataBus;
				if ((triDataPenaulty == 0) || (triDataPenaulty.bit(15) == '1')) {
					cout << "Sensor2 FAILED" << ", |Sensor2Data - Sensor2Threshhold| =" << -(triDataPenaulty.to_int()) << endl;
				}
				else {
					cout << "Sensor2 PASSED" << ", |Sensor2Data - Sensor2Threshhold| =" << (triDataPenaulty.to_int()) << endl;
				}
			}
			wait();
		}
	}
};

/////////////////////////////////////////////////////////////////
//////////////////////////Decoder///////////////////////////////
///////////////////////////////////////////////////////////////

SC_MODULE(IO_Decoder) {
	sc_in<sc_lv<16>> DataFromSineIO, DataFromTriIO;
	sc_in<sc_logic> timeOut;
	sc_in<sc_logic> readIO, writeIO;
	sc_in<sc_lv<16>> AddrBus;
	sc_inout_rv<16> DataBus;
	sc_out<sc_logic> startTimer;
	sc_out<sc_logic> Enablereg0, Enablereg1;

	SC_CTOR(IO_Decoder) {
		SC_THREAD(evl);
		sensitive << DataFromSineIO << DataFromTriIO << AddrBus << DataBus << readIO << writeIO  << timeOut;
	}
	void evl() {
		while (true) {
			if (writeIO->read() == SC_LOGIC_1) {
				if ((AddrBus->read().bit(3) & ~AddrBus->read().bit(2) & ~AddrBus->read().bit(1) & ~AddrBus->read().bit(0)) == '1') { // enable the Timer 
					DataBus = "ZZZZZZZZZZZZZZZZ";
					startTimer = SC_LOGIC_1;
					Enablereg0 = SC_LOGIC_0;
					Enablereg1 = SC_LOGIC_0;
				}
				else { // free the bus
					startTimer = SC_LOGIC_0;
					DataBus = "ZZZZZZZZZZZZZZZZ";
					Enablereg0 = SC_LOGIC_0;
					Enablereg1 = SC_LOGIC_0;
				}
			}

			else if (readIO->read() == SC_LOGIC_1) {
				if ((AddrBus->read().bit(3) & ~AddrBus->read().bit(2) & ~AddrBus->read().bit(1) & AddrBus->read().bit(0)) == '1') { // read the result of timer
					if (timeOut.read() == SC_LOGIC_1) {
						startTimer = SC_LOGIC_0;
						Enablereg0 = SC_LOGIC_0;
						Enablereg1 = SC_LOGIC_0;
						DataBus = "0000000000000001";
					}
					else {
						startTimer = SC_LOGIC_0;
						Enablereg0 = SC_LOGIC_0;
						Enablereg1 = SC_LOGIC_0;
						DataBus = "0000000000000000";
					}
				}
				else if ((~AddrBus->read().bit(3) & ~AddrBus->read().bit(2) & ~AddrBus->read().bit(1) & ~AddrBus->read().bit(0)) == '1') { // read the result of Sine
					Enablereg0 = SC_LOGIC_1;
					Enablereg1 = SC_LOGIC_0;
					startTimer = SC_LOGIC_0;
					DataBus = DataFromSineIO;
				}
				else if ((~AddrBus->read().bit(3) & AddrBus->read().bit(2) & ~AddrBus->read().bit(1) & ~AddrBus->read().bit(0)) == '1') { // read the result of Tri
					DataBus = DataFromTriIO;
					Enablereg1 = SC_LOGIC_1;
					Enablereg0 = SC_LOGIC_0;
					startTimer = SC_LOGIC_0;
				}

				else { // free the bus
					DataBus = "ZZZZZZZZZZZZZZZZ";
					startTimer = SC_LOGIC_0;
					Enablereg0 = SC_LOGIC_0;
					Enablereg1 = SC_LOGIC_0;
				}
			}
			else { // free the bus
				DataBus = "ZZZZZZZZZZZZZZZZ";
				startTimer = SC_LOGIC_0;
				Enablereg0 = SC_LOGIC_0;
				Enablereg1 = SC_LOGIC_0;
			}
			wait();
		}
	}
};


//////////////////////////////////////////////////////////////
////////////////////////SAYAC Bracketing/////////////////////
////////////////////////////////////////////////////////////

SC_MODULE(SAYACBracketing) {
	sc_in<sc_logic> clk;
	sc_out<sc_logic> readIO, writeIO;
	sc_inout_rv<16> DataBus;
	sc_out<sc_lv<16>> AddrBus;

	sc_lv<16> SineData, TriData;

	SC_CTOR(SAYACBracketing) {
		SC_THREAD(veryAbstract);
		sensitive << clk.pos();
	}
	void veryAbstract() {
		while (true) {

			writeIO = SC_LOGIC_1;
			AddrBus = "1111111100011000"; // enable the timer
			DataBus = "ZZZZZZZZZZZZZZZZ";
			wait();

			writeIO = SC_LOGIC_0;

			while (DataBus.read() != "0000000000000001") { // waiting till the time out 
				readIO = SC_LOGIC_1;
				AddrBus = "1111111100011001";
				wait();
			}

			readIO = SC_LOGIC_1;
			AddrBus = "1111111100010000"; // read the data of Sine

			wait();
			SineData = DataBus.read();
			cout << "Data on the Bus: " << DataBus.read() << endl;
			cout << "Sine data : " << SineData << "     Sensor1's data recieved at " << sc_time_stamp() << endl << endl;
			

			readIO = SC_LOGIC_1;
			AddrBus = "1111111100010100"; // read data of Tri

			wait();
			TriData = DataBus.read();
			cout << "Data on the Bus: " << DataBus.read() << endl;
			cout << "Tri data : " << TriData << "     Sensor2's data recieved at " << sc_time_stamp() << endl << endl;


			//comparing with Vth
			readIO = SC_LOGIC_0;
			sc_lv<16> diff1 = SineData.to_int() - 1; // Vth1 = 1
			sc_lv<16> diff2 = TriData.to_int() - 2; // Vth2 = 2
			wait();

			cout << endl;
			if (diff1.to_int() <= 0) {
				cout << "time: " << sc_time_stamp() << endl;
				cout << "sensor1 Data is" << SineData <<"   and the difference from Vth is: " << diff1.to_int() << endl;;
				cout << " ----------------------Sensor 1 FAILED!--------------------" << endl << endl;
			}
			else {
				cout << "time: " << sc_time_stamp() << endl;
				cout << "sensor1 Data is" << SineData << "   and the difference from Vth is: " << diff1.to_int() << endl;;
				cout << " ----------------------Sensor 1 PASSED!--------------------" << endl << endl;
			}
			wait();

			
			if (diff2.to_int() <= 0) {
				cout << "time: " << sc_time_stamp() << endl;
				cout << "sensor2 Data is" << TriData << "   and the difference from Vth1(1) is: " << diff2.to_int() << endl;;
				cout << " ----------------------Sensor 2 FAILED!--------------------" << endl << endl;
			}
			else {
				cout << "time: " << sc_time_stamp() << endl;
				cout << "sensor2 Data is" << TriData << "   and the difference from Vth2(2) is: " << diff2.to_int() << endl;;
				cout << " ----------------------Sensor 2 PASSED!--------------------" << endl << endl;
			}
			wait();

		}
	}
};

//////////////////////////////////////////////////////////////
//////////////////Embeded System Bracketing//////////////////
////////////////////////////////////////////////////////////

SC_MODULE(EmbededBracketing) {
	sc_signal<sc_logic> clk;
	sc_signal<sc_logic> readIO, writeIO;
	sc_signal < sc_lv<16>> addrBus;
	sc_signal_rv<16> dataBus;
	sc_signal<sc_lv<16>> dataFromSlave0, dataFromSlave1;
	sc_signal<sc_logic> startTimer, timeOut, enableReg0, enableReg1;

	SAYACBracketing* P;
	Timer_IO* T;
	SINE_IO* S0;
	TRI_IO* S1;
	IO_Decoder* D;

	SC_CTOR(EmbededBracketing) {
		T = new Timer_IO("timer");
		T->startTimer(startTimer);
		T->timeOut(timeOut);

		S0 = new SINE_IO("s0");
		S0->Enable(enableReg0);
		S0->dataFromSineIO(dataFromSlave0);

		S1 = new TRI_IO("s1");
		S1->Enable(enableReg1);
		S1->dataFromTriIO(dataFromSlave1);

		D = new IO_Decoder("d");
		D->DataFromSineIO(dataFromSlave0);
		D->DataFromTriIO(dataFromSlave1);
		D->timeOut(timeOut);
		D->readIO(readIO);
		D->writeIO(writeIO);
		D->AddrBus(addrBus);
		D->DataBus(dataBus);
		D->startTimer(startTimer);
		D->Enablereg0(enableReg0);
		D->Enablereg1(enableReg1);

		P = new SAYACBracketing("SP");
		P->clk(clk);
		P->readIO(readIO);
		P->writeIO(writeIO);
		P->DataBus(dataBus);
		P->AddrBus(addrBus);

		SC_THREAD(clocking);
	}

	void clocking() {
		while (true) {
			clk = SC_LOGIC_0;
			wait(5, SC_NS);
			clk = SC_LOGIC_1;
			wait(5, SC_NS);
		}
	}
};
