#Completely stolen.
CPPFLAGS=-I $(BOOST_INC) \
         -I $(CANVAS_INC) \
         -I $(CETLIB_INC) \
         -I $(FHICLCPP_INC) \
         -I $(GALLERY_INC) \
         -I $(LARCOREOBJ_INC) \
         -I $(LARDATAOBJ_INC) \
         -I $(NUSIMDATA_INC) \
         -I $(ROOT_INC)

CXXFLAGS=-std=c++14 -Wall
CXX=g++
LDFLAGS=$$(root-config --libs) \
        -L $(CANVAS_LIB) -l canvas_Utilities -l canvas_Persistency_Common -l canvas_Persistency_Provenance \
        -L $(CETLIB_LIB) -l cetlib \
        -L $(GALLERY_LIB) -l gallery \
        -L $(NUSIMDATA_LIB) -l nusimdata_SimulationBase \
        -L $(LARCOREOBJ_LIB) -l larcoreobj_SummaryData \
        -L $(LARDATAOBJ_LIB) -l lardataobj_RecoBase

UNAME := $(shell uname -s)

ifeq ($(UNAME), Darwin)
  EXEC=example_program
else
  EXEC=example_program
endif

$(EXEC): test.cc 
	@echo Building $(EXEC)
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ $<

