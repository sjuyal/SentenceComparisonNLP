CC = gcc
CCP = g++
#Using -Ofast instead of -O3 might result in faster code, but is supported only by newer GCC versions
CFLAGS = -lm -pthread -O3 -march=native -Wall -funroll-loops -Wno-unused-result

all: word2vec distance

word2vec : nword2vec.cpp
	$(CCP) nword2vec.cpp -o word2vec $(CFLAGS)
distance : ndistance.cpp
	$(CCP) ndistance.cpp porter.c -o distance -lm -pthread -O3 -march=native -w -funroll-loops -Wno-unused-result
	chmod +x *.sh
clean:
	rm -rf word2vec distance
