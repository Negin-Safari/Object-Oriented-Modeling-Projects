#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include "functions.h"

using namespace std;


class QM {
	list<string> minterm;
	list<string> PIs;
	list<string> EPIs;
	list <list <string>> cubes;
	list <list <string>> num_of_ones;

public:
	QM() {};

	list<string> get_EPIs() {
		return EPIs;
	}
	
	void print_EPIs() {
		list<string> ::iterator it = EPIs.begin();
		cout << "EPIs : ";
		for (int i = 0; i < EPIs.size(); i++) {
			cout << (*it) << "  ";
			it++;
		}
		cout << endl;
	}

	void execute_minterm(vector <string> line) {
		for (int i = 4; i < line.size(); i++) {
			string dummy;
			while (line[i] != "|" && line[i] != ";") {
				if (line[i][0] == '~')
					dummy = dummy + '0';
				else
					dummy = dummy + '1';
				i++;
			}
			minterm.push_back(dummy);
		}
		cubes.push_back(minterm);  // minterms are 0 cubes
	}

	void fill_num_of_ones() {
		int count;
		list<string> ones;
		list<list<string>>::iterator it = cubes.begin();
		for (int i = 0; i < cubes.size() - 1; i++) {
			it++;
		}
		list<string> minterm = *it;
		for (int i = 0; i < minterm.size(); i++) {
			count = 0;
			string dummy = read_From_list(minterm, i);
			for (int j = 0; j < dummy.size(); j++) {
				if (dummy[j] == '1')
					count++;
			}
			ones.push_back(to_string(count));
		}
		num_of_ones.push_back(ones);
	}

	int create_new_cube() {

		list< list<string>>::iterator cube = cubes.begin();
		for (int i = 0; i < cubes.size() - 1; i++) { //achieve last cube
			cube++;
		}
		list<string> it = *cube;
		list<string> new_cube;
		list< list<string>>::iterator cuber = num_of_ones.begin();
		for (int i = 0; i < num_of_ones.size() - 1; i++) { //achieve last cube num of ones
			cuber++;
		}
		list<string> iter = *cuber;
		vector<int> checklist(it.size(), 0); // a check list for each of terms in the cube
		for (int i = 0; i < it.size(); i++) {
			int refnum = stoi(read_From_list(iter, i));
			int has_match = 0;
			for (int j = i + 1; j < it.size(); j++) {  // check the ref term with others
				int num = stoi(read_From_list(iter, j));
				if (abs(num - refnum) == 1) {  // if there was only 1 difference in num of ones 
					string ref = read_From_list(it, i);
					string check = read_From_list(it, j);
					int count = 0;
					string dummy;
					for (int k = 0; k < ref.size(); k++) {
						if (ref[k] != check[k]) {
							count++;
							dummy = dummy + "X";
						}
						else {
							dummy = dummy + ref[k];
						}
					}
					if (count == 1) { // this means that these 2 only had one difference *** new element in next cube ***
						new_cube.push_back(dummy);
						checklist[j] = 1;
						has_match = 1;  // in order to know that the current ref had a match
					}

				}
			}
			if (has_match == 1 || checklist[i] == 1) {
				erase_from_list(*cube, i - (iter.size() - (*cube).size())); // erasing the checked ref that had match or if it used to had one
			}

		}
		if (new_cube.size() != 0) {
			cubes.push_back(new_cube);
			return 1;
		}
		else {
			return 0;
		}
	}

	void delete_repetitives() { // deleting repetitive items in a cube and keep only one of them
		list< list<string>>::iterator cube = cubes.begin();
		for (int i = 0; i < cubes.size() - 1; i++) {
			cube++;
		}
		list<string> it = *cube;
		for (int i = 0; i < it.size(); i++) {
			string ref = read_From_list(it, i);
			for (int j = i + 1; j < it.size(); j++) {
				string check = read_From_list(it, j);
				if (check == ref) {
					erase_from_list(*cube, i - (it.size() - (*cube).size()));
				}
			}
		}
	}

	void create_cubes() {
		int check = 1;
		while (check) { // do it till it didnt make new cube
			fill_num_of_ones();
			check = create_new_cube();
			delete_repetitives();
		}

		list<list<string>> ::iterator it = cubes.begin();
		for (int i = 0; i < cubes.size(); i++) { //saving PIs in a list
			list<string> ::iterator  iter = (*it).begin();
			for (int j = 0; j < (*it).size(); j++) {
				PIs.push_back(*iter);
				iter++;
			}
			it++;
		}
	}

	void finding_EPIs() {
		list<list<int>> table; // a table to tik if a PI included a minterm

		list<string> ::iterator pi = PIs.begin();
		list<string> ::iterator mint = minterm.begin();

		for (int i = 0; i < PIs.size(); i++) {  // filling the table with 1 and 0
			list<int> check_line;
			for (int j = 0; j < minterm.size(); j++) {
				check_line.push_back(fill_table(*pi, *mint));
				mint++;
			}
			table.push_back(check_line);
			mint = minterm.begin();
			pi++;
		}

		mint = minterm.begin();
		pi = PIs.begin();

		list<list<int>> ::iterator row = table.begin();
		int idx = 0;
		int cnt = 0;
		int flag = 1;

		while (flag) {
			int i = 0;
			int flag1 = 1;

			int flag2 = 0;


			mint = minterm.begin();
			while ((i < minterm.size()) && flag1) {
				cnt = 0;
				idx = 0;
				row = table.begin();
				for (int j = 0; j < PIs.size(); j++) {
					list<int>::iterator column = (*row).begin();
					for (int k = 0; k < i; k++) {
						column++;
					}
					if (*column == 1) { // counting the ones in a column of table
						cnt++;
						idx = j;
					}
					row++;
				}

				if (cnt == 1) { // if it was one so the PI related to that 1 is an EPI
					flag1 = 0;
					row = table.begin();
					for (int k = 0; k < idx; k++) {
						pi++;
						row++;
					}
					EPIs.push_back(*pi); // pushing back the new EPI
					erase_from_list(PIs, idx); // erasing the EPI from PI list

					 // erasing the column related to EPI and that minterm in table:
					list<int> ::iterator itt = (*row).begin();                 ////
					int flag3 = 1;                                            /////
					while (flag3 == 1) {   // erasing the column related to EPI and that minterm in table
						mint = minterm.begin();
						int control = 0;
						itt = (*row).begin();
						for (int t = 0; t < (*row).size(); t++) {  // erasing all of the minterms that new EPI can include them
							if (*itt == 1) {
								control = 1;
								itt++;
								erase_column(table, t);
								minterm.erase(mint);  // erasing from minterm list
								break;
							}
							else {
								itt++;
								mint++;
							}

						}

						if (control == 0) {
							flag3 = 0;
						}
					}
					//////////////////////////////////////////////////////////////


					table.erase(row); // erasing the row in the table ( row of the new EPI)
					pi = PIs.begin();
					flag2 = 1;
				}

				if (flag1 == 1) {
					mint++;
					i++;
				}
				row = table.begin();
			}


			if ((flag2 == 0) && (table.size() != 0)) { // in this part we should choose a PIs covering most of left midterms
				int index = 0;
				int max = 0;
				row = table.begin();
				for (int j = 0; j < PIs.size(); j++) {
					int count = 0;
					list<int> ::iterator iter = (*row).begin();
					for (int k = 0; k < minterm.size(); k++) {
						count = count + *iter;
						iter++;
					}
					if (count > max) { // choosing a PI with the most Ones in front of it
						index = j;     // index of PI
						max = count;
					}
					row++;
				}
				pi = PIs.begin();
				row = table.begin();
				for (int j = 0; j < index; j++) {  // reaching the PI with maximum cover till now
					pi++;
					row++;
				}
				EPIs.push_back(*pi);
				PIs.erase(pi);


				// erasing the column related to EPI and that minterm in table:
				list<int> ::iterator itt = (*row).begin();               /////
				int flag3 = 1;                                          //////
				while (flag3 == 1) {
					mint = minterm.begin();
					int control = 0;
					itt = (*row).begin();
					for (int t = 0; t < (*row).size(); t++) {
						if (*itt == 1) {
							control = 1;
							itt++;
							erase_column(table, t);
							minterm.erase(mint);
							break;
						}
						else {
							itt++;
							mint++;
						}

					}

					if (control == 0) {
						flag3 = 0;
					}
				}
				/////////////////////////////////////////////////////////////
				table.erase(row);
			}

			///// ending the while:
			if (table.size() == 0) {
				flag = 0;
			}
			else {
				list<list<int>> ::iterator empty_itr = table.begin();
				if ((*empty_itr).size() == 0) {
					flag = 0;
				}
			}
		}



	}
};

struct and {
	string input1;
	string input2;
	string output;
	int delay = 5;
};

struct or {
	string input1;
	string input2;
	string output;
	int delay = 5;
};

struct not {
	string input1;
	string output;
	int delay = 3;
};

int if_not_existed(vector<not> &nots, string name);


void make_gates(vector <vector<string>> Data, vector<not> &nots, vector <and> &ands, vector< or > &ors, QM *current, int i);


vector<QM> find_assign_statements(vector <vector<string>> Data, vector<not> &nots, vector <and> &ands, vector< or > &ors);


void make_output_file(vector<not> nots, vector<and> ands, vector< or > ors, vector <vector<string>> Data);




int main() {
	vector <string> sv_Line;
	vector <vector<string>> Data;
	sv_Line = get_input("main.sv");
	Data = break_line_to_words_sv(sv_Line);

	vector<not> nots;
	vector <and> ands;
	vector < or > ors;

	vector<QM> qm = find_assign_statements(Data, nots, ands, ors);

	make_output_file(nots, ands, ors, Data);

	getchar();
	return 0;
}





int if_not_existed(vector<not> &nots, string name) {
	for (int i = 0; i < nots.size(); i++) {
		if (nots[i].input1 == name) {
			return 0;
		}
	}
	return 1;

}


void make_gates(vector <vector<string>> Data, vector<not> &nots, vector <and> &ands, vector< or > &ors, QM *current, int i) {


	list<string>epis = current->get_EPIs();
	list<string> ::iterator epi_p = epis.begin();
	// not gates:
	for (int j = 0; j < (*epi_p).size(); j++) {
		not newNot;
		newNot.delay = 3;
		if (Data[i][j + 4][0] != '~') {
			newNot.input1 = Data[i][j + 4];
			newNot.output = Data[i][j + 4] + "BAR";
			int check = if_not_existed(nots, newNot.input1);
			if (check == 1) {
				nots.push_back(newNot);
			}
		}
		else {
			newNot.input1 = Data[i][j + 4].substr(1);
			newNot.output = Data[i][j + 4].substr(1) + "BAR";
			int check = if_not_existed(nots, newNot.input1);
			if (check == 1) {
				nots.push_back(newNot);
			}
		}
	}

	// and gates:
	int cnt = 0;
	vector<string> temp_and_IO;
	vector<string> temp_or_IO;
	vector<string> inputs;
	vector<and> current_ands;
	and current_and;
	vector < or > current_ors;
	or current_or;
	int numwire = 0;
	vector<string> comparing(count_num_of_input(Data[i]));
	for (int t = 0; t < count_num_of_input(Data[i]); t++) {
		if (Data[i][4 + t][0] == '~') {
			comparing[t] = Data[i][4 + t].substr(1);
		}
		else {
			comparing[t] = Data[i][4 + t];
		}

	}
	for (int j = 0; j < epis.size(); j++) {
		cnt = 0;

		for (int k = 0; k < (*epi_p).size(); k++) {  // making 2 input ands for one of EPIs 
			if (((*epi_p)[k] == '1') || ((*epi_p)[k] == '0')) {
				cnt++;
				if (((*epi_p)[k] == '1')) {
					inputs.push_back(comparing[k]);
				}
				else {
					inputs.push_back(comparing[k] + "BAR");
				}
			}
			if (cnt == 2) { // we should make 2 input ands
				cnt = 0;
				current_and.input1 = inputs[0];
				current_and.input2 = inputs[1];
				temp_and_IO.push_back(Data[i][2] + to_string(numwire));
				current_and.output = temp_and_IO[temp_and_IO.size() - 1];
				current_ands.push_back(current_and);
				numwire++;
				inputs.clear();
			}
		}

		if (cnt == 1) { // if only one term was not X, or if the number of inputs other than X was odd
			current_and.input1 = inputs[0];
			inputs.clear();
			current_and.input2 = temp_and_IO[0]; // the other input is the output of first and
			temp_and_IO.erase(temp_and_IO.begin());
			temp_and_IO.push_back(Data[i][2] + to_string(numwire)); // temp has all ands outputs other than inner and
			current_and.output = temp_and_IO[temp_and_IO.size() - 1];
			numwire++;
			current_ands.push_back(current_and);
		}

		while (temp_and_IO.size() > 1) { // do this till there was only one and output for each EPI 
			if (temp_and_IO.size() % 2) { // odd number of inputs for new ands
				int size = temp_and_IO.size();
				for (int k = 0; k < size - 1; k = k + 2) {
					current_and.input1 = temp_and_IO[0];
					temp_and_IO.erase(temp_and_IO.begin());
					current_and.input2 = temp_and_IO[0];

					temp_and_IO.erase(temp_and_IO.begin());
					temp_and_IO.push_back(Data[i][2] + to_string(numwire));
					numwire++;
					current_and.output = temp_and_IO[temp_and_IO.size() - 1];
					current_ands.push_back(current_and);
				}
				current_and.input1 = temp_and_IO[0];
				temp_and_IO.erase(temp_and_IO.begin());

				current_and.input2 = temp_and_IO[0];
				temp_and_IO.erase(temp_and_IO.begin());

				temp_and_IO.push_back(Data[i][2] + to_string(numwire));
				numwire++;
				current_and.output = temp_and_IO[temp_and_IO.size() - 1];
				current_ands.push_back(current_and);
			}

			else {  // even number of inputs for new ands
				int size = temp_and_IO.size();
				for (int k = 0; k < size; k = k + 2) {
					current_and.input1 = temp_and_IO[0];
					temp_and_IO.erase(temp_and_IO.begin());
					current_and.input2 = temp_and_IO[0];
					temp_and_IO.erase(temp_and_IO.begin());
					temp_and_IO.push_back(Data[i][2] + to_string(numwire));
					numwire++;
					current_and.output = temp_and_IO[temp_and_IO.size() - 1];
					current_ands.push_back(current_and);
				}
			}

		}
		temp_or_IO.push_back(temp_and_IO[0]);
		temp_and_IO.clear();
		ands.insert(ands.end(), current_ands.begin(), current_ands.end());
		current_ands.clear();
		epi_p++;

	}

	//creating or;
	// the same thing that we did for and gates:
	while (temp_or_IO.size() > 1) {
		if (temp_or_IO.size() % 2) {
			int size = temp_or_IO.size();
			for (int k = 0; k < size - 1; k = k + 2) {
				current_or.input1 = temp_or_IO[0];
				temp_or_IO.erase(temp_or_IO.begin());
				current_or.input2 = temp_or_IO[0];
				temp_or_IO.erase(temp_or_IO.begin());
				temp_or_IO.push_back(Data[i][2] + to_string(numwire));
				numwire++;
				current_or.output = temp_or_IO[temp_or_IO.size() - 1];
				current_ors.push_back(current_or);
			}
			current_or.input1 = temp_or_IO[0];
			temp_or_IO.erase(temp_or_IO.begin());
			current_or.input2 = temp_or_IO[0];
			temp_or_IO.erase(temp_or_IO.begin());
			temp_or_IO.push_back(Data[i][2] + to_string(numwire));
			numwire++;
			current_or.output = temp_or_IO[temp_or_IO.size() - 1];
			current_ors.push_back(current_or);
		}
		else {
			int size = temp_or_IO.size();
			for (int k = 0; k < size - 1; k = k + 2) {
				current_or.input1 = temp_or_IO[0];
				temp_or_IO.erase(temp_or_IO.begin());
				current_or.input2 = temp_or_IO[0];
				temp_or_IO.erase(temp_or_IO.begin());
				temp_or_IO.push_back(Data[i][2] + to_string(numwire));
				numwire++;
				current_or.output = temp_or_IO[temp_or_IO.size() - 1];
				current_ors.push_back(current_or);
			}
		}

	}
	current_ors[current_ors.size() - 1].output = Data[i][2];
	temp_or_IO.clear();
	ors.insert(ors.end(), current_ors.begin(), current_ors.end());
	current_ors.clear();


}


vector<QM> find_assign_statements(vector <vector<string>> Data, vector<not> &nots, vector <and> &ands, vector< or > &ors) {
	vector<QM> result;

	for (int i = 0; i < Data.size(); i++) {
		if ((Data[i][0] == "assign") && (Data[i][5] != "?")) {
			QM *current = new QM;
			current->execute_minterm(Data[i]);
			current->create_cubes();
			current->finding_EPIs();
			current->print_EPIs();
			result.push_back(*current);
			//////////////////////////////
			make_gates(Data, nots, ands, ors, current, i);
		}
		else if ((Data[i][0] == "assign") && (Data[i][5] == "?")) {
			and and1;
			and and2;
			or or1;
			not not1;
			not1.input1 = Data[i][4];
			not1.output = Data[i][4] + "BAR";
			and1.input1 = Data[i][4];
			and1.input2 = Data[i][6];
			and1.output = Data[i][2] + "0";
			and2.input1 = Data[i][4] + "BAR";
			and2.input2 = Data[i][8];
			and2.output = Data[i][2] + "1";
			or1.input1 = Data[i][2] + "0";
			or1.input2 = Data[i][2] + "1";
			or1.output = Data[i][2];
			ands.push_back(and1);
			ands.push_back(and2);
			nots.push_back(not1);
			ors.push_back(or1);
		}



	}
	return result;
}


void make_output_file(vector<not> nots, vector<and> ands, vector< or > ors, vector <vector<string>> Data) {
	vector<string> inputs;
	vector<string> outputs;
	vector<string> wires;
	int flag;
	int flag1 = 1;
	int i = 0;
	while (flag1) {
		if (Data[0][i] == "input") {
			flag = 1;
			while (flag == 1) {
				if (Data[0][i] == "output") {
					flag = 0;
					inputs.pop_back();
					break;
				}
				i++;
				inputs.push_back(Data[0][i]);
			}
			while (flag == 0) {
				if (Data[0][i] == ";") {
					flag1 = 0;
					outputs.pop_back();
					break;
				}
				i++;
				outputs.push_back(Data[0][i]);
			}

		}
		i++;
	}

	for (int i = 0; i < nots.size(); i++) {
		for (int j = 0; j < outputs.size(); j++) {
			if (nots[i].output != outputs[j]) {
				wires.push_back(nots[i].output);
			}
		}
	}

	for (int i = 0; i < ands.size(); i++) {
		for (int j = 0; j < outputs.size(); j++) {
			if (ands[i].output != outputs[j]) {
				wires.push_back(ands[i].output);
			}
		}
	}

	for (int i = 0; i < ors.size(); i++) {
		for (int j = 0; j < outputs.size(); j++) {
			if (ors[i].output != outputs[j]) {
				wires.push_back(ors[i].output);
			}
		}
	}


	ofstream resultFile("result.sv");
	resultFile << "module " << Data[0][1] << " ( ";
	for (int i = 2; i < Data[0].size(); i++) {
		if ((Data[0][i] != "input") && (Data[0][i] != "output") && (Data[0][i] != ";")) {
			if (i == Data[0].size() - 2) {
				resultFile << Data[0][i] << " ); " << endl;
			}
			else if (i < Data[0].size() - 1) {
				resultFile << Data[0][i] << " , ";
			}
		}
	}


	for (int i = 0; i < inputs.size(); i++) {
		resultFile << "input " << inputs[i] << " ;" << endl;
	}
	for (int i = 0; i < outputs.size(); i++) {
		resultFile << "output " << outputs[i] << " ;" << endl;
	}

	for (int i = 0; i < wires.size(); i++) {
		resultFile << "wire " << wires[i] << " ;" << endl;
	}

	for (int i = 0; i < nots.size(); i++) {
		resultFile << "nand " << "#( " << nots[i].delay << " , " << nots[i].delay << " ) " << ('G' + nots[i].output) << " ( " << nots[i].output << " , " << nots[i].input1 << " , " << nots[i].input1 << " );" << endl;

	}
	for (int i = 0; i < ands.size(); i++) {
		resultFile << "and " << "#( " << ands[i].delay << " , " << ands[i].delay << " ) " << ('G' + ands[i].output) << " ( " << ands[i].output << " , " << ands[i].input1 << " , " << ands[i].input2 << " );" << endl;

	}
	for (int i = 0; i < ors.size(); i++) {
		resultFile << "or " << "#( " << ors[i].delay << " , " << ors[i].delay << " ) " << ('G' + ors[i].output) << " ( " << ors[i].output << " , " << ors[i].input1 << " , " << ors[i].input2 << " );" << endl;

	}


	resultFile << "endmodule" << endl;



}


