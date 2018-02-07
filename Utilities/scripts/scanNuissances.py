import ROOT
import subprocess
import math

def getMuAndErr(arg):
    subprocess.call(["bash", "runCombine.sh", "mu", arg])
    rtfile = ROOT.TFile("mlfit.root")
    tree = rtfile.Get("tree_fit_sb")
    row = tree.__iter__().next()

    return (row.mu, row.muLoErr, row.muHiErr)

muCentral, muCentralErrDown, muCentralErrUp = getMuAndErr("")

results = {}

nuisances = {"jes" : "simple",
    "jer" : "simple",
    "lumi" : "simple",
    "MET" : "simple",
    "btag" : "simple",
    "wzjj-ewk_scale" : "simple",
    "nonprompt_all" : "group" ,
    "mc_stat" : "group" ,
    "mu_unc" : "group" ,
    "e_unc" : "group" ,
    "wz_qcd_all": "group" ,
    "other_theory" : "group",
}
for nuisance, nutype in nuisances.iteritems():
    if nutype == "simple":
        arg = "--freezeNuisances=%s" % nuisance
    else:
        arg = "--freezeNuisanceGroup=%s" % nuisance
    mu, muErrDown, muErrUp = getMuAndErr(arg)
    nuErrDown = math.sqrt(muCentralErrDown**2-muErrDown**2)
    nuErrUp = math.sqrt(muCentralErrUp**2-muErrUp**2)
    results[nuisance] = { "mu" : mu,
        "errDown" : muErrDown,
        "errUp" : muErrUp,
        "nuOnlyErrDown" : nuErrDown,
        "nuOnlyErrUp" : nuErrUp,
    }

print "-"*80
print "    Full fit gives %0.3f^{%0.3f}_{%0.3f}" % (muCentral, muCentralErrDown, muCentralErrUp)
print ""
for nuisance in nuisances:
    res = results[nuisance]
    print "-"*80
    print "    Nuisance %s" % nuisance
    print "    Removing nuisance gives %0.3f^{%0.3f}_{%0.3f}" % \
            (res["mu"], res["errUp"], res["errDown"])
    print "    Only from nuisance: +%0.3f -%0.3f" % \
            (res["nuOnlyErrUp"], res["nuOnlyErrDown"])

