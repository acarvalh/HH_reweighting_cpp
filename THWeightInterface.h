//#ifndef THWeightInterface_h
//#define THWeightInterface_h
#include <Python.h>
//#include <Python.h> // PyObject
#include <fstream>
#include <iostream> // std::cerr, std::fixed
#include <vector> // std::vector<>
#include <string> // std::string
#include <map> // std::map<,>
#include <TLorentzVector.h> // TLorentzVector

double comp_cosThetaS(const TLorentzVector& hadTauP4_lead, const TLorentzVector& hadTauP4_sublead);

class THWeightInterface
{
public:
  THWeightInterface(
              double & CX, int & BM, double & Norm,
              const double & kl,
              const double & kt,
              const double & c2,
              const double & cg,
              const double & c2g,
              std::vector<double> & NormBM
             );
  ~THWeightInterface();

  double
  operator()(
    const double & mhh_gen,
    const double & costSgen_gen,
    const double & kl,
    const double & kt,
    const double & c2,
    const double & cg,
    const double & c2g,
    const double & normalization,
    std::vector<double> & WeightBM,
    const double & energy
  ) const;

private:
  PyObject* modeldata_;
  PyObject* moduleMainString_;
  PyObject* moduleMain_;
  PyObject* cms_base;

  double klJHEP[13]  = {1.0,  7.5,  1.0,  1.0,  -3.5, 1.0, 2.4, 5.0, 15.0, 1.0, 10.0, 2.4, 15.0};
  double ktJHEP[13]  = {1.0,  1.0,  1.0,  1.0,  1.5,  1.0, 1.0, 1.0, 1.0,  1.0, 1.5,  1.0, 1.0};
  double c2JHEP[13]  = {0.0,  -1.0, 0.5, -1.5, -3.0,  0.0, 0.0, 0.0, 0.0,  1.0, -1.0, 0.0, 1.0};
  double cgJHEP[13]  = {0.0,  0.0, -0.8,  0.0, 0.0,   0.8, 0.2, 0.2, -1.0, -0.6, 0.0, 1.0, 0.0};
  double c2gJHEP[13] = {0.0, 0.0, 0.6, -0.8, 0.0, -1.0, -0.2,-0.2,  1.0,  0.6, 0.0, -1.0, 0.0};

};

//#endif // THWeightInterface_h
