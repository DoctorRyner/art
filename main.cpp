#include <fplus/fplus.hpp>
#include <iostream>
#include <fstream>

std::string errorText;

enum Mode { looking, log, ask, var, equal };

std::string mainBlockLex(std::string gottenFileContent) {
	std::string builtInFunctions[] = { "log", "ask" };
	std::string keywords[] = { "let", "var", "and" };

	std::string toReturn;

	std::string key = "";

	Mode mode = Mode::looking;

	// log setup
	int logBracesState;
	int logState;

	// ask setup
	int askBraceState;
	int askState;

	// var setup
	std::map<std::string, std::string> vars;
	int varState;
	int varNameState;
	int varTypeState;
	std::string varName;
	std::string varType;

	// var equat operation
	int varEState = 0;

	int i = 0;

	for(char ch : gottenFileContent) {

		std:: cout << ch;

		if(mode == Mode::log) {
			if(ch == '\n') return "err";
			if(logBracesState == 0) {
				if(ch == ' ') {
					key = "";
					continue;
				}

				key += ch;

		 		if(ch == '(') {
		 			key = "";
		 			logBracesState = 1;
		 		}
		 	}
		 	else if(logBracesState == 1) {
		 		if(ch == '+') key += "<<";
		 		else if(ch == ')') {
		 			key += ";\n\t";
		 			toReturn += key;
		 			key = "";
		 			mode = Mode::looking;
		 		} else key += ch;
		 	}
		}		

		if(mode == Mode::var) {
			if(varState == 0) {
				if(varNameState == 0) {
					if(ch != ' ') {
						if(ch == '\n') {
							errorText = "Can't find var start";
							return "err";
						}
						key += ch;
						varNameState = 1;
					}
				} else if(varNameState == 1) {
					if(ch == ':') {
						varName = key;
						varState = 1;
						key = "";
						continue;
					} else if(ch == '\n' || ch == ' ') {
						errorText = "unexpected var end";
						return "err";
					}
					key += ch;
				}
			} else if(varState == 1) {
				key += ch;
				if(ch == ' ') {
					varState = 2;
					key = "";
				} else {
					errorText = "Unecpected type declaration end";
					return "err";
				}
			} else if(varState == 2) {
				if(varTypeState == 0) {
					key += ch;
					if(ch == ' ') continue;
					else varTypeState = 1;
				} else if(varTypeState == 1) {
					if(ch != ' ' && ch != '\n') {
						key += ch;
						continue;
					}
					else {
						varType = key;
						toReturn += varType + " " + varName + ";\n\t";
						vars.insert(std::pair(varName, varType));
						key = "";
						mode = Mode::looking;
					}
				}
			}
		}

		if(mode == Mode::ask) {
			if(askBraceState == 0) {
				if(ch == ' ') continue;
				else if(ch == '(') askBraceState = 1;
				else {
					errorText = "expected (";
					return "err";
				}
			} else if(askBraceState == 1) {
				if(ch == '\n' || ch == ')')  {
					bool isValid = false;
					for(auto& var : vars) {
						if(key != var.first) continue;
						isValid = true;
						break;
					}
					if(!isValid) {
						errorText = "invalid input in ask, expected var";
						return "err";
					}

					toReturn += key + ";\n\t";
					key = "";
					mode = Mode::looking;
				} else if(ch == ',') {
					key += " >> ";
					toReturn += key;
					key = "";
				}
				else if(ch != ' ') key += ch;
			}
		}

		if(mode == Mode::equal) {
			if(varEState == 0) {
				key += ch;
				if(key == " = ") {
					varEState = 1;
					continue;
				} else if(ch == '\n') {
					errorText = "wrong var assignment";
					return "err";
				}
			} else if(varEState == 1) {
				if(ch == '\n' || ch == ';') {
					toReturn += varName + key + ";\n\t";
					key = "";
					mode = Mode::looking;
				} else key += ch;
			}
		}

		if(mode == Mode::looking) {
			if(ch == ' ') {
				key = "";
				continue;
			} else if(ch == '\n') {
				if(key.size() > 1) toReturn += ";\n\t";
				key = "";
				continue;
			}

			key += ch;

			for(auto& cmpString : builtInFunctions) {
				if(key == cmpString) {
					if(key == "log") {
						mode = Mode::log;
						logBracesState = 0;
						logState = 0;
						toReturn += "cout << ";
						key = "";
					} else if(key == "ask") {
						askBraceState = 0;
						askState = 0;
						toReturn += "cin >> ";
						key = "";
						mode = Mode::ask;
					}
					else key = "";
					break;
				}
			}

			for(auto& cmpString : keywords) {
				if(key != cmpString) continue;

				if(key == "var") {
					key = "";
					varState = 0;
					varNameState = 0;
					varTypeState = 0;
					mode = Mode::var;
				} else if(key == "and") toReturn += " ; ";
				else key = "";

				break;
			}

			for(auto& var : vars) {
				if(key != var.first) continue;
				else {
					std::cout << "FOUND VAR NOTICING!!!\n";
					varName = var.first;
					varEState = 0;
					key = "";
					mode = Mode::equal;
				}
			}
		}
	}



	return toReturn;
}

int main() {
	FILE* file = fopen("new.art", "r");

	std::string fileContent;

	// 1
	std::string header = "#include <iostream>\n\nusing namespace std;\n\n";

	// 2
	std::string mainBlock = "";

	// 3
	std::string globalVars = "";

	// Reading from input file
	char ch;
	while((ch = getc(file)) != EOF) fileContent += ch;
	fclose(file);
	fileContent += '\n';

	// Lexing mainBlock
	if((mainBlock = mainBlockLex(fileContent)) == "err") {
		std::cout << "ERROR: " << errorText << std::endl;
		return 1;
	}

	// Forming output file
	std::ofstream outputFile("art.cpp");
	outputFile << header << globalVars << "\n" << "int main() {\n\t" << mainBlock << "\n}\n";
}