#include "FullTreeConversion.h"
int main(int argv, char** argc) {

  TTree * fTree;  
  double mcx;
  double mcy;
  double mcz;
  double vtx;
  double vty;
  double vtz;
  double dist;
  double Enu;
  double pass;
  double weight;
  double POTperEvent;
  bool inAV;
  bool TrackExit;
  int CCNC;
  int Event;
  int Run;
  int Subrun;
  double TrackL;
  double TrackEndX;
  double TrackEndY;
  double TrackEndZ;
  double TrackAvgdQdx;
  double TrackMeddQdx;
  double TrackSTDdQdx;
  double TrackTLMeandQdx;
  int TrackMult;
  int NTrack;
  int TrackNDaughters;
  int TrackNnulldQdx;
  
  double TrackMaxDeflection;
    
  double TrueLeptonE;
  double TrueQ2;

  fTree = new TTree("Vertices","ObjectKinematics");
  fTree->Branch("mcx",&mcx);
  fTree->Branch("mcy",&mcy);
  fTree->Branch("mcz",&mcz);
  fTree->Branch("vtx",&vtx);
  fTree->Branch("vty",&vty);
  fTree->Branch("vtz",&vtz);
  fTree->Branch("dist",  &dist);
  fTree->Branch("Enu",  &Enu);
  fTree->Branch("pass",  &pass);
  fTree->Branch("inAV",  &inAV);
  fTree->Branch("CCNC",  &CCNC);
  fTree->Branch("weight",  &weight);
  fTree->Branch("POTperEvent",  &POTperEvent);
  fTree->Branch("Event",  &Event);
  fTree->Branch("Run",  &Run);
  fTree->Branch("Subrun",  &Subrun);
  fTree->Branch("TrackL",  &TrackL);
  fTree->Branch("TrackEndX",  &TrackEndX);
  fTree->Branch("TrackEndY",  &TrackEndY);
  fTree->Branch("TrackEndZ",  &TrackEndZ);
  fTree->Branch("TrackExit", &TrackExit);
  fTree->Branch("NTrack",  &NTrack);
  fTree->Branch("TrackMult",  &TrackMult);
  fTree->Branch("TrackNDaughters",  &TrackNDaughters);
  fTree->Branch("TrackAvgdQdx", &TrackAvgdQdx);
  fTree->Branch("TrackNnulldQdx", &TrackNnulldQdx);
  fTree->Branch("TrackMeddQdx", &TrackMeddQdx);
  fTree->Branch("TrackSTDdQdx", &TrackSTDdQdx);
  fTree->Branch("TrackTLMeandQdx", &TrackTLMeandQdx);
  fTree->Branch("TrackMaxDeflection", &TrackMaxDeflection);
  fTree->Branch("TrueLeptonE",&TrueLeptonE);
  fTree->Branch("TrueQ2",&TrueQ2);
  
  FullTreeConversionFunctions funcs;
  
  std::vector<std::string> filename;
  for (int i = 1; i < argv; ++i) { 
    std::cout << "FILE : " << argc[i] << std::endl; 
    filename.push_back(std::string(argc[i]));
  }
      
  for (gallery::Event ev(filename) ; !ev.atEnd(); ev.next()) {
    
    mcx = 0;
    mcy = 0;
    mcz = 0;
    vtx = 0;
    vty = 0;
    vtz = 0;
    dist = 0;
    Enu = 0;
    pass = 0;
    weight = 0;
    POTperEvent = 0;
    inAV = 0;
    CCNC = 0;
    TrackAvgdQdx = 0;
    TrackMeddQdx = 0; 
    TrackMult = 0;
    TrackTLMeandQdx = 0;
    TrackMaxDeflection = 0;
    TrueLeptonE = 0;
    TrueQ2 = 0;
    
    art::InputTag tracks_tag("pandoraNu");
    auto const & track_list_ptr = ev.getValidHandle<std::vector <recob::Track> >(tracks_tag);
    auto const & track_list = (*track_list_ptr);

    art::FindMany<recob::Vertex> vertex_for_track(track_list_ptr, ev, "NuMuCCSelectionII");
    auto const &assoc_handle =
      ev.getValidHandle<art::Assns<recob::Vertex, recob::Track>>("NuMuCCSelectionII");

    art::FindMany<recob::PFParticle> pfp_for_track(track_list_ptr, ev, "pandoraNu");

    std::vector<recob::Vertex> Sel2_vtx_list;
    std::vector<recob::Track> Sel2_trk_list;
    std::vector<recob::PFParticle> Sel2_pfp_list;

    for (auto &ass : *assoc_handle) {
      art::Ptr<recob::Vertex> Sel2_vtx = ass.first;
      Sel2_vtx_list.emplace_back(*Sel2_vtx);

      art::Ptr<recob::Track> Sel2_trk = ass.second;
      Sel2_trk_list.emplace_back(*Sel2_trk);

    }
    
    art::InputTag eventweight_tag("eventweight");
    auto const & eventweights_list = *(ev.getValidHandle<std::vector <evwgh::MCEventWeight> >(eventweight_tag));

    art::InputTag mctruth_tag("generator");
    auto const & mctruth_list = *(ev.getValidHandle<std::vector <simb::MCTruth> >(mctruth_tag));

    art::InputTag trig_tag("TriggerResults::TPCNeutrinoIDFilter2");
    auto const & trig = *(ev.getValidHandle< art::TriggerResults >(trig_tag));

    double BNBWeight = 1;

    for(int i = 0; i < int(mctruth_list.size()); i++){
      auto const & mctruth = mctruth_list.at(i);
      
      CCNC = mctruth.GetNeutrino().CCNC();
      TrueQ2 = mctruth.GetNeutrino().QSqr(); 
      
      auto const & eventweights = eventweights_list.at(i);
      
      for(auto last : eventweights.fWeight){
	if(last.first.find("bnbcorrection") != std::string::npos){
	  BNBWeight *= last.second.at(0);
	}
      }
      
      
      if(abs(mctruth.GetNeutrino().Nu().PdgCode()) == 12 || 
	 abs(mctruth.GetNeutrino().Nu().PdgCode()) == 14  ){
	
	Enu = mctruth.GetNeutrino().Nu().Trajectory().Momentum(0).E(); 
	TrueLeptonE = mctruth.GetNeutrino().Lepton().Trajectory().Momentum(0).E();
	mcx = mctruth.GetNeutrino().Nu().EndX();
	mcy = mctruth.GetNeutrino().Nu().EndY();
	mcz = mctruth.GetNeutrino().Nu().EndZ();	
	inAV = funcs.isAV(mcx,mcy,mcz);
	
      }
    }
    
      double vtx_XYZ[3] = {0.0, 0.0, 0.0};
      
      //for(auto & trk : track_list){
      for(int trk = 0; trk < int(track_list.size()); trk++){    
	std::vector<recob::Vertex const*> vertex;
	vertex_for_track.get(trk, vertex);
	
	if(vertex.size() == 0){continue;}
	std::cout << "Size : " << vertex.size() << std::endl;
	
	auto const & vtx = vertex.at(0);
	
	vtx->XYZ(vtx_XYZ);
	//std::cout << "X : " << vtx_XYZ[0] << " Y : " << vtx_XYZ[1] << " Z : " << vtx_XYZ[2] << std::endl; 
	
	std::vector<recob::PFParticle const*> pfp;
	pfp_for_track.get(trk, pfp);
	std::cout << "size of PFP list : " << pfp.size() << std::endl;
	if(pfp.size() != 0){
	  std::cout << "N Daughters : " <<  pfp.at(0)->NumDaughters() << std::endl;
	  TrackNDaughters = pfp.at(0)->NumDaughters();
	}
      }
      
      int N_mult = 0;
      
      for(auto const & trk : track_list){    
	if(funcs.AssTrack(trk, vtx_XYZ[0], vtx_XYZ[1], vtx_XYZ[2])){ N_mult++;}
      }
      
      TrackMult = N_mult;
      
      std::cout << "Vertex Multiplicity : " << N_mult << std::endl;
      
      NTrack = Sel2_trk_list.size();
      
      double max_L = 0;

    for(auto const & trk : Sel2_trk_list){
           
      TrackMaxDeflection = funcs.MaxDeflection(trk);

      if(trk.Length() > max_L){
	max_L = trk.Length();
	TrackEndX = trk.End().X();
	TrackEndY = trk.End().Y();
	TrackEndZ = trk.End().Z();
	TrackExit = funcs.nearAV(TrackEndX, TrackEndY, TrackEndZ);
	
	double avg = 0;
	int N = 0;
	int Nnull = 0;
	std::vector<double> dqdx; 
	//	std::cout << "\t N dqdx : " << int(trk.NumberdQdx(geo::kW)) << std::endl;
	
	for( int i = 0; i < int(trk.NumberdQdx(geo::kW)); i++){	  
	  
	  if(trk.DQdxAtPoint(i,geo::kW) < 0) continue; 
	  if(trk.DQdxAtPoint(i,geo::kW) == 0){ Nnull++; continue;} 
	  N++;
	  //  std::cout << "\t \t " << N << " = " << trk.DQdxAtPoint(i,geo::kW) << std::endl;	    
	  avg += trk.DQdxAtPoint(i,geo::kW);
	  dqdx.push_back(trk.DQdxAtPoint(i,geo::kW));
	  
	}
	
	if(N == 0){dqdx.clear(); continue;};
	
	avg /= N;
	std::sort(dqdx.begin(),dqdx.end());
	
	if (N % 2 == 0)
	  {
	    TrackMeddQdx = (dqdx[((N/2) - 1)] + dqdx[N/2]) / 2;
	  }
	else if( N == 1)
	  {
	    TrackMeddQdx = dqdx[N];
	  }	  
	else{
	  TrackMeddQdx = dqdx[N/2];
	}
	
	double sq_sum = std::inner_product(dqdx.begin(), dqdx.end(), dqdx.begin(), 0.0);
	double stdev = std::sqrt(sq_sum / double(N) - avg * avg);	  
	
	TrackSTDdQdx = stdev;

	TrackTLMeandQdx = funcs.TrunMean(dqdx);
	/*	
	std::vector<double> TrackLin = Local3DLinearity(trk);
	
	double* TrackLinArray = &TrackLin[0];

	std::vector<double>::iterator it;
	it = TMath::LocMin(TrackLin.begin(), TrackLin.end());
	std::cout << *it << std::endl;



	TrackMinLin =   *it;
	TrackRMSLin =   TMath::RMS(TrackLin.begin(), TrackLin.end());
	TrackMeanLin=   TMath::Mean(TrackLin.begin(), TrackLin.end());
	TrackMedianLin= TMath::Median(TrackLin.size(),TrackLinArray);
	*/
	dqdx.clear();

	TrackNnulldQdx = Nnull;
	TrackAvgdQdx = avg;
	  	  
      }
      
    }
     
       
      TrackL = max_L;

      vtx = vtx_XYZ[0];
      vty = vtx_XYZ[1];
      vtz = vtx_XYZ[2];

      dist = sqrt( pow(vtx-mcx, 2) + pow(vty-mcy, 2) + pow(vtz-mcz, 2));
      pass = trig.accept();
      
      weight = BNBWeight;
      POTperEvent = 0;
      Event = ev.eventAuxiliary().event();
      Run = ev.eventAuxiliary().run();
      Subrun = ev.eventAuxiliary().subRun();
       
      fTree->Fill();
    
    }

  TFile *out = new TFile("Output_Sel2Tree.root","RECREATE");
  out->cd();
  fTree->Write();
  out->Close();

  return 1;
}

