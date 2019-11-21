#variables="pt mass zpt leppt dphiz1z2 drz1z2"
variables="pt"
for var in $variables;do
  echo $var
  ./Utilities/scripts/plotUnfolded.py -a ZZ4l2018 -s TightLeptonsWGen -l 137.1 -f ZZ4l2018 -vr ${var} --test 
  #--makeTotals 
  #--plotResponse
done
