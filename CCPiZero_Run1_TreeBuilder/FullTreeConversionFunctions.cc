#include "FullTreeConversionFunctions.h"

double FullTreeConversionFunctions::Median(std::vector<double> input){
  
  int N = input.size();
  
  double median;
  
    std::sort(input.begin(), input.end());   
    if (N % 2 == 0){ median = (input[((N/2) - 1)] + input[N/2]) / 2;}
    else if( N == 1){median = input[N];}
    else{            median = input[N/2];}
    return median;    
}

double FullTreeConversionFunctions::TrunMean(std::vector <double> vec){
  
    double RMS = TMath::RMS(vec.begin(),vec.end());
    double median = FullTreeConversionFunctions::Median(vec);
    
    std::vector<double> TLMean;
    
    for(int i = 0; i < int(vec.size()); i++){
      if(vec[i] < median+RMS && vec[i] > median-RMS){TLMean.push_back(vec[i]);}
    }
    
    return TMath::Mean(TLMean.begin(), TLMean.end());
}


bool FullTreeConversionFunctions::isFromNeutrinoVertex(const simb::MCTruth & mc, const sim::MCShower & show){
    double x_truth = mc.GetNeutrino().Nu().Trajectory().Position(0).X();
    double y_truth = mc.GetNeutrino().Nu().Trajectory().Position(0).Y();
    double z_truth = mc.GetNeutrino().Nu().Trajectory().Position(0).Z();

    double x_obj   = show.Start().X();
    double y_obj   = show.Start().Y();
    double z_obj   = show.Start().Z();

    double x_diff = x_truth - x_obj;
    double y_diff = y_truth - y_obj;
    double z_diff = z_truth - z_obj;

    double dist = sqrt(x_diff*x_diff+
                       y_diff*y_diff+
                       z_diff*z_diff);

    if(dist < 0.01){return true;}

    return false;
}

bool FullTreeConversionFunctions::isAV(double x, double y, double z){
	const double x_boundary1 = 0;
	const double x_boundary2 = 256.35;
	const double y_boundary1 = -116.5;
	const double y_boundary2 = 116.5;
	const double z_boundary1 = 0;
	const double z_boundary2 = 1036.8;

	if(x > x_boundary1 &&
	   x < x_boundary2 &&
	   y > y_boundary1 &&
	   y < y_boundary2 &&
	   z > z_boundary1 &&
	   z < z_boundary2)
	{
		return true;
	}
	return false;

}
  
bool FullTreeConversionFunctions::AssTrack(recob::Track trk, double vtx_x, double vtx_y, double vtx_z){
    
    double trk_v_x = trk.Vertex().X();
    double trk_v_y = trk.Vertex().Y();
    double trk_v_z = trk.Vertex().Z();

    double trk_e_x = trk.End().X();
    double trk_e_y = trk.End().Y();
    double trk_e_z = trk.End().Z();

    double end_dist = sqrt(pow((trk_e_x-vtx_x),2) + 
			   pow((trk_e_y-vtx_y),2) + 
			   pow((trk_e_z-vtx_z),2));

    double strt_dist = sqrt(pow((trk_v_x-vtx_x),2) + 
			    pow((trk_v_y-vtx_y),2) + 
			    pow((trk_v_z-vtx_z),2));

    std::cout << "Start : " << strt_dist << ", End: " << end_dist << std::endl; 

    if(end_dist < 3){       return true;  }
    else if(strt_dist < 3){ return true;  }
    else{                   return false; }

}

bool FullTreeConversionFunctions::nearAV(double x, double y, double z){
    const double x_boundary1 = 0;
    const double x_boundary2 = 256.35;
    const double y_boundary1 = -116.5;
    const double y_boundary2 = 116.5;
    const double z_boundary1 = 0;
    const double z_boundary2 = 1036.8;
    
    if(x > x_boundary1 &&
       x < x_boundary2 &&
       y > y_boundary1 &&
       y < y_boundary2 &&
       z > z_boundary1 &&
       z < z_boundary2 && 
       (x < x_boundary1+10 ||
	x > x_boundary2-10 ||
	y < y_boundary1+10 ||
	y > y_boundary2-10 ||
	z < z_boundary1+10 ||
	z > z_boundary2-10)
       )
      {
	return true;
      }

    return false;
    
}
  
double FullTreeConversionFunctions::MaxDeflection(recob::Track trk){
    
    int first = trk.Trajectory().FirstValidPoint();
    int last  = trk.Trajectory().LastValidPoint();
    
    double max = 0;

    std::cout << "FIRST : " << first <<" LAST : " << last << std::endl;

    std::vector<TVector3> mom;

    for(int i = first; i < last; i++){
      
      if(trk.Trajectory().HasValidPoint(i)){
	TVector3 nextpos;
	TVector3 nextmom;
	trk.Trajectory().TrajectoryAtPoint(i,nextpos,nextmom);      
	mom.push_back(nextmom);       
      }
    }

    std::cout << "size : " << mom.size() << std::endl;

    for(int i = 0; i < int(mom.size())-1; i++){
      if(max <mom.at(i).Angle(mom.at(i+1))) max = mom.at(i).Angle(mom.at(i+1));
    }
    //    std::cout << "MAX : " <<  max*(180./3.14159265)  << std::endl;
    return max*(180./3.14159265);
    
}

  //Taken from David C. 
std::vector<double> FullTreeConversionFunctions::Local3DLinearityXY(recob::Track trk){
    
    int first = trk.Trajectory().FirstValidPoint();
    int last  = trk.Trajectory().LastValidPoint();
    
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> z;

    for(int i = first; i < last; i++){      
      if(trk.Trajectory().HasValidPoint(i)){
	TVector3 nextpos;
	TVector3 nextmom;
	trk.Trajectory().TrajectoryAtPoint(i,nextpos,nextmom);      
	x.push_back(nextpos.X());       
	y.push_back(nextpos.Y());       
	z.push_back(nextpos.Z());       
      }
    }
    
    std::vector<double> linearity;
    
    if(int(x.size()) < 6){ 
      linearity.push_back(0.);
      return linearity;
    }
        
    for(int i = 3; i < int(x.size()-3); i++){

      std::vector<double> temp_x = {x[i-2],x[i-1],x[i],x[i+1],x[i+2]};
      std::vector<double> temp_y = {y[i-2],y[i-1],y[i],y[i+1],y[i+2]};
      std::vector<double> temp_z = {z[i-2],z[i-1],z[i],z[i+1],z[i+2]};

      double avg_x = TMath::Mean(temp_x.begin(),temp_x.end());
      double avg_y = TMath::Mean(temp_y.begin(),temp_y.end());
            
      double cov_xy = 0;
      double std_x = 0;
      double std_y = 0;
      for(int j = 0; j < int(temp_x.size()); j++){
	cov_xy += (temp_x[j] - avg_x)*(temp_y[j] - avg_y);
 	std_x += pow(temp_x[j] - avg_x,2);
	std_y += pow(temp_y[j] - avg_y,2);
      }

      cov_xy /= double(temp_x.size());
      std_x /= double(temp_x.size());
      std_y /= double(temp_y.size());
 
      linearity.push_back(fabs(cov_xy/(sqrt(std_x)*sqrt(std_y))));

    }
    
    return linearity;
    
}
  

  //Taken from David C. 
std::vector<double> FullTreeConversionFunctions::Local3DLinearityXZ(recob::Track trk){
    
    int first = trk.Trajectory().FirstValidPoint();
    int last  = trk.Trajectory().LastValidPoint();
    
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> z;

    for(int i = first; i < last; i++){      
      if(trk.Trajectory().HasValidPoint(i)){
	TVector3 nextpos;
	TVector3 nextmom;
	trk.Trajectory().TrajectoryAtPoint(i,nextpos,nextmom);      
	x.push_back(nextpos.X());       
	y.push_back(nextpos.Y());       
	z.push_back(nextpos.Z());       
      }
    }
    
    std::vector<double> linearity;
    
    if(int(x.size()) < 6){ 
      linearity.push_back(0.);
      return linearity;
    }
        
    for(int i = 3; i < int(x.size()-3); i++){

      std::vector<double> temp_x = {x[i-2],x[i-1],x[i],x[i+1],x[i+2]};
      std::vector<double> temp_y = {y[i-2],y[i-1],y[i],y[i+1],y[i+2]};
      std::vector<double> temp_z = {z[i-2],z[i-1],z[i],z[i+1],z[i+2]};

      double avg_x = TMath::Mean(temp_x.begin(),temp_x.end());
      double avg_z = TMath::Mean(temp_z.begin(),temp_z.end());
            
      double cov_xz = 0;
      double std_x = 0;
      double std_z = 0;

      for(int j = 0; j < int(temp_x.size()); j++){
	cov_xz += (temp_x[j] - avg_x)/*(temp_y[j] - avg_y)*/*(temp_z[j] - avg_z);
 	std_x += pow(temp_x[j] - avg_x,2);
	std_z += pow(temp_z[j] - avg_z,2);
      }

      cov_xz /= double(temp_x.size());
      std_x /= double(temp_x.size());
      std_z /= double(temp_z.size());
 
      linearity.push_back(fabs(cov_xz/(sqrt(std_x)*sqrt(std_z))));

    }
    
    return linearity;
    
}

  //Taken from David C. 
std::vector<double> FullTreeConversionFunctions::Local3DLinearityYZ(recob::Track trk){
    
    int first = trk.Trajectory().FirstValidPoint();
    int last  = trk.Trajectory().LastValidPoint();
    
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> z;

    for(int i = first; i < last; i++){      
      if(trk.Trajectory().HasValidPoint(i)){
	TVector3 nextpos;
	TVector3 nextmom;
	trk.Trajectory().TrajectoryAtPoint(i,nextpos,nextmom);      
	x.push_back(nextpos.X());       
	y.push_back(nextpos.Y());       
	z.push_back(nextpos.Z());       
      }
    }
    
    std::vector<double> linearity;
    
    if(int(x.size()) < 6){ 
      linearity.push_back(0.);
      return linearity;
    }
        
    for(int i = 3; i < int(x.size()-3); i++){

      std::vector<double> temp_x = {x[i-2],x[i-1],x[i],x[i+1],x[i+2]};
      std::vector<double> temp_y = {y[i-2],y[i-1],y[i],y[i+1],y[i+2]};
      std::vector<double> temp_z = {z[i-2],z[i-1],z[i],z[i+1],z[i+2]};

      double avg_y = TMath::Mean(temp_y.begin(),temp_y.end());
      double avg_z = TMath::Mean(temp_z.begin(),temp_z.end());
            
      double cov_yz = 0;
      double std_y = 0;
      double std_z = 0;

      for(int j = 0; j < int(temp_x.size()); j++){
	cov_yz += /*(temp_x[j] - avg_x)*/(temp_y[j] - avg_y)*(temp_z[j] - avg_z);
	std_y += pow(temp_y[j] - avg_y,2);
	std_z += pow(temp_z[j] - avg_z,2);
      }

      cov_yz /= double(temp_x.size());
      std_y /= double(temp_y.size());
      std_z /= double(temp_z.size());
 
      linearity.push_back(fabs(cov_yz/(sqrt(std_y)*sqrt(std_z))));

    }
    
    return linearity;
    
}
