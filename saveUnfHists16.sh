variables="pt mass zpt leppt dphiz1z2 drz1z2"
#variables="dphiz1z2"
for var in $variables;do
  echo $var
  ./Utilities/scripts/saveUnfolded.py -a ZZ4l2016 -s TightLeptonsWGen -l 35.9 -f ZZ4l2016 -sf data/scaleFactorsZZ4l2016.root -ls 2016fWUnc_full -vr ${var} --test --makeTotals 
  #--plotResponse
done
