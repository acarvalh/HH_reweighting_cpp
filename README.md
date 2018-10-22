# HH_reweighting_cpp

Install the reweighting tools:

'''
cd $CMSSW_8+/src
cmsenv
curl -s https://raw.githubusercontent.com/cms-hh/HHStatAnalysis/master/install_ana_models.sh | bash -s
git clone https://github.com/cms-hh/Support.git
scram b -j 8
'''

It was tested on CMSSW_9_4_6_patch1 and CMSSW_8_1_0.
Even if the README of HHStatanalysis says that you need to install CombineHavester as well, if you follow the recipe above there is NO need for installing CombineHavester.

The cpp class to calculate the weights is bellow:

'''
cd HH_reweighting_cpp
make make_weights
./make_weights
'''

There are two
