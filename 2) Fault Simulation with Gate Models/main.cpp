#include <iostream>
#include <string>
#include <vector>
#include <fstream>

///////////////////////////////////////////////////////// Negin Safari 810197525 CA2

using namespace std;

#define NOT_DEFINED -1
#define GOOD_SIMULATION 0
#define FAULT_SIMULATION 1

class WIRE {
	string name;
	string type;
	char Faulty_Wire;
	int input_of_gate;
	int output_of_gate;
	vector <char> Fault_simulation_Values;
	vector <char> Good_simulation_values;
	vector <string> possible_stucks;
	vector <int> stuck_checkList;

public:
	WIRE() {
		Faulty_Wire = 'X';
		name = "";
	}

	WIRE(char a, vector <char> b, string n) {
		Faulty_Wire = a;
		Fault_simulation_Values = b;
		name = n;
	}

	void fill_output_of_gate(int a) {
		output_of_gate = a;
	}

	void fill_name(string n) {
		name = n;
	}

	void fill(char a) {
		Faulty_Wire = a;
	}

	void pushback_fault_value(char a) {
		Fault_simulation_Values.push_back(a);
	}

	void fill_type(string t) {
		type = t;
	}

	void fill_possible_stucks(vector<string> a) {
		for (int i = 0; i < a.size(); i++) {
			stuck_checkList.push_back(0);
		}
		possible_stucks = a;
	}

	void tik_possible_stucks(int a) {
		stuck_checkList[a] = 1;
	}

	void push_back_goodsimulation(char a) {
		Good_simulation_values.push_back(a);
	}

	char get_faultywire() {
		return Faulty_Wire;
	}

	int get_output_of_gate() {
		return output_of_gate;
	}

	vector <char> get_faultvalue() {
		return Fault_simulation_Values;
	}

	vector <char> get_good_simulation() {
		return Good_simulation_values;
	}

	void fill_gateIdx(int a) {
		input_of_gate = a;
	}

	string get_type() {
		return type;
	}

	string get_name() {
		return name;
	}

	int get_gateIdx() {
		return input_of_gate;
	}

	void clear_fault_value() {
		Fault_simulation_Values.clear();
	}

	vector<int> get_marked() {
		return stuck_checkList;
	}

	vector <string> get_possible_stucks() {
		return possible_stucks;
	}

	~WIRE() {};
};

class GATE {
	string operation;
	WIRE* first_input;
	WIRE* second_input;
	vector<WIRE*>output;

public:
	GATE() {}

	GATE(WIRE* a, WIRE* b, vector<WIRE*> c, string op) {
		first_input = a;
		second_input = b;
		output = c;
		operation = op;
	}

	void fill_operation(string op) {
		operation = op;
	}

	void fill_data(WIRE* a, WIRE* b, vector<WIRE*> c) {
		first_input = a;
		second_input = b;
		output = c;
	}

	string get_operation() {
		return operation;
	}

	void fault_collapsing() {
		if (operation == "nand") {
			first_input->fill_possible_stucks({ "SA1" });
			second_input->fill_possible_stucks({ "SA1" });
		}
		else if (operation == "nor") {
			first_input->fill_possible_stucks({ "SA0" });
			second_input->fill_possible_stucks({ "SA0" });
		}
		else if (operation == "fanout") {
			first_input->fill_possible_stucks({ "SA0" ,"SA1" });

		}
	}

	void eval(int j) {
		char value;
		if (j == GOOD_SIMULATION) {//good simulation
			for (int i = 0; i < first_input->get_good_simulation().size(); i++) {
				if (operation == "nand") {
					if (first_input->get_good_simulation()[i] == '0' || second_input->get_good_simulation()[i] == '0')
						output[0]->push_back_goodsimulation('1');
					else
						output[0]->push_back_goodsimulation('0');
				}
				else if (operation == "nor") {
					if (first_input->get_good_simulation()[i] == '1' || second_input->get_good_simulation()[i] == '1')
						output[0]->push_back_goodsimulation('0');
					else
						output[0]->push_back_goodsimulation('1');
				}
				else if (operation == "not") {
					if (first_input->get_good_simulation()[i] == '1')
						output[0]->push_back_goodsimulation('0');
					else
						output[0]->push_back_goodsimulation('1');
				}
				else if (operation == "fanout") {
					if (first_input->get_good_simulation()[i] == '1')
						value = '1';
					else
						value = '0';
					for (int k = 0; k < output.size(); k++) {
						output[k]->push_back_goodsimulation(value);
					}
				}
			}
		}
		else if (j == FAULT_SIMULATION) { // fault simulation
			for (int i = 0; i < first_input->get_faultvalue().size(); i++) {
				if (output[0]->get_faultywire() == '0') { // there is no faulty on this wire so we start the calculation
					if (operation == "nand") {
						if (first_input->get_faultvalue()[i] == '0' || second_input->get_faultvalue()[i] == '0')
							output[0]->pushback_fault_value('1');
						else
							output[0]->pushback_fault_value('0');
					}
					else if (operation == "nor") {
						if (first_input->get_faultvalue()[i] == '1' || second_input->get_faultvalue()[i] == '1')
							output[0]->pushback_fault_value('0');
						else
							output[0]->pushback_fault_value('1');
					}
					else if (operation == "not") {
						if (first_input->get_faultvalue()[i] == '1')
							output[0]->pushback_fault_value('0');
						else
							output[0]->pushback_fault_value('1');
					}
				}
				// fan out has more than 1 output so it is different :
				if (operation == "fanout") {
					if (first_input->get_faultvalue()[i] == '1')
						value = '1';
					else
						value = '0';
					for (int k = 0; k < output.size(); k++) {
						if (output[k]->get_faultywire() == '0') { // if the fault was not on this output we assign the value
							output[k]->pushback_fault_value(value);
						}
					}
				}
			}
		}

	}

	void push_back_output(WIRE* a) {
		output.push_back(a);
	}

	vector<WIRE*> get_output() {
		return output;
	}

	vector<WIRE*> get_input() {
		vector<WIRE*> temp = { first_input,second_input };
		return temp;
	}

	~GATE() {};
};



vector <string> line_to_words_txt(string Line);

vector <vector<string>> break_line_to_words_txt(vector<string> Line);

vector <string> get_input(string filename);

vector <string> line_to_words_sv(string Line);

vector <vector<string>> break_line_to_words_sv(vector<string> Line);

WIRE generate_wires(vector<string> line);

void execute_fanout(vector<WIRE> &wires, vector<GATE> &gates, int idx_fanout_input);

void generate_gates(vector<string> line, vector<WIRE> &wires, vector<GATE> &gates);

void decode_svFile(vector <vector<string>> wordsSV, vector<WIRE> &wires, vector<GATE> &gates);

void sort_path(vector<int> &sort_check_list, vector <WIRE> wires, vector <GATE> gates, int index, vector <int> &sorted);

vector <int> sort_wires(vector <WIRE> wires, vector <GATE> gates);

void fault_collapsing(vector <GATE> &gates);

void get_input_values(vector <vector <string>> Input, vector <WIRE> &wires);

void empty_fault_simulation_results(vector <WIRE> &wires);

int assign_faultSim_value(vector <WIRE> &wires);

vector<int> generate_time(vector <vector <string>> wordsTXT);

void compare_results(vector <WIRE> &wires, int wire_index, vector<int> times, ofstream &myfile);

void simulation(vector <GATE> &gates, vector<int> sorted, vector <WIRE> &wires, vector <vector <string>> wordsTXT, ofstream &myfile);




int main() {

	ofstream myfile;
	myfile.open("Result.txt");

	vector<WIRE> wires;
	vector<GATE> gates;
	vector <int> sorted;

	vector <string> inputSV = get_input("IN.sv");
	vector <string> inputTXT = get_input("test.txt");
	wires.reserve(inputSV.size() * 3);  // for using pointers this is needed
	vector <vector<string>> wordsSV = break_line_to_words_sv(inputSV);
	vector <vector <string>> wordsTXT = break_line_to_words_txt(inputTXT);

	decode_svFile(wordsSV, wires, gates);
	sorted = sort_wires(wires, gates);
	fault_collapsing(gates);
	get_input_values(wordsTXT, wires);
	simulation(gates, sorted, wires, wordsTXT, myfile);

	getchar();
	return 0;
}





vector <string> line_to_words_txt(string Line) {
	vector <string> all_words;
	string word = "";

	while (true) {

		if (Line.size() && Line.at(0) != ' ') {

			word = word + Line[0];

			if (Line.size()>0)
				Line = Line.substr(1);
		}
		else {
			if (word.size()) {
				all_words.push_back(word);
			}
			word = "";
			if (Line.size()>0)
				Line = Line.substr(1);
			else
			{
				break;
			}

		}
	}
	return all_words;
}
vector <vector<string>> break_line_to_words_txt(vector<string> Line) {
	vector <vector<string>> Data;
	for (int i = 0; i < Line.size(); i++) {
		Data.push_back(line_to_words_txt(Line[i]));
	}
	return Data;
}

vector <string> get_input(string filename) {
	ifstream file(filename);
	string Str;
	vector <string> Line;
	while (getline(file, Str)) {
		if (Str.size() != 0)
			Line.push_back(Str);

	}
	return Line;
}

vector <string> line_to_words_sv(string Line) {

	vector <string> All_Words;
	string Word = "";
	for (auto x : Line)
	{
		if (x == ' ' || x == '(' || x == ')' || x == ',' || x == ';')
		{
			if (Word != "") {
				All_Words.push_back(Word)

					;
			}
			Word = "";
		}
		else {
			Word = Word + x;

			if (Word == "endmodule") {
				All_Words.push_back(Word)

					;
			}
		}
	}
	return All_Words;
}

vector <vector<string>> break_line_to_words_sv(vector<string> Line) {
	vector <vector<string>> Data;
	for (int i = 0; i < Line.size(); i++) {
		Data.push_back(line_to_words_sv(Line[i]));
	}
	return Data;
}

WIRE generate_wires(vector<string> line) {
	WIRE temp;
	temp.fill_name(line[1]);
	temp.fill_type(line[0]);
	if (line[0] == "output") {
		temp.fill_possible_stucks({ "SA0","SA1" });
	}
	temp.fill_gateIdx(NOT_DEFINED);
	temp.fill_output_of_gate(NOT_DEFINED);

	return temp;
}

void execute_fanout(vector<WIRE> &wires, vector<GATE> &gates, int idx_fanout_input) {
	GATE new_gate;
	WIRE current_wire;
	WIRE current_wire1;
	int gate_idx = wires[idx_fanout_input].get_gateIdx();
	int new_wire_idx = 0;
	string new_wire_name;
	if (gate_idx == -1) { // a new fan out (both inputs of a gate are the same)
		gates.push_back(new_gate);
		current_wire.fill_gateIdx(gate_idx);
		current_wire.fill_name(wires[idx_fanout_input].get_name() + "0"); // outputs of fan out has the name of input with an index
		current_wire.fill_type("wire");
		current_wire.fill_output_of_gate(gates.size() - 1);
		wires[idx_fanout_input].fill_gateIdx(gates.size() - 1);
		current_wire1.fill_name(wires[idx_fanout_input].get_name() + "1");
		current_wire1.fill_type("wire");
		current_wire1.fill_output_of_gate(gates.size() - 1);
		wires.push_back(current_wire);
		wires.push_back(current_wire1);
		gates[gates.size() - 1].fill_operation("fanout");
		gates[gates.size() - 1].fill_data(&wires[idx_fanout_input], &wires[idx_fanout_input], { &wires[wires.size() - 2], &wires[wires.size() - 1] });
	}

	else if (gates[gate_idx].get_operation() == "fanout") { // this wire is already input of a fan out
		new_wire_idx = new_wire_idx + gates[gate_idx].get_output().size();
		new_wire_name = wires[idx_fanout_input].get_name() + to_string(new_wire_idx);
		current_wire.fill_name(new_wire_name);
		current_wire.fill_type("wire");
		current_wire.fill_output_of_gate(gate_idx);
		wires.push_back(current_wire);
		gates[gate_idx].push_back_output(&wires[wires.size() - 1]);
	}

	else { // this wire used to be the input of gates other than fan out *
		gates.push_back(new_gate);
		current_wire.fill_gateIdx(gate_idx); // the out put of this new fan out should be the input of the mentioned gate *
		current_wire.fill_name(wires[idx_fanout_input].get_name() + "0");
		current_wire.fill_type("wire");
		current_wire.fill_output_of_gate(gates.size() - 1); // the last gate added to the gates is the new fan out
		wires[idx_fanout_input].fill_gateIdx(gates.size() - 1);
		current_wire1.fill_name(wires[idx_fanout_input].get_name() + "1");
		current_wire1.fill_type("wire");
		current_wire1.fill_output_of_gate(gates.size() - 1);
		wires.push_back(current_wire);
		wires.push_back(current_wire1);
		gates[gates.size() - 1].fill_operation("fanout");
		gates[gates.size() - 1].fill_data(&wires[idx_fanout_input], &wires[idx_fanout_input], { &wires[wires.size() - 2], &wires[wires.size() - 1] });

		if (gates[gate_idx].get_input()[0] == &wires[idx_fanout_input]) { // connecting the first fan out output to the mentioned gate *
			gates[gate_idx].fill_data(&wires[wires.size() - 2], gates[gate_idx].get_input()[1], gates[gate_idx].get_output());
			//cout << gates[gate_idx].get_input()[0] << endl;
			//cout << &wires[wires.size() - 2] << endl;
		}

		if (gates[gate_idx].get_input()[1] == &wires[idx_fanout_input]) { // connecting the second fan out output to the mentioned gate * if it was needed
			gates[gate_idx].fill_data(gates[gate_idx].get_input()[0], &wires[wires.size() - 2], gates[gate_idx].get_output());
		}

	}



}

void generate_gates(vector<string> line, vector<WIRE> &wires, vector<GATE> &gates) {
	GATE temp;
	vector<WIRE*> currentOutput;
	vector<WIRE*> currentInput;
	int startIO = 2;
	temp.fill_operation(line[0]);
	if (line[1] == "#") { // in case the gate had delay
		startIO = 5;
	}

	for (int i = 0; i < wires.size(); i++) {
		if (wires[i].get_name() == line[startIO]) {  // selected wire is the output of current gate
			currentOutput.push_back(&wires[i]);
		}
		if (line[0] == "not") {
			if (wires[i].get_name() == line[startIO + 1]) { // selected wire is the input of current gate
				if (wires[i].get_gateIdx() == NOT_DEFINED) { // input fan out nabashe

					currentInput.push_back(&wires[i]);
					currentInput.push_back(&wires[i]);
				}
				else { // this input came from a fan out
					execute_fanout(wires, gates, i);
					currentInput.push_back(&wires[wires.size() - 1]);
					currentInput.push_back(&wires[wires.size() - 1]);
				}
			}
		}

		else {
			if (wires[i].get_name() == line[startIO + 1] || wires[i].get_name() == line[startIO + 2]) {
				if (wires[i].get_gateIdx() == -1) {  // input fan out nabashe
					currentInput.push_back(&wires[i]);
				}
				else {
					execute_fanout(wires, gates, i);
					currentInput.push_back(&wires[wires.size() - 1]);
					//cout << &wires[wires.size() - 2] << "   " << wires[wires.size() - 2].get_name() << endl;
				}
				if (line[startIO + 1] == line[startIO + 2]) {
					execute_fanout(wires, gates, i); // if inputs are the same this can be a new fan out output
					currentInput.clear();
					currentInput.push_back(&wires[wires.size() - 2]);
					currentInput.push_back(&wires[wires.size() - 1]);
				}
			}
		}

	}
	temp.fill_data(currentInput[0], currentInput[1], currentOutput);
	gates.push_back(temp);
	vector<WIRE*> input = gates[gates.size() - 1].get_input(); // filling the gate idexes of wires that recently found their gates
	input[0]->fill_gateIdx(gates.size() - 1);
	input[1]->fill_gateIdx(gates.size() - 1);
	vector<WIRE*> output = gates[gates.size() - 1].get_output();
	for (int i = 0; i < output.size(); i++) {
		output[i]->fill_output_of_gate(gates.size() - 1);
	}


}

void decode_svFile(vector <vector<string>> wordsSV, vector<WIRE> &wires, vector<GATE> &gates) {
	for (int i = 0; i < wordsSV.size(); i++) {
		if (wordsSV[i][0] == "input" || wordsSV[i][0] == "output" || wordsSV[i][0] == "wire") {
			wires.push_back(generate_wires(wordsSV[i]));
		}
		else if (wordsSV[i][0] == "nand" || wordsSV[i][0] == "nor" || wordsSV[i][0] == "not") {
			generate_gates(wordsSV[i], wires, gates);
		}
	}


}

void sort_path(vector<int> &sort_check_list, vector <WIRE> wires, vector <GATE> gates, int index, vector <int> &sorted) {
	vector <WIRE*> gate_output;
	int gateindex = wires[index].get_gateIdx();
	if (gateindex != -1) {
		gate_output = gates[gateindex].get_output();
		for (int i = 0; i < wires.size(); i++) {
			for (int j = 0; j < gate_output.size(); j++) {
				if (wires[i].get_name() == gate_output[j]->get_name()) {
					if (sort_check_list[i] == 0) {
						sort_path(sort_check_list, wires, gates, i, sorted);
					}
				}
			}
		}
	}

	sort_check_list[index] = 1;
	sorted.push_back(index);
}

vector <int> sort_wires(vector <WIRE> wires, vector <GATE> gates) {
	vector <int> sorted;
	vector <int> sort_check_list(wires.size(), 0);
	for (int i = 0; i < wires.size(); i++) {
		if (sort_check_list[i] == 0) {
			sort_path(sort_check_list, wires, gates, i, sorted);
		}
	}
	return sorted;
}

void fault_collapsing(vector <GATE> &gates) {
	for (int i = 0; i < gates.size(); i++) {
		gates[i].fault_collapsing();
	}
}

void get_input_values(vector <vector <string>> Input, vector <WIRE> &wires) {
	int num = 0;
	for (int i = 0; i < wires.size(); i++) {
		if (wires[i].get_type() == "input") {
			for (int j = 0; j < Input.size(); j++) {
				wires[i].push_back_goodsimulation(Input[j][1][num]);
			}
			num++;
		}
	}
}

void empty_fault_simulation_results(vector <WIRE> &wires) {
	for (int i = 0; i < wires.size(); i++) {
		wires[i].clear_fault_value();
	}
}

int assign_faultSim_value(vector <WIRE> &wires) {
	vector<char> value;
	char stuck;
	int fault_is_selected = 0;
	for (int i = 0; i < wires.size(); i++) {
		wires[i].fill('0');
		if (wires[i].get_possible_stucks().size() != 0 && !fault_is_selected) { // get a wire that has possible stuck
			for (int j = 0; j < wires[i].get_possible_stucks().size(); j++) {
				if (wires[i].get_marked()[j] == 0) {
					if (wires[i].get_possible_stucks()[j] == "SA0") {
						stuck = '0';
					}
					else {
						stuck = '1';
					}
					value.assign(wires[i].get_good_simulation().size(), (stuck)); // filling the stucked wire values
					wires[i].tik_possible_stucks(j); // tik the checked stuck
					wires[i].fill('1');
					for (int k = 0; k < value.size(); k++) {
						wires[i].pushback_fault_value(value[k]);
					}
					fault_is_selected = 1;
					break;
				}
			}
		}

		if (wires[i].get_type() == "input" && wires[i].get_faultvalue().size() == 0) { // if the input doesnt have stuck it should get the test vector values
			for (int j = 0; j < wires[i].get_good_simulation().size(); j++) {
				wires[i].pushback_fault_value(wires[i].get_good_simulation()[j]);
			}
		}

	}


	return fault_is_selected;
}

vector<int> generate_time(vector <vector <string>> wordsTXT) {
	vector<int> result;
	int temp = 0;
	for (int i = 0; i < wordsTXT.size(); i++) {
		for (int j = 0; j < 1; j++) {
			wordsTXT[i][j].erase(wordsTXT[i][j].begin());
			temp = temp + stoi(wordsTXT[i][j]);
			result.push_back(temp);
		}
	}
	return result;
}

void compare_results(vector <WIRE> &wires, int wire_index, vector<int> times, ofstream &myfile) {

	cout << " stucked wire name: " << wires[wire_index].get_name() << endl;
	cout << "				 *** stuck value: SA" << wires[wire_index].get_faultvalue()[0] << " ***" << endl;
	myfile << " stucked wire name: " << wires[wire_index].get_name() << endl;
	myfile << "				 *** stuck value: SA" << wires[wire_index].get_faultvalue()[0] << " ***" << endl;
	for (int i = 0; i < wires.size(); i++) {
		if (wires[i].get_type() == "output") {
			for (int j = 0; j < wires[i].get_good_simulation().size(); j++) {
				cout << " test case " << j << " at time " << times[j] << " :";
				myfile << " test case " << j << " at time " << times[j] << " :";
				if (wires[i].get_good_simulation()[j] == wires[i].get_faultvalue()[j]) {
					cout << "  no fault was detected            expected value: " << wires[i].get_good_simulation()[j] << "       current value: " << wires[i].get_faultvalue()[j] << endl;
					myfile << "  no fault was detected            expected value: " << wires[i].get_good_simulation()[j] << "       current value: " << wires[i].get_faultvalue()[j] << endl;
				}
				else
				{
					cout << "  fault was detected               expected value: " << wires[i].get_good_simulation()[j] << "       current value: " << wires[i].get_faultvalue()[j] << endl;
					myfile << "  fault was detected               expected value: " << wires[i].get_good_simulation()[j] << "       current value: " << wires[i].get_faultvalue()[j] << endl;
				}
			}
		}
	}
	cout << "-------------------------------------------------------------------------------------------------------" << endl;
	myfile << "-------------------------------------------------------------------------------------------------------" << endl;

}

void simulation(vector <GATE> &gates, vector<int> sorted, vector <WIRE> &wires, vector <vector <string>> wordsTXT, ofstream &myfile) {
	vector<int> times = generate_time(wordsTXT);
	int idx_of_working_gate;
	int flag = 1;
	int wire_index = -1;
	// good simulation:
	for (int i = sorted.size() - 1; i > -1; i--) {
		if (wires[sorted[i]].get_good_simulation().size() == 0) {
			idx_of_working_gate = wires[sorted[i]].get_output_of_gate();
			gates[idx_of_working_gate].eval(GOOD_SIMULATION);
		}
	}

	// fault simulation:
	while (flag) {
		flag = assign_faultSim_value(wires);
		if (flag) {
			for (int i = sorted.size() - 1; i > -1; i--) {
				if (wires[sorted[i]].get_faultywire() == '1') {
					wire_index = sorted[i]; // saving the index of the faulty wire
				}
				if (wires[sorted[i]].get_faultvalue().size() == 0) {
					idx_of_working_gate = wires[sorted[i]].get_output_of_gate();
					gates[idx_of_working_gate].eval(FAULT_SIMULATION);
				}
			}
			compare_results(wires, wire_index, times, myfile);
			empty_fault_simulation_results(wires);
		}
	}
}