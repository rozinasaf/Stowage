#ifndef UTILS_HEADER
#define UTILS_HEADER
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <regex>
#include <iomanip>

/*
split a string to vector, seperate by ", "
*/
std::vector<std::string> splitByComma(const std::string& line);

/*
vector of string -> vector of integers
*/
std::vector<int> stoiVector(const std::vector<std::string>& strVector);
std::vector<std::string> getFilesWithSuffix(const std::string path, const std::string suffix);
std::vector<std::string> getSOAlgorithms(const std::string path);
std::string getShipPlanFile(const std::string path);
std::string getRoutFile(const std::string path);

/*take care of the command line arguments*/
std::vector<std::string> getArgs(int argc, char** argv);

/*
return the char value of each charecter according to ISO 6346
*/
int charValue(char c);
std::string getErrorString(int errorIndex);
std::string createDir(const std::string& parent, const std::string& newdir);

/* 
return number of set bits of errors integer
*/
int getNumOfErrors(int errors);
std::string removeControlChar(const std::string &str);
std::string removeSpaces(const std::string& str);
std::vector<std::string> getFileNames(const std::string path);
int isFile(const std::string path);

/*
return all different ports in rout
*/
std::vector<std::string> getDiffPorts(const std::string file_name_rout);

/*
reading a file and return all lines on it as a vector
*/
std::vector<std::string> getLines(const std::string file_name_rout);

/*
removes a <travel, algorithm> pair output directory
*/
void removeFailedAlgorithmFiles(const std::string path);

/*
gets the simulate results and write to file name
*/
void toTable(const std::vector<std::vector<std::string>>& mat, const std::string file_name_out);
int isNewError(int s, int c);
int isLegalTravel(int errors);
int lenOfInt(int n);

/*
get an integer stands for an error/s code/s and return a string vector with the correct errors
*/
std::vector<std::string> getErrorVec(int errors);
void writeErrors(const std::string& path, const std::vector<std::string>& errorsVec);

#endif