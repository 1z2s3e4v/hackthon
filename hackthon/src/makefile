CC = g++
CFLAGS = -O3 -std=c++11 -g
CTHREAD = -lpthread -lrt

# edit this block to the directory of the Gurobi optimizer
# GUROBI_DIR = /usr/gurobi902/linux64
# GUROBI_HEADER_DIR = /include
# GUROBI_LIB = /lib

# GUROBI_INCLUDE_HEADER = -I$(GUROBI_DIR)$(GUROBI_HEADER_DIR)
# GUROBI_LINK_LIGRARY = -L$(GUROBI_DIR)$(GUROBI_LIB) -lgurobi_c++ -lgurobi90 

OBJECTS = main.o SA.o Graph.o


# EXECUTABLE = ../bin/SA.exe
EXECUTABLE = test.exe

CONST_HEADER = Include.h

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(CONST_OBJ) $(CONST_HEADER)
	$(CC) $(OBJECTS) $(CONST_OBJ) -o $(EXE_PATH)$(EXECUTABLE) $(GUROBI_INCLUDE_HEADER) $(CTHREAD) $(GUROBI_LINK_LIGRARY)

Graph.o: $(CONST_HEADER)
	$(CC) $(CFLAGS) $(GUROBI_INCLUDE_HEADER) -c  Graph.cpp $(CTHREAD) $(GUROBI_LINK_LIGRARY)

SA.o: $(CONST_HEADER)
	$(CC) $(CFLAGS) $(GUROBI_INCLUDE_HEADER) -c  SA.cpp $(CTHREAD) $(GUROBI_LINK_LIGRARY)

main.o: SA.o $(CONST_HEADER)
	$(CC) $(CFLAGS) $(GUROBI_INCLUDE_HEADER) -c  main.cpp $(CTHREAD) $(GUROBI_LINK_LIGRARY)

clean:
	rm -f *.o $(EXECUTABLE)
