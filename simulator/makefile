COMP = g++-9.3.0
INTERFACEDIR = ../interfaces
COMMONDIR = ../common
OBJS = main.o Simulator.o AlgorithmRegistrar.o AlgorithmRegistration.o $(COMMONDIR)/Utils.o $(COMMONDIR)/Validations.o $(COMMONDIR)/Ship.o WeightBalanceCalculator.o
EXEC = simulator
CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG -g
CPP_LINK_FLAG = -lstdc++fs -ldl -export-dynamic -pthread

$(EXEC): $(OBJS)
	$(COMP) $(OBJS) $(CPP_LINK_FLAG) -o $@

main.o: main.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
Simulator.o: Simulator.cpp Simulator.h
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
AlgorithmRegistrar.o: AlgorithmRegistrar.cpp AlgorithmRegistrar.h
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
AlgorithmRegistration.o: AlgorithmRegistration.cpp $(INTERFACEDIR)/AlgorithmRegistration.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
$(COMMONDIR)/Utils.o: $(COMMONDIR)/Utils.cpp $(COMMONDIR)/Utils.h
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
$(COMMONDIR)/Validations.o: $(COMMONDIR)/Validations.cpp $(COMMONDIR)/Validations.h
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
$(COMMONDIR)/Ship.o: $(COMMONDIR)/Ship.cpp $(COMMONDIR)/Ship.h
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
WeightBalanceCalculator.o: WeightBalanceCalculator.cpp 
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
$(INTERFACEDIR)/AbstractAlgorithm.o: $(INTERFACEDIR)/AbstractAlgorithm.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

clean:
	rm -f $(OBJS) $(EXEC)