# evolve makefile

CXX		 := g++
CXXFLAGS := -g -Wall -std=c++11

#CXX47 := $(shell echo `g++ -dumpversion`'>'4.7 | bc -l)

# ifeq ($(CXX47),  1)
#	CXXFLAGS += -std=c++11
# else
#	CXXFLAGS += -std=c+0x
# endif

# ROOTLIBS	:= $(shell root-config --libs)
# ROOTFLAGS := $(shell root-config --cflags)

LIBS := -lconfig++

OBJDIR := obj
SRCDIR := src

_OBJ   := main.o population.o individual.o random.o variable.o
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

TARGET := evolve

#------------------------------------------------------------------------------

all: $(TARGET)

$(TARGET): $(OBJDIR) $(OBJ)
	@echo "Linking $@"
	@$(CXX) $(CXXFLAGS) -o $@ $(OBJ)
	@echo "$@ done."

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "Compiling $<"
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR):
	@mkdir -p $(OBJDIR)

clean:
	rm -f $(TARGET) $(OBJDIR)/*.o

.PHONY: clean all