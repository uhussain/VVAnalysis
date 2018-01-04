import ROOT
import os
from python import ConfigureJobs,ApplySelection
from python.prettytable import PrettyTable
import argparse
import datetime

def chunks(l, n):
    """Yield successive n-sized chunks from l."""
    for i in range(0, len(l), n):
        yield l[i:i + n]

def getEventSelectionExpr(path, comparison, channel):
    events = []
    
    file_name = "%s/%s_%s.txt" % (path, comparison, channel)
    print file_name
    with open(file_name, "r") as events_file:
        for line in events_file.readlines():
            event_num = line.strip().split(":")
            if len(event_num) == 3:
                events.append("(run == {0} && lumi == {1} && evt == {2})".format(*event_num))
    return events

parser = argparse.ArgumentParser() 
parser.add_argument("-f", "--filelist", 
                    type=lambda x : [i.strip() for i in x.split(',')],
                    required=True, help="List of input file names "
                    "to be processed (separated by commas)")
parser.add_argument("-s", "--selection", required=True)
parser.add_argument("--output_selection", required=False, default="")
parser.add_argument("-m", "--cut_string", required=False, type=str,
                    default="")
parser.add_argument("-c", "--channels", required=False, type=str,
                    default="eee,eem,emm,mmm")
parser.add_argument("-o", "--output_file", required=False, type=str,
                    default=".")
args = parser.parse_args()

isfile = any(os.path.isfile(name) or os.path.exists(name.rstrip("/*")) 
                for name in args.filelist)
filelist = ConfigureJobs.getListOfFiles(args.filelist, args.selection) if \
    not isfile else args.filelist
states = [x.strip() for x in args.channels.split(",")]
file_paths = []
output_dir = os.path.dirname(args.output_file)
for name in filelist:
    if not isfile:
        try:
            label = name
            file_paths.append((label, ConfigureJobs.getInputFilesPath(name, 
                args.selection, "WZxsec2016")))
        except ValueError as e:
            print e
            continue
    else:
        label = file_path.split(".")[0].split("/")[-1]
        file_paths.append((label, name))
    try:
        os.makedirs(output_dir + "/" + label)
    except OSError as e:
        print e
        pass

for state in states:
    first = True
    for file_path in file_paths:
        state = state.strip()
        chain = ROOT.TChain("%s/ntuple" % state)
        chain.Add(file_path[1])
        run_expr = "run:lumi:evt"
        trig_expr = "singleESingleMuPass:singleMuSingleEPass:doubleMuPass:doubleMuDZPass:doubleEPass"
        filter_expr = "Flag_duplicateMuonsPass:Flag_BadPFMuonFilterPass:Flag_badMuonsPass:Flag_goodVerticesPass"
        lepid_expr = "e1IsCBVIDHLTSafe:e1IsCBVIDTight:e1Pt:e1Eta:e1Phi:e1PVDXY:e1PVDZ:" \
                            "e2IsCBVIDHLTSafe:e2IsCBVIDTight:e2Pt:e2Eta:e2Phi:e2PVDXY:e2PVDZ:" \
                            "e3IsCBVIDHLTSafe:e3IsCBVIDTight:e3Pt:e3Eta:e3Phi:e3PVDXY:e3PVDZ"
        veto_expr = "nCBVIDHLTSafeElec:nWZMediumMuon:nCBVIDTightElec:nWZTightMuon"
        scan_expr = ":".join([run_expr,lepid_expr,veto_expr,filter_expr])

        outfile_name = "/".join([output_dir, file_path[0], args.output_file.split("/")[-1].replace("chan", state)])
        events = getEventSelectionExpr("/eos/user/k/kelong/WZAnalysisData/SyncWithJakob/Differences_20Dec2017/InclusiveSelection",
                #"KennethNotJakob",
                "JakobNotKenneth",
                state
        )
        print events
        for i, eventlist in enumerate(chunks(events, 40)):
            if i > 0:
                outfile_name = outfile_name.replace(".txt", "%i.txt" % i)
            evtstring = " || ".join(eventlist)
            chain.GetPlayer().SetScanRedirect(True)
            chain.GetPlayer().SetScanFileName(outfile_name)
            chain.Scan(scan_expr, evtstring,"colsize=30")
            with open(args.output_file.replace("chan", state), "w" if first else "a") as allfiles:
                if not os.path.isfile(outfile_name):
                    continue
                with open(outfile_name) as subfile:
                    lines = subfile.readlines()
                    if not first:
                        lines = lines[3:-1]
                    allfiles.write("\n".join(lines))
                    first = False
