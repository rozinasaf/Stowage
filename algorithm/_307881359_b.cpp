#include <iostream>
#include <fstream>
#include <vector>
#include "../common/Utils.h"
#include "../common/Ship.h"
#include "_307881359_b.h"
#include "../common/Validations.h"
#include "../interfaces/AlgorithmRegistration.h"

#define LAST "LAST"
#define LOAD 'L'
#define UNLOAD 'U'
#define PORT_CODE_SIZE 5
#define REJECT 'R'
#define EMPTY "empty"
#define DELIMTER_SIZE 2

REGISTER_ALGORITHM(_307881359_b)

int _307881359_b::getInstructionsForCargo(const std::string& input_file_name, const std::string& output_file_name) {
	int stopIndex = s->getIndexOfStop(input_file_name);
	int errors = 0;
	if (stopIndex != -1) {
		std::ofstream file_;
		file_.open(output_file_name);
		errors = operate(stopIndex, input_file_name);
		for (int i = 1; i < (int)s->getOperations(stopIndex).size(); i++)
			file_ << s->getOperations(stopIndex).at(i) + "\n";
		file_.close();
		return errors;
	}
	else
		return 1 << 14;
}
int _307881359_b::readShipPlan(const std::string& file_name) {
	s = std::make_shared<Ship>();
	return s->readShipPlan(file_name);
}
int _307881359_b::readShipRoute(const std::string& file_name_rout) {
	int errors = s->readShipRoute(file_name_rout);
	rout.clear();
	rout = s->getShipRout();
	s->initOperations();
	return errors;
}
std::vector<int> _307881359_b::getEmptyIndex(int notI, int notJ) {
	std::vector<int> res;
	for (int i = 0; i < s->getyMax(); i++)
		for (int j = 0; j < s->getxMax(); j++)
			if (j != notI && i != notJ && s->getShipPlanNow(j, i) < s->getShipPlanMax(j, i)) {
				res.push_back(j);
				res.push_back(i);
				res.push_back(s->getShipPlanNow(j, i) + s->getShipPlanOccupied(j, i));
				return res;
			}
	return { -1,-1,-1 };
}

void _307881359_b::tryMoveAllAbove(int i, int j, int k, int n) {
	std::vector<int> empty = getEmptyIndex(i, j);
	if (!(empty.at(0) == -1 || empty.at(1) == -1 || empty.at(2) == -1)){
		for (int p = (int)s->getShipPlanNow(i, j) + s->getShipPlanOccupied(i, j) - 1; p > k; p--) {
			s->move(i, j, p, empty.at(0), empty.at(1), empty.at(2), n, s->getContainer(i, j, p));
		}
	}
}
void _307881359_b::unloadAtPort(const std::string& port, int n) {
	for (int i = 0; i < (int)s->getxMax(); i++)
		for (int j = 0; j < (int)s->getyMax(); j++)
			for (int k = (int)s->getShipPlanNow(i, j)+ s->getShipPlanOccupied(i, j) - 1; k >= s->getShipPlanOccupied(i, j); k--)
				if (s->getContainer(i, j, k).substr(s->getContainer(i, j, k).size() - PORT_CODE_SIZE, PORT_CODE_SIZE).compare(port) == 0) {
					if (this->calculator.tryOperation(UNLOAD, i, j, k) == this->calculator.APPROVED) {
						if (k > 0) tryMoveAllAbove(i, j, k, n);
						s->unload(i, j, k, n);
					}
					else {
						//TODO
					}
				}
}
std::vector<std::vector<std::string>> _307881359_b::getContainersFromCloseToFar(int currentStopIndex, const std::string& travel_port_file_name) {
	std::string dest;
	std::vector<std::string> containers = getLines(travel_port_file_name);
	std::vector<std::string> conts, reject, visited;
	for (int j = 0; j < (int)containers.size(); j++) {
		//CHECK IF DESTENATION OF THE CONTAINER IS IN THE REST OF THE ROUT
		int counter = 0;
		dest = containers.at(j).substr(containers.at(j).size() - PORT_CODE_SIZE, PORT_CODE_SIZE);
		for (int k = currentStopIndex + 1; k < (int)rout.size(); k++)
			if (dest.compare(rout.at(k)) == 0)
				counter++;
		//IF NOT (counter = 0) REJECT IT
		if (counter == 0)
			reject.push_back(containers.at(j));
	}
	for (int i = currentStopIndex + 1; i < (int)rout.size(); i++) {
		if (!std::count(visited.begin(), visited.end(), rout.at(i))) {
			for (int j = 0; j < (int)containers.size(); j++) {
				dest = containers.at(j).substr(containers.at(j).size() - PORT_CODE_SIZE, PORT_CODE_SIZE);
				if (dest.compare(rout.at(i)) == 0 && !std::count(reject.begin(), reject.end(), containers.at(j))) {
					conts.push_back(containers.at(j));
				}
			}
			visited.push_back(rout.at(i));
		}
	}
	return { conts,reject };
}
int _307881359_b::tryLoading(const std::string& travel_port_file_name, const std::string& container, int stopIndex) {
	int errors = 0;
	std::vector<int> empty;
	int lineError = validateCargoLine(travel_port_file_name, container, s);
	if (lineError != 1 << 14) {
		//IF THERE IS ID-REJECT ERROR - REJECT IT
		if (lineError) {
			s->reject(container, stopIndex);
			if (isNewError(errors, lineError))
				errors += lineError;
		}
		//IF SHIP IS FULL - REJECT IT
		else if (!s->notFull()) {
			s->reject(container, stopIndex);
			if (isNewError(errors, 1 << 18))
				errors += 1 << 18;
		}
		//ELSE - LOAD IT
		else {
			empty = getEmptyIndex(-1, -1);
			if (s->notFull() && this->calculator.tryOperation(LOAD, empty.at(0), empty.at(1), empty.at(2)) == WeightBalanceCalculator::BalanceStatus::APPROVED)
				s->load(empty.at(0), empty.at(1), empty.at(2), stopIndex, container);
			else {
				//TODO
			}
		}
	}
	else if (lineError != 0 && isNewError(errors, 1 << 14)) errors = 1 << 14;
	return errors;
}
int _307881359_b::operate(int i, const std::string& travel_port_file_name) {
	int errors = 0;
	unloadAtPort(rout.at(i), i);
	if ((int)getLines(travel_port_file_name).size() > 0 && i < (int)rout.size() - 1) {// IF THERE ARE CONATAINERS AWAITING AT PORT AND THIS STOP IS NOT LAST
		std::vector<std::vector<std::string>> containers = getContainersFromCloseToFar(i, travel_port_file_name);
		std::vector<std::string> toLoad = containers.at(0);
		if (containers.size() == 2) {
			std::vector<std::string> toReject = containers.at(1);
			for (int j = 0; j < (int)toReject.size(); j++) {
				int lineError = validateCargoLine(travel_port_file_name, toReject.at(j), s);
				if (lineError != 1 << 14) // IF THERE IS NO IGNORE ERROR
					s->reject(toReject.at(j), i);
				if (lineError != 0 && isNewError(errors, lineError))
					errors += lineError;
			}
		}
		for (int j = 0; j < (int)toLoad.size(); j++) {
			int loadErrors = tryLoading(travel_port_file_name, toLoad.at(j), i);
			if (loadErrors != 0 && isNewError(errors, loadErrors))
				errors += loadErrors;
		}
	}
	else if ((int)getLines(travel_port_file_name).size() == 0 && i < (int)rout.size() - 1) return 1 << 16;
	else if ((int)getLines(travel_port_file_name).size() > 0 && i == (int)rout.size() - 1) errors += 1 << 17;
	return errors;
}

