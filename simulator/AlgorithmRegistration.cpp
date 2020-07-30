// AlgorithmRegistration.cpp
#include "../interfaces/AlgorithmRegistration.h"
#include "AlgorithmRegistrar.h"

AlgorithmRegistration::AlgorithmRegistration(std::function<std::unique_ptr<AbstractAlgorithm>()> algorithm_factory)
{
	
	AlgorithmRegistrar::getInstance().registerFactory(algorithm_factory);
}