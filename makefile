# evolve makefile

CXX		 := g++
CXXFLAGS := -g -Wall -std=c++11

ROOTLIBS  := $(shell root-config --libs) -lRooStats
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
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $@ $(ROOTLIBS)
	@echo "$@ done."

$(OBJDIR)/main.o: $(SRCDIR)/main.cpp
	@echo "Compiling $<"
	@$(CXX) $(CXXFLAGS) $(ROOTFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(SRCDIR)/%.h
	@echo "Compiling $<"
	@$(CXX) $(CXXFLAGS) $(ROOTFLAGS) -c -o $@ $<

$(OBJDIR):
	@mkdir -p $(OBJDIR)

clean:
	rm -rf $(TARGET) $(OBJDIR)

.PHONY: clean all
