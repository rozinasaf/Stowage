#ifndef VALIDATIONS_HEADER
#define VALIDATIONS_HEADER
#include <regex>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Utils.h"
#include "Ship.h"

int validOperationLine(const std::string line);
int valideMoveOperation(const std::string line);
int isPortsSymbol(const std::string& word);
int validateISO_6346(const std::string word);
int validateCargoFileName(const std::string file_name);
int validateCargoLine(const std::string& input_cargo_data_file, const std::string line, std::shared_ptr<Ship> s);
int getCheckDigit(const std::string& id);


#endif