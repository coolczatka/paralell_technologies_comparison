#include <iostream>
#include <vector>
#include <ctime>
#include <cstddef>
#include <cmath>

#define VECLENGTH 100000

__global__ void kernel(long *disarium, long *result); 
void fillVector(long *disarium);


int main(int argc, char** argv) {
  long *disarium;
  long *result;
  disarium = (long*)malloc(VECLENGTH*sizeof(long));
  result = (long*)malloc(VECLENGTH*sizeof(long));
  fillVector(disarium);
  int threads;
  if(argc > 1)
    threads = atoi(argv[1]);
  else
    threads = 1;
  
  long *dev_disarium;
  long *dev_result;
  cudaMalloc(&dev_disarium, VECLENGTH*sizeof(long));
  cudaMalloc(&dev_result, VECLENGTH*sizeof(long));
  
  cudaMemcpy(dev_disarium, disarium, VECLENGTH*sizeof(long), cudaMemcpyHostToDevice);
  
  int blocksize = ceil(VECLENGTH/threads);

  cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaEventRecord(start,0);
  
  kernel<<<blocksize, threads>>>(dev_disarium, dev_result);
  
  cudaEventRecord(stop, 0);
	cudaEventSynchronize(stop); //asynchroniczność CPU,GPU

  cudaMemcpy(result, dev_result, VECLENGTH*sizeof(long), cudaMemcpyDeviceToHost);
  
  cudaFree(dev_disarium);
  cudaFree(dev_result);

  FILE *file = fopen("out", "w");

  for(int i=0;i<VECLENGTH;i++) {
    char line[50];
    sprintf(line, "%d - %d \n", *(disarium+i), *(result+i));
    fputs(line, file);
  }
  
  fclose(file);
  free(disarium);
  free(result);

  float milliseconds = 0;
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("%f\n", milliseconds);

  return 0;
}
__global__ void kernel(long *disarium, long *result) {
  int tid = blockIdx.x * blockDim.x + threadIdx.x;
  
  if(tid < VECLENGTH)
  {
    int number = *(disarium+tid);
    long last = 0, sum = 0;
    int size = floor(__log10f(abs(number))) + 1;
    long tempNumber = number;
    int i = 0;
    while(tempNumber > 0) {
      last = tempNumber % 10;
      tempNumber = tempNumber / 10;
      sum += pow(last, size - i);
      i++;
    }
    if(sum == number) {
      *(result+tid) = 1;
    }
    else
      *(result+tid) = 0;
  }
}

void fillVector(long *disarium){
  
  for(int i=0;i<VECLENGTH;i++){
    long number = i+1;
    disarium[i] = number;
  }
  //printf("%f\n",end-start); 
}
