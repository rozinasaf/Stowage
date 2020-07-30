#ifndef _307881359_B_HEADER
#define _307881359_B_HEADER

#include "../common/Ship.h"
#include "../interfaces/AbstractAlgorithm.h"
#include "../interfaces/WeightBalanceCalculator.h"
#include "../common/Validations.h"
#include "../common/Utils.h"

/*
algorithm b, at each stop:
unload all containers need to be unloaded at current port.
before unloading, moving all containers above if possible to a legal space - 
search for the first legal space you see on the ship, scanning X/Y.
then rejecting far containers and loading to a legal space like described above.
*/

class _307881359_b : public AbstractAlgorithm
{
public:
	_307881359_b(){
	}
	/*return an empty space on the ship as a vector size 3 will not return empty space on position (x,y) x = notI, y = notJ*/
	std::vector<int> getEmptyIndex(int notI, int notJ);

	/*unloads all containers need to be unload. using function: void unload(int i, int j, int k, int n) from ../common/Ship.h*/
	void unloadAtPort(const std::string& port, int n);

	/*return one or two vectors, both containes containeres (as string).
	one is the containers that we shall try loading, sorted from close to far
	the other one is containers that need to be rejected from several reasons*/
	std::vector<std::vector<std::string>> getContainersFromCloseToFar(int currentStopIndex, const std::string& travel_port_file_name);

	/*try loading a container, return 0 if succeded or an error code otherwise*/
	int tryLoading(const std::string& travel_port_file_name, const std::string& container, int stopIndex);

	/*the function called by getInstructionsForCargo.
	doing all operations for all containers on stop*/
	int operate(int i, const std::string& travel_port_file_name);

	/*make a new Ship object as a shared_ptr and calls readShipPlan from ../common/Ship.h
	returns an error code*/
	int readShipPlan(const std::string& full_path_and_file_name);

	/*calls readShipRout from ../common/Ship.h
	returns an error code*/
	int readShipRoute(const std::string& full_path_and_file_name);

	int setWeightBalanceCalculator(WeightBalanceCalculator& calculator) {
		this->calculator = calculator;
		return 0;
	}

	/*
	check if move is ok for position (i,j,p) when p>k. if yes -> move it.
	*/
	void tryMoveAllAbove(int i, int j, int k, int n);
	int getInstructionsForCargo(const std::string& input_file_name, const std::string& output__file_name);
	
protected:
	std::shared_ptr<Ship> s;
	std::vector<std::string> rout;
	WeightBalanceCalculator calculator;

};
#endif