#ifndef SHIP_HEADER
#define SHIP_HEADER
#include "Utils.h"


class Ship : public std::enable_shared_from_this<Ship>
{
public:
	Ship() {
		shipPlan.clear();
		shipRout.clear();
	}
	int& getShipPlanNow(int i, int j) {
		return this->shipPlanNow[i][j];
	}
	int& getShipPlanMax(int i, int j) {
		return this->shipPlanMax[i][j];
	}
	int& getShipPlanOccupied(int i, int j) {
		return this->shipPlanOccupied[i][j];
	}
	int getxMax() {
		return this->xMax;
	}
	int getyMax() {
		return this->yMax;
	}
	const std::string getContainer(int i, int j, int k) {
		return shipPlan.at(i).at(j).at(k);
	}

	/*return vector of all operations made on stop i on rout*/
	const std::vector<std::string> getOperations(int i) {
		return this->operations.at(i);
	}
	const std::vector<std::string> getShipRout() {
		return this->shipRout;
	}
	std::string getLastPort();
	void setContainer(int x, int y, int f, const std::string& container);

	/*read ship_plan file from line 2 and on
	return error code
	part of readShipPlan function*/
	int readIndexPlan(const std::vector<std::string>& shipPlanData);
	int readShipPlan(const std::string& file_name);

	/*valid that each stop in rout has a cargo_data file.
	return error code*/
	int validNumOfFilesOfPort(const std::string file_name_rout);
	int countValidPortsInRout(const std::string& file_name_rout);
	int readShipRoute(const std::string& file_name_rout);
	/*checks if a position is full to the last floor*/
	int posNotFull(int x, int y);

	/*checks is ship is not full*/
	int notFull();

	/*get a cargo data file name and returns the index of its stop in the rout.
	for example the rout:
	AAAAA
	PPPPP
	BBBBB
	PPPPP
	and the file PPPPP_2.cargo_data
	return 3
	*/
	int getIndexOfStop(const std::string& file_name_cargo_data);

	/*
	return how many times a port occurres in the rest (from j+1 and on) of the rout
	*/
	int timeInRout(int j, const std::string& port_name);

	/*
	validates the a container is not "in the air"
	*/
	int noEmptyCellsBelow(int i, int j, int k);

	/*
	unloads all containers at shipPlan[i][j][S] when S>k
	save them, unload shipPlan[i][j][k]
	loads all containers we saved
	decrease shipPlanNow[i][j] by 1
	*/
	void unload(int i, int j, int k, int n);
	
	/*
	loads a container to shipPlan[i][j][k]
	increase shipPlanNow[i][j] by one
	*/
	void load(int i, int j, int k, int n, const std::string& container);
	void reject(std::string container, int n);
	void move(int si, int sj, int sk, int di, int dj, int dk, int n, const std::string& container);

	/*
	initialize 2D vector size of shipRout.size
	first element is the name of the port
	*/
	void initOperations();

	/*
	return first stop at port portName after currentIndex on rout
	*/
	int firstOccurenceAfterIndex(const std::string& portName, int currentIndex);
	
	/*
	checks if ship is empty
	*/
	int isEmpty();
	int isNewToShip(const std::string& id);

	/*get a carne instuctions file and search for the right cargo_data file, then searches the ID of the container and returns his destenation*/
	std::string getDestOfContFromInstructions(const std::string& input_cargo_data_file, int indexOfStop, const std::string& operation);

	/*get a carne instuctions file and search for the right cargo_data file, then searches the ID of the container and returns the line it was founded at*/
	std::string getCargoDataFromOperation(const std::string& input_cargo_data_file, int indexOfStop, const std::string& opertaion);
private:
	int xMax = 0;
	int yMax = 0;
	int fMax = 0;
	std::vector<std::vector<int>> shipPlanMax;
	std::vector<std::vector<int>> shipPlanNow;
	std::vector<std::vector<int>> shipPlanOccupied;
	std::vector<std::vector<std::vector<std::string>>> shipPlan;
	std::vector<std::string> shipRout;
	std::vector<std::vector<std::string>> operations;
	int countPortInRout(std::string& port);
	void initShipPlan();

};
#endif