#input the training file 
if [ $# != 1 ]; then
	echo "./demo-word.sh <inputfile>"
	exit 1
fi
time ./word2vec -train $1 -output vectors.bin -outputpos posfile -cbow 1 -size 200 -window 8 -negative 25 -hs 0 -sample 1e-4 -threads 20 -binary 1 -iter 15

echo "Done!"
echo "./distance vectors.bin <SampleFileToTest>"
