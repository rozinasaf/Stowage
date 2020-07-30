#include <regex>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Utils.h"
#include <iomanip>
#include <math.h> 
#include "Validations.h"
#define ISO_6346_SIZE 11
#define ISO_6346_UPPER_SIZE 3
#define ISO_6346_CATEGORY_IDENTIFIER_INDEX 3
#define ISO_6346_SERIAL_NUMBER_AND_CHECK_DIGIT_SIZE 7
#define CARGO_DATA ".cargo_data"
#define SHIP_PLAN ".ship_plan"
#define ROUT ".rout"
#define NO_FILE "NO_FILE"

int isPortsSymbol(const std::string& word)
{
	if (word.size() != 5 || !std::regex_match(word, std::regex("[A-Z]+"))) {
		return 0;
	}
	return 1;
}
int validateCargoFileName(const std::string file_name) { //file_name = PPPPP_N
	int _index = file_name.find("_");
	if (_index == (int)std::string::npos) {
		return 0;
	}
	else {												  //is number	
		if (isPortsSymbol(file_name.substr(0, _index)) && std::regex_match(file_name.substr(_index + 1, file_name.size()), std::regex("[0-9]+")))
			return 1;
		else {
			return 0;
		}
	}
}
int getCheckDigit(const std::string& id) {
	double sum = 0;
	int n;
	for (int i = 0; i < (int)id.size() - 1; i++)
	{
		if (i < 4)
			sum += charValue(id.at(i)) * pow(2.0, (double)i);
		else
			sum += ((id.at(i)) - '0') * pow(2.0, (double)i);
	}
	n = ((int)(sum / 11.0)) * 11;
	if ((int)sum - n == 10)
		return 0;
	return (int)sum - n;
}
int validateISO_6346(const std::string word) {
	int errors = 0;
	if ((int)word.size() != ISO_6346_SIZE) {
		return 1 << 14;	
	}
	else {
		if (!std::regex_match(word.substr(0, ISO_6346_UPPER_SIZE), std::regex("[A-Z]+"))) {
		if (isNewError(errors, 1 << 14)) errors += 1 << 14;	
		}
		if ((word.at(ISO_6346_CATEGORY_IDENTIFIER_INDEX) != 'U') && (word.at(ISO_6346_CATEGORY_IDENTIFIER_INDEX) != 'J') && (word.at(ISO_6346_CATEGORY_IDENTIFIER_INDEX) != 'Z')) {

		if (isNewError(errors, 1 << 15)) errors += 1 << 15;	
		}
		if (!std::regex_match(word.substr(ISO_6346_CATEGORY_IDENTIFIER_INDEX + 1), std::regex("[0-9]+"))) {
		if (isNewError(errors, 1 << 14)) errors += 1 << 14;	
		}
		if (getCheckDigit(word) != (word.back() - '0')) {
		if (isNewError(errors, 1 << 15)) errors += 1 << 15;	
		}
		return errors;
	}
}
int validateCargoLine(const std::string& input_cargo_data_file, const std::string line, std::shared_ptr<Ship> s){
	std::vector<std::string> cargoData = splitByComma(line), inputFileLines;
	std::string containerID, weight, dest;
	int errors = 0;
	if (cargoData.size() != 3)
		if (isNewError(errors, 1 << 14)) errors += 1 << 14;	
	containerID = cargoData.at(0), weight = cargoData.at(1), dest = cargoData.at(2);
	for (auto& c : dest) c = toupper(c);
	if (!s->isNewToShip(containerID))
		if (isNewError(errors, 1 << 11)) errors += 1 << 11;	
	if (validateISO_6346(containerID) != 0)
		return validateISO_6346(containerID);
	if (!std::regex_match(weight, std::regex("[0-9]+")))
		if (isNewError(errors, 1 << 12)) errors += 1 << 12;	
	if (!isPortsSymbol(dest))
		if (isNewError(errors, 1 << 13)) errors += 1 << 13;	
	inputFileLines = getLines(input_cargo_data_file);
	for (int j = 0; j < (int)inputFileLines.size(); j++) {
		if (inputFileLines.at(j) == line)
			break;
		else if (containerID == splitByComma(inputFileLines.at(j)).at(0))
			if (isNewError(errors, 1 << 10)) errors += 1 << 10;	
	}
	return errors;
}
int validOperationLine(const std::string line) {
	std::vector<std::string> data = splitByComma(line);
	if (data.size() == 0) return 0;
	char operation = data.at(0).at(0);
	if (!(operation == 'R' || operation == 'L' || operation == 'U' || operation == 'M')) return 0;
	if (data.size() == 1) return 0;
	std::string contId = data.at(1);
	if (validateISO_6346(contId)) return 0;
	if (operation == 'M') return valideMoveOperation(line);
	if (operation == 'R')	return 1;
	if (data.size() != 5)	return 0;
	std::vector<std::string> indexesStr = std::vector(data.begin() + 2, data.end());
	std::vector<int> indexes = stoiVector(indexesStr);
	if (indexes.size() != 3)	return 0;
	if (indexes.at(0) < 0 || indexes.at(1) < 0 || indexes.at(2) < 0)	return 0;
	else	return 1;
}
int valideMoveOperation(const std::string line) {
	std::vector<std::string> dataMove = splitByComma(line);
	if (dataMove.size() != 8)
		return 0;
	std::vector<std::string> indexesUStr = std::vector(dataMove.begin() + 2, dataMove.begin() + 5);
	std::vector<std::string> indexesLStr = std::vector(dataMove.begin() + 5, dataMove.end());
	std::vector<int> indexesU = stoiVector(indexesUStr);
	std::vector<int> indexesL = stoiVector(indexesLStr);
	if (indexesU.size() != 3 || indexesL.size() != 3)
		return 0;
	if (indexesU.at(0) < 0 || indexesU.at(1) < 0 || indexesU.at(2) < 0)
		return 0;
	if (indexesL.at(0) < 0 || indexesL.at(1) < 0 || indexesL.at(2) < 0)
		return 0;
	else
		return 1;
}
