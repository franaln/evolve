# evolve makefile

CXX		 := g++
CXXFLAGS := -g -Wall -std=c++11

#CXX47 := $(shell echo `g++ -dumpversion`'>'4.7 | bc -l)

# ifeq ($(CXX47),  1)
#	CXXFLAGS += -std=c++11
# else
#	CXXFLAGS += -std=c+0x
# endif

ROOTLIBS  := $(shell root-config --libs)
ROOTFLAGS := $(shell root-config --cflags)

OBJDIR := obj
SRCDIR := src

_OBJ   := main.o ga.o individual.o random.o significance.o
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

TARGET := evolve

#------------------------------------------------------------------------------

all: $(TARGET)

$(TARGET): $(OBJDIR) $(OBJ)
	@echo "Linking $@"
	@$(CXX) $(CXXFLAGS) $(ROOTLIBS) -o $@ $(OBJ)
	@echo "$@ done."

$(OBJDIR)/main.o: $(SRCDIR)/main.cpp
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) $(ROOTFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(SRCDIR)/%.h
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) $(ROOTFLAGS) -c -o $@ $<

$(OBJDIR):
	@mkdir -p $(OBJDIR)

clean:
	rm -f $(TARGET) $(OBJDIR)/*.o

.PHONY: clean all
