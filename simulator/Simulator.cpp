#include <iostream>
#include <fstream>
#include "../interfaces/AbstractAlgorithm.h"
#include "../interfaces/AlgorithmRegistration.h"
#include "../common/Utils.h"
#include "../common/Ship.h"
#include "Simulator.h"
#include "../common/Validations.h"
#include "../interfaces/WeightBalanceCalculator.h"
#include "AlgorithmRegistrar.h"
#include "PairTaskProducer.h"
#include "ThreadPoolExecuter.h"
#include "ThreadsStrongTypes.h"
#include "PairTask.h"

#define LAST "LAST"
#define EMPTY "empty"
#define LOAD 'L'
#define UNLOAD 'U'
#define REJECT 'R'
#define MOVE 'M'
#define DONT_WRITE_OPERATION -1
#define RESULTS "Results,"
#define NO_FILE "NO_FILE"
#define NONE "NONE"

int PairTask::simulateStop(std::unique_ptr<AbstractAlgorithm>& curr_algo, const std::string& input_cargo_data_file, const std::string& output_cargo_data_file, const std::string& craneInstDir, const std::string& type, int indexOfStop) {

	errorVec.push_back("[VISIT #" + std::to_string(simulatorShip->timeInRout(indexOfStop, simulatorShip->getShipRout().at(indexOfStop))) +
		" AT PORT " + simulatorShip->getShipRout().at(indexOfStop) + "]:");

	int algoErrors = 0;
	try {
		algoErrors = curr_algo->getInstructionsForCargo(input_cargo_data_file, output_cargo_data_file);
	}
	catch (...)
	{
		removeFailedAlgorithmFiles(craneInstDir);
		pairOperations = -1;
		errorVec.push_back("-------------------ALGORITHM " + type + " RUN TIME ERROR!-------------------");
		return -1;
	}

	std::vector<std::string> algoErrorVec = getErrorVec(algoErrors);
	errorVec.insert(errorVec.end(), algoErrorVec.begin(), algoErrorVec.end());
	return algoErrors;
}
void PairTask::simulatePair() {
	int algoBehaviorErrors = 0;
	int numOfErrors = 0;
	std::string input_cargo_data_file, output_cargo_data_file;
	std::string craneInstDir = createDir(output, type + "_" + travel_folders.at(travelIndex) + "_crane_instructions");
	std::unique_ptr<AbstractAlgorithm> curr_algo = algoFunc();
	WeightBalanceCalculator* calculator = new WeightBalanceCalculator();
	setSimulatorShip(travel);
	errorVec.push_back(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::");
	errorVec.push_back("[PAIR TRAVEL - " + travel.substr(travel.rfind("/") + 1) + ", ALGORITHM - " + type + " ]:");
	int algoTravelErrors = initAlgorithm(travel, calculator, curr_algo);
	if (algoTravelErrors != 0) {
		errorVec.push_back("[ALGORITHM " + type + ", RECOGNIZED THE FOLLOWING ERRORS]:");
		std::vector<std::string> algoTravelErrorVec = getErrorVec(algoTravelErrors);
		errorVec.insert(errorVec.end(), algoTravelErrorVec.begin(), algoTravelErrorVec.end());
	}
	if (isLegalTravel(algoTravelErrors)) {
		for (int k = 0; k < (int)simulatorShip->getShipRout().size(); k++) {
			input_cargo_data_file = travel + "/" + simulatorShip->getShipRout().at(k) + "_" + std::to_string(simulatorShip->timeInRout(k, simulatorShip->getShipRout().at(k))) + ".cargo_data";
			if (!isFile(input_cargo_data_file)) {
				std::ofstream input(input_cargo_data_file);
				input.close();
			}
			output_cargo_data_file = craneInstDir + "/" + simulatorShip->getShipRout().at(k) + "_" + std::to_string(simulatorShip->timeInRout(k, simulatorShip->getShipRout().at(k))) + ".crane_instructions";
			int algoErrors = simulateStop(curr_algo, input_cargo_data_file, output_cargo_data_file, craneInstDir, type, k);
			if (algoErrors != 0) {
				if (algoErrors == -1) { //RUN TIME ERROR
					pairOperations = -1;
					numOfErrors += 1;
					break;
				}
			}
			try {
				algoBehaviorErrors = validateAlgorithmOperations(input_cargo_data_file, output_cargo_data_file, k);
				numOfErrors += getNumOfErrors(algoBehaviorErrors);
				if (algoBehaviorErrors != 0) {
					errorVec.push_back("Algorithm" + type + "[BAD ALGORITHM BEHAVIOR ERRORS]: ");
					std::vector<std::string> algoErrorVec = getErrorVec(algoBehaviorErrors);
					errorVec.insert(errorVec.end(), algoErrorVec.begin(), algoErrorVec.end());
					errorVec.push_back("PAIR: ALGORITHM " + type + " Travel " + travel_folders.at(travelIndex) + " STOPED DUE TO BAD ALGORITHM BEHAVIOR/S. PAIR OUTPUT FILES DELETED!");
					pairOperations = -1;
					removeFailedAlgorithmFiles(craneInstDir);
					break;
				}
				else {
					pairOperations += getNumOfOperations(output_cargo_data_file);
				}
			}
			catch (...) {
				errorVec.push_back("Algorithm" + type + "[BAD ALGORITHM BEHAVIOR ERRORS]: ");
				errorVec.push_back("PAIR: ALGORITHM " + type + " Travel " + travel_folders.at(travelIndex) + " STOPED DUE TO BAD ALGORITHM BEHAVIOR/S. PAIR OUTPUT FILES DELETED!");
				errorVec.push_back("ERROR- algorithm behavior: algorithm did an illegal operation");
				pairOperations = -1;
				removeFailedAlgorithmFiles(craneInstDir);
				break;
			}
		}
	}
	simulatorErrorVec.insert(simulatorErrorVec.end(), errorVec.begin(), errorVec.end());
	this->results.at(algoIndex + 1).at(travelIndex + 1) = std::to_string(pairOperations) + ",";
	setTotals(algoIndex, pairOperations, numOfErrors);
}
void Simulator::simulateTravel(const std::string& travel, const std::string& output, const std::vector<std::string>& travel_folders, int travelIndex) {
	std::string type = NONE;
	for (int j = 0; j < (int)algoVec.size(); j++) {
		type = algorithmsNames.at(j);
		addPairTask(results, errorVec, 0, type, travel, output, travel_folders, travelIndex, j, algoVec.at(j));
	}
}
void Simulator::simulate() {
	std::string travels = arguments.at(0), output = arguments.at(2);
	if (travels.compare(NO_FILE) == 0) {
		errorVec.push_back("Simulator [FATAL ERRORS]:");
		errorVec.push_back("ERROR: missing or bad -travel_path argument (fatal error)");
	}
	else {
		std::vector<std::string> travel_folders = getFileNames(travels);
		int travelErrors = 0;
		for (int i = 0; i < (int)travel_folders.size(); i++) {
			std::string travel = travels + "/" + travel_folders.at(i);
			errorVec.push_back(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::");
			errorVec.push_back("[TRAVEL " + travel_folders.at(i) + " ERRORS  DETECTED BY SIMULATOR]:");
			travelErrors = getTravelErrors(travel);
			std::vector<std::string> travelErrorVec = getErrorVec(travelErrors);
			errorVec.insert(errorVec.end(), travelErrorVec.begin(), travelErrorVec.end());
			if (isLegalTravel(travelErrors)) {
				simulateTravel(travel, output, travel_folders, i);
			}
		}
		numThreads == 1 ? executeSimulationsSingleThread(tasks) : executeSimulationsMultiThread(tasks);
		toTable(results, output + "/simulation.results");

	}
	writeErrors(output + "/simulation.errors", errorVec);
}
int PairTask::setSimulatorShip(const std::string& path) {
	int errors = 0;
	simulatorShip = std::make_shared<Ship>();
	errors += simulatorShip->readShipPlan(path + "/" + getShipPlanFile(path));
	errors += simulatorShip->readShipRoute(path + "/" + getRoutFile(path));
	errors += simulatorShip->validNumOfFilesOfPort(path + "/" + getRoutFile(path));
	simulatorShip->initOperations();
	return errors;
}
int Simulator::getTravelErrors(const std::string& path) {
	int errors = 0;
	std::unique_ptr<Ship> s = std::make_unique<Ship>();
	errors += s->readShipPlan(path + "/" + getShipPlanFile(path));
	errors += s->readShipRoute(path + "/" + getRoutFile(path));
	if (isFile(path + "/" + getRoutFile(path))) {
		errors += s->validNumOfFilesOfPort(path + "/" + getRoutFile(path));
	}
	return errors;
}
void PairTask::setTotals(int algoIndex, int numOfOperationsToAdd, int numOfErrors) {
	if (numOfOperationsToAdd >= 0) {
		std::string currTotalStr = this->results.at(algoIndex + 1).at(results.at(algoIndex + 1).size() - 2);
		int currentTotal;
		if (currTotalStr.size() == 0)
			this->results.at(algoIndex + 1).at(results.at(algoIndex + 1).size() - 2) = std::to_string(numOfOperationsToAdd) + ",";
		else {
			currentTotal = std::stoi(this->results.at(algoIndex + 1).at(results.at(algoIndex + 1).size() - 2).substr(0, currTotalStr.size() - 1));
			this->results.at(algoIndex + 1).at(results.at(algoIndex + 1).size() - 2) = std::to_string(currentTotal + numOfOperationsToAdd) + ",";
		}
	}
	this->results.at(algoIndex + 1).at(results.at(algoIndex + 1).size() - 1) = std::to_string(stoi(this->results.at(algoIndex + 1).at(results.at(algoIndex + 1).size() - 1)) + numOfErrors);
}
int PairTask::getNumOfOperations(const std::string& output_cargo_data_file) {
	int count = 0;
	std::vector<std::string> ops = getLines(output_cargo_data_file);
	for (int i = 0; i < (int)ops.size(); i++) {
		if (ops.at(i).at(0) == MOVE) count += 3;
		if (ops.at(i).at(0) == LOAD || ops.at(i).at(0) == UNLOAD) count += 5;
	}
	return count;
}
int PairTask::initAlgorithm(const std::string& travel, WeightBalanceCalculator* calculator, std::unique_ptr<AbstractAlgorithm>& curr_algo) {
	int errors = 0;
	errors += curr_algo->readShipPlan(travel + "/" + getShipPlanFile(travel));
	errors += curr_algo->readShipRoute(travel + "/" + getRoutFile(travel));
	errors += curr_algo->setWeightBalanceCalculator(*calculator);
	return errors;
}
void Simulator::initResults() {
	if (arguments.at(0).compare(NO_FILE) != 0) {
		std::string type;
		std::vector<std::string> travelNames = getFileNames(arguments.at(0)); //arguments.at(0) = -travel_path
		this->results.resize(algoVec.size() + 1); //+2 for cell Sum
		for (int i = 0; i < (int)algoVec.size() + 1; i++)
			this->results[i].resize(travelNames.size() + 3); // +2 for cell = "RESULTS" cell = sum and cell = Num errors
		results.at(0).at(0) = RESULTS;
		for (int i = 0; i < (int)travelNames.size(); i++)
			results.at(0).at(i + 1) = travelNames.at(i) + ",";
		results.at(0).at(travelNames.size() + 1) = "Sum,";
		results.at(0).at(travelNames.size() + 2) = "Num errors";
		for (int i = 0; i < (int)algoVec.size(); i++) {
			type = algorithmsNames.at(i);
			results.at(i + 1).at(0) = type + ",";
		}
		for (int i = 1; i < (int)results.size(); i++)
			for (int j = 1; j < (int)results.at(i).size(); j++)
				if (j == (int)results.at(i).size() - 1) this->results.at(i).at(j) = "0";
				else this->results.at(i).at(j) = "0,";
	}
}
int PairTask::validateAlgorithmOperations(const std::string& input_cargo_data_file, const std::string& output_cargo_data_file, int indexOfStop) {
	std::vector<std::string> operationVec = getLines(output_cargo_data_file), rejectedBecauseShipWasFull, loaded, opData;
	std::string dest, contID;
	for (int i = 0; i < (int)operationVec.size(); i++) {
		if (indexOfStop == (int)simulatorShip->getShipRout().size() - 1 && operationVec.at(i).at(0) == LOAD)
			return 1 << 21;
		else if (validOperationLine(operationVec.at(i))) {
			if (validateOperation(operationVec, rejectedBecauseShipWasFull, loaded, i, indexOfStop, input_cargo_data_file))
				return validateOperation(operationVec, rejectedBecauseShipWasFull, loaded, i, indexOfStop, input_cargo_data_file);
		}
		else if (operationVec.at(i).at(0) != REJECT) {
			if (simulatorShip->getCargoDataFromOperation(input_cargo_data_file, indexOfStop, operationVec.at(i)) == NONE)
				return 1 << 22;
		}
		if (!validateMap())
			return 1 << 23;
		else if (operationVec.at(i).at(0) == LOAD)
			loaded.push_back(dest);
	}
	if (indexOfStop == (int)simulatorShip->getShipRout().size() - 1 && !simulatorShip->isEmpty())
		return 1 << 24;
	return 0;
}
int PairTask::validateOperation(const std::vector<std::string>& operationVec, std::vector<std::string>& rejectedBecauseShipWasFull, const std::vector<std::string>& loaded, int opIndex, int indexOfStop, const std::string& input_cargo_data_file) {
	std::vector<std::string> opData;
	std::string dest, contID;
	dest = simulatorShip->getDestOfContFromInstructions(input_cargo_data_file, indexOfStop, operationVec.at(opIndex));
	opData = splitByComma(operationVec.at(opIndex));
	contID = opData.at(1);
	if (operationVec.at(opIndex).at(0) == LOAD) {
		if (!simulatorShip->firstOccurenceAfterIndex(dest, indexOfStop)) return 1 << 27;
		if (!simulatorShip->isNewToShip(contID)) 	return 1 << 28;
	}
	if (simulatorShip->notFull() || operationVec.at(opIndex).at(0) == UNLOAD) {
		if (operationVec.at(opIndex).at(0) == LOAD)
			if (validatePriorityLoading(rejectedBecauseShipWasFull, dest, indexOfStop)) { //CANT LOAD IF A CLOSER ONE WAS REJECTED
				doOperation(operationVec.at(opIndex));
				return 0;
			}
			else	return 1 << 26;
		else {
			doOperation(operationVec.at(opIndex));
			return 0;
		}
	}
	else if (operationVec.at(opIndex).at(0) == REJECT) {
		rejectedBecauseShipWasFull.push_back(dest);
		if (validateFarRejecting(loaded, dest, indexOfStop)) { //CANT REJECT IF A FAREST ONE WAS LOADED
			doOperation(operationVec.at(opIndex));
			return 0;
		}
		else	return 1 << 26;
	}
	else if (operationVec.at(opIndex).at(0) != MOVE) return 1 << 25;
	else return 0;
}

int PairTask::validateMap() {
	for (int i = 0; i < simulatorShip->getxMax(); i++) {
		for (int j = 0; j < simulatorShip->getyMax(); j++)
			for (int k = 0; k < simulatorShip->getShipPlanNow(i, j); k++)
				if (simulatorShip->getContainer(i, j, k + simulatorShip->getShipPlanOccupied(i, j)).compare(EMPTY) != 0
					&& (k >= simulatorShip->getShipPlanMax(i, j) || !simulatorShip->noEmptyCellsBelow(i, j, k + simulatorShip->getShipPlanOccupied(i, j)))) {

					return 0;
				}
	}
	return 1;
}
void PairTask::doOperation(const std::string operation) {
	std::vector<std::string> vecStr = splitByComma(operation);
	std::vector<int> vec = stoiVector(std::vector(vecStr.begin() + 2, vecStr.end()));
	if (operation.at(0) == LOAD) {
		simulatorShip->load(vec.at(1), vec.at(2), vec.at(0), DONT_WRITE_OPERATION, "1");
	}
	else if (operation.at(0) == UNLOAD) {
		simulatorShip->unload(vec.at(1), vec.at(2), vec.at(0), DONT_WRITE_OPERATION);
	}
	else if (operation.at(0) == MOVE) {
		std::vector<std::string> dataMove = splitByComma(operation);
		std::vector<std::string> indexesUStr = std::vector(dataMove.begin() + 2, dataMove.begin() + 5);
		std::vector<std::string> indexesLStr = std::vector(dataMove.begin() + 5, dataMove.end());
		std::vector<int> indexesU = stoiVector(indexesUStr);
		std::vector<int> indexesL = stoiVector(indexesLStr);
		simulatorShip->unload(indexesU.at(1), indexesU.at(2), indexesU.at(0), DONT_WRITE_OPERATION);
		simulatorShip->load(indexesL.at(1), indexesL.at(2), indexesL.at(0), DONT_WRITE_OPERATION, "1");
	}
	else {
		//REJECT=NOTHING TO CHECK (MAP WISE), SHIP IS NOT FULL
	}
}
int PairTask::validatePriorityLoading(const std::vector<std::string>& rejected, const std::string& operationPort, int currentStopIndex) {
	int thisOperationIndexInRout = simulatorShip->firstOccurenceAfterIndex(operationPort, currentStopIndex);
	for (int i = 0; i < (int)rejected.size(); i++)
	{
		if (simulatorShip->firstOccurenceAfterIndex(rejected.at(i), currentStopIndex) < thisOperationIndexInRout)
			return 0;
	}
	return 1;
}
int PairTask::validateFarRejecting(const std::vector<std::string>& loaded, const std::string& operationPort, int currentStopIndex) {
	int thisOperationIndexInRout = simulatorShip->firstOccurenceAfterIndex(operationPort, currentStopIndex);
	for (int i = 0; i < (int)loaded.size(); i++)
	{
		if (simulatorShip->firstOccurenceAfterIndex(loaded.at(i), currentStopIndex) > thisOperationIndexInRout)
			return 0;
	}
	return 1;
}
void Simulator::executeSimulationsSingleThread(std::vector<PairTask>& tasks) {
	errorVec.push_back(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::");
	errorVec.push_back(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::");
	errorVec.push_back("::::::::::::::::::::::::::::::::: P A I R S :::::::::::::::::::::::::::::::::");
	errorVec.push_back(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::");
	for (auto& task : tasks) {
		task.simulatePair();
	}
}
void Simulator::executeSimulationsMultiThread(std::vector<PairTask>& tasks) {
	errorVec.push_back(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::");
	errorVec.push_back(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::");
	errorVec.push_back("::::::::::::::::::::::::::::::::: P A I R S :::::::::::::::::::::::::::::::::");
	errorVec.push_back(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::");
	ThreadPoolExecuter executor{
		PairTaskProducer(tasks),
		NumThreads{numThreads}
	};
	executor.start();
	executor.wait_till_finish();
}


