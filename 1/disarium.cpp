#include <iostream>
#include <omp.h>
#include <vector>
#include <ctime>
#include <cstddef>
#include <cmath>
// #include <algorhitm>

#define VECLENGTH 100000

void fillVectors(std::vector<long> &disarium, std::vector<short> &result);
bool isDisarium(long number);
std::vector<short> split(long number);
void saveToFile(std::vector<long> &disarium, std::vector<short> &result);

int main( int argc, char** argv ) {
  int threads = 0;
  if(argc > 1) {
	threads = atoi(argv[1]);
        omp_set_num_threads(threads);
  }
  std::vector<long> disarium;
  std::vector<short> result;

  disarium.resize(VECLENGTH);
  result.resize(VECLENGTH);
  fillVectors(disarium, result);
  saveToFile(disarium, result);
  return 0;
}

void fillVectors(std::vector<long> &disarium, std::vector<short> &result){
  //unsigned int seed = time(NULL);
  //unsigned int* sp = &seed;
  double start, end;
  start = omp_get_wtime();
  #pragma omp parallel for
  for(int i=0;i<disarium.size();i++){
    long number = i+1;
    disarium[i] = number;
    result[i] = isDisarium(number);
  }
  end = omp_get_wtime();
  printf("%f\n",end-start); 
}

bool isDisarium(long number){
  long tempNumber = number;
  long last = 0, sum = 0;
  long degree=1;

  std::vector<short> splittedNumber = split(number);
  for(int i=splittedNumber.size()-1; i>=0; i--) {
    sum += pow(splittedNumber[i], degree++);
  }
  if(sum == number)
    return 1;
  else
    return 0;

}

//liczba odwrocona
std::vector<short> split(long number) {
  std::vector<short> result;
  long tempNumber = number;
  long last = 0, sum = 0;
  long degree=1;

  while(tempNumber > 0) {
    last = tempNumber % 10;
    tempNumber = tempNumber / 10;
    result.push_back(last);
  }
  return result;
}
void saveToFile(std::vector<long> &disarium, std::vector<short> &result) {
  FILE *file = fopen("outputfile", "w");

  for(int i=0;i<disarium.size();i++) {
    char line[50];
    sprintf(line, "%d - %d \n", disarium[i], result[i]);
    fputs(line, file);
  }
  fclose(file);

}
