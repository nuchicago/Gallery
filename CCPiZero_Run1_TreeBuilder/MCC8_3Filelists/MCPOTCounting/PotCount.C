void PotCount(){

  TChain *fluxC = new TChain("addingUp");
  string dirname = "/pnfs/uboone/scratch/users/jaz8600/MCC8_3_SelectionII/BNB_Plus_Cosmics/";

  TSystemDirectory dir(dirname.c_str(), dirname.c_str());
  TList *files = dir.GetListOfFiles();

  std::cout << "Got List of Files" << std::endl;

  if(files){
    TSystemFile *file;
    TString fname;
    TIter next(files);

    std::cout << "Grabbing File: " << std::endl;
    
    int n = 0;
    
    while( (file = (TSystemFile*)next())){          
      fname = file->GetName();
      
      std::cout << "\t : " << fname << std::endl;
      if(file->IsDirectory()){
	TString newdirname = dirname+file->GetName();
	TSystemDirectory newdir(newdirname, newdirname);
	TList *newfiles = newdir.GetListOfFiles();
	TSystemFile *newfile;
	TString newfname;
	TIter next(newfiles);
	
	while( (newfile = (TSystemFile*)next())){
	  //std::cout << "\t \t : " << newfile->GetName() << std::endl;
	  newfname = newfile->GetName();
 
	  if( !newfile->IsDirectory() && newfname.EndsWith("artroot_output.root")){
	    n++;
	    std::cout << " loading : " << newdirname+"/"+newfname << std::endl;
	    
	    TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(newdirname+"/"+newfname);
	    if (!f || !f->IsOpen()) {
	      f = new TFile(newdirname+"/"+newfname);
	    }
	    
	    fluxC->Add(newdirname+"/"+newfname+"/SubRuns");
	    
	    std::cout << " size of tree " << fluxC->GetNtrees() << std::endl;
	    std::cout << " number of entries " << fluxC->GetEntries() << std::endl;
	    std::cout << " closing file... " << std::endl;
	    f->Close();
	  }
	}
      }
    }
  }



  TTree* flux = fluxC;
  
  TFile* out = new TFile("outPOTTree.root","RECREATE");
  flux->Write();
  out->Close();

  /*
  const int flux_entries = flux->GetEntries();  
  std::cout  << "N entries : "<< flux_entries << std::endl;
  TH1D* POT = new TH1D("pot", "", 100, 1e12, 1e20);
  for(int i = 0; i < flux_entries; i++) {
    flux->GetEntry(i);    
    //   POT->Fill(sumdata::POTSummary_generator__GenieGen.obj.totgoodpot);    
  }
  POT->Draw();
  */
}
