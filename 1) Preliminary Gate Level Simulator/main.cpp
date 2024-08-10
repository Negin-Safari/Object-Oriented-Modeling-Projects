// Negin Safari    810197525
#include<iostream>
#include <vector>
#include <fstream>
#include <string>
using namespace std;

typedef struct gate gate;

typedef struct node node;

typedef struct nodeTransition nodeTransition;

typedef struct gateTransition gateTransition;

struct gate {
	int d1;
	int d0;
	string operation;
	string output;
	vector<string> input;
	vector<int> inputValue = {-1,-1};  //2input gates
	int outputValue;
};

struct node {
	string name;
	vector<nodeTransition> Trans;
	int idxOfGate;
};

struct nodeTransition {
	int value;
	int executeTime;
};

struct gateTransition {
	int input1;
	int input2;
	int executeTime;
};



nodeTransition nand(int d1, int d0, gateTransition g);

nodeTransition and (int d1, int d0, gateTransition g);

nodeTransition xor (int d1, int d0, gateTransition g);

nodeTransition or (int d1, int d0, gateTransition g);

nodeTransition nor(int d1, int d0, gateTransition g);

vector<string> getIN(string str);

vector<string> removeAdditionalWord(string str);

vector<vector<string>> splitIntoKeywords(vector<string> text);

vector<gate> generateGates(vector<vector<string>> v);

vector<vector<int>> getInputValue(vector < vector < string>> testTable);

vector<node> addInputs(vector<vector<string>>v, vector < vector < string>> testTable, int numOfCases);

void schedule(vector<node> &nodes, vector<gate> &gates);

void GATE(vector<gateTransition> &g, int d1, int d0, vector<nodeTransition> & Trans, string gateName); 

int getIndexOfGate(string str, vector<gate> &gates);

vector<string> generateOutputs(vector<vector<string>> v);

vector<node> printResult(vector<node>nodes, vector<string> outputs);

vector<gateTransition> generateGateTransitions(vector<nodeTransition>a, vector<nodeTransition>b);

void reachResult(vector<node> &nodes, vector<gate> &gates, int numofInputs);

vector<string> generateInputs(vector<vector<string>> v);

vector < vector < string>> provideInputFile(string filename);

void printOutputs(vector<node>nodes);


int main() {

	// get text(sv):
	vector < vector < string>> res = provideInputFile("IN.sv"); //res is the whole splitted sv input
	// generating gates:
	vector<gate> gates = generateGates(res);
	// generate outputs:
	vector<string> outputs = generateOutputs(res);
	// generating inputs:
	vector<string>inputs = generateInputs(res);
	// get test:
	vector < vector < string>> testTable = provideInputFile("test.txt");
	int numberOfCases = testTable.size(); //number of test cases 
	vector<node>nodes = addInputs(res, testTable, numberOfCases); // adds all transitions of inputs
    // scheduling nodes:
	schedule(nodes, gates);
	// solving:
	reachResult(nodes, gates, inputs.size());

	printOutputs(nodes);
	cin.get();
	return 0;
}



nodeTransition nand(int d1, int d0, gateTransition g) {
	nodeTransition y;
	y.value = -1;

	if (g.input1 == 0 || g.input2 == 0) {
		y.value = 1;
		y.executeTime = g.executeTime + d1;
	}

	else if (g.input1 == 1 && g.input2 == 1)
	{
		y.value = 0;
		y.executeTime = g.executeTime + d0;
	}

	return y;
}

nodeTransition and (int d1, int d0, gateTransition g) {
	nodeTransition y;
	y.value = -1;

	if (g.input1 == 0 || g.input2 == 0) {
		y.value = 0;
		y.executeTime = g.executeTime + d0;
	}

	else if (g.input1 == 1 && g.input2 == 1)
	{
		y.value = 1;
		y.executeTime = g.executeTime + d1;
	}

	return y;
}

nodeTransition xor (int d1, int d0, gateTransition g) {
	nodeTransition y;
	y.value = -1;

	if ((g.input1 == 1 && g.input2 == 0) || (g.input1 == 0 && g.input2 == 1)) {
		y.value = 1;
		y.executeTime = g.executeTime + d1;
	}

	else if ((g.input1 == 0 && g.input2 == 0) || (g.input1 == 1 && g.input2 == 1))
	{
		y.value = 0;
		y.executeTime = g.executeTime + d0;
	}

	return y;
}

nodeTransition or (int d1, int d0, gateTransition g) {
	nodeTransition y;
	y.value = -1;

	if (g.input1 == 1 || g.input2 == 1) {
		y.value = 1;
		y.executeTime = g.executeTime + d1;
	}

	else if (g.input1 == 0 && g.input2 == 0)
	{
		y.value = 0;
		y.executeTime = g.executeTime + d0;
	}

	return y;
}

nodeTransition nor(int d1, int d0, gateTransition g) {
	nodeTransition y;
	y.value = -1;

	if (g.input1 == 1 || g.input2 == 1) {
		y.value = 0;
		y.executeTime = g.executeTime + d0;
	}

	else if (g.input1 == 0 && g.input2 == 0)
	{
		y.value = 1;
		y.executeTime = g.executeTime + d1;
	}

	return y;
}

vector<string> getIN(string str) {
	ifstream file(str);
	string s;
	vector<string> line;
	while (getline(file, s)) {
		line.push_back(s);
	}

	file.close();

	return line;
}


vector<string> removeAdditionalWord(string str)
{
	vector<string> result;
	string word = "";
	for (auto x : str)
	{
		if ((x == ' ') || (x == '(')|| (x == ')') || (x == ',') || (x == ';') || (x == '\n'))
		{
			if (word != "") {
				result.push_back(word);
			}
			word = "";
		}
		else {
			word = word + x;


		}
	}
	result.push_back(word);
	return result;
}

vector<vector<string>> splitIntoKeywords(vector<string> text) {

	vector<vector<string>> result;
	for (int i = 0; i < text.size(); i++) {
		result.push_back(removeAdditionalWord(text[i]));
	}

	return result;
}



vector<gate> generateGates(vector<vector<string>> v) {
	vector<gate> G;
	string name;
	vector<string> in;
	for (int i = 0; i < v.size(); i++) {
		for (int j = 0; j < v[i].size(); j++) {
			if (v[i][j] == "nand" | v[i][j] == "or" | v[i][j] == "nor" | v[i][j] == "xor" | v[i][j] == "and") {
				gate g;
				g.operation = v[i][j];
				for (int k = 6; k < v[i].size(); k++) {
					(g.input).push_back(v[i][k]);
				}
				g.output = v[i][5];
				g.d1 = stoi(v[i][2]);
				g.d0 = stoi(v[i][3]);
				G.push_back(g);

			}
		}
	}

	return G;

}



vector<vector<int>> getInputValue(vector < vector < string>> testTable) {
	vector<vector<int>> values;

	for (int i = 0; i < testTable.size(); i++) {
		vector<int> b;
		b.push_back(stoi(testTable[i][0].substr(1, testTable[i][0].size())));
		for (int j = 0; j < testTable[i][1].size(); j++) {
			b.push_back(stoi(testTable[i][1].substr(j, 1)));
		}
		values.push_back(b);

	}

	return values;


}



vector<node> addInputs(vector<vector<string>>v, vector < vector < string>> testTable, int numOfCases) { // step=0 start
	vector<vector<int>> values = getInputValue(testTable);
	vector<node> result;
	int cnt = 0;
	for (int i = 0; i < v.size(); i++) {
		if (v[i][0] == "input") {
			node temp;
			for (int k = 0; k < numOfCases; k++) {
				//node temp;
				temp.name = v[i][1];
				int time = 0;
				for (int j = 0; j < k + 1; j++) {
					time = time + values[j][0];
				}
				nodeTransition current;
				current.executeTime = time;
				current.value = values[k][cnt + 1];
				// dar khone sefr delay hast, from index 1 value starts
				if (temp.Trans.size() != 0) {
					if (temp.Trans[temp.Trans.size() - 1].value != current.value) {
						temp.Trans.push_back(current);
					}
				}

				else {
					temp.Trans.push_back(current);
				}


			}
			result.push_back(temp);
			cnt++;
		}
	}

	return result;
}




void schedule(vector<node> &nodes, vector<gate> &gates) {

	vector<gate> tempGates = gates;
	int reach2 = 0;
	while (tempGates.size() != 0) {
		for (int i = 0; i < tempGates.size(); i++) { // kodom gate
			for (int j = 0; j < nodes.size(); j++) { // kodom node
				for (int k = 0; k < 2; k++) { // kodom input
					if ((tempGates[0].input)[k] == nodes[j].name) {
						reach2++;
					}

				}

			}

			if (reach2 == 2) {
				node temp;
				temp.name = tempGates[0].output;
				nodes.push_back(temp);
				tempGates.erase(tempGates.begin()); // pop front
			}

			else {
				tempGates.push_back(tempGates[0]);
				tempGates.erase(tempGates.begin());  // pop front
			}
			reach2 = 0;
		}

	}
}


void GATE(vector<gateTransition> &g, int d1, int d0, vector<nodeTransition> & Trans, string gateName) {
	nodeTransition result;
	for (int i = 0; i < g.size(); i++) {
		if (gateName == "nand") {
			result = nand(d1, d0, g[i]);
		}
		else if (gateName == "and") {
			result = and (d1, d0, g[i]);
		}
		else if (gateName == "xor") {
			result = xor (d1, d0, g[i]);
		}
		else if (gateName == "or") {
			result = or (d1, d0, g[i]);
		}
		else if (gateName == "nor") {
			result = nor(d1, d0, g[i]);
		}


		if (result.value != -1) {
			if (Trans.size() == 0) {
				Trans.push_back(result);
			}
			else if (Trans[Trans.size() - 1].executeTime > result.executeTime) { // the new result appears faster than the result of previous transition
				Trans.pop_back();
				if (Trans[Trans.size() - 1].value != result.value) { // if there was a change in value we will save it
					Trans.push_back(result);
				}
			}
			else {
				if (Trans[Trans.size() - 1].value != result.value) {
					Trans.push_back(result);
				}

			}

		}

	}


}

int getIndexOfGate(string str, vector<gate> &gates) {
	int result = -1;
	for (int i = 0; i < gates.size(); i++) {
		if (gates[i].output == str) {
			result = i;
			break;
		}
	}
	return result;
}









vector<string> generateOutputs(vector<vector<string>> v) {

	vector<string> output;
	for (int i = 0; i < v.size(); i++) {
		for (int j = 0; j < v[i].size(); j++) {
			if (v[i][j] == "output") {
				string str;
				str = v[i][j + 1];
				output.push_back(str);

			}
		}
	}

	return output;

}


vector<node> printResult(vector<node>nodes, vector<string> outputs) {
	vector<node> results;
	for (int i = 0; i < outputs.size(); i++) {
		for (int j = 0; j < nodes.size(); j++) {
			if (outputs[i] == nodes[j].name) {
				results.push_back(nodes[j]);

			}
		}
	}

	return results;
}


vector<gateTransition> generateGateTransitions(vector<nodeTransition>a, vector<nodeTransition>b) {
	vector<gateTransition> result;
	gateTransition temp;


	int aIdx = 0;
	int bIdx = 0;

	for (int i = 0; i < a.size() + b.size(); i++) { //maximum iterations


		if (bIdx < b.size() && aIdx < a.size()) {

			if (a[aIdx].executeTime > b[bIdx].executeTime) {
				temp.executeTime = b[bIdx].executeTime;
				temp.input1 = b[bIdx].value;
				if (result.size() != 0) {
					temp.input2 = result[result.size() - 1].input2;

				}
				else {
					temp.input2 = -1; // empty
				}


				bIdx++;

			}

			else if (a[aIdx].executeTime < b[bIdx].executeTime) {
				temp.executeTime = a[aIdx].executeTime;
				temp.input2 = a[aIdx].value;
				if (result.size() != 0) {
					temp.input1 = result[result.size() - 1].input1;

				}
				else {
					temp.input1 = -1; // empty
				}

				aIdx++;

			}

			else if (a[aIdx].executeTime == b[bIdx].executeTime) {
				temp.executeTime = a[aIdx].executeTime;
				temp.input2 = a[aIdx].value;
				temp.input1 = b[bIdx].value;


				aIdx++;
				bIdx++;

			}

		}

		else if ((aIdx >= a.size()) && (bIdx < b.size())) {
			temp.executeTime = b[bIdx].executeTime;
			temp.input1 = b[bIdx].value;
			temp.input2 = result[result.size() - 1].input2;
			bIdx++;
		}

		else if (bIdx >= b.size() && aIdx < a.size()) {
			temp.executeTime = a[aIdx].executeTime;
			temp.input1 = result[result.size() - 1].input1;
			temp.input2 = a[aIdx].value;
			aIdx++;
		}
		else {
			break;
		}

		result.push_back(temp);


	}


	return result;

}


void reachResult(vector<node> &nodes, vector<gate> &gates, int numofInputs) {



	for (int i = numofInputs; i < nodes.size(); i++) { // choose a node
		vector<nodeTransition> a;
		vector<nodeTransition> b;
		/////////
		int d1, d0;
		string gateName;
		for (int j = 0; j < gates.size(); j++) {
			if (gates[j].output == nodes[i].name) {   //which gates output is the chosen node
				d1 = gates[j].d1;
				d0 = gates[j].d0;
				gateName = gates[j].operation;
				for (int k = 0; k < nodes.size(); k++) {
					if (nodes[k].name == gates[j].input[0]) {
						a = nodes[k].Trans;
					}
					if (nodes[k].name == gates[j].input[1]) {
						b = nodes[k].Trans;
					}
				}
			}
		}
		////////////
		vector<gateTransition> gateTrans = generateGateTransitions(a, b);
		GATE(gateTrans, d1, d0, nodes[i].Trans, gateName);
		//cout << nodes[i].name << endl;
		/*
		for (int j = 0; j < gateTrans.size(); j++) {
		cout << "1: " << gateTrans[j].input1 << " 2:" << gateTrans[j].input2 <<"       "<< gateTrans[j].executeTime << endl;
		}
		*/
	}


}


vector<string> generateInputs(vector<vector<string>> v) {

	vector<string> input;
	for (int i = 0; i < v.size(); i++) {
		for (int j = 0; j < v[i].size(); j++) {
			if (v[i][j] == "input") {
				string str;
				str = v[i][j + 1];
				input.push_back(str);

			}
		}
	}

	return input;

}

vector < vector < string>> provideInputFile(string filename) {
	vector<string> line = getIN(filename);
	vector < vector < string>> res = splitIntoKeywords(line);
	return res;
}


void printOutputs(vector<node>nodes) {

	for (int i = 0; i < nodes.size(); i++) {
		cout << "  name:" << (nodes[i].name) << "  ->  ";
		for (int j = 0; j < (nodes[i].Trans).size(); j++) {
			cout << " value:" << (nodes[i].Trans)[j].value << "  time:" << (nodes[i].Trans)[j].executeTime;
			if (j != (nodes[i].Trans).size() - 1) {
				cout << "  /   ";
			}
		}

		cout << endl;
	}



	ofstream myfile;
	myfile.open("result.txt");

	for (int i = 0; i < nodes.size(); i++) {
		myfile << "  name:" << (nodes[i].name) << "  ->  ";
		for (int j = 0; j < (nodes[i].Trans).size(); j++) {
			myfile << " value:" << (nodes[i].Trans)[j].value << "  time:" << (nodes[i].Trans)[j].executeTime;
			if (j != (nodes[i].Trans).size() - 1) {
				myfile << "  /   ";
			}
		}

		myfile << endl;
	}
	myfile.close();

}
