variables="pt mass zpt leppt dphiz1z2 drz1z2"
#variables="dphiz1z2"
for var in $variables;do
  echo $var
  ./Utilities/scripts/plotUnfolded.py -a ZZ4l2016 -s TightLeptonsWGen -l 35.9 -f ZZ4l2016 -vr ${var} --test --makeTotals 
  #--plotResponse
done
