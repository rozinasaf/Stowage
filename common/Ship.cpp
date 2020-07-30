#include <regex>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <set>
#include <vector>
#include "Validations.h"
#include "Utils.h"
#include "Ship.h"
#define EMPTY "empty"
#define LAST "LAST"
#define PORT_CODE_SIZE 5
#define DELIMTER_SIZE 2
#define UNLOAD "U, "
#define LOAD "L, "
#define REJECT "R, "
#define MOVE "M, "
#define CONTAINER_NOT_FOUND "NA"
#define CARGO_DATA ".cargo_data"
#define NONE "NONE"
#define DONT_WRITE_OPERATION -1
#define OCCUPIED_SPACE "OCC"


int Ship::posNotFull(int x, int y) {
	if (this->shipPlanNow.at(x).at(y) < shipPlanMax.at(x).at(y)) {
		return 1;
	}
	else {
		return 0;
	}
}
void Ship::initShipPlan() {
	shipPlan.clear();
	this->shipPlan.resize(xMax);
	for (int i = 0; i < xMax; i++) {
		this->shipPlan[i].resize(yMax);
		for (int j = 0; j < yMax; j++)
			this->shipPlan[i][j].reserve(fMax);
	}
	for (int i = 0; i < xMax; i++)
		for (int j = 0; j < yMax; j++)
			for (int k = 0; k < fMax; k++)
				this->shipPlan.at(i).at(j).push_back(EMPTY);
	this->shipPlanMax.resize(xMax);
	this->shipPlanNow.resize(xMax);
	this->shipPlanOccupied.resize(xMax);

	for (int i = 0; i < xMax; i++) {
		this->shipPlanMax.at(i).resize(yMax);
		this->shipPlanNow.at(i).resize(yMax);
		this->shipPlanOccupied.at(i).resize(yMax);

	}
	for (int i = 0; i < xMax; i++) {
		for (int j = 0; j < yMax; j++) {
			this->shipPlanMax.at(i).at(j) = -1;
			this->shipPlanNow.at(i).at(j) = 0;
			this->shipPlanOccupied.at(i).at(j) = 0;

		}
	}
}
int Ship::readIndexPlan(const std::vector<std::string>& shipPlanData) {
	int x, y, f, errors = 0;
	for (int i = 1; i < (int)shipPlanData.size(); i++) {
		std::vector<int> line = stoiVector(splitByComma(shipPlanData.at(i)));
		if (line.at(0) == -1 || line.size() != 3)
			errors += 1 << 2;
		else {
			x = line.at(0), y = line.at(1), f = line.at(2);
			if (f >= this->fMax)		errors += 1 << 0;
			else if (x >= xMax || y >= yMax) errors += 1 << 1;
			else {
				if (shipPlanMax.at(x).at(y) == f) errors += 1 << 2;
				if (shipPlanMax.at(x).at(y) != f && shipPlanMax.at(x).at(y) != -1) {
					errors += 1 << 4;
					break;
				}
				else {
					this->shipPlanMax.at(x).at(y) = f;
					int occ = fMax - f;
					this->shipPlanOccupied.at(x).at(y) = occ;
					for (int i = 0; i < occ; i++)
						setContainer(x, y, i, OCCUPIED_SPACE);
				}
			}
		}
	}
	for (int i = 0; i < xMax; i++)
		for (int j = 0; j < yMax; j++)
			if (shipPlanMax.at(i).at(j) == -1)
				this->shipPlanMax.at(i).at(j) = fMax;
	return errors;
}
int Ship::readShipPlan(const std::string& file_name) {
	std::vector<std::string> shipPlanData = getLines(file_name);
	if (shipPlanData.size() == 0)
		return 1 << 3;
	int shipFloors, shipX, shipY, errors = 0;
	std::vector<int> firstLine = stoiVector(splitByComma(shipPlanData.at(0)));
	if (firstLine.at(0) == -1 || firstLine.size() != 3)
		return 1 << 3;
	else {
		shipFloors = firstLine.at(0), shipX = firstLine.at(1), shipY = firstLine.at(2);
		this->xMax = shipX, this->yMax = shipY, this->fMax = shipFloors;
		initShipPlan();
		errors += readIndexPlan(shipPlanData);
	}
	return errors;
}
void Ship::setContainer(int x, int y, int f, const std::string& container) {
	if ((this->shipPlanNow.at(x).at(y) < this->shipPlanMax.at(x).at(y)) &&
		this->noEmptyCellsBelow(x, y, f) &&
		(this->getContainer(x, y, f).compare(EMPTY) == 0))
	{
		this->shipPlan.at(x).at(y).at(f) = container;
	}

}
int Ship::readShipRoute(const std::string& file_name_rout) {
	this->shipRout.clear();
	std::vector<std::string> lines = getLines(file_name_rout);
	int validPorts = countValidPortsInRout(file_name_rout);
	if (lines.size() == 0 || validPorts == 0) return 1 << 7;
	if (validPorts <= 1) return 1 << 8;
	int errors = 0;
	for (int i = 0; i < (int)lines.size(); i++) {
		std::string line = lines.at(i), nextLine = NONE, prevLine = NONE;
		for (auto& c : line) c = toupper(c);
		if (i != (int)lines.size() - 1) {
			nextLine = lines.at(i + 1);
			for (auto& c : nextLine) c = toupper(c);
		}
		if (i != 0) {
			prevLine = lines.at(i - 1);
			for (auto& c : prevLine) c = toupper(c);
		}
		if (((i != (int)lines.size() - 1) && line.compare(nextLine) == 0) || (i != 0 && line.compare(prevLine) == 0)) errors = 1 << 5;
		if (!isPortsSymbol(line))
			if (errors == 1 << 5)
				errors = (1 << 5) + (1 << 6);
			else errors = 1 << 6;
		else {
			this->shipRout.push_back(line);
		}
	}
	if ((int)std::set<std::string>(shipRout.begin(), shipRout.end()).size() < 2)
		return 1 << 7;
	return 0;
}
int Ship::notFull() {
	for (int i = 0; i < xMax; i++)
	{
		for (int j = 0; j < yMax; j++)
		{
			if (this->shipPlanNow.at(i).at(j) < this->shipPlanMax.at(i).at(j)) {
				return 1;
			}
		}
	}
	return 0;
}
int Ship::countValidPortsInRout(const std::string& file_name_rout) {
	int counter = 0;
	std::vector<std::string> ports = getLines(file_name_rout);
	for (int i = 0; i < (int)ports.size(); i++) {
		for (auto& c : ports.at(i)) c = toupper(c);
		if (isPortsSymbol(ports.at(i)))
			counter++;
	}
	return counter;
}
int Ship::getIndexOfStop(const std::string& cargo_data) {
	if (std::regex_match(cargo_data.substr(cargo_data.find(".cargo_data") - 1, 1), std::regex("[0-9]+"))) {
		int desiredStop, counter = 0;
		if (std::regex_match(cargo_data.substr(cargo_data.find(".cargo_data") - 1, 1), std::regex("[0-9]+"))){
			desiredStop = stoi(cargo_data.substr(cargo_data.find(".cargo_data") - 1, 1));
		}else{
			return -1;
		}

		std::string port_name = cargo_data.substr(cargo_data.find(".cargo_data") - (PORT_CODE_SIZE + DELIMTER_SIZE), PORT_CODE_SIZE);
		for (auto& c : port_name) c = toupper(c);
		for (int i = 0; i < (int)shipRout.size(); i++)
		{
			if (shipRout.at(i).compare(port_name) == 0)
				counter++;
			if (counter == desiredStop)
				return i;
		}
	}
	return -1;
}
int Ship::timeInRout(int j, const std::string& port_name) {
	int counter = 0;
	for (int i = 0; i <= j; i++)
		if (shipRout.at(i).compare(port_name) == 0)
			counter++;
	return counter;
}
int Ship::noEmptyCellsBelow(int i, int j, int k) {
	for (int p = k - 1; p >= 0; p--)
	{	
		if (shipPlan.at(i).at(j).at(p).compare(EMPTY) == 0)
			return 0;
	}
	return 1;
}
int Ship::isEmpty() {
	for (int i = 0; i < xMax; i++)
	{
		for (int j = 0; j < yMax; j++)
		{
			if (this->shipPlanNow[i][j] != 0) {
				return 0;
			}
		}
	}
	return 1;
}
std::string Ship::getLastPort() {
	if (shipRout.size() > 0)
		return this->shipRout.at(shipRout.size() - 1);
	else
		return NONE;
}
int Ship::validNumOfFilesOfPort(const std::string fileNameRout) {
	int errors = 0;
	int portInRout, portInFolder;
	std::string portName, fileNameFolder = fileNameRout.substr(0, fileNameRout.rfind("/")), lastPort = getLastPort();
	if (lastPort == NONE)return 0;
	std::vector<std::string> files = getFilesWithSuffix(fileNameFolder, CARGO_DATA);
	for (int i = 0; i < (int)files.size(); i++) {
		portName = files.at(i).substr(0, PORT_CODE_SIZE);
		for (auto& c : portName) c = toupper(c);
		
		portInRout = countPortInRout(portName);
		if (std::regex_match(files.at(i).substr(PORT_CODE_SIZE + 1, 1), std::regex("[0-9]+"))){
			portInFolder = stoi(files.at(i).substr(PORT_CODE_SIZE + 1, 1));
		}else{
			if (isNewError(errors, 1 << 19)) errors += 1 << 19;//ignored files
		}
		
		if (portInRout < portInFolder || ((!portName.compare(lastPort)) && (portInRout == portInFolder) && (getLines(fileNameFolder + "/" + files.at(i)).size() > 0)))
					if (isNewError(errors, 1 << 19)) errors += 1 << 19;//ignored files

	}
	std::vector<std::string> rout = getLines(fileNameRout);
	for (int i = 1; i < (int)rout.size() - 1; i++) {
		portInRout = timeInRout(i, rout.at(i));
		if (!isFile(fileNameFolder + "/" + rout.at(i) + "_" + std::to_string(portInRout) + CARGO_DATA))
			if (isNewError(errors, 1 << 20)) errors += 1 << 20; //missing files
	}
	return errors;
}
int Ship::countPortInRout(std::string& port) {
	for (auto& c : port) c = toupper(c);
	int count = 0;
	for (int i = 0; i < (int)shipRout.size(); i++)
		if (shipRout.at(i).compare(port) == 0) {
			count++;
		}

	return count;
}
void Ship::unload(int i, int j, int k, int n) {
	std::vector<std::string> temp;
	for (int p = shipPlanNow.at(i).at(j) + shipPlanOccupied.at(i).at(j) - 1; p > k; p--) {
		temp.push_back(shipPlan.at(i).at(j).at(p));
		if (n != DONT_WRITE_OPERATION) {
			operations.at(n).push_back(UNLOAD
				+ shipPlan.at(i).at(j).at(p).substr(0, shipPlan.at(i).at(j).at(p).find(","))
				+ ", " + std::to_string(p) + ", " + std::to_string(i) + ", " + std::to_string(j) );
		}
		shipPlan.at(i).at(j).at(p) = EMPTY;
	}
	if (n != DONT_WRITE_OPERATION) {
		operations.at(n).push_back(UNLOAD
			+ shipPlan.at(i).at(j).at(k).substr(0, shipPlan.at(i).at(j).at(k).find(","))
			+ ", " + std::to_string(k) + ", " + std::to_string(i) + ", " + std::to_string(j));
	}
	shipPlan.at(i).at(j).at(k) = EMPTY;
	for (int t = 0; t < (int)temp.size(); t++) {
		shipPlan.at(i).at(j).at(k + t) = temp.at(t);
		if (n != DONT_WRITE_OPERATION) operations.at(n).push_back(LOAD + temp.at(t).substr(0, temp.at(t).find(",")) + ", " + std::to_string(k + t) + ", " + std::to_string(i) + ", " + std::to_string(j));
	}
	shipPlanNow.at(i).at(j) = shipPlanNow.at(i).at(j) - 1;
}

void Ship::load(int i, int j, int k, int n, const std::string& container) {
	std::vector<std::string> temp;
	for (int p = shipPlanNow.at(i).at(j)+ shipPlanOccupied.at(i).at(j) - 1; p > k; p--)
	{
		temp.push_back(shipPlan.at(i).at(j).at(p));
		if (n != DONT_WRITE_OPERATION) {
			operations.at(n).push_back(UNLOAD
				+ shipPlan.at(i).at(j).at(p).substr(0, shipPlan.at(i).at(j).at(p).find(","))
				+ ", " + std::to_string(p) + ", " + std::to_string(i) + ", " + std::to_string(j));
		}
		shipPlan.at(i).at(j).at(p) = EMPTY;
	}

	setContainer(i, j, k, container);
	if (n != DONT_WRITE_OPERATION) {
		operations.at(n).push_back(LOAD + container.substr(0, container.find(","))
			+ ", " + std::to_string(k) + ", " + std::to_string(i) + ", " + std::to_string(j));
	}
	for (int t = 0; t < (int)temp.size(); t++)
	{
		shipPlan.at(i).at(j).at(k + 1) = temp.at(t);
		if (n != DONT_WRITE_OPERATION) {
			operations.at(n).push_back(LOAD
				+ temp.at(t).substr(0, temp.at(t).find(","))
				+ ", " + std::to_string(k + 1 + t) + ", " + std::to_string(i) + ", " + std::to_string(j));
		}
	}
	shipPlanNow.at(i).at(j) = shipPlanNow.at(i).at(j) + 1;

}
void Ship::reject(std::string container, int n) {
	operations.at(n).push_back(REJECT + container.substr(0, container.find(",")) + ", " + +"-1, -1, -1");
}
void Ship::move(int si, int sj, int sk, int di, int dj, int dk, int n, const std::string& container) {
	unload(si, sj, sk, DONT_WRITE_OPERATION);
	load(di, dj, dk, DONT_WRITE_OPERATION, container);
	operations.at(n).push_back(MOVE + container.substr(0, container.find(","))
		+ ", " + std::to_string(sk) + ", " + std::to_string(si) + ", " + std::to_string(sj)  +
		", " + std::to_string(dk) + ", " + std::to_string(di) + ", " + std::to_string(dj));
}
void Ship::initOperations() {
	this->operations.clear();
	for (int i = 0; i < (int)shipRout.size(); i++) {
		std::vector<std::string> port;
		port.push_back(shipRout.at(i));
		this->operations.push_back(port);
	}
}
int Ship::isNewToShip(const std::string& id) {
	for (int i = 0; i < (int)shipPlan.size(); i++)
	{
		for (int j = 0; j < (int)shipPlan.at(i).size(); j++)
		{
			for (int k = 0; k < (int)shipPlan.at(i).at(j).size(); k++)
			{
				if (shipPlan.at(i).at(j).at(k).find(id) != std::string::npos) {
					return 0;
				}
			}
		}
	}
	return 1;
}
int Ship::firstOccurenceAfterIndex(const std::string& portName, int currentIndex) {
	for (int i = currentIndex + 1; i < (int)shipRout.size(); i++)
		if (shipRout.at(i) == portName)
			return i;
	return 0;


}
std::string Ship::getDestOfContFromInstructions(const std::string& input_cargo_data_file, int indexOfStop, const std::string& operation) {
	std::string cd = getCargoDataFromOperation(input_cargo_data_file, indexOfStop, operation);
	if (cd != NONE) {
		std::vector<std::string> cargoData = splitByComma(cd);
		if (cargoData.size() == 3)
			return cargoData.at(2);

	}
	return NONE;
}
std::string Ship::getCargoDataFromOperation(const std::string& input_cargo_data_file, int indexOfStop, const std::string& operation) {
	std::vector<std::string> opVec = splitByComma(operation), allContsInStop, currCont;
	std::string containerID = opVec.at(1), containerDest = NONE, cargoDataFile, travel = input_cargo_data_file.substr(0, input_cargo_data_file.rfind("/"));
	for (int i = 0; i <= indexOfStop; i++)
	{
		allContsInStop = getLines(travel + "/" + getShipRout().at(i) + "_" + std::to_string(timeInRout(i, getShipRout().at(i))) + ".cargo_data");
		for (int j = 0; j < (int)allContsInStop.size(); j++) {
			if (validateCargoLine(input_cargo_data_file, allContsInStop.at(j), shared_from_this()) == 0) {
				currCont = splitByComma(allContsInStop.at(j));
				if (currCont.at(0) == containerID) {
					return allContsInStop.at(j);
				}
			}
		}
	}
	return NONE;
}