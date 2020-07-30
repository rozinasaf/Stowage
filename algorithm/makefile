COMP = g++-9.3.0
INTERFACEDIR = ../interfaces
COMMONDIR = ../common
SHARED_OBJS = _307881359_a.so _307881359_b.so
OBJS = _307881359_a.o _307881359_b.o $(COMMONDIR)/Ship.o $(COMMONDIR)/Utils.o $(COMMONDIR)/Validations.o 

CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG -fPIC -fvisibility=hidden
CPP_LINK_FLAG = -lstdc++fs -shared

all: $(SHARED_OBJS)

_307881359_a.so:  _307881359_a.o $(COMMONDIR)/Ship.o $(COMMONDIR)/Utils.o $(COMMONDIR)/Validations.o
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^
_307881359_b.so:  _307881359_b.o $(COMMONDIR)/Ship.o $(COMMONDIR)/Utils.o $(COMMONDIR)/Validations.o
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^

_307881359_a.o: _307881359_a.cpp _307881359_a.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
_307881359_b.o: _307881359_b.cpp _307881359_b.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
$(COMMONDIR)/Ship.o: $(COMMONDIR)/Ship.cpp $(COMMONDIR)/Ship.h
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
$(COMMONDIR)/Utils.o: $(COMMONDIR)/Utils.cpp $(COMMONDIR)/Utils.h
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
$(COMMONDIR)/Validations.o: $(COMMONDIR)/Validations.cpp $(COMMONDIR)/Validations.h
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp



clean:
	rm -f $(OBJS) $(SHARED_OBJS)