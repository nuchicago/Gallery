// Standard things to include
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>

// These are the includes to use "Root" things 
#include "TInterpreter.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TH2D.h"
#include "TFile.h"
// These are the larsoft includes that let you
// have access to data-products and the event 
// details
#include "canvas/Utilities/InputTag.h"
#include "gallery/Event.h"
#include "gallery/ValidHandle.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "nusimdata/SimulationBase/MCNeutrino.h"

// Same as above but this lets you have access to 
// associations 
#include "canvas/Persistency/Common/FindMany.h"

//This way you can be lazy
using namespace art;
using namespace std;


int main(int argv, char** argc) {
  /* Here you name the thing that "produced" the data product that you want to look at.
     In our event dump we see two things:
     GenieGen.. | corsika............. | .... | std::vector<simb::MCTruth>.... | ....1
     GenieGen.. | generator........... | .... | std::vector<simb::MCTruth>.... | ....1

     This means that if you want to look at the simb::MCTruth data product there are two "producers"
     We want to look at neutrinos so we choose "generator" if you wanted to look at cosmics you
     could pick "coriska"
  */
  InputTag mctruths_tag { "generator" };
 
  //We have passed the input file as an argument to the function 
  vector<string> filename;
  for (int i = 1; i < argv; ++i) { 
    std::cout << "FILE : " << argc[i] << std::endl; 
    filename.push_back(string(argc[i]));
  }

  //Define the histograms you want here
  TH1F* n_nu_hist = new TH1F("Nnu", "Title; x-axis (N-particles);Events", 51, -0.5, 50.5);
  TH1F* nu_pdg_hist = new TH1F("pdg", "", 31, -15.5, 15.5);
  TH2D* nu_vtx_YZ_hist = new TH2D("nu_vtx_YZ", "", 100, -1000, 1000, 100, -1000, 1000);
  TH2D* nu_vtx_XZ_hist = new TH2D("nu_vtx_XZ", "", 100, -1000, 1000, 100, -1000, 1000);

  for (gallery::Event ev(filename) ; !ev.atEnd(); ev.next()) {
  
    //Next we want to grab from the event the data-product that you want
    auto const& mctruths = *ev.getValidHandle<vector<simb::MCTruth>>(mctruths_tag);

    //dump how many MC truth entries there are
    // (This is the number of neutrino interactions there are in the event)
    // Knowing what volume you are generating your events in is important! 
    if (!mctruths.empty()) n_nu_hist->Fill(mctruths.size());
    
    //Now we'll iterate through these 

    for (size_t i = 0; i < mctruths.size(); i++) {
      auto const& mctruth = mctruths.at(i);

      //Now for each simb::MCTruth we look at two things      
      nu_pdg_hist->Fill(mctruth.GetNeutrino().Nu().PdgCode());
      nu_vtx_YZ_hist->Fill(mctruth.GetNeutrino().Nu().Vy(),mctruth.GetNeutrino().Nu().Vz());
      nu_vtx_XZ_hist->Fill(mctruth.GetNeutrino().Nu().Vx(),mctruth.GetNeutrino().Nu().Vz());

    }//Iterate through neutrino interactions
  }// Iterate through events

  //Drawing the histograms don't work like they do with macros

  //We can also write the output to a output root file like this:
  TFile* output = new TFile("output.root","RECREATE");
  output->cd();
  nu_vtx_XZ_hist->Write();
  nu_vtx_YZ_hist->Write();
  nu_pdg_hist->Write(); 
  n_nu_hist->Write(); 
  output->Close();
  
  return 1;
}
