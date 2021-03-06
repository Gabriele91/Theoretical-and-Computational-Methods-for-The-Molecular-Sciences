
PRECISION?= DOUBLE
COMPILER ?= clang++
ARCH     ?= $(uname -m)
CPP 	  = $(COMPILER) 
MKDIR_P   = mkdir -p

PRECISION_LOW := $(shell echo $(PRECISION)     | tr "[:upper:]" "[:lower:]")
PRECISION_LOW := $(shell echo $(PRECISION_LOW) | tr "_" "-")

O_BASE  = build/makefile
O_DIR   = $(O_BASE)-$(PRECISION_LOW)-$(ARCH)
O_MPI   = $(O_BASE)-$(PRECISION_LOW)-$(ARCH)/mpi
I_DIR   = include
I_MPI   = include/mpi
S_DIR   = source
S_MPI   = source/mpi
OUTPUT  = bin

MPI_DIR    ?= /usr/local/Cellar/open-mpi/1.10.2
MPI_INCLUDE = $(MPI_DIR)/include
MPI_LIB     = $(MPI_DIR)/lib
MPI_CH     ?= false

ifeq ($(MPI_CH),true)
	MPI_REF = -lmpich -lmpichcxx
else
	MPI_REF = -lmpi -lmpi_cxx
endif

C_FLAGS   = -I$(I_MPI) -I$(I_DIR) -I$(MPI_INCLUDE) -DUSE_${PRECISION} -O3 
CPP_FLAGS = -std=c++11
LD_FLAGS  = -L$(MPI_LIB) -Xlinker $(MPI_REF) 

IS_INTEL ?= false
## if use a intel compiler 
## enable the ipo flag
ifeq ($(IS_INTEL),true)
#   CPP_FLAGS += -ipo
	LD_FLAGS  += -march=$(ARCH)
else
	LD_FLAGS  += -arch $(ARCH) -no_deduplicate -stdlib=libc++ 
endif

HIDE_MPI_STATUS?=true
#if use mpich disable "HIDE_MPI_STATUS"
ifeq ($(HIDE_MPI_STATUS),true)
	CPP_FLAGS += -DDIABLE_HIDE_MPI_STATUS
endif

SERIAL_CPP_FILES := $(S_DIR)/main-serial.cpp $(S_DIR)/complexmath.cpp
SERIAL_OBJ_FILES := $(addprefix $(O_DIR)/,$(notdir $(SERIAL_CPP_FILES:.cpp=.o)))

SLB_CPP_FILES := $(S_DIR)/main-slb.cpp $(S_DIR)/complexmath.cpp
SLB_OBJ_FILES := $(addprefix $(O_DIR)/,$(notdir $(SLB_CPP_FILES:.cpp=.o)))

DLB_CPP_FILES := $(S_DIR)/main-dlb.cpp $(S_DIR)/complexmath.cpp
DLB_OBJ_FILES := $(addprefix $(O_DIR)/,$(notdir $(DLB_CPP_FILES:.cpp=.o)))

MPI_CPP_FILES := $(wildcard $(S_MPI)/*.cpp)
MPI_OBJ_FILES := $(addprefix $(O_MPI)/,$(notdir $(MPI_CPP_FILES:.cpp=.o)))

all: serial slb dlb

serial: directories $(SERIAL_OBJ_FILES) $(MPI_OBJ_FILES)
	$(CPP) $(LD_FLAGS) -o $(OUTPUT)/serial-$(PRECISION_LOW) $(SERIAL_OBJ_FILES) $(MPI_OBJ_FILES)

slb: directories $(SLB_OBJ_FILES) $(MPI_OBJ_FILES)
	$(CPP) $(LD_FLAGS) -o $(OUTPUT)/slb-$(PRECISION_LOW) $(SLB_OBJ_FILES) $(MPI_OBJ_FILES)

dlb: directories $(DLB_OBJ_FILES) $(MPI_OBJ_FILES)
	$(CPP) $(LD_FLAGS) -o $(OUTPUT)/dlb-$(PRECISION_LOW) $(DLB_OBJ_FILES) $(MPI_OBJ_FILES)

directories: $(O_DIR) $(O_MPI) $(OUTPUT)

${OUTPUT}:
	${MKDIR_P} ${OUTPUT}

$(O_DIR):
	${MKDIR_P} ${O_DIR}

$(O_MPI):
	${MKDIR_P} ${O_MPI}

$(O_DIR)/%.o: $(S_DIR)/%.cpp
	$(CPP) $(C_FLAGS) $(CPP_FLAGS) -c $< -o $@

$(O_MPI)/%.o: $(S_MPI)/%.cpp
	$(CPP) $(C_FLAGS) $(CPP_FLAGS) -c $< -o $@ 

clean:
	rm -R $(O_BASE)*
