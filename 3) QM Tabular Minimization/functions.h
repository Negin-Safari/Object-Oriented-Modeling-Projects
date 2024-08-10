#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>

using namespace std;





// read file
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

// decode SV file
vector <string> line_to_words_sv(string Line) {

	vector <string> All_Words;
	string Word = "";
	for (auto x : Line)
	{
		if (x == ' ' | x == '(' | x == ')' | x == ',' | x == '&')
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
				All_Words.push_back(Word);
			}
		}
	}
	All_Words.push_back(";");
	return All_Words;
}
vector <vector<string>> break_line_to_words_sv(vector<string> Line) {
	vector <vector<string>> Data;
	for (int i = 0; i < Line.size(); i++) {
		Data.push_back(line_to_words_sv(Line[i]));
	}
	return Data;
}

//decode text file
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
			all_words.push_back(word);
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

string read_From_list(list <string> line, int i) {
	string result;
	list<string>::iterator it = line.begin();
	for (int j = 0; j < i; j++) {
		it++;
	}
	return *it;
}

void erase_from_list(list<string> &line, int i) {
	list<string>::iterator it = line.begin();
	for (int j = 0; j < i; j++) {
		it++;
	}
	line.erase(it);

}

list <string> execute_minterm(vector <string> line) {
	list <string> result;
	for (int i = 4; i < line.size(); i++) {
		string dummy;
		while (line[i] != "|" && line[i] != ";") {
			if (line[i][0] == '~')
				dummy = dummy + '0';
			else
				dummy = dummy + '1';
			i++;
		}
		result.push_back(dummy);
	}
	return result;
}

int num_of_elements(list<list<string>> a) {
	int size = 0;
	list<list<string>> ::iterator it = a.begin();
	for (int i = 0; i < a.size(); i++) {
		size = size + (*it).size();
		it++;
	}
	return size;
}

int fill_table(string PI, string minterm) {
	for (int i = 0; i < PI.size(); i++) {
		if ((PI[i] == '1' && minterm[i] == '0') || (minterm[i] == '1' && PI[i] == '0')) {
			return 0;
		}
	}
	return 1;
}


void erase_column(list<list<int>> &table, int col_idx) {
	if (table.size() != 0) {
		list<list<int>> ::iterator row = table.begin();
		list<int > ::iterator col = (*row).begin();

		for (int i = 0; i < table.size(); i++) {
			col = (*row).begin();
			for (int j = 0; j < col_idx; j++) {
				col++;
			}
			(*row).erase(col);
			row++;
		}
	}

}

int count_num_of_input(vector<string> line) {
	int count = 0;
	for (int i = 4; i < line.size(); i++) {
		if (line[i] == "|") {
			break;
		}
		else {
			count++;
		}
	}
	return count;
}
