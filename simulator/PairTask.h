#pragma once
#include <iostream>
#include "../common/Ship.h"
#include "../interfaces/AbstractAlgorithm.h"

/*
this module is a part of the simulator - simulate a pair.
every object is a task for the producer.
implements are at Simulator.cpp.
*/

class PairTask 
{
	std::vector<std::vector<std::string>>& results; 
	std::vector<std::string> errorVec; 
	std::vector<std::string>& simulatorErrorVec; 
	int pairOperations;
	std::shared_ptr<Ship> simulatorShip;
	std::string type;
	const std::string travel;
	const std::string output;
	const std::vector<std::string>& travel_folders;
	int travelIndex;
	int algoIndex;
	std::function<std::unique_ptr<AbstractAlgorithm>()> algoFunc;
public:

	PairTask(std::vector<std::vector<std::string>>& results, std::vector<std::string>& simulatorErrorVec, int pairOperations, const std::string& type, const std::string& travel, const std::string& output,
		const std::vector<std::string>& travel_folders, int travelIndex, int algoIndex, std::function<std::unique_ptr<AbstractAlgorithm>()> algoFunc) :
		results(results), simulatorErrorVec(simulatorErrorVec), pairOperations(pairOperations),  type(type), travel(travel), output(output), travel_folders(travel_folders), travelIndex(travelIndex), algoIndex(algoIndex), algoFunc(algoFunc) {
	}

	void simulatePair();
	int simulateStop(std::unique_ptr<AbstractAlgorithm>& curr_algo, const std::string& input_cargo_data_file, const std::string& output_cargo_data_file, const std::string& craneInstDir, const std::string& type, int indexOfStop);
	int getNumOfOperations(const std::string& output_cargo_data_file);
	int validateAlgorithmOperations(const std::string& input_cargo_data_file, const std::string& output_cargo_data_file, int indexOfStop);
	int validateOperation(const std::vector<std::string>& operationVec, std::vector<std::string>& rejectedBecauseShipWasFull, const std::vector<std::string>& loaded, int opIndex, int indexOfStop, const std::string& input_cargo_data_file);
	/*
	check if map status is legal (no containers "in the air" etc...)
	*/
	int validateMap();
	
	/*
	calls algorithm readShipPlan, readShipRout, setWeightBalanceCalculator
	*/
	int initAlgorithm(const std::string& travel, WeightBalanceCalculator* calculator, std::unique_ptr<AbstractAlgorithm>& curr_algo);
	
	/*
	update total operations and errors of algorithm
	*/
	void setTotals(int algoIndex, int numOfOperationsToAdd, int numOfErrors);
	
	/*
	do operation on simulator ship
	*/
	void doOperation(const std::string operation);
	
	/*
	if algorithm rejected container X becuase ship was full and now loading container Y -> valids that Y is closer then X
	*/
	int validatePriorityLoading(const std::vector<std::string>& rejected, const std::string& operationPort, int currentStopIndex);

	/*
	if algorithm loaded container X and now rejecting Y becuase ship is full -> valids that X is closer then Y
	*/
	int validateFarRejecting(const std::vector<std::string>& loaded, const std::string& operationPort, int currentStopIndex);
	int setSimulatorShip(const std::string& path);

};
