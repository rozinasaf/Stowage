# Stowage
Simulator that runs several Algorithms on several Travels and outputs their result

An exercise given in the course Advanced Topics In Programing.

Guidelines:

1. Implement a Simulator that runs several Algorithms on several Travels and outputs their result in a defined format.
2. Use C++ smart pointers to avoid or bring to minimum direct heap memory allocation and deallocation - using std::make_unique and std::make_shared appropriately.
3. Implement two Algorithms as .so (shared objects) so that the Algorithms can be loaded dynamically by the Simulator as explained below.
4. Allow your Simulator to run with multiple threads.

Running the program:
1. compile algorithm and simulator (using the makefiles), algorithm's makefile will create an .so (shared object) file.
2. in the simulator folder (after compiling), run the command line:

simulator -travel_path X -algorithm_path Y -output Z -num_threads N

In ANY order.

Where: X = travel path, Y = algorithms .so files folder path, Z = output files folder path, N = number of threads

-algorithm_path, -output and -num_threads arguments are optional:

In case -algorithm_path is missing, look for the algorithm .so files in the run directory / current working directory.

In case -output is missing:

generate the simulation.results and errors under the run directory / current working directory.
Create a crane instructions folder per <algorithm, travel> pair, under the run directory / current working directory, according to crane instruction file location rules. Those folders shall host the crane_instrucitons files associated with the “pair run”.
The travel folder will contain one or more sub folders, each for every Travel. 

In case -num_threads is missing:
-num_threads = 1

A missing -travel_path argument should result in a fatal error, no algorithm is to be run in this case.

More detailed instuctions on README.pdf file.
