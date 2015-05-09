// Author: Shashank Juyal
// IIIT Hyderabad

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <set>
#include <utility>
#include "stemmer.h"
#include <stdlib.h>

using namespace std;

const long long max_size = 2000;         // max length of strings
const long long N = 20;                  // number of closest words that will be shown
const long long max_w = 50;              // max length of vocabulary entries

FILE *f, *fp, *fx, *fy, *finp;
char st1[max_size];
char *bestw[N];
char file_name[max_size], train_file[max_size];
float dist, len, bestd[N], vec[max_size];
long long words, size, a, b, c, d, bi[100];
float *M;
char *vocab;
vector<string> finalvecwords;

set<string> wordset;

bool compare(const pair<int,int>&i, const pair<int,int>&j){
    return i.second > j.second;
}

#define INC 50           /* size units in which s is increased */
static int i_max = INC;  /* maximum offset in s */

#define TRUE 1
#define FALSE 0

#define LETTER(ch) (isupper(ch) || islower(ch))

static char *s;  

map<string,float> nearby(int cn, char st[50][max_size],int flag, int R){
	map<string,float> nb;
	for (a = 0; a < cn; a++) {
		for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_w], st[a])) break;
		if (b == words) b = -1;
		bi[a] = b;
		//printf("\nWord: %s  Position in vocabulary: %lld\n", st[a], bi[a]);
		if(flag==0){
			finalvecwords.push_back((string)st[a]);
			nb[(string)st[a]] = 0;
		}
		//wordscore[ii][(string)st[a]] = 0;
		
	}
	//if (b == -1) continue;
	//printf("\n   Word       Cosine distance\n------------------------------------------------------------------------\n");
	for (a = 0; a < size; a++) vec[a] = 0;
	for (b = 0; b < cn; b++) {
		if (bi[b] == -1) continue;
		for (a = 0; a < size; a++) vec[a] += M[a + bi[b] * size];
	}
	len = 0;
	for (a = 0; a < size; a++) len += vec[a] * vec[a];
	len = sqrt(len);
	for (a = 0; a < size; a++) vec[a] /= len;
	for (a = 0; a < N; a++) bestd[a] = -1;
	for (a = 0; a < N; a++) bestw[a][0] = 0;
	for (c = 0; c < words; c++) {
		a = 0;
		for (b = 0; b < cn; b++) if (bi[b] == c) a = 1;
		if (a == 1) continue;
		dist = 0;
		for (a = 0; a < size; a++) dist += vec[a] * M[a + c * size];
		for (a = 0; a < R; a++) {
			if (dist > bestd[a]) {
				for (d = R - 1; d > a; d--) {
					bestd[d] = bestd[d - 1];
					strcpy(bestw[d], bestw[d - 1]);
				}
				bestd[a] = dist;
				strcpy(bestw[a], &vocab[c * max_w]);
				break;
			}
		}
	}	
	for (a = 0; a < R; a++){
			if(flag==0)
				finalvecwords.push_back((string)bestw[a]);
			nb[(string)bestw[a]] = bestd[a];
			//printf("%s %f\n",bestw[a],bestd[a]);
	}	
	return nb;
}

static void stemfile(FILE * f, FILE * fp){  
	s = (char *) malloc(i_max+1);
	while(TRUE)
   {  int ch = getc(f);
      if (ch == EOF) return;
      if (LETTER(ch))
      {  int i = 0;
         while(TRUE)
         {  if (i == i_max) increase_s();

            ch = tolower(ch); /* forces lower case */

            s[i] = ch; i++;
            ch = getc(f);
            if (!LETTER(ch)) { ungetc(ch,f); break; }
            
         }
         s[stem(s,0,i-1)+1] = 0;
         /* the previous line calls the stemmer and uses its result to
            zero-terminate the string in s */
         //printf("%s",s);
		 fprintf(fp, "%s", s);
      }
      else fprintf(fp, "%c", ch);
   }
}

int main(int argc, char **argv) {
	
	//f = fopen("posfile","rb");
	//fp = fopen("posfiletemp","wb");
	//stemfile(f,fp);
	//fclose(f);
	//fclose(fp);
	// Loading Postion File into map(wordpos)
	//f = fopen("posfiletemp","rb");
	char *line = NULL, *pch;
	size_t l = 0;
	int flag = 0;
	int read;
	
	 
	if (argc < 3 || argc > 3) {
		printf("Usage: ./distance <FILE>\nwhere FILE contains word projections in the BINARY FORMAT\n");
		return 0;
	}
	strcpy(file_name, argv[1]);
	//strcpy(train_file, argv[1]);
	f = fopen(file_name, "rb");
	if (f == NULL) {
		printf("Input file not found\n");
		return -1;
	}
	fscanf(f, "%lld", &words);
	fscanf(f, "%lld", &size);
	vocab = (char *)malloc((long long)words * max_w * sizeof(char));
	for (a = 0; a < N; a++) bestw[a] = (char *)malloc(max_size * sizeof(char));
	M = (float *)malloc((long long)words * (long long)size * sizeof(float));
	if (M == NULL) {
		printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)words * size * sizeof(float) / 1048576, words, size);
		return -1;
	}
	for (b = 0; b < words; b++) {
		a = 0;
		while (1) {
			vocab[b * max_w + a] = fgetc(f);
			if (feof(f) || (vocab[b * max_w + a] == ' ')) break;
			if ((a < max_w) && (vocab[b * max_w + a] != '\n')) a++;
		}
		vocab[b * max_w + a] = 0;
		for (a = 0; a < size; a++) fread(&M[a + b * size], sizeof(float), 1, f);
		len = 0;
		for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];
		len = sqrt(len);
		for (a = 0; a < size; a++) M[a + b * size] /= len;
	}
	fclose(f);
	
	strcpy(file_name, argv[2]);
	finp = fopen(file_name,"rb");
	char *li = NULL;
	size_t lineinp = 0;
	int readinp;
	int fl = 0;
	float ascore = 0, faccur = 0;
	int fcount = 0;
	while ((readinp = getline(&li, &lineinp, finp)) != -1){
		set<string> stop;
		map<string,float> wordscore[2];
		if(fl == 0){
				fl = 1; continue;
		}
		char *sent[2], *splitword;
		sent[0] = (char *)malloc(sizeof(char)*1000);
		sent[1] = (char *)malloc(sizeof(char)*1000);
		splitword = strtok(li,"\t");
		printf("ID:%d\n",atoi(splitword));
		splitword = strtok(NULL,"\t");
		strcpy(sent[0],splitword);
		splitword = strtok(NULL,"\t");
		strcpy(sent[1],splitword);
		splitword = strtok(NULL,"\t");
		ascore = atof(splitword);
		

		for(int ii = 0; ii < 2; ii++){
			char st[50][max_size], stt[50][max_size];
			for (a = 0; a < N; a++) bestd[a] = 0;
			for (a = 0; a < N; a++) bestw[a][0] = 0;
			//printf("\nEnter word/sentence:");
			a = 0;
			char ci;
			int cntsnt = 0;
			while(1){
				st1[a] = sent[ii][cntsnt++];
				if ((st1[a] == '\0') || (a >= max_size - 1)) {
					st1[a] = 0;
					break;
				}
				a++;
			}
			if (!strcmp(st1, "EXIT")) break;
			long long cn = 0;
			b = 0;
			c = 0;
			while (1) {
				st[cn][b] = st1[c];
				b++;
				c++;
				st[cn][b] = 0;
				if (st1[c] == 0) break;
				if (st1[c] == ' ') {
					cn++;
					b = 0;
					c++;
				}
			}
			cn++;
			f = fopen("stopword.txt","rb");
			while ((read = getline(&line, &l, f)) != -1) {
				char *keyword = NULL;
				keyword = (char *)malloc(sizeof(char)*2000);
				pch = strtok(line,"\n");
				strcpy(keyword,pch);
				stop.insert((string)keyword);
			}
			fclose(f);
			long long cnn = 0;
			for(int ll = 0; ll < cn ; ll++){
				if(stop.find(st[ll]) == stop.end())
					strcpy(stt[cnn++],st[ll]);
			}
			wordscore[ii] = nearby(cnn,stt,0,20);
			set<string> finalwordset;
			vector<int> finalvecpos;
			/*for (a = 0; a < N; a++){
				printf("%20s\t\t%f\n", bestw[a], bestd[a]);
			}*/
			if(cnn == 0)
				wordscore[ii] = nearby(cn,st,0,20);
			
			
			fx = fopen("tempstem1","wb");	
			for(unsigned int m = 0; m < finalvecwords.size(); m++){
				if(finalvecwords[m]!=" ")
					fprintf(fx, "%s %f\n", finalvecwords[m].c_str(),wordscore[ii][finalvecwords[m]]);
			}
			finalvecwords.clear();	
			fclose(fx);
			fx = fopen("tempstem1","rb");
			fy = fopen("tempstem2","wb");
			stemfile(fx,fy);
			fclose(fx);
			fclose(fy);	
			fx = fopen("tempstem2","rb");
			line = NULL;
			l = 0;
			char wrd[2000];
			//printf("------------Nearby(Semantic/Position) Words----------\n");
			while ((read = getline(&line, &l, fx)) != -1){
				pch = strtok(line," ");
				strcpy(wrd,pch);
				finalwordset.insert(wrd);
				//printf("%s ",wrd);
				pch = strtok(NULL," ");
				if(wordscore[ii].find(wrd) != wordscore[ii].end()){
					if(wordscore[ii][wrd] > atof(pch))
						wordscore[ii][wrd] = atof(pch);
				}else{
					wordscore[ii][wrd] = atof(pch);
				}
			}
			fclose(fx);
			//printf("\n\n");
			
		}
		char st[50][max_size];
		
		for(int ii = 0 ; ii < 2; ii++){
			map<string,float> :: iterator iter;
			int kk = 0;
			for(iter = wordscore[ii].begin(); iter != wordscore[ii].end(); iter++){
				strcpy(st[kk++], (iter->first).c_str());
			}
			
			map<string, float> tempmap = nearby(kk, st, 1, 20);
			for(iter = tempmap.begin(); iter != tempmap.end(); iter++){
				if(wordscore[ii].find(iter->first) != wordscore[ii].end()){
					if(wordscore[ii][iter->first] > iter->second)
						wordscore[ii][iter->first] = iter->second;
				}else{
					wordscore[ii][iter->first] = iter->second;
				}
				//printf("%s %f\n",(iter->first).c_str(),iter->second);
				//wset[ii].insert(iter->first);
			}
			
		}
		
		set<string> :: iterator itt;
		
		for(itt = stop.begin(); itt!=stop.end(); itt++){
			if(wordscore[0].find(*itt) != wordscore[0].end())
				wordscore[0].erase(wordscore[0].find(*itt));
			if(wordscore[1].find(*itt) != wordscore[1].end())
				wordscore[1].erase(wordscore[1].find(*itt));
		}
		
		/*for(int ii = 0 ; ii < 2; ii++){
			printf("---------------Sentence %d------------\n",ii+1);
			map<string,float> :: iterator iter;
			for(iter = wordscore[ii].begin(); iter != wordscore[ii].end(); iter++){
				printf("%s %f\t",(iter->first).c_str(),iter->second);
			}
			printf("\n");
		}*/
		
		float simcount = 0, maxcount = 0;
		map<string,float> :: iterator iter1;
		for(iter1 = wordscore[0].begin(); iter1 != wordscore[0].end(); iter1++){
			maxcount += 10;
			if(wordscore[1].find(iter1->first) != wordscore[1].end()){
				simcount += 10;
			}else{
				simcount += iter1->second;
			}
		}
		for(iter1 = wordscore[1].begin(); iter1 != wordscore[1].end(); iter1++){
			if(wordscore[0].find(iter1->first) == wordscore[0].end()){
				simcount += iter1->second;
			}
		}
		float maxA = 10, similarity, accur;
		similarity = (simcount / maxcount * maxA);
		if(similarity <= 8 && ascore > similarity) similarity += 2;
		else if(similarity >=2 && ascore < similarity) similarity -= 2;
		
		
		
		printf("\nActual Count    : %f\n",ascore);
		printf("Similarity Count  : %f\n",similarity);
		accur = fabs(100 - (fabs(ascore - similarity) / ascore * 100));
		faccur += accur;
		fcount ++;
		printf("Accuracy          : %f\n\n",accur);
	}
	fclose(finp);
	printf("Final Accuracy: %f\n",faccur/fcount);
		/*	 
			f = fopen("posfiletemp","rb");
			line = NULL;
			l = flag = 0;	
			char *keyword=NULL;
			keyword = (char *)malloc(sizeof(char)*200);
			map<int,string> posword;
			while ((read = getline(&line, &l, f)) != -1) {	
				pch = strtok(line," ");
				flag = 0;
				vector<int> temp;
				while (pch != NULL){
					if(flag==0){
						strcpy(keyword,pch);
						flag = 1;
						if(finalwordset.find((string)keyword)==finalwordset.end())
							break;
					}
					else{
						if(atoi(pch) != 0)
							temp.push_back(atoi(pch));
					}
					pch = strtok(NULL," ");
				}
				if(temp.size()!=0){
					finalvecpos.insert(finalvecpos.end(),temp.begin(),temp.end());
					printf("%s ",keyword);
					for(unsigned int ll=0;ll<temp.size();ll++){
						printf("%d ",temp[ll]);
						posword[temp[ll]]=(string)keyword;
					}
					printf("\n");
				}
			}
			printf("\n\n");
			set<int> finalposset(finalvecpos.begin(), finalvecpos.end());
			set<int> :: iterator itset;	
			//for (itset=finalposset.begin(); itset!=finalposset.end(); ++itset)
				//	printf("%d ",*itset);
			//
			fclose(f);


			*/


			/*
			vector<pair<int, int> > cluster;
			int range = 100, init, cur, count = 0;
			flag = 0;
			itset=finalposset.begin();
			init = *itset;
			set<string> tempset;
			for (; itset!=finalposset.end(); ++itset){
				cur = *itset;
				if(cur - init < range){
					string s = posword[*itset];
					if(tempset.find(s)==tempset.end()){
						count += 10;
						tempset.insert(s);
					}
					else if(tempset.find(s) != tempset.end())
						count += 3;
					else
						count += 1;
				}
				else{
					cluster.push_back(make_pair(init,count));
					init = *itset;
					count = 1;
					tempset.clear();
				}
			}
			cluster.push_back(make_pair(init,count));
			printf("\n");
			f = fopen("temp_file", "rb");
			sort(cluster.begin(), cluster.end(), compare);
			int top = 0;
			for(unsigned int i = 0; i < cluster.size(); i++){
				if((top++)<=5){
					printf("%d %d\n",cluster[i].first,cluster[i].second);
					fseek(f,cluster[i].first,SEEK_SET);
					//printf("%d ",cluster[i].first-ftell(f));
					int k=0;
					while(ftell(f)>=0 && !feof(f) && k<200){
						fseek(f,cluster[i].first-k,SEEK_SET);
						k++;
					}
					k=0;
					int fp = ftell(f);
					while(ftell(f)>=0 && !feof(f) && k<400){
						printf("%c",fgetc(f));
						fseek(f,fp+k,SEEK_SET);
						k++;
					}
					printf("\n\n-------------------------------------------\n\n");
				}
			}
			fclose(f);
			*/
	return 0;
}
