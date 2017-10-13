#ifndef FULLTREECONVERSIONFUNCTIONS_H
#define FULLTREECONVERSIONFUNCTIONS_H

#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <numeric>
#include <functional>

#include "TROOT.h"
#include "TMath.h"

// larsoft object includes
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/Vertex.h"
#include "lardataobj/RecoBase/PFParticle.h"
#include "nusimdata/SimulationBase/MCFlux.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "lardataobj/MCBase/MCShower.h"
#include "lardataobj/MCBase/MCTrack.h"
#include "lardataobj/MCBase/MCHit.h"
#include "lardataobj/MCBase/MCHitCollection.h"

class FullTreeConversionFunctions{

 public:
    
  double Median(std::vector<double> input);
  
  double TrunMean(std::vector <double> vec);
  
  bool isFromNeutrinoVertex(const simb::MCTruth & mc, const sim::MCShower & show);

  bool isAV(double x, double y, double z);
  
  bool AssTrack(recob::Track trk, double vtx_x, double vtx_y, double vtx_z);

  bool nearAV(double x, double y, double z);
  
  double MaxDeflection(recob::Track trk);

  std::vector<double> Local3DLinearityXY(recob::Track trk);
  
  std::vector<double> Local3DLinearityXZ(recob::Track trk);

  std::vector<double> Local3DLinearityYZ(recob::Track trk);

};

#endif
