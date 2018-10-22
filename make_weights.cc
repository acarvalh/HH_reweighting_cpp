#include <Python.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "THWeightInterface.h" // HadTopKinFit
//#include <LorentzVector.h>
//#include <TTree.h>
//#include <TFile.h>
#include <map>
//#include <TLorentzVector.h> // TLorentzVector

class THWeightInterface;

int main()
{
  bool isDEBUG = true;
  /////////////////////////////
  // read the genHiggses -- those are dumb values for example
  Double_t  genHiggs1_px = 267.332;
  Double_t  genHiggs1_py = -528.941;
  Double_t  genHiggs1_pz = -854.402;
  Double_t  genHiggs1_E = 1047.32;
  Double_t  genHiggs2_px = -8.45553;
  Double_t  genHiggs2_py = 19.6435;
  Double_t  genHiggs2_pz = -28.823;
  Double_t  genHiggs2_E = 130.05;
  TLorentzVector genHiggs1, genHiggs2;
  genHiggs1.SetPxPyPzE(genHiggs1_px, genHiggs1_py, genHiggs1_pz, genHiggs1_E);
  genHiggs2.SetPxPyPzE(genHiggs2_px, genHiggs2_py, genHiggs2_pz, genHiggs2_E);
  /////////////////////////////
  double CX = 1.0;
  int BM = 1.0;
  double Norm = 1.0;
  double kl = 1.0;
  double kt = 1.0;
  double c2 = 0.0;
  double cg = 0.0;
  double c2g = 0.0;
  double energy = 13.; // TeV
  std::vector<double> NormBM;
  THWeightInterface THWeight_calc(CX, BM, Norm, kl, kt, c2, cg, c2g, NormBM, energy);
  double mhh_gen = 0.;
  double costS_gen = 0.;
  mhh_gen = ( genHiggs1 + genHiggs2 ).M();
  costS_gen = comp_cosThetaS( genHiggs1 , genHiggs1 );
  std::vector<double> WeightBM;
  double THWeight = 1.0;
  if (mhh_gen > 247.) THWeight = THWeight_calc(mhh_gen, costS_gen, kl, kt, c2, cg, c2g, Norm, WeightBM );
  if ( isDEBUG ) {
    std::cout << "entry: (gen_mhh = " << mhh_gen << ", gen_cosTheta* = " << costS_gen << ")" << std::endl;
    std::cout<< "genHiggses weights to input entry on the requested point: " << THWeight << " ( closest shape benchmark : " << BM << " )" << std::endl;
    std::cout << "Weights for " << WeightBM.size() << " theory points = SM + 12 shape benchmarks: \n ";
    for (unsigned int bm_list = 0; bm_list < WeightBM.size(); bm_list++) std::cout <<  WeightBM[bm_list] << " ";
    std::cout << '\n';
  }
  return EXIT_SUCCESS;
}
