CC = g++
CFLAGS = -O3 -std=c++11
CTHREAD = -lpthread -lrt

# edit this block to the directory of the Gurobi optimizer
# GUROBI_DIR = /usr/gurobi902/linux64
# GUROBI_HEADER_DIR = /include
# GUROBI_LIB = /lib

# GUROBI_INCLUDE_HEADER = -I$(GUROBI_DIR)$(GUROBI_HEADER_DIR)
# GUROBI_LINK_LIGRARY = -L$(GUROBI_DIR)$(GUROBI_LIB) -lgurobi_c++ -lgurobi90 

OBJECTS = src/main.o src/SA.o src/Graph.o


# EXECUTABLE = ../bin/SA.exe
EXECUTABLE = bin/sa_3dbp

CONST_HEADER = src/Include.h

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(CONST_OBJ) $(CONST_HEADER)
	$(CC) $(OBJECTS) $(CONST_OBJ) -o $(EXE_PATH)$(EXECUTABLE) $(GUROBI_INCLUDE_HEADER) $(CTHREAD) $(GUROBI_LINK_LIGRARY)

Graph.o: $(CONST_HEADER)
	$(CC) $(CFLAGS) -c  src/Graph.cpp $(CTHREAD) $(GUROBI_LINK_LIGRARY)

SA.o: $(CONST_HEADER)
	$(CC) $(CFLAGS) -c  src/SA.cpp $(CTHREAD) $(GUROBI_LINK_LIGRARY)

main.o: src/SA.o $(CONST_HEADER)
	$(CC) $(CFLAGS) -c  src/main.cpp $(CTHREAD) $(GUROBI_LINK_LIGRARY)

clean:
	rm -f src/*.o $(EXECUTABLE)
