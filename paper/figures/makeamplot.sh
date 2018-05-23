date=20170116

for type in phonetics letter bouma
do
   echo "EPSF" > ${type}.amplot
   cat ../../analysis/${type}_${date}.bars >> ${type}.amplot
   amplot ${type}.amplot > ${type}.eps
done


