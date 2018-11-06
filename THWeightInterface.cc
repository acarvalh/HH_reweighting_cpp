#include <Python.h>
#include "THWeightInterface.h"
#include <TLorentzVector.h> // TLorentzVector
#include <fstream>
#include <iostream> // std::cerr, std::fixed
#include <boost/filesystem.hpp> // boost::filesystem::

double comp_cosThetaS(const TLorentzVector& hadTauP4_lead, const TLorentzVector& hadTauP4_sublead)
{
  TLorentzVector hadTauBoost = hadTauP4_lead;
  hadTauBoost.Boost(-(hadTauP4_lead + hadTauP4_sublead).BoostVector());
  return std::fabs(hadTauBoost.CosTheta());
}

THWeightInterface::THWeightInterface(
  double & CX, int & BM, double & Norm,
  const double & kl,
  const double & kt,
  const double & c2,
  const double & cg,
  const double & c2g,
  std::vector<double> & NormBM,
  const double & energy
)
  //: mvaFileName_(LocalFileInPath(mvaFileName).fullPath())
  //, mvaInputVariables_(mvaInputVariables)
{
  // AC: limit number of threads running in python to one
  setenv("OMP_NUM_THREADS", "1", 0);
  std::cout << "Entered CX interface " << '\n';

  // read the python file that we're about to execute
  const std::string applicationLoadPath = (
    boost::filesystem::path(std::getenv("CMSSW_BASE")) /
    boost::filesystem::path("src/HH_reweighting_cpp/do_weight.py")
  ).string();
  std::ifstream applicationLoadFile(applicationLoadPath);
  std::string applicationLoadStr;
  std::cout << "loaded application " << '\n';

  applicationLoadFile.seekg(0, std::ios::end);
  applicationLoadStr.reserve(applicationLoadFile.tellg());
  applicationLoadFile.seekg(0, std::ios::beg);
  applicationLoadStr.assign(std::istreambuf_iterator<char>(applicationLoadFile), std::istreambuf_iterator<char>());
  std::cout << "set program" << '\n';

  Py_Initialize();
  PyObject * kl_py = PyFloat_FromDouble(static_cast<double>(kl));
  PyObject * kt_py = PyFloat_FromDouble(static_cast<double>(kt));
  PyObject * c2_py = PyFloat_FromDouble(static_cast<double>(c2));
  PyObject * cg_py = PyFloat_FromDouble(static_cast<double>(cg));
  PyObject * c2g_py = PyFloat_FromDouble(static_cast<double>(c2g));
  std::cout << "converted doubles "<< kl << " " << kt << " " << c2 << '\n';

  // https://ubuntuforums.org/archive/index.php/t-324544.html
  // https://stackoverflow.com/questions/4060221/how-to-reliably-open-a-file-in-the-same-directory-as-a-python-script
  // https://gist.github.com/rjzak/5681680
  Py_SetProgramName(const_cast<char *>("do_weight"));
  moduleMainString_ = PyString_FromString("__main__");
  moduleMain_ = PyImport_Import(moduleMainString_);
  PyRun_SimpleString(applicationLoadStr.c_str());
  std::cout << "set program" << '\n';

  // Load the class
  cms_base = PyString_FromString(std::getenv("CMSSW_BASE"));
  PyObject* func_load = PyObject_GetAttrString(moduleMain_, "load");
  PyObject* args_load = PyTuple_Pack(2, cms_base, PyFloat_FromDouble(static_cast<double>(energy)));
  modeldata_ = PyObject_CallObject(func_load, args_load);
  std::cout << "loaded modeldata " << '\n';

  // calculate and return CX
  PyObject* func_CX = PyObject_GetAttrString(moduleMain_, "getCX");
  PyObject* args_CX = PyTuple_Pack(6, kl_py, kt_py, c2_py, cg_py, c2g_py, modeldata_);
  CX = PyFloat_AsDouble(PyObject_CallObject(func_CX, args_CX));
  std::cout << "CX " << '\n';

  // calculate and return normalization
  PyObject* args_BM = PyTuple_Pack(8,
    kl_py, kt_py, c2_py, cg_py, c2g_py,
    PyFloat_FromDouble(static_cast<double>(energy)),
    modeldata_, cms_base
  );
  PyObject* func_Norm = PyObject_GetAttrString(moduleMain_, "getNorm");
  Norm   = PyFloat_AsDouble(PyObject_CallObject(func_Norm, args_BM));

  // calculate and return BM
  if (energy == 13) {
  // -- it takes a bit of time (~30sec), but it can be done previouslly for large lists
  PyObject* func_BM = PyObject_GetAttrString(moduleMain_, "getBM");
  BM   = PyInt_AsLong(PyObject_CallObject(func_BM, args_BM));

  for (unsigned int bm_list = 0; bm_list < 13; bm_list++){
    std::cout << "BM " << bm_list << '\n';
    PyObject* args_BM_list = PyTuple_Pack(7,
      PyFloat_FromDouble(static_cast<double>(klJHEP[bm_list])),
      PyFloat_FromDouble(static_cast<double>(ktJHEP[bm_list])),
      PyFloat_FromDouble(static_cast<double>(c2JHEP[bm_list])),
      PyFloat_FromDouble(static_cast<double>(cgJHEP[bm_list])),
      PyFloat_FromDouble(static_cast<double>(c2gJHEP[bm_list])),
      modeldata_, cms_base);
    NormBM.push_back(PyFloat_AsDouble(PyObject_CallObject(func_Norm, args_BM_list)));
    Py_XDECREF(args_BM_list);
  }
  Py_XDECREF(func_BM);
  Py_XDECREF(func_Norm);
  }

  Py_XDECREF(func_CX);
  Py_XDECREF(args_CX);
  Py_XDECREF(args_BM);

  Py_XDECREF(kl_py);
  Py_XDECREF(kt_py);
  Py_XDECREF(c2_py);
  Py_XDECREF(cg_py);
  Py_XDECREF(c2g_py);

  Py_XDECREF(func_load);
  Py_XDECREF(args_load);

}

THWeightInterface::~THWeightInterface()
{
  Py_XDECREF(modeldata_);
  Py_XDECREF(moduleMainString_);
  Py_XDECREF(moduleMain_);
}

double
THWeightInterface::operator()(
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
) const
{

  // calculate and return normalization
  std::cout << "evaluate_weight " << '\n';
  PyObject* func_Weight = PyObject_GetAttrString(moduleMain_, "evaluate_weight");
  PyObject* args_Weight = PyTuple_Pack(11,
    PyFloat_FromDouble(static_cast<double>(kl)),
    PyFloat_FromDouble(static_cast<double>(kt)),
    PyFloat_FromDouble(static_cast<double>(c2)),
    PyFloat_FromDouble(static_cast<double>(cg)),
    PyFloat_FromDouble(static_cast<double>(c2g)),
    PyFloat_FromDouble(static_cast<double>(mhh_gen)),
    PyFloat_FromDouble(static_cast<double>(costSgen_gen)),
    PyFloat_FromDouble(static_cast<double>(normalization)),
    PyFloat_FromDouble(static_cast<double>(energy)),
    cms_base, modeldata_
  );
  PyObject* Weightpy = PyObject_CallObject(func_Weight, args_Weight);
  double THWeight   = PyFloat_AsDouble(Weightpy);

  if (energy == 13) {
    for (unsigned int bm_list = 0; bm_list < 13; bm_list++){
      PyObject* args_BM_list = PyTuple_Pack(11,
        PyFloat_FromDouble(static_cast<double>(klJHEP[bm_list])),
        PyFloat_FromDouble(static_cast<double>(ktJHEP[bm_list])),
        PyFloat_FromDouble(static_cast<double>(c2JHEP[bm_list])),
        PyFloat_FromDouble(static_cast<double>(cgJHEP[bm_list])),
        PyFloat_FromDouble(static_cast<double>(c2gJHEP[bm_list])),
        PyFloat_FromDouble(static_cast<double>(mhh_gen)),
        PyFloat_FromDouble(static_cast<double>(costSgen_gen)),
        PyFloat_FromDouble(static_cast<double>(normalization)),
        PyFloat_FromDouble(static_cast<double>(energy)),
        cms_base, modeldata_);
      WeightBM.push_back(PyFloat_AsDouble(PyObject_CallObject(func_Weight, args_BM_list)));
      Py_XDECREF(args_BM_list);
    }
  }

  return THWeight;
}
