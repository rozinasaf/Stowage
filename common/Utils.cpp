#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <math.h>
#include <iomanip>
#include <filesystem>
#include <vector>
#include <regex>
#include "Utils.h"

#define CURRENT_DIRECTORY "./"
#define SHIP_PLAN ".ship_plan"
#define ROUT ".route"
#define NO_FILE "NO_FILE"
#define NUM_OF_ERRORS 31

std::vector<std::string> getArgs(int argc, char** argv) {
	std::vector<std::string> arguments;
	std::string travel = "", algo = "", output = "", numThreads = "";
	for (int i = 1; i < argc; i += 2) {
		if (!strcmp(argv[i], "-travel_path") && i != argc - 1) {
			if (i < argc && std::filesystem::is_directory(argv[i + 1]))
				travel = argv[i + 1];
			else
				travel = NO_FILE;
		}
		else if (!strcmp(argv[i], "-algorithm_path") && i != argc - 1) {
			if (i < argc && std::filesystem::is_directory(argv[i + 1]))
				algo = argv[i + 1];
			else
				algo = CURRENT_DIRECTORY;
		}
		else if (!strcmp(argv[i], "-output") && i != argc - 1) {
			if (i < argc && std::filesystem::is_directory(argv[i + 1]))
				output = argv[i + 1];
			else
				output = CURRENT_DIRECTORY;
		}
		else if (!strcmp(argv[i], "-num_threads") && i != argc - 1) {
			if (i < argc)
				numThreads = argv[i + 1];
			else
				numThreads = "1";
		}
	}
	if ((int)travel.size() == 0) travel = NO_FILE;
	if ((int)algo.size() == 0) algo = CURRENT_DIRECTORY;
	if ((int)output.size() == 0) output = CURRENT_DIRECTORY;
	if ((int)numThreads.size() == 0 || !std::regex_match(numThreads, std::regex("[0-9]+"))) numThreads = "1";
	arguments = { travel, algo, output, numThreads };
	return arguments;
}
std::string createDir(const std::string& parent, const std::string& newdir) {
	std::error_code errorCode;
	std::filesystem::create_directories(parent + "/" + newdir, errorCode);
	return parent + "/" + newdir;
}
std::vector<std::string> splitByComma(const std::string& line) {

	std::vector<std::string> res;
	size_t pos_start = 0, pos_end;
	std::string token;
	while ((pos_end = line.find(",", pos_start)) != std::string::npos) {
		token = line.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + 1;
		while (pos_start != line.size() && line.at(pos_start) == ' ') pos_start++;
		res.push_back(removeSpaces(removeControlChar(token)));
	}
	res.push_back(line.substr(pos_start));
	return res;
}
std::vector<int> stoiVector(const std::vector<std::string>& strVector) {
	std::vector<int> data;
	for (int i = 0; i < (int)strVector.size(); i++)
	{
		if (std::regex_match(strVector.at(i), std::regex("[0-9]+"))) {
			data.push_back(stoi(strVector.at(i)));
		}

		else
			return { -1 };
	}
	return data;
}
std::string removeControlChar(const std::string& str) {
	std::string res = str;
	for (int i = 0; i < (int)str.size(); i++)
		if (iscntrl(res.at(i)))
			res.erase(i);
	return res;
}
std::string removeSpaces(const std::string& str) {
	return std::regex_replace(str, std::regex("^ +| +$"), "");
}
std::vector<std::string> getFileNames(const std::string full_path) {
	std::vector<std::string> v;
	std::error_code errorCode;
	for (const auto& entry : std::filesystem::directory_iterator(full_path, errorCode)) {
		std::string fwpath = entry.path().string();
		std::string file_name = fwpath.substr(full_path.size() + 1);
		v.push_back(removeControlChar(file_name));
	}
	return v;
}
int isFile(const std::string path) {
	std::error_code errorCode;
	if (std::filesystem::is_regular_file(path, errorCode))
		return 1;
	return 0;
}
void removeFailedAlgorithmFiles(const std::string path) {
	std::error_code errorCode;
	if (std::filesystem::is_directory(path, errorCode)) {
		if (!std::filesystem::remove_all(path, errorCode)) {
			std::cout << errorCode.message() << std::endl;
		}
		if (std::filesystem::is_directory(path, errorCode))
			if (!std::filesystem::remove(path, errorCode))
				std::cout << errorCode.message() << std::endl;
	}
}
std::vector<std::string> getDiffPorts(const std::string file_name_rout) {
	std::vector<std::string> ports = getLines(file_name_rout);
	for (int i = 0; i < (int)ports.size(); i++)
	{
		if (std::count(ports.begin(), ports.end(), ports.at(i)) == 0) {
			ports.push_back(ports.at(i));
		}
	}
	return ports;
}
std::vector<std::string> getFilesWithSuffix(const std::string path, const std::string suffix) {
	std::vector<std::string> files;
	std::vector<std::string> pathFiles = getFileNames(path);
	std::error_code errorCode;

	for (int i = 0; i < (int)pathFiles.size(); i++) {
		if (pathFiles.at(i).rfind(".") != std::string::npos) {
			if (std::filesystem::is_regular_file(path + "/" + pathFiles.at(i), errorCode) && pathFiles.at(i).rfind(".") && pathFiles.at(i).substr(pathFiles.at(i).rfind(".")) == suffix)
				files.push_back(pathFiles.at(i));
		}
	}

	return files;
}
std::vector<std::string> getSOAlgorithms(const std::string path) {
	std::vector<std::string> nameWithSo = getFilesWithSuffix(path, ".so");
	std::vector<std::string> cleanNames;
	for (int i = 0; i < (int)nameWithSo.size(); i++) {
		cleanNames.push_back(nameWithSo.at(i).substr(0, (int)nameWithSo.at(i).rfind(".")));
	}

	return cleanNames;
}
std::string getRoutFile(const std::string path) {
	std::vector<std::string> files = getFilesWithSuffix(path, ROUT);
	if (files.size() == 0)
		return NO_FILE;
	return files.at(0);
}
std::string getShipPlanFile(const std::string path) {
	std::vector<std::string> files = getFilesWithSuffix(path, SHIP_PLAN);
	if (files.size() == 0)
		return NO_FILE;
	return files.at(0);
}
std::vector<std::string> getLines(const std::string file_name) {
	std::string line;
	std::vector<std::string> ports;
	std::ifstream file;
	file.open(file_name.c_str());
	if (file.is_open())
		while (getline(file, line)) {
			line = removeSpaces(removeControlChar(line));
			if (line.size() != 0 && line.at(0) != '#')
				ports.push_back(line);
		}
	file.close();
	return ports;
}
void toTable(const std::vector<std::vector<std::string>>& mat, const std::string file_name_out) {
	std::vector<std::pair<int, std::vector<std::string>>> algoErrors;
	algoErrors.push_back(std::make_pair(-1, mat.at(0)));
	for (int i = 1; i < (int)mat.size(); i++)
		algoErrors.push_back(std::make_pair(stoi(mat.at(i).at(mat.at(i).size() - 1)), mat.at(i)));
	std::sort(algoErrors.begin() + 1, algoErrors.end());
	int startSorting = 1;
	for (int i = 1; i < (int)algoErrors.size(); i++)
		if (i != (int)algoErrors.size() - 1 || ((i == (int)algoErrors.size() - 1) && startSorting != i))
			if (((i == (int)algoErrors.size() - 1) && startSorting != i) || algoErrors.at(i + 1).first != algoErrors.at(i).first) {
				for (int j = startSorting; j <= i; j++)
					if (std::regex_match(algoErrors.at(j).second.at(algoErrors.at(j).second.size() - 2).substr(0, algoErrors.at(j).second.at(algoErrors.at(j).second.size() - 2).rfind(",")), std::regex("[0-9]+")))
						algoErrors.at(j).first = stoi(algoErrors.at(j).second.at(algoErrors.at(i).second.size() - 2).substr(0, algoErrors.at(j).second.at(algoErrors.at(j).second.size() - 2).rfind(",")));
				std::sort(algoErrors.begin() + startSorting, algoErrors.begin() + i + 1);
				startSorting = i + 1;
			}
	std::ofstream file_;
	file_.open(file_name_out);
	for (int i = 0; i < (int)algoErrors.size(); i++) {
		for (int j = 0; j < (int)algoErrors.at(i).second.size(); j++)
			file_ << std::left << std::setw(20) << std::setfill(' ') << algoErrors.at(i).second.at(j);
		file_ << '\n';
	}
	file_.close();
}
int charValue(char c) {
	int n = 10 + (c - 'A');
	if (n == 10)
		return n;
	if (n > 10 && n <= 20)
		return n + 1;
	if (n > 20 && n <= 30)
		return n + 2;
	if (n > 30 && n <= 35)
		return n + 3;
	else
		return 0;

}
int isNewError(int s, int c) {
	int l = log2(c);
	return !(s & (1 << l));
}
int isLegalTravel(int travelErrors) {
	if (!isNewError(travelErrors, 1 << 3) || !isNewError(travelErrors, 1 << 4) || !isNewError(travelErrors, 1 << 7) || !isNewError(travelErrors, 1 << 8))
		return 0;
	return 1;
}
int lenOfInt(int n) {
	unsigned int numOfDigits = 0;
	do {
		++numOfDigits;
		n /= 10;
	} while (n);
	return numOfDigits;
}
std::string getErrorString(int errorIndex) { // -------ERROR LIST
	std::string error;
	switch (errorIndex) {
	case 0:error = "ship plan: a position has an equal number of floors, or more, than the number of floors provided in the first line(ignored)"; break;
	case 1:error = "ship plan: a given position exceeds the X/Y ship limits (ignored)"; break;
	case 2:error = "ship plan: bad line format after first line or duplicate x,y appearance with same data (ignored)"; break;
	case 3:error = "ship plan: travel error - bad first line or file cannot be read altogether (cannot run this travel)"; break;
	case 4:error = "ship plan: travel error - duplicate x,y appearance with different data (cannot run this travel)"; break;
	case 5:error = "travel route: a port appears twice or more consecutively (ignored)"; break;
	case 6:error = "travel route: bad port symbol format (ignored)"; break;
	case 7:error = "travel route: travel error - empty file or file cannot be read altogether (cannot run this travel)"; break;
	case 8:error = "travel route: travel error - file with only a single valid port (cannot run this travel)"; break;
	case 9:error = "reserved"; break;
	case 10:error = "containers at port: duplicate ID on port (ID rejected)"; break;
	case 11:error = "containers at port: ID already on ship (ID rejected)"; break;
	case 12:error = "containers at port: bad line format, missing or bad weight (ID rejected)"; break;
	case 13:error = "containers at port: bad line format, missing or bad port dest (ID rejected)"; break;
	case 14:error = "containers at port: bad line format, ID cannot be read (ignored)"; break;
	case 15:error = "containers at port: illegal ID check ISO 6346 (ID rejected)"; break;
	case 16:error = "containers at port: file cannot be read altogether (assuming no cargo to be loaded at this port)"; break;
	case 17:error = "containers at port: last port has waiting containers (ignored)"; break;
	case 18:error = "containers at port: total containers amount exceeds ship capacity (rejecting far containers)"; break;
	case 19:error = "travel directory: there are cargo_data files that will be ignored"; break;
	case 20:error = "travel directory: there are missing cargo_data files"; break;
	case 21:error = "algorithm behavior: algorithm loaded containers on last stop"; break;
	case 22:error = "algorithm behavior: algorithm did not rejected a container with bad cargo data line"; break;
	case 23:error = "algorithm behavior: algorithm did an illegal move"; break;
	case 24:error = "algorithm behavior: containers left on the ship when rout was finished"; break;
	case 25:error = "algorithm behavior: algorithm tried loading when ship is full"; break;
	case 26:error = "algorithm behavior: algorithm did not reject far containers when ship is full"; break;
	case 27:error = "algorithm behavior: algorithm loaded containers with bad or not in rout destenation"; break;
	case 28:error = "algorithm behavior: algorithm loaded container ID already in ship"; break;
	case 29:error = "algorithm behavior: algorithm loaded two or more containers with the same ID"; break;
	case 30:error = "algorithm behavior: algorithm exceeds ship limits"; break;
	case 31:error = "algorithm behavior: algorithm did an illegal operation"; break;
	default:error = "general error";
	}
	return "ERROR- " + error;
}
std::vector<std::string> getErrorVec(int errors) {
	std::vector<std::string> errorVec;
	for (int i = 0; i <= NUM_OF_ERRORS; i++)
		if (!isNewError(errors, 1 << i))
			errorVec.push_back(getErrorString(i));

	return errorVec;
}
void writeErrors(const std::string& path, const std::vector<std::string>& errorsVec) {
	int needToWrite = 0;
	for (int i = 0; i < (int)errorsVec.size(); i++)
		if (errorsVec.at(i).back() != ':')
			needToWrite = 1;
	if (needToWrite) {
		std::ofstream file_;
		file_.open(path);
		for (int i = 0; i < (int)errorsVec.size(); i++)
			file_ << errorsVec.at(i) << "\n";
		file_.close();
	}
}
int getNumOfErrors(int errors) {
	int count = 0;
	while (errors) {
		count += errors & 1;
		errors >>= 1;
	}
	return count;

}

