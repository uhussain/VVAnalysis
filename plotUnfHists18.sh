variables="pt mass zpt leppt dphiz1z2 drz1z2"
#variables="dphiz1z2"
for var in $variables;do
  echo $var
  ./Utilities/scripts/plotUnfolded.py -a ZZ4l2018 -s TightLeptonsWGen -l 59.7 -f ZZ4l2018 -vr ${var} --test --makeTotals 
  #--plotResponse
done
