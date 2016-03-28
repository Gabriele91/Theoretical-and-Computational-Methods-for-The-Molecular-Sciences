
PRECISION?= DOUBLE
COMPILER ?= clang++
ARCH     ?= x86_64
CPP 	  = $(COMPILER) -arch $(ARCH)
MKDIR_P   = mkdir -p

O_DIR   = build/makefile
O_MPI   = build/makefile/mpi
I_DIR   = include
I_MPI   = include/mpi
S_DIR   = source
S_MPI   = source/mpi
OUTPUT  = bin

MPI_DIR    ?= /usr/local/Cellar/open-mpi/1.10.2
MPI_INCLUDE = $(MPI_DIR)/include
MPI_LIB     = $(MPI_DIR)/lib

C_FLAGS   = -I$(I_MPI) -I$(I_DIR) -I$(MPI_INCLUDE) -DUSE_${PRECISION} -O3 
CPP_FLAGS = -std=c++11
LD_FLAGS  = -L$(MPI_LIB) -Xlinker -no_deduplicate -lmpi -lmpi_cxx -stdlib=libc++ 

SERIAL_CPP_FILES := $(S_DIR)/main-serial.cpp $(S_DIR)/complexmath.cpp
SERIAL_OBJ_FILES := $(addprefix $(O_DIR)/,$(notdir $(SERIAL_CPP_FILES:.cpp=.o)))

SLB_CPP_FILES := $(S_DIR)/main-slb.cpp $(S_DIR)/complexmath.cpp
SLB_OBJ_FILES := $(addprefix $(O_DIR)/,$(notdir $(SLB_CPP_FILES:.cpp=.o)))

DLB_CPP_FILES := $(S_DIR)/main-dlb.cpp $(S_DIR)/complexmath.cpp
DLB_OBJ_FILES := $(addprefix $(O_DIR)/,$(notdir $(DLB_CPP_FILES:.cpp=.o)))

MPI_CPP_FILES := $(wildcard $(S_MPI)/*.cpp)
MPI_OBJ_FILES := $(addprefix $(O_MPI)/,$(notdir $(MPI_CPP_FILES:.cpp=.o)))


serial: directories $(SERIAL_OBJ_FILES) $(MPI_OBJ_FILES)
	$(CPP) $(LD_FLAGS) -o $(OUTPUT)/serial $(SERIAL_OBJ_FILES) $(MPI_OBJ_FILES)

slb: directories $(SLB_OBJ_FILES) $(MPI_OBJ_FILES)
	$(CPP) $(LD_FLAGS) -o $(OUTPUT)/slb $(SLB_OBJ_FILES) $(MPI_OBJ_FILES)

dlb: directories $(DLB_OBJ_FILES) $(MPI_OBJ_FILES)
	$(CPP) $(LD_FLAGS) -o $(OUTPUT)/dlb $(DLB_OBJ_FILES) $(MPI_OBJ_FILES)

directories: ${O_DIR} ${O_MPI} ${OUTPUT}

${OUTPUT}:
	${MKDIR_P} ${OUTPUT}

${O_DIR}:
	${MKDIR_P} ${O_DIR}

${O_MPI}:
	${MKDIR_P} ${O_MPI}

$(O_DIR)/%.o: $(S_DIR)/%.cpp
	$(CPP) $(C_FLAGS) $(CPP_FLAGS) -c $< -o $@

$(O_MPI)/%.o: $(S_MPI)/%.cpp
	$(CPP) $(C_FLAGS) $(CPP_FLAGS) -c $< -o $@ 

clean:
	rm -f $(O_DIR)/*.o
	rm -f $(O_MPI)/*.o
	rm -R $(O_MPI)
	rm -R $(O_DIR)
