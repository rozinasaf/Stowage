#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../interfaces/WeightBalanceCalculator.h"


int WeightBalanceCalculator::tryOperation(char loadUnload, int kg, int X, int Y)
{
	(void)loadUnload;
	(void)kg;
	(void)X;
	(void)Y;
	int status = WeightBalanceCalculator::BalanceStatus::APPROVED;
	return status;
}