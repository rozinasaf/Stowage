#pragma once
#include <vector>
#include <string>
#include <iostream>

class WeightBalanceCalculator
{
public:
	WeightBalanceCalculator() {
		//TODO
	}
	WeightBalanceCalculator(const std::string& file_name) {
		(void)file_name;
	}
	enum BalanceStatus {APPROVED, X_IMBALANCED, Y_IMBALANCED, X_Y_IMBALANCED };
	int tryOperation(char loadUnload, int kg, int X, int Y);
};