#include <systemc.h>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;


// buffer :

class put_if : virtual public sc_interface
{
public:
	virtual void put(sc_lv<32768>) = 0;
};

class get_if : virtual public sc_interface
{
public:
	virtual void get(sc_lv<32768> &) = 0;
};

class buffer : public put_if, public get_if
{
	bool full;
	sc_lv<32768> saved;
	sc_event put_event, get_event;
public:
	buffer() : full(false) {};
	~buffer() {};
	void put(sc_lv<32768> data) {
		if (full == true) wait(get_event);
		saved = data;
		full = true;
		put_event.notify();
	}
	void get(sc_lv<32768> &data) {
		if (full == false) wait(put_event);
		data = saved;
		full = false;
		get_event.notify();
	}
};

////////////////////


// modules:

SC_MODULE(GRAY_SCALE) {
	sc_in<sc_logic> clk, rst;
	sc_in<sc_logic> start;
	sc_out<sc_logic> done;
	sc_port<put_if> out;

	sc_uint<8> red[262144], blue[262144], green[262144];

	sc_lv<32768> gray;

	SC_CTOR(GRAY_SCALE) {
		SC_THREAD(sending);
		sensitive << clk.pos() << rst;
	}
	void sending() {
		while (true) {
			if (rst->read() == SC_LOGIC_1) {
				done = SC_LOGIC_0;
				ifstream redfile("red_pixel.txt");
				ifstream greenfile("green_pixel.txt");
				ifstream bluefile("blue_pixel.txt");

				string Rpix, Gpix, Bpix;
				int i = 0;
				gray = 0;
				while ((getline(redfile, Rpix)) && (getline(greenfile, Gpix)) && (getline(bluefile, Bpix))) {
					red[i] = 0.2126*stoi(Rpix);
					green[i] = 0.7152*stoi(Gpix);
					blue[i] = 0.0722*stoi(Bpix);

					i++;
					if (i == 262144) {
						break;
					}
				}
			}
			else if (start->read() == SC_LOGIC_1) {
				int blockNUM = 0;
				while (blockNUM < 64) {
					int PIXELindx = 0;
					for (int j = 8 * blockNUM; j < (8 * blockNUM + 8); j++) {
						for (int k = 512 * j; k < (512 * j + 512); k++) {
							gray.range(8 * PIXELindx + 7, 8 * PIXELindx) = (red[k].to_uint() + blue[k].to_uint() + green[k].to_uint()) / 3;
							PIXELindx++;
						}
						wait(clk->posedge_event());
					}
					out->put(gray);
					cout << "block of data number (" << blockNUM << ") was transmitted at: " << sc_time_stamp() << '\n';
					blockNUM++;
				}
				done = SC_LOGIC_1;
			}
			wait();
		}
	}
};


SC_MODULE(EDGE_DETECT) {
	sc_in<sc_logic> clk, rst, start;
	sc_out<sc_logic> done;
	sc_port<get_if> in;

	sc_lv <16> ResultMap[512][512];
	sc_lv<8> currentBlock[8][512];
	sc_lv<32768> datainput;


	SC_CTOR(EDGE_DETECT) {
		SC_THREAD(processing);
		sensitive << rst << clk.pos();
		SC_THREAD(file_generating);
		sensitive << done;
	}

	void processing() {
		while (true) {
			if (rst->read() == SC_LOGIC_1) {
				done = SC_LOGIC_0;
			}

			else if (start->read() == SC_LOGIC_1) {
				done = SC_LOGIC_0;
				int numOfSegment = 0;
				sc_lv<8> lastrow[512];
				for (int i = 0; i < 512; i++) { // initializing the last row of previous segment with 0
					lastrow[i] = 0;
				}

				while (numOfSegment < 64) {
					wait(clk->posedge_event());
					in->get(datainput); // waiting to get input

					for (int i = 0; i < 8; i++) {
						for (int j = 0; j < 512; j++) {
							currentBlock[i][j] = datainput.range((512 * 8 * i + 8 * j + 7), (512 * 8 * i + 8 * j));
						}
					}

					//first row of block:
					for (int j = 0; j < 512; j++) {
						if (j == 0) {
							ResultMap[8 * numOfSegment][j] = 8 * currentBlock[0][j].to_uint() - (currentBlock[0][j + 1].to_uint() + currentBlock[1][j].to_uint() + currentBlock[1][j + 1].to_uint() + lastrow[j].to_uint() + lastrow[j + 1].to_uint());
							if (numOfSegment != 0) {
								ResultMap[(numOfSegment - 1) * 8 + 7][j] = ResultMap[(numOfSegment - 1) * 8 + 7][j].to_uint() + (-1)*(currentBlock[0][j].to_uint() + currentBlock[0][j + 1].to_uint());
							}
						}
						else if (j == 511) {
							ResultMap[8 * numOfSegment][j] = 8 * currentBlock[0][j].to_uint() - (currentBlock[0][j - 1].to_uint() + currentBlock[1][j].to_uint() + currentBlock[1][j - 1].to_uint() + lastrow[j].to_uint() + lastrow[j - 1].to_uint());
							if (numOfSegment != 0) {
								ResultMap[(numOfSegment - 1) * 8 + 7][j] = ResultMap[(numOfSegment - 1) * 8 + 7][j].to_uint() + (-1)*(currentBlock[0][j].to_uint() + currentBlock[0][j - 1].to_uint());
							}
						}
						else {
							ResultMap[8 * numOfSegment][j] = 8 * currentBlock[0][j].to_uint() - (currentBlock[0][j - 1].to_uint() + currentBlock[0][j + 1].to_uint() + currentBlock[1][j - 1].to_uint() + currentBlock[1][j].to_uint() + currentBlock[1][j + 1].to_uint() + lastrow[j - 1].to_uint() + lastrow[j].to_uint() + lastrow[j + 1].to_uint());
							if (numOfSegment != 0) {
								ResultMap[(numOfSegment - 1) * 8 + 7][j] = ResultMap[(numOfSegment - 1) * 8 + 7][j].to_uint() + (-1)*(currentBlock[0][j - 1].to_uint() + currentBlock[0][j].to_uint() + currentBlock[0][j + 1].to_uint());
							}
						}
					}

					// middle rows of block:
					for (int i = 1; i < 7; i++) {
						for (int j = 0; j < 512; j++) {
							if (j == 0) {
								ResultMap[numOfSegment * 8 + i][j] = -1 * (currentBlock[i - 1][j].to_uint() + currentBlock[i - 1][j + 1].to_uint() + currentBlock[i][j + 1].to_uint() + currentBlock[i + 1][j].to_uint() + currentBlock[i + 1][j + 1].to_uint()) + 8 * currentBlock[i][j].to_uint();
							}
							else if (j == 511) {
								ResultMap[numOfSegment * 8 + i][j] = -1 * (currentBlock[i - 1][j].to_uint() + currentBlock[i - 1][j - 1].to_uint() + currentBlock[i][j - 1].to_uint() + currentBlock[i + 1][j].to_uint() + currentBlock[i + 1][j - 1].to_uint()) + 8 * currentBlock[i][j].to_uint();
							}
							else {
								ResultMap[numOfSegment * 8 + i][j] = -1 * (currentBlock[i - 1][j - 1].to_uint() + currentBlock[i - 1][j].to_uint() + currentBlock[i - 1][j + 1].to_uint() + currentBlock[i][j - 1].to_uint() + currentBlock[i][j + 1].to_uint() + currentBlock[i + 1][j - 1].to_uint() + currentBlock[i + 1][j].to_uint() + currentBlock[i + 1][j + 1].to_uint()) + 8 * currentBlock[i][j].to_uint();

							}
							if (i == 6) { // filling the last row
								lastrow[j] = currentBlock[i + 1][j];
							}
						}
					}

					//last row of block:
					for (int j = 0; j < 512; j++) {
						if (j == 0) {
							ResultMap[numOfSegment * 8 + 7][j] = 8 * currentBlock[7][j].to_uint() - (currentBlock[6][j].to_uint() + currentBlock[6][j + 1].to_uint() + currentBlock[7][j + 1].to_uint());
						}
						else if (j == 511) {
							ResultMap[numOfSegment * 8 + 7][j] = 8 * currentBlock[7][j].to_uint() - (currentBlock[6][j].to_uint() + currentBlock[6][j - 1].to_uint() + currentBlock[7][j - 1].to_uint());
						}
						else {
							ResultMap[numOfSegment * 8 + 7][j] = 8 * currentBlock[7][j].to_uint() - (currentBlock[6][j - 1].to_uint() + currentBlock[6][j].to_uint() + currentBlock[6][j + 1].to_uint() + currentBlock[7][j - 1].to_uint() + currentBlock[7][j + 1].to_uint());
						}
					}

					cout << "block of data number (" << numOfSegment << ") was received at: "
						<< sc_time_stamp() << endl;
					numOfSegment++;
					wait();
				}
				done = SC_LOGIC_1;
			}
			wait();

		}
	}

	void file_generating() {
		while (1) {
			if (done->read() == SC_LOGIC_1) {
				ofstream outputfile;
				outputfile.open("recievedImage.txt");
				for (int i = 0; i < 512; i++) {
					for (int j = 0; j < 512; j++) {
						outputfile << ResultMap[i][j].to_uint() << endl;
					}
				}
				cout << "Output file is made!" << endl;
				outputfile.close();
			}
			wait();
		}
	}
};


SC_MODULE(TB) {

	sc_signal<sc_logic> clk, rst, start, doneGray, doneEdge;

	buffer* B;
	GRAY_SCALE* G;
	EDGE_DETECT* E;

	SC_CTOR(TB) {
		B = new buffer();
		G = new GRAY_SCALE("gray_scale");
		G->clk(clk);
		G->rst(rst);
		G->out(*B);
		G->start(start);
		G->done(doneGray);
		E = new EDGE_DETECT("edge_detect");
		E->clk(clk);
		E->rst(rst);
		E->in(*B);
		E->start(start);
		E->done(doneEdge);

		SC_THREAD(clocking);
		SC_THREAD(signaling);
	}

	void clocking() {
		while (true)
		{
			clk = SC_LOGIC_0;
			wait(10, SC_NS);
			clk = SC_LOGIC_1;
			wait(10, SC_NS);
		}
	}
	void signaling() {
		while (true) {
			rst = SC_LOGIC_1;
			wait(10, SC_NS);
			rst = SC_LOGIC_0;
			start = SC_LOGIC_1;
			wait(20, SC_NS);
			start = SC_LOGIC_0;
			wait(2000, SC_NS);

			wait();
		}

	}
};

////////////////