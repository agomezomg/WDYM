#include "Learner.h"

Learner::Confidence::Confidence(string command, int inc, double confidenceVal) {
	this -> command = command;
	this -> increment = inc;
	this -> confidenceVal = confidenceVal;
}

Learner::Confidence::~Confidence() {}

void Learner::Confidence::growIncrement(int increment) {
	this -> increment += increment;
}

void Learner::Confidence::calcConfidence(int size) {
	this -> confidenceVal = (this -> increment)/size;
}

Learner::Learner() {
	//load();
}

Learner::~Learner() {

}

/*void Learner::specifyConfidence(string command, string typo, int increment) {
	cout << "debug: method specifyConfidence(string,string,int) invoked." << endl;
	cout << "debug: declared occurence" << endl;
	if (confidenceMap.find(typo)!=confidenceMap.end()) {
		cout << "debug: before calling occurence..." << endl;
		//confidence map learns new value
		confidenceMap.insert({typo, new Confidence(command, 1, (1/umap.at(command).size()))});
	} else {
		Confidence* occurence;
		//confidence map gets updated
		occurence = confidenceMap.at(typo);
		occurence.increment = increment;
		occurence.calcConfidence(umap.at(command).size());
		if (occurence.calcConfidence(umap.at(command).size()) < .25 && occurence.increment > 10) {
			confidenceMap.erase(typo);
			cout << "Confidence level for: " << typo << " was too low and got removed." <<endl;			
		}
		confidenceMap.at(typo) = occurence;
	}
	
}*/

string Learner::Suggest(string typo) {
	for (auto x : regExMap) {
		cout << "checking for: " << typo << " against -> " << x.second << endl;
		if (regex_match(typo, regex(x.second))) {
			cout << "Possible match found!" << endl;
			return x.first;
		}
	}
	return "";
}

//checks if basic command exists.po
bool Learner::commandExists(string wdym) {
	if(umap.find(wdym)==umap.end()) {
		cout << "Command must be created." << endl;
		return false;
	}
	cout << "Command was found!" << endl;
	printAllValues();
	return true;
}

string Learner::retTypo(string command) {
	return regExMap.at(command);
}

int Learner::pushValue(string command, string value) {
	vector<string> temp_values;
	if (commandExists(command)) {
		temp_values = umap.at(command);
		if (temp_values.size() > 1) {
			temp_values.push_back(value);
			temp_values = pruneDuplicates();	
			umap.at(command) = temp_values;
		} else if (value.compare(temp_values.at(0)) != 0) {
			umap.at(command).push_back(value);
		}
		cout << "Updated command variation." << endl;
	} else {
		temp_values.push_back(value);
		umap.insert({command, temp_values});
		cout << "New key created." << endl;
	}
	buildRegexMap();
	return temp_values.size();
}

void Learner::murderMatch(string command, string typo) {
	if (commandExists(command)) {
		int index = 0;
		vector<string> tempVal = umap.at(command);
		for (int i = 0; i < tempVal.size(); i++) {
			if (typo.compare(tempVal.at(i)) == 0) {
				index = i;
				break;
			}
		}
		tempVal.erase(tempVal.begin() + index);
		umap.at(command) = tempVal;
	} else {
		pushValue(command, typo);
	}
}

vector<string> Learner::pruneDuplicates() {
	vector <string> rcandidate;
	vector <string> prunedList;
	int repetitions;
	bool unique;
	for (auto x : umap) {
		rcandidate = x.second;
		for (int i = 0; i < rcandidate.size(); i++) {
			unique = true;
			for (int j = 0; j < rcandidate.size(); j++) {
				if (rcandidate.at(i).compare(rcandidate.at(j))==0) {
					unique = false;
				}
			}
			if (unique) {
				prunedList.push_back(rcandidate.at(i));
			}
		}
	}
	return prunedList;
}

void Learner::printAllValues() {
	vector<string> temp_values;
	cout << "VALUES LEARNT (umap): " << endl;
	for (auto elem : umap) {
		temp_values = elem.second;
		cout << elem.first << ": " << endl;
		for (int i = 0; i < temp_values.size(); i++) {
			cout << i << ". " << temp_values.at(i) << endl;
		}
		cout << endl;		
	}

	string temporaryStr;
	cout << "VALUES LEARNT (regexmap): " << endl;
	for (auto elem : regExMap) {
		temporaryStr = elem.second;
		cout << "\t" << elem.first << " -> ";
		cout << elem.second << endl;
		cout << endl;		
	}
}

/**/
void Learner::buildRegexMap() {
	vector<string> temp_values;
	string* concat = new string();
	for (auto x : umap) {
		temp_values = x.second;
		for (int i = 0; i < temp_values.size(); i++) {
			cout << "inserting to " << x.first << ": " << endl;
			generateRegex(i, concat, temp_values.at(i));
			cout << i << ". " << concat[i] << endl;
			if ((i+1) >= temp_values.size()) {
				regExMap.insert({x.first, concat[i]});
				break;
			} else {
				concat[i] += "|";
			}
			regExMap.insert({x.first, concat[i]});
		}
	}

	for (int i = 0; i < umap.size(); i++) {
		
	}
	
}

void Learner::generateRegex(int i, string* concat, string value) {
	for (int j = 0; j < value.length(); j++) {
		concat[i] += "[";
		concat[i] += value[j];
		concat[i] += "]";		
	}
}
