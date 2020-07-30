#pragma once
#include "../common/Utils.h"
#include "AlgorithmRegistrar.h"
#include "../interfaces/AlgorithmRegistration.h"
#include "../common/Ship.h"
#include "PairTask.h"
#include "../interfaces/WeightBalanceCalculator.h"
#define FILE_TYPE_NAME ".so"


class Simulator
{
public:
	Simulator(int argc, char** argv) {
		this->arguments = getArgs(argc, argv);
		numThreads = stoi(arguments.at(3));
		std::vector<std::string> SOAlgorithms = getFilesWithSuffix(arguments.at(1), FILE_TYPE_NAME);
		if ((int)SOAlgorithms.size() == 0)
			errorVec.push_back("There are no SO files in algorithm folder");
		std::vector<std::string> SOAlgorithmsNames = getSOAlgorithms(arguments.at(1));
		int before = (int)AlgorithmRegistrar::getInstance().getFactories().size();
		for (int i = 0; i < (int)SOAlgorithms.size(); i++) {
			std::string SOAlgoNumberI = arguments.at(1) + "/" + SOAlgorithms.at(i);
			if (!AlgorithmRegistrar::getInstance().loadAlgorithmFromFile(SOAlgoNumberI.c_str())) {
				errorVec.push_back("Problem on loading in the algorithm's SO file " + SOAlgorithmsNames.at(i));
			}
			else if (before == (int)AlgorithmRegistrar::getInstance().getFactories().size()) {
				errorVec.push_back("Problem on storage of the algorithm " + SOAlgorithmsNames.at(i));
			}
			else {
				before = AlgorithmRegistrar::getInstance().getFactories().size();
				algorithmsNames.push_back(SOAlgorithmsNames.at(i));
			}
		}
		this->algoVec = AlgorithmRegistrar::getInstance().getFactories();
		initResults();
	}
	/*
	simulate all pairs <travel , algorithm>
	*/
	void simulate();
	/*
	initialize resilts table
	*/
	void initResults();
	void simulateTravel(const std::string& travel, const std::string& output, const std::vector<std::string>& travel_folders, int travelIndex);
	
	int getTravelErrors(const std::string& path);

	void executeSimulationsSingleThread(std::vector<PairTask>& tasks);

	void executeSimulationsMultiThread(std::vector<PairTask>& tasks);

	void addPairTask(std::vector<std::vector<std::string>>& results, std::vector<std::string>& errorVec, int pairOperations, const std::string& type, const std::string& travel, const std::string& output,
		const std::vector<std::string>& travel_folders, int travelIndex, int algoIndex, std::function<std::unique_ptr<AbstractAlgorithm>()>& algoFunc) {
		PairTask task(results, errorVec, pairOperations, type, travel, output, travel_folders, travelIndex, algoIndex, algoFunc);
		tasks.push_back(task);
	}

	int numThreads;
	std::vector<PairTask> tasks;
	std::vector<std::string> algorithmsNames;
	std::vector<std::string> errorVec;
	std::vector<std::string> arguments;
	std::vector<std::function<std::unique_ptr<AbstractAlgorithm>()>> algoVec;
	std::vector<std::vector<std::string>> results;
};



