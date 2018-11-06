#!/usr/bin/env python
import ROOT
import shutil,subprocess
from HHStatAnalysis.AnalyticalModels.NonResonantModel import NonResonantModel

def load(cms_base, energy) :
    print ("load model py", energy)
    model = NonResonantModel()
    if energy == 13 :
        print ("load model for 13 TeV")
        dumb = model.ReadCoefficients(cms_base+"/src/HHStatAnalysis/AnalyticalModels/data/coefficientsByBin_extended_3M_costHHSim_19-4.txt")
    if energy == 14 :
        print ("load model for 14 TeV")
        model.ReadCoefficients2(14, cms_base)
    return model

def getCX(kl, kt, c2, cg, c2g, model) :
    if energy == 14 : return model.functionGF(kl, kt, c2, cg, c2g, model.A14tev)
    else : return model.functionGF(kl, kt, c2, cg, c2g, model.A13tev)

def getBM(kl, kt, c2, cg, c2g, model, cms_base) :
    histfile = cms_base+"/src/Support/NonResonant/Distros_5p_SM3M_sumBenchJHEP_13TeV_19-4.root"
    histtitle = "H1bin4"
    return model.getCluster(kl, kt, c2, cg, c2g, histfile, histtitle)

def getNorm(kl, kt, c2, cg, c2g, energy, model, cms_base) :
    histfile = cms_base+"/src/Support/NonResonant/Distros_5p_SM3M_sumBenchJHEP_13TeV_19-4.root"
    histtitle = "H1bin4"
    if energy == 14 :
        return model.getNormalization2(kl, kt, c2, cg, c2g)
    else :
        return model.getNormalization(kl, kt, c2, cg, c2g, histfile, histtitle)

def evaluate_weight(kl, kt, c2, cg, c2g, mhh_gen, cost_gen, calcSumOfWeights, energy, cms_base, model) :
    if energy == 14 :
        ### this histogram bellow should be adapted to our input events
        print ("entered evaluate weights bleble", energy)
        fileAllHisto = ROOT.TFile(cms_base+"/src/Support/NonResonant/HistoInputEvents_SUM_BMv2_14TeV.root")
        inputSamplesHisto = fileAllHisto.Get("EventsSum")
        return model.getScaleFactor2(mhh_gen, cost_gen, kl, kt, c2, cg, c2g, inputSamplesHisto)
    else :
        ### this histogram bellow should be adapted to our input events
        histfile = cms_base+"/src/Support/NonResonant/Distros_5p_SM3M_sumBenchJHEP_13TeV_19-4.root"
        histtitle = "H1bin4"
        fileHH=ROOT.TFile(histfile)
        sumEvt = fileHH.Get(histtitle)
        denominator = sumEvt.GetBinContent(sumEvt.GetXaxis().FindBin(mhh_gen), sumEvt.GetYaxis().FindBin(abs(cost_gen)))
        return model.getScaleFactor(mhh_gen, cost_gen, kl, kt, c2, cg, c2g, denominator, calcSumOfWeights)
