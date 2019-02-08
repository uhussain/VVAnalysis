# Setup ScaleFactor objects to be "registered" with ROOT,
# allowing them to be called from TTree.Draw(), for example.
# Currently used for lepton scale factors and pileup weights.
#
# Modified from N. Smith, U. Wisconsin
# 

#!/usr/bin/env python
import ROOT
import argparse
import os
ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True

def float2double(hist):
    if hist.ClassName() == 'TH1D' or hist.ClassName() == 'TH2D':
        return hist
    elif hist.ClassName() == 'TH1F':
        new = ROOT.TH1D()
        hist.Copy(new)
    elif hist.ClassName() == 'TH2F':
        new = ROOT.TH2D()
        hist.Copy(new)
    else:
        raise Exception("Bad hist, dummy")
    return new

def invert2DHist(hist):
    new_hist = hist.Clone()
    ROOT.SetOwnership(new_hist, False)
    for x in range(hist.GetNbinsX()+1):
        for y in range(hist.GetNbinsY()+1):
            value = hist.GetBinContent(x, y)
            new_hist.SetBinContent(y, x, value)
    new_hist.GetXaxis().SetTitle(hist.GetXaxis().GetTitle())
    new_hist.GetYaxis().SetTitle(hist.GetYaxis().GetTitle())
    return new_hist

parser = argparse.ArgumentParser()
parser.add_argument("-t", "--tightfr_file", type=str,
        default='data/fakeRate17Dec2018-ZplusLSkimZZ.root')
#parser.add_argument("-m", "--medfr_file", type=str,
#        default='data/fakeRate18Apr2017-3LooseLeptons-MediumMuons.root')
args = parser.parse_args()
output_file = 'data/scaleFactorsZZ4l2017.root'
fScales = ROOT.TFile(output_file, 'recreate')

# For nTruePU reweighting
pileupSF = ROOT.ScaleFactor("pileupSF", "Run2017B-F 41.5/fb Pileup profile over RunIIFall17 MC Scale Factor, x=NTruePU")
pileupFile = ROOT.TFile.Open('PileupWeights17/PU_Central.root')
pileupFileUp = ROOT.TFile.Open('PileupWeights17/PU_minBiasUP.root')
pileupFileDown = ROOT.TFile.Open('PileupWeights17/PU_minBiasDOWN.root')
pileupSF.Set1DHist(pileupFile.Get('pileup'), pileupFileUp.Get('pileup'), pileupFileDown.Get('pileup'))
fScales.cd()
pileupSF.Write()

#Electron (Pt<20 Reco SF from POG https://twiki.cern.ch/twiki/bin/viewauth/CMS/Egamma2017DataRecommendations
electronLowReco18SF = ROOT.ScaleFactor("electronLowReco18SF", "Moriond '18 Electron Low Reco SF, x=Eta, y=Pt")
eLowRecoFile = ROOT.TFile.Open('data/egammaEffi.txt_EGM2D_runBCDEF_passingRECO_lowEt.root')
electronLowReco18SF.Set2DHist(float2double(eLowRecoFile.Get('EGamma_SF2D')))
fScales.cd()
electronLowReco18SF.Write()

#Electron (Pt>20 Reco SF from POG https://twiki.cern.ch/twiki/bin/viewauth/CMS/Egamma2017DataRecommendations
electronReco18SF = ROOT.ScaleFactor("electronReco18SF", "Moriond '18 Electron Reco SF, x=Eta, y=Pt")
eRecoFile = ROOT.TFile.Open('data/egammaEffi.txt_EGM2D_runBCDEF_passingRECO.root')
electronReco18SF.Set2DHist(float2double(eRecoFile.Get('EGamma_SF2D')))
fScales.cd()
electronReco18SF.Write()
#
electronMoriond18SF = ROOT.ScaleFactor("electronMoriond18SF", "Moriond '18 HZZ ID SF, x=Eta, y=Pt")
eidFile = ROOT.TFile.Open('data/egammaEffi.txt_EGM2D_Moriond2018v1.root')
electronMoriond18SF.Set2DHist(float2double(eidFile.Get('EGamma_SF2D')))
fScales.cd()
electronMoriond18SF.Write()

electronMoriond18GapSF = ROOT.ScaleFactor("electronMoriond18GapSF", "Moriond '18 GapElectron HZZ ID SF, x=Eta, y=Pt")
eleGsfFile = ROOT.TFile.Open('data/egammaEffi.txt_EGM2D_Moriond2018v1_gap.root')
electronMoriond18GapSF.Set2DHist(float2double(eleGsfFile.Get('EGamma_SF2D')))
fScales.cd()
electronMoriond18GapSF.Write()

muonMoriond18SF = ROOT.ScaleFactor("muonMoriond18SF", "Moriond '18 Trk+ID+ISO SF, x=abs(Eta), y=Pt")
midFile = ROOT.TFile.Open('data/ScaleFactors_mu_Moriond2018_final.root')
muonMoriond18SF.Set2DHist(float2double(midFile.Get('FINAL')),float2double(midFile.Get('ERROR')),float2double(midFile.Get('ERROR')))
fScales.cd()
muonMoriond18SF.Write()


if os.path.isfile(args.tightfr_file):
    print "INFO: Adding tight fake rates to %s" % output_file
    fakeRateFile = ROOT.TFile.Open(args.tightfr_file)
    eZZTightFakeRate = ROOT.ScaleFactor("eZZTightFakeRate", "Fake rate from Z+jet")
    eZZTightFakeRate.Set2DHist(fakeRateFile.Get('DataEWKCorrected/ratioE2D_allE'), 0, 0, ROOT.ScaleFactor.AsInHist)
    mZZTightFakeRate = ROOT.ScaleFactor("mZZTightFakeRate", "Fake rate from Z+jet")
    mZZTightFakeRate.Set2DHist(fakeRateFile.Get('DataEWKCorrected/ratioMu2D_allMu'), 0, 0, ROOT.ScaleFactor.AsInHist)
    fScales.cd()
    mZZTightFakeRate.Write()
    eZZTightFakeRate.Write()
