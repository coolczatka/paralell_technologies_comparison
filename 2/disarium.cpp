#include <iostream>
#include <mpi.h>
#include <vector>
#include <ctime>
#include <cstddef>
#include <cmath>

#define VECLENGTH 100000
#define ROOT 0

void fillVector(std::vector<long> &disarium);
std::vector<short> disariumVector(std::vector<long> &disarium);
bool isDisarium(long number);
std::vector<short> split(long number);
void saveToFile(std::vector<long> &disarium, std::vector<short> &result);
void saveToFileVectorOfVectors(std::vector<std::vector<long>> &memory, std::vector<std::vector<short>> &result);

int main( int argc, char** argv ) {
  std::vector<long> disarium;
  std::vector<short> result;

  disarium.resize(VECLENGTH);
  result.resize(VECLENGTH);
  fillVector(disarium);
  
  
  MPI_Init(&argc, &argv);
  
  int npes, myrank;
  // Pobierz rozmiar globalnego komunikatora
  MPI_Comm_size(MPI_COMM_WORLD, &npes);
  // Pobierz numer procesu w globalnym komunikatorze
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  
  std::vector<std::vector<long>> memory;
  std::vector<std::vector<short>> responses;
  if(myrank == ROOT) {
	for(int i = 1; i < npes; i++) {
		int zakres_od, zakres_do;
		int ps = floor(VECLENGTH/(npes-1));
		zakres_od = (i-1)*ps;
		zakres_do = (i != npes-1) ? i*ps-1 : i*ps+1;
		std::vector<long>::const_iterator first = disarium.begin() + zakres_od;
		std::vector<long>::const_iterator last = disarium.begin() + zakres_do;
		std::vector<long> localVector(first, last);
		memory.push_back(localVector);
		//printf("%d - %d : %d / %d \n", zakres_od, zakres_do, localVector.size(), localVector[localVector.size()-1]);
	}
	responses.resize(memory.size());
	for(int i=0; i<responses.size(); i++) {
	  responses[i].resize(memory[i].size());
	}
	
	double start = MPI_Wtime();
	for(int i=0; i<memory.size(); i++) {
		//1 - rozmiar, 0 - wektor
	  int size = memory[i].size();
	  MPI_Send(&size, 1, MPI_INT, i+1, 1, MPI_COMM_WORLD);
	  MPI_Send(&memory[i][0], memory[i].size(), MPI_LONG, i+1, 0, MPI_COMM_WORLD);
	}
	for(int i=0; i< responses.size(); i++) {
	  MPI_Recv(&responses[i][0], memory[i].size(), MPI_SHORT, i+1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	double end = MPI_Wtime();
	int a = 0;
	for(int h=0;h<responses.size();h++) {
	  for(int g=0;g<responses[h].size(); g++) {
	    result[a] = responses[h][g];
		a++;
	  }
	}
	saveToFile(disarium, result);
  }
  else {
	int size;
	std::vector<long> localVector;
	MPI_Recv(&size, 1, MPI_INT, ROOT, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	localVector.resize(size);
    MPI_Recv(&localVector[0], size, MPI_LONG, ROOT, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//printf("proces %d - ilosc %d\n", myrank, size);
	//printf("proces %d - dane %d\n", myrank, localVector.size());
	auto localResponse = disariumVector(localVector);

	MPI_Send(&localResponse[0], size, MPI_SHORT, ROOT, 2, MPI_COMM_WORLD);
  }
  
  MPI_Finalize();
  return 0;
}

void fillVector(std::vector<long> &disarium){
  for(int i=0;i<disarium.size();i++){
    long number = i+1;
    disarium[i] = number;
  }
  //printf("%f\n",end-start); 
}

std::vector<short> disariumVector(std::vector<long> &disarium) {
  std::vector<short> response;
  response.resize(disarium.size());
  
  for(int i=0;i<disarium.size();i++){
    long number = disarium[i];
	response[i] = isDisarium(number);
  }
  
  return response;
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

void saveToFileVectorOfVectors(std::vector<std::vector<long>> &memory, std::vector<std::vector<short>> &result) {
  FILE *file = fopen("outputfile", "w");

  for(int i=0;i<memory.size();i++) {
	for(int j=0;j<memory[i].size();i++) {
	  char line[50];
      sprintf(line, "%d - %d \n", memory[i][j], result[i][j]);
      fputs(line, file);
	}
  }
  fclose(file);

}
