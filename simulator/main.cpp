#include <iostream>
#include "../interfaces/AbstractAlgorithm.h"
#include "../interfaces/AlgorithmRegistration.h"
#include "Simulator.h"
#include "../common/Utils.h"
#include "AlgorithmRegistrar.h"


int main(int argc, char** argv) {
	Simulator sim(argc, argv);
	sim.simulate();
}