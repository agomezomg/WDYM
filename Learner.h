#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <regex>
using namespace std; 

class Learner {
private:
	class Confidence {
		private:
			string command;
			int increment;
			double confidenceVal;
		public:
			Confidence(string, int, double);
			~Confidence();
			void calcConfidence(int);
			int getIncrement() { return this -> increment; };
			double getConfidence() { return this -> confidenceVal; };
			void growIncrement(int);
	};
	//umap to get possible spellings (command, typo)
	unordered_map<string, vector<string>> umap;
	//regexmap (command, regex)
	unordered_map<string, string> regExMap;
	//map with confidence values per unique typo (regex, command, confidence)
	unordered_map<string, Confidence*> confidenceMap;
public:
	Learner();
	~Learner();
	int pushValue(string, string);
	vector<string> pruneDuplicates();
	string findMatch(string);
	void specifyConfidence(string, string, int);
	void murderMatch(string, string);
	string Suggest(string);
	double getConfidence(string);
	bool commandExists(string);
	string retTypo(string);
	void printAllValues();
	void buildRegexMap(); //uses the current map to create regex assosciated to command
	void write(); //writes learnt combinations to file
	void generateRegex(int, string*, string); //this is still n^3 but at least it's kinda modular.
};