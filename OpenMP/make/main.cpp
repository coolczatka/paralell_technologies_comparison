#include <iostream>
#include "opencv2/opencv.hpp"
#include<omp.h> 
#include <string>

using namespace std;

void bradley(int width, int height, cv::Mat image,cv::Mat result, int MAX_W, int MAX_H);

int main( int argc, char** argv ) {
  
  if (argc != 3) {
	cout <<  "Uruchomienie programu: ./bradley 4 „ścieżka do pliku”, gdzie 4 to liczba wątków, a „ścieżka do plików” to ścieżka do obrazu" << endl ;
	return -1;
  }
  int thread_num = stoi(argv[1]);
  string file_name = argv[2];
  omp_set_num_threads(thread_num);
  
  cv::Mat image;
  image = cv::imread(file_name, CV_LOAD_IMAGE_GRAYSCALE);
  
  if(! image.data ) {
		cout <<  "Nie udało się otworzyć obrazu." << endl ;
		return -1;
  }
  //cout <<  "Oraz otworzony poprawnie" << endl ;
  
  cv::Size sz = image.size();
	int width = sz.width;
	int height = sz.height;
  //cout << "Type: " << image.type() << endl;
  //cout <<  "Width: "<< width << "  Height: "<< height << endl ;
  
  
  cv::Mat result(height, width,CV_8UC1, cv::Scalar(0));
  int offset = width / thread_num;
  int rest = width % thread_num;
  
  double start, end;
  start = omp_get_wtime();
  #pragma omp parallel for
  for (int i = 0; i < thread_num; i++){
	if (i == thread_num-1){
		bradley(offset+rest,i*offset, image, result, width, height);	
	}
	else{
		bradley(offset,i*offset, image, result, width, height);
	}
  }
  end = omp_get_wtime();
  double time = end - start;
  cout << time << endl;
  sz = result.size();
  width = sz.width;
  height = sz.height;
  //cout << "New Image" << endl;
  //cout <<  "Width: "<< width << "  Height: "<< height << endl ;
  
  
  cv::imwrite("output.jpg", result);
  //cout <<  "Oraz zapisany poprawnie" << endl ;
  
  return 0;
}


void bradley(int width, int offset, cv::Mat image, cv::Mat result, int MAX_W, int MAX_H){
	
	double sum = 0;
	/*
	for (int i = 0; i < 1; i++){
		for (int j = 0; j < height; j++){
			sum+=1;
			int pixelGrayValue = image.at<uchar>(j,i);
			cout << pixelGrayValue << endl;
		}
		cout << sum << " "  << endl;
	}*/
	
	int height = MAX_H;

	for (int i = 0; i < height; i++){
		sum = 0;
		for (int j = offset; j < offset+width; j++){
			double avg = 0;
			double sum = 0;
			int value = image.at<uchar>(i+1,j);
			if (i==0){
				//corners
				if (j==0){
					avg = (image.at<uchar>(i+1,j) + image.at<uchar>(i+1,j+1) + image.at<uchar>(i,j+1))/3;
				}
				else if (j==MAX_W-1){
					avg = (image.at<uchar>(i+1,j) + image.at<uchar>(i+1,j-1) + image.at<uchar>(i,j-1))/3;
				}
				//upper edge
				else {
					avg = (image.at<uchar>(i+1,j+1)+image.at<uchar>(i+1,j) + image.at<uchar>(i+1,j-1) + image.at<uchar>(i,j-1) + image.at<uchar>(i,j+1) )/5;
				}
			}
			else if (i==height-1){
				//corners
				if (j==0){
					avg = (image.at<uchar>(i-1,j) + image.at<uchar>(i-1,j+1) + image.at<uchar>(i,j+1))/3;
				}
				else if (j==MAX_W-1){
					avg = (image.at<uchar>(i-1,j) + image.at<uchar>(i-1,j-1) + image.at<uchar>(i,j-1))/3;
				}
				//lower edge
				else {
					avg = (image.at<uchar>(i-1,j+1)+image.at<uchar>(i-1,j) + image.at<uchar>(i-1,j-1) + image.at<uchar>(i,j-1) + image.at<uchar>(i,j+1) )/5;
				}
			}
			//left edge
			else if (j==0){
				avg = (image.at<uchar>(i-1,j+1)+image.at<uchar>(i-1,j) + image.at<uchar>(i,j+1) + image.at<uchar>(i+1,j+1) + image.at<uchar>(i+1,j) )/5;
			}
			//right
			else if (j==MAX_W-1)
			{
				avg = (image.at<uchar>(i-1,j-1)+image.at<uchar>(i-1,j) + image.at<uchar>(i,j-1) + image.at<uchar>(i+1,j-1) + image.at<uchar>(i+1,j) )/5;
			}
			//middle
			else{
				sum+= image.at<uchar>(i-1,j-1);
				sum+= image.at<uchar>(i,j-1);
				sum+= image.at<uchar>(i+1,j-1);
				sum+= image.at<uchar>(i-1,j);
				
				sum+= image.at<uchar>(i+1,j);
				sum+= image.at<uchar>(i-1,j+1);
				sum+= image.at<uchar>(i,j+1);
				sum+= image.at<uchar>(i+1,j+1);
				
				avg = sum/8;
			}
			if (value >= avg){
				result.at<uchar>(i,j) = 255;
			}
		}
	}

	return;
}
 