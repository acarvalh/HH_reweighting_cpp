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
//--- throw an exception in case ROOT encounters an error
  //gErrorAbortLevel = kError;
  bool isDEBUG = true;

  double CX = 1.0;
  int BM = 1.0;
  double Norm = 1.0;
  double kl = 1.0;
  double kt = 1.0;
  double c2 = 0.0;
  double cg = 0.0;
  double c2g = 0.0;
  std::vector<double> NormBM;
  THWeightInterface THWeight_calc(CX, BM, Norm, kl, kt, c2, cg, c2g, NormBM);
  if ( isDEBUG ) {
    std::cout << "(" << NormBM.size() << ") ";
    for (unsigned int bm_list = 0; bm_list < NormBM.size(); bm_list++) std::cout <<  NormBM[bm_list] << " ";
    std::cout << '\n';
  }
    double mhh_gen = 0.;
    double costS_gen = 0.;
    if ( 1 > 0 ) { //genHiggses.size() == 2 ) {
      mhh_gen = 300.; //( genHiggses[0].p4() + genHiggses[1].p4() ).mass();
      costS_gen = 0.5; //comp_cosThetaS( genHiggses[0].p4() , genHiggses[1].p4() );
    }
    std::vector<double> WeightBM;
    double THWeight = 1.0;
    if (mhh_gen > 247.) THWeight = THWeight_calc(mhh_gen, costS_gen, kl, kt, c2, cg, c2g, Norm, WeightBM );
    if ( isDEBUG ) {
      std::cout<< " genHiggses weights " << " " << THWeight << " " << mhh_gen << " " << costS_gen << std::endl;
      std::cout << "(" << WeightBM.size() << ") ";
      for (unsigned int bm_list = 0; bm_list < WeightBM.size(); bm_list++) std::cout <<  WeightBM[bm_list] << " ";
      std::cout << '\n';
    }


  return EXIT_SUCCESS;
}
