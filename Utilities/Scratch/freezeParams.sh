# Commands taken from https://indico.cern.ch/event/577649/contributions/2388797/attachments/1380376/2098158/HComb-Tutorial-Nov16-Impacts.pdf
#--freezeParameters CMS_eff_b,CMS_eff_e,CMS_eff_m,CMS_norm_nonprompt_eee,CMS_norm_nonprompt_eem,CMS_norm_nonprompt_emm,CMS_norm_nonprompt_mmm,CMS_pileup,CMS_res_j,CMS_scale_e,CMS_scale_j,CMS_scale_m,CMS_scale_unclEnergy,QCDscale_top-ewk,QCDscale_vv,QCDscale_zg,lumi2016_13TeV,pdf_top-ewk,pdf_vv,pdf_zg,prop_bineee_bin0,prop_bineem_bin0,prop_binemm_bin0,prop_binmmm_bin0
#--freezeParameters QCDscale_top-ewk,QCDscale_vv,QCDscale_zg,pdf_top-ewk,pdf_vv,pdf_zg
#--freezeParameters CMS_eff_e,CMS_eff_m,CMS_scale_e,CMS_scale_m
#--freezeParameters CMS_eff_b,CMS_eff_e,CMS_eff_m,CMS_norm_QCD-WZjj,CMS_norm_nonprompt_eee,CMS_norm_nonprompt_eem,CMS_norm_nonprompt_emm,CMS_norm_nonprompt_mmm,CMS_pileup,CMS_res_j,CMS_scale_e,CMS_scale_j,CMS_scale_m,CMS_scale_unclEnergy,QCDscale_EW-WZjj,QCDscale_QCD-WZjj,QCDscale_top-ewk,QCDscale_vv,QCDscale_zg,lumi2016_13TeV,pdf_EW-WZjj,pdf_QCD-WZjj,pdf_top-ewk,pdf_vv,pdf_zg,prop_bineee_bin0,prop_bineem_bin0,prop_binemm_bin0,prop_binmmm_bin0
#--freezeParameters CMS_norm_nonprompt_eee,CMS_norm_nonprompt_eem,CMS_norm_nonprompt_emm,CMS_norm_nonprompt_mmm
runVariation () {
    param=$1
    breakdown=$2
    filename=$3
    echo "Param is $param"
    combine -M MultiDimFit --algo grid --points 50 --rMin -1 --rMax 4 -m 125 -n stat \
        higgsCombinebestfit.MultiDimFit.mH125.root --snapshotName MultiDimFit \
        --freezeParameters "$param" $4
    plot1DScan.py higgsCombinenominal.MultiDimFit.mH125.root \
        --others 'higgsCombinestat.MultiDimFit.mH125.root:Freeze all:2' \
        --breakdown "$breakdown"
    cp scan.pdf ~/www/WZjjImpacts/PrefiringStudy/LikelihoodScans/CombinedXSec/scan_${filename}.pdf
    #cp scan.pdf ~/www/WZjjImpacts/PAS/LikelihoodScans/CombinedXSec/scan_${filename}.pdf
    echo "Scan for param $param copied to scan_${filename}.pdf"
}
card=WZjj.root
combine -M MultiDimFit --algo grid --points 50 --rMin -1 --rMax 4 $card -m 125 -n nominal
combine -M MultiDimFit --algo none --rMin -1 --rMax 4 $card -m 125 -n bestfit --saveWorkspace
runVariation "all" "syst,stat" "stat" "--fastScan"

#param=prop_bineee_bin0,prop_bineem_bin0,prop_binemm_bin0,prop_binmmm_bin0
#name="Bkgd stat"
#filename="BkgdStat"
#runVariation "$param" "$name,other" "$filename"
#
#param="CMS_eff_e,CMS_eff_m,CMS_scale_m,CMS_scale_e"
#name="lep. scale"
#filename="lepScale"
#runVariation "$param" "$name,other" "$filename"
#
#param="CMS_scale_j"
#name="JES"
#filename="JES"
#runVariation "$param" "$name,other" "$filename"
#
#param="CMS_res_j"
#name="JER"
#filename="JER"
#runVariation "$param" "$name,other" "$filename"
#
#param="CMS_scale_j,CMS_res_j"
#name="jet en."
#filename="JESandJER"
#runVariation "$param" "$name,other" "$filename"
#
#param="CMS_res_j"
#name="JER"
#filename="JER"
#runVariation "$param" "$name,other" "$filename"
#
#param="CMS_eff_b"
#name="b eff."
#filename="beff"
#runVariation "$param" "$name,other" "$filename"
#
#param="CMS_norm_nonprompt_eee,CMS_norm_nonprompt_eem,CMS_norm_nonprompt_emm,CMS_norm_nonprompt_mmm"
#name="np norm"
#filename="NonpromptNorm"
#runVariation "$param" "$name,other" "$filename"

param="lumi2016_13TeV"
name="lumi"
filename="lumi"
runVariation "$param" "$name,other" "$filename"

#param="QCDscale_top-ewk,QCDscale_vv,QCDscale_zg,pdf_top-ewk,pdf_vv,pdf_zg"
#name="bk theory"
#filename="BkgdTheory"
#runVariation "$param" "$name,other" "$filename"
#
