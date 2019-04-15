#!/usr/bin/env python
import ROOT
from python import SelectorTools
from python import UserInput
from python import OutputTools
from python import ConfigureJobs
from PlotTools import PlotStyle as Style, pdfViaTex
import os
import sys
import datetime
from ROOT import vector as Vec
VFloat = Vec('float')

style = Style()
gStyle.SetLineScalePS(1.8)

channels = ["eeee", "eemm","mmmm"]
Z_MASS = 91.1876
#channels = ["eeee"]
def getComLineArgs():
    parser = UserInput.getDefaultParser()
    parser.add_argument("--proof", "-p", 
        action='store_true', help="Don't use proof")
    parser.add_argument("--lumi", "-l", type=float,
        default=41.5, help="luminosity value (in fb-1)")
    parser.add_argument("--output_file", "-o", type=str,
        default="", help="Output file name")
    parser.add_argument("--scalefactors_file", "-sf", type=str,
        default="", help="ScaleFactors file name")
    parser.add_argument("--leptonSelections", "-ls", type=str,
        default="TightLeptons", help="Either All Loose or Tight")
    parser.add_argument("--test", action='store_true',
        help="Run test job (no background estimate)")
    parser.add_argument("--output_selection", type=str,
        default="", help="Selection stage of output file "
        "(Same as input if not give)")
    parser.add_argument("-b", "--hist_names", 
                        type=lambda x : [i.strip() for i in x.split(',')],
                        default=["all"], help="List of histograms, "
                        "as defined in ZZ4lAnalysisDatasetManager, separated "
                        "by commas")
    parser.add_argument('--variables', type=str, nargs='*',
                        default=varListNoFull,
                        help=('Names of variables to use. Options are: {}. '
                              'If not specified, all are used except massFull'
                              ).format(', '.join(varList)))
    parser.add_argument('--noNorm', action='store_true',
                        help='Leave differential cross sections in abolute normalization rather than normalizing to unity area.')
    parser.add_argument('--logy', '--logY', '--log', action='store_true',
                        help='Put vertical axis on a log scale.')
    parser.add_argument('--plotDir', type=str, nargs='?',
                        default='/afs/cern.ch/user/u/uhussain/www/UWVVPlots/unfold2017',
                        help='Directory to put plots in')
    return vars(parser.parse_args())


ROOT.gROOT.SetBatch(True)

args = getComLineArgs()


manager_path = ConfigureJobs.getManagerPath()
sys.path.append("/".join([manager_path, 
    "ZZ4lAnalysisDatasetManager", "Utilities/python"]))
import ConfigHistTools 


#fScales = ROOT.TFile(args['scalefactors_file'])
#mZZTightFakeRate = fScales.Get("mZZTightFakeRate")
#eZZTightFakeRate = fScales.Get("eZZTightFakeRate")
#if mZZTightFakeRate:
#    print "Yes muon fake rates"
#    mZZTightFakeRate.SetName("fakeRate_allMu")
#if eZZTightFakeRate:
#    print "Yes electron fake rates"
#    eZZTightFakeRate.SetName("fakeRate_allE")
##
##muonIsoSF = fScales.Get('muonIsoSF')
#muonMoriond18SF= fScales.Get('muonMoriond18SF')
##Electron Reco SF (POG) https://twiki.cern.ch/twiki/bin/viewauth/CMS/Egamma2017DataRecommendations
#electronLowReco18SF = fScales.Get('electronLowReco18SF')
#electronReco18SF = fScales.Get('electronReco18SF')
##Electron ID + SIP HZZ Scale factors
#electronMoriond18SF = fScales.Get('electronMoriond18SF')
#electronMoriond18GapSF = fScales.Get('electronMoriond18GapSF')
#pileupSF = fScales.Get('pileupSF')

selection = args['selection']
if selection == "":
    selection = "LooseLeptons"
    print "Info: Using BasicZZSelections for hist defintions"
analysis = "/".join([args['analysis'], selection])

if args['proof']:
    ROOT.TProof.Open('workers=12')


# set up variables, selection, binnings etc.
# (jet-related variables and selections done later)
variables = {
    'pt' : {c:'Pt' for c in channels},
    'mass' : {c:'Mass' for c in channels},
    'massFull' : {c:'Mass' for c in channels},
    'eta' : {c:'abs(Eta)' for c in channels},
    'z1Mass' : {'eeee':'e1_e2_Mass', 'mmmm':'m1_m2_Mass',
                'eemm':['e1_e2_Mass','m1_m2_Mass']},
    'z2Mass' : {'eeee':'e3_e4_Mass', 'mmmm':'m3_m4_Mass',
                'eemm':['m1_m2_Mass','e1_e2_Mass']},
    'z1Pt' : {'eeee':'e1_e2_Pt', 'mmmm':'m1_m2_Pt',
              'eemm':['e1_e2_Pt','m1_m2_Pt']},
    'z2Pt' : {'eeee':'e3_e4_Pt', 'mmmm':'m3_m4_Pt',
              'eemm':['m1_m2_Pt','e1_e2_Pt']},
    'zHigherPt' : {
        'eeee' : ['e1_e2_Pt', 'e3_e4_Pt'],
        'mmmm' : ['m1_m2_Pt', 'm3_m4_Pt'],
        'eemm' : ['e1_e2_Pt','m1_m2_Pt']
        },
    'zLowerPt' : {
        'eeee' : ['e1_e2_Pt', 'e3_e4_Pt'],
        'mmmm' : ['m1_m2_Pt', 'm3_m4_Pt'],
        'eemm' : ['e1_e2_Pt','m1_m2_Pt']
        },
    'zPt' : {
        'eeee' : ['e1_e2_Pt', 'e3_e4_Pt'],
        'mmmm' : ['m1_m2_Pt', 'm3_m4_Pt'],
        'eemm' : ['e1_e2_Pt','m1_m2_Pt']
        },
    'lPt' : {
        'eeee' : ['e1Pt', 'e2Pt', 'e3Pt', 'e4Pt'],
        'eemm' : ['e1Pt', 'e2Pt', 'm1Pt', 'm2Pt'],
        'mmmm' : ['m1Pt', 'm2Pt', 'm3Pt', 'm4Pt'],
        },
    'l1Pt' : {
        'eeee' : 'max(e1Pt, e3Pt)',
        'eemm' : 'max(e1Pt, m1Pt)',
        'mmmm' : 'max(m1Pt, m3Pt)',
        },
    }


binning = {
    'pt' : [25.*i for i in range(4)] + [100., 150., 200., 300.],
    'mass' : [100.] + [200.+50.*i for i in range(5)] + [500.,600.,800.],
    'massFull' : [80.,100.,120.,130.,150.,180.,200.,240.,300.,400.,1000],
    'eta' : [6,0.,6.],
    'z1Mass' : [60., 80., 84., 86.] + [87.+i for i in range(10)] + [98., 102., 120.], #[12, 60., 120.],
    'z2Mass' : [60., 75., 83.] + [84.+i for i in range(14)] + [105., 120.],#[12, 60., 120.],
    'z1Pt' : [i * 25. for i in range(7)] + [200., 300.],
    'z2Pt' : [i * 25. for i in range(7)] + [200., 300.],
    'zPt' : [i * 25. for i in range(7)] + [200., 300.],
    'zHigherPt' : [i * 25. for i in range(7)] + [200., 300.],
    'zLowerPt' : [i * 25. for i in range(7)] + [200., 300.],
    'lPt' : [15, 0., 150.],
    'l1Pt' : [0.,15.,30.,40.,50.]+[60.+15.*i for i in range(9)]+[195.,225.],#[14,0.,210.],#[15, 0., 150.],
    }

units = {
    'pt' : 'GeV',
    'mass' : 'GeV',
    'massFull' : 'GeV',
    'eta' : '',
    'z1Mass' : 'GeV',
    'z2Mass' : 'GeV',
    'zPt' : 'GeV',
    'z1Pt' : 'GeV',
    'z2Pt' : 'GeV',
    'zHigherPt' : 'GeV',
    'zLowerPt' : 'GeV',
    'lPt' : 'GeV',
    'l1Pt' : 'GeV',
    }

prettyVars = {
    'pt' : 'p_\\text{T}^{\\text{ZZ}}',
    'mass' : 'm_{\\text{ZZ}}',
    'massFull' : 'm_{4\\ell}',
    'eta' : '\\eta_{\\text{ZZ}}',
    'z1Mass' : 'm_{\\text{Z}_{1}}',
    'z2Mass' : 'm_{\\text{Z}_{2}}',
    'z1Pt' : 'p_\\text{T}^{\\text{Z}_{1}}',
    'z2Pt' : 'p_\\text{T}^{\\text{Z}_{2}}',
    'zPt' : 'p_\\text{T}^{\\text{Z}}',
    'zHigherPt' : 'p_\\text{T}^{\\text{Z}_{\\text{lead}}}',
    'zLowerPt' : 'p_\\text{T}^{\\text{Z}_{\\text{sublead}}}',
    'lPt' : 'p_\\text{T}^{\\ell}',
    'l1Pt' : 'p_\\text{T}^{\\ell_1}',
    }

xTitle = {}
yTitle = {}
yTitleNoNorm = {}

yTitleTemp = '{prefix} \\frac{{d\\sigma_{{\\text{{fid}}}}}}{{d{xvar}}} {units}'
for var, prettyVar in prettyVars.iteritems():
    xt = prettyVar
    if units[var]:
        xt += ' \\, \\left(\\text{{{}}}\\right)'.format(units[var])
        yt = yTitleTemp.format(xvar=prettyVar,
                                prefix='\\frac{1}{\\sigma_{\\text{fid}}}',
                                units='\\, \\left( \\frac{{1}}{{\\text{{{unit}}}}} \\right)'.format(unit=units[var]))
        ytnn = yTitleTemp.format(xvar=prettyVar, prefix='',
                                  units='\\, \\left( \\frac{{\\text{{fb}}}}{{\\text{{{unit}}}}} \\right)'.format(unit=units[var]))
    else:
        yt = yTitleTemp.format(prefix='\\frac{1}{\\sigma_{\\text{fid}}}',
                                xvar=prettyVar, units='')
        ytnn = yTitleTemp.format(prefix='', xvar=prettyVar, units='\\left( \\text{fb} \\right)')

    xTitle[var] = xt
    yTitle[var] = yt
    yTitleNoNorm[var] = ytnn
selections = {
    'pt' : {c:'' for c in channels},
    'mass' : {c:'' for c in channels},
    'massFull' : {c:'' for c in channels},
    'eta' : {c:'' for c in channels},
    'z1Mass' : {'eeee':'','mmmm':'',
                'eemm':['abs(e1_e2_Mass - {0}) < abs(m1_m2_Mass - {0})'.format(Z_MASS),
                        'abs(e1_e2_Mass - {0}) > abs(m1_m2_Mass - {0})'.format(Z_MASS)],
                },
    'z2Mass' : {'eeee':'','mmmm':'',
                'eemm':['abs(e1_e2_Mass - {0}) < abs(m1_m2_Mass - {0})'.format(Z_MASS),
                        'abs(e1_e2_Mass - {0}) > abs(m1_m2_Mass - {0})'.format(Z_MASS)],
                },
    'z1Pt' : {'eeee':'','mmmm':'',
              'eemm':['abs(e1_e2_Mass - {0}) < abs(m1_m2_Mass - {0})'.format(Z_MASS),
                      'abs(e1_e2_Mass - {0}) > abs(m1_m2_Mass - {0})'.format(Z_MASS)],
              },
    'z2Pt' : {'eeee':'','mmmm':'',
              'eemm':['abs(e1_e2_Mass - {0}) < abs(m1_m2_Mass - {0})'.format(Z_MASS),
                      'abs(e1_e2_Mass - {0}) > abs(m1_m2_Mass - {0})'.format(Z_MASS)],
              },
    'zPt' : {c:'' for c in channels},
    'zHigherPt' : {
        'eeee' : ['e1_e2_Pt > e3_e4_Pt', 'e1_e2_Pt < e3_e4_Pt'],
        'mmmm' : ['m1_m2_Pt > m3_m4_Pt', 'm1_m2_Pt < m3_m4_Pt'],
        'eemm' : ['e1_e2_Pt > m1_m2_Pt', 'e1_e2_Pt < m1_m2_Pt'],
        },
     'zLowerPt' : {
        'eeee' : ['e1_e2_Pt < e3_e4_Pt', 'e1_e2_Pt > e3_e4_Pt'],
        'mmmm' : ['m1_m2_Pt < m3_m4_Pt', 'm1_m2_Pt > m3_m4_Pt'],
        'eemm' : ['e1_e2_Pt < m1_m2_Pt', 'e1_e2_Pt > m1_m2_Pt'],
        },
    'lPt' : {c:'' for c in channels},
    'l1Pt' : {c:'' for c in channels},
    }

SIPSelections = {
    v : {
        'eeee' : 'e1SIP3D < 4.0 && e2SIP3D < 4.0 && e3SIP3D < 4.0 && e4SIP3D < 4.0',
        'eemm' : 'e1SIP3D < 4.0 && e2SIP3D < 4.0 && m1SIP3D < 4.0 && m2SIP3D < 4.0', 
        'mmmm' : 'm1SIP3D < 4.0 && m2SIP3D < 4.0 && m3SIP3D < 4.0 && m4SIP3D < 4.0', 
        } for v in selections
    }
trueSelections = {
    v : {
        'eeee' : 'e1_e2_Mass > 60. && e3_e4_Mass > 60.',
        'eemm' : 'e1_e2_Mass > 60. && m1_m2_Mass > 60.',
        'mmmm' : 'm1_m2_Mass > 60. && m3_m4_Mass > 60.',
        } for v in selections
    }
trueSelections['massFull'] = {c:'' for c in channels}

# Names of compiled C++ classes to make response matrices fast
# (this is extremely slow in Python because it requires a combination of
# information from multiple trees, which can't be done with TTree::Draw())
responseClassNames = {
    'mass' : {c:'FloatBranchResponseMatrixMaker' for c in channels},
    #'massFull' : {c:'FullSpectrumFloatResponseMatrixMaker' for c in channels},
    #'pt' : {c:'FloatBranchResponseMatrixMaker' for c in channels},
    #'eta' : {c:'AbsFloatBranchResponseMatrixMaker' for c in channels},
    #'z1Mass' : {'eeee':'FloatBranchResponseMatrixMaker',
    #            'mmmm':'FloatBranchResponseMatrixMaker',
    #            'eemm':'Z1ByMassResponseMatrixMaker',},
    #'z2Mass' : {'eeee':'FloatBranchResponseMatrixMaker',
    #            'mmmm':'FloatBranchResponseMatrixMaker',
    #            'eemm':'Z2ByMassResponseMatrixMaker',},
    #'z1Pt' : {'eeee':'FloatBranchResponseMatrixMaker',
    #          'mmmm':'FloatBranchResponseMatrixMaker',
    #          'eemm':'Z1ByMassResponseMatrixMaker',},
    #'z2Pt' : {'eeee':'FloatBranchResponseMatrixMaker',
    #          'mmmm':'FloatBranchResponseMatrixMaker',
    #          'eemm':'Z2ByMassResponseMatrixMaker',},
    #'zHigherPt' : {c:'Z1ByPtResponseMatrixMaker' for c in channels},
    #'zLowerPt' : {c:'Z2ByPtResponseMatrixMaker' for c in channels},
    #'lPt' : {c:'AllLeptonBranchResponseMatrixMaker' for c in channels},
    #'l1Pt' : {c:'LeptonMaxBranchResponseMatrixMaker' for c in channels},
    #'zPt' : {c:'BothZsBranchResponseMatrixMaker' for c in channels},
    }

# Variable names usable by response maker classes
varNamesForResponseMaker = {
    'mass' : {c:'Mass' for c in channels},
    #'massFull' : {c:'Mass' for c in channels},
    #'pt' : {c:'Pt' for c in channels},
    #'eta' : {c:'Eta' for c in channels},
    #'z1Mass' : {'eeee':'e1_e2_Mass','mmmm':'m1_m2_Mass','eemm':'Mass'}, # 4e/4mu just use 1 variable because that's easy
    #'z2Mass' : {'eeee':'e3_e4_Mass','mmmm':'m3_m4_Mass','eemm':'Mass'}, # for 2e2mu, the response maker class will figure it out
    #'z1Pt' : {'eeee':'e1_e2_Pt','mmmm':'m1_m2_Pt','eemm':'Pt'}, # 4e/4mu just use 1 variable because that's easy
    #'z2Pt' : {'eeee':'e3_e4_Pt','mmmm':'m3_m4_Pt','eemm':'Pt'}, # for 2e2mu, the response maker class will figure it out
    #'zPt' : {c:'Pt' for c in channels},
    #'zHigherPt' : {c:'Pt' for c in channels},
    #'zLowerPt' : {c:'Pt' for c in channels},
    #'lPt' : {c:'Pt' for c in channels},
    #'l1Pt' : {c:'Pt' for c in channels},
    #'zPt' : {c:'Pt' for c in channels},
}

# list of variables not counting systematic shifts
varList = [v for v in variables if 'Up' not in v and 'Down' not in v]
varListNoFull = varList[:]
varListNoFull.remove("massFull")

# Sometimes need to more or resize legend
legDefaults = {
    'textsize' : 0.034, #.027,#2,
    'leftmargin' : 0.35,
    'entryheight' : 0.037,
    'rightmargin' : 0.03,
    }
legParams = {v:legDefaults.copy() for v in varList}
legParams['z1Mass'] = {
    'textsize' : .026,
    'leftmargin' : .03,
    'rightmargin' : .46,
    'entryheight' : .034,#23
    'entrysep' : .007,
    }
legParams['z2Mass'] = legParams['z1Mass'].copy()
legParams['eta'] = legParams['deltaEtajj'].copy()
legParams['massFull']['leftmargin'] = 0.25

legParamsLogy = {v:p.copy() for v,p in legParams.iteritems()}
legParamsLogy['l1Pt']['topmargin'] = 0.65
legParamsLogy['l1Pt']['leftmargin'] = 0.2
legParamsLogy['l1Pt']['rightmargin'] = 0.18
legParamsLogy['mass']['topmargin'] = 0.075
legParamsLogy['mass']['leftmargin'] = 0.35
legParamsLogy['mass']['rightmargin'] = 0.025
legParamsLogy['mass']['textsize'] = 0.033
legParamsLogy['lPt']['topmargin'] = 0.05
legParamsLogy['zHigherPt']['topmargin'] = 0.045
legParamsLogy['massFull']['topmargin'] = 0.035

def normalizeBins(h):
    binUnit = 1 # min(h.GetBinWidth(b) for b in range(1,len(h)+1))
    for ib in xrange(1,len(h)+1):
        w = h.GetBinWidth(ib)
        h.SetBinContent(ib, h.GetBinContent(ib) * binUnit / w)
        h.SetBinError(ib, h.GetBinError(ib) * binUnit / w)
        if h.GetBinError(ib) > h.GetBinContent(ib):
            h.SetBinError(ib, h.GetBinContent(ib))
    h.sumw2()

def unnormalizeBins(h):
    binUnit = 1 # min(h.GetBinWidth(b) for b in range(1,len(h)+1))
    for ib in xrange(1,len(h)+1):
        w = h.GetBinWidth(ib)
        h.SetBinContent(ib, h.GetBinContent(ib) * w / binUnit)
        h.SetBinError(ib, h.GetBinError(ib) * w / binUnit)
        if h.GetBinError(ib) > h.GetBinContent(ib):
            h.SetBinError(ib, h.GetBinContent(ib))
    h.sumw2()

#Work on this function but what _rootComp is doing I already did by loading starting line100

ROOT.gSystem.Load('ResponseMatrixMaker_cxx')
def _generateResponseClass(varName, channel):
    
    className = responseClassNames[varName][channel]

    #for example C=<class 'ROOT.BranchValueResponseMatrixMaker<float>'>
    C = getattr(ROOT, className)

    #sigFileNames = {s.name : [f for f in s.getFileNames()]
    #                for s in samples['reco'].values()[0].getBaseSamples()}
    
    filelist= args['filenames']
    sigFileNames = {dataset : ConfigureJobs.getInputFilesPath(dataset,selection,analysis)  
                    for dataset in ConfigureJobs.getListOfFiles(filelist, selection)}
    
    #sigConstWeights = {dataset : s.xsec * s.intLumi * float(s.kFactor) / s.sumW
    #                   for dataset in ConfigureJobs.getListOfFiles(filelist, args['selection'])}

    sigConstWeights = {dataset : 1.0
                       for dataset in ConfigureJobs.getListOfFiles(filelist, selection)}

    binning = binning[varName]
    vBinning = VFloat()
    if len(binning) == 3:
        binningTemp = [binning[1] + i * (binning[2] - binning[1])/float(binning[0]) for i in xrange(binning[0]+1)]
        for b in binningTemp:
            vBinning.push_back(b)
    else:
        for b in binning:
            vBinning.push_back(b)

    responseMakers = {}
    for sample, fNameList in sigFileNames.iteritems():
        resp = C(channel, varNamesForResponseMaker[varName][channel], vBinning)

        for fName in fNameList:
            resp.registerFile(fName)

        resp.setConstantScale(sigConstWeights[sample])

        responseMakers[sample] = resp
    
    return responseMakers

#Load the RooUnfold library into ROOT
ROOT.gSystem.Load("libRooUnfold")
def getResponse(varName,chan):
    #get responseMakers from the function above- this is the whole game.
    responseMakers = generateResponseClass(varName, chan, samples)

    hResponseNominal = {s:resp for s,resp in responseMakers.iteritems()}
    hResponseNominalTotal = sum(resp for resp in hResponseNominal.values())
    
    #Actually hResponseNominalTotal is alread a TH2D histo which goes as input to RooUnfoldResponse
    #dont need hUnfolded actually.
    #hUnfolded[''], hCov, hResp = _getUnfolded(hSigNominal,
    #                                          hBkgMCNominal+hBkgNominal,
    #                                          hTrue[''],
    #                                          hResponseNominalTotal,
    #                                          hData, nIter, True)
    print "type of hResp: " hResponseNominalTotal
    #hResp = hResponseNominalTotal

    # But its better to use RooUnfoldResponse here
    #RooUnfoldResponse constructor - create from already-filled histograms
    # "response" gives the response matrix, measured X truth.
    #  "measured" and "truth" give the projections of "response" onto the X-axis and Y-axis respectively,
    #   but with additional entries in "measured" for measurements with no corresponding truth (fakes/background) and
    #    in "truth" for unmeasured events (inefficiency).
    #     "measured" and/or "truth" can be specified as 0 (1D case only) or an empty histograms (no entries) as a shortcut
    #      to indicate, respectively, no fakes and/or no inefficiency.

    Response = getattr(ROOT,"RooUnfoldResponse")

    ## Give hSig and hTrue in the form of histograms
    response = Response(hSig, hTrue.clone(), hResponse.clone())
    
    #Response matrix as a 2D-histogram: (x,y)=(measured,truth)
    hResp = response.Hresponse()
    # plot covariance and response
    if plotDir:
        cRes = Canvas(1000,1000)
        if varName == 'massFull':
            cRes.SetLogx()
            cRes.SetLogy()
        hResp.drawstyle = 'colztext'
        hResp.xaxis.title = '\\text{Reco} '+xTitle[varName]
        hResp.yaxis.title = '\\text{True} '+xTitle[varName]
        hResp.draw()
        style.setCMSStyle(cRes, '', dataType='Internal', intLumi=35860.)
        cRes.Print(os.path.join(plotDir, 'pngs', "response_{}_{}.png".format(varName, chan)))
        cRes.Print(os.path.join(plotDir, 'Cs', "response_{}_{}.C".format(varName, chan)))

        #cCov = Canvas(1000,1000)
        #if varName == 'massFull':
        #    cCov.SetLogx()
        #    cCov.SetLogy()
        #hCov.Draw("colztext")
        #style.setCMSStyle(cCov, '', dataType='Internal', intLumi=35860.)
        #cCov.Print(os.path.join(plotDir, 'pngs', "covariance_{}_{}.png".format(varName, chan)))
        #cCov.Print(os.path.join(plotDir, 'Cs', "covariance_{}_{}.C".format(varName, chan)))

#if __name__ == "__main__":

if not os.path.exists(args['plotDir']):
    os.makedirs(args['plotDir'])
elif not os.path.isdir(args['plotDir']):
    raise IOError("There is already some non-directory object called {}.".format(args[plotDir]))
varName="mass"
for chan in channels:
    getResponse(varName,chan)

if args['test']:
    exit(0)


