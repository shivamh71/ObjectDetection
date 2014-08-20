#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "cv.h"
#include "highgui.h"
#include <cmath>
#define DIFF 100

using namespace cv;
using namespace std;

typedef Point3_<int> Point3i;
Mat matImg( 400 , 400 , CV_8UC3);
Vec3b value;
int minh ;
int mins ;
int minv ;
int maxh ;
int maxs ;
int maxv;
int h,s,v;
int diff;
void recurse(int y , int x, Vec3b & min , Vec3b & max , Vec3b prev){
	// cout<<x<<":"<<y<<":"<<matImg.cols<<":"<<matImg.rows<<endl;
	// cout<<"A"<<endl;
	
	if(y < matImg.rows && y > 0 && x < matImg.cols && x > 0){
		value = matImg.at<Vec3b>(y , x);
		if(value.val[0] == 0 || value.val[1] == 0 || value.val[2] == 0)	return;
		matImg.at<Vec3b>(y , x) = Vec3b(0,0,0);
	}
	else
		return;

	h = (int)value.val[0];
	s = (int)value.val[1];
	v = (int)value.val[2];
	// cout<<h<<":"<<s<<":"<<v<<endl;
	minh = (int)min.val[0];
	mins = (int)min.val[1];
	minv = (int)min.val[2];
	maxh = (int)max.val[0];
	maxs = (int)max.val[1];
	maxv = (int)max.val[2];
	diff = (h-(int)prev.val[0])*(h-(int)prev.val[0])+(s-(int)prev.val[1])*(s-(int)prev.val[1])+(v-(int)prev.val[2])*(v-(int)prev.val[2]);
	// cout<<"b"<<endl;
	if(diff <= DIFF){
		if(h < minh){
			min.val[0] = h;
		}
		else if(h > maxh){
			max.val[0] = h;
		}

		if(s < mins){
			min.val[1] = s;
		}
		else if(s > maxs){
			max.val[1] = s;
		}

		if(v < minv){
			min.val[2] = v;
		}
		else if(v > maxv){
			max.val[2] = v;
		}
		prev = value;
		// cout<<"C"<<endl;
		recurse( y+1 , x , min , max , prev);
		recurse( y , x+1 , min , max , prev);
		recurse( y-1 , x , min , max , prev);
		recurse( y , x-1 , min , max , prev);
		// cout<<"D"<<endl;
	}
	return;

}

void findObject(IplImage * frame ,int  y ,int  x){
	cvNamedWindow("mywindow1" , CV_WINDOW_AUTOSIZE);
	cvShowImage("mywindow1" , frame);
	cvWaitKey(0);
	cvDestroyWindow( "mywindow1" );
	Mat matImgcopy(frame);
	resize(matImgcopy , matImg , matImg.size() , 0 , 0 , INTER_CUBIC);
	cvtColor(matImg , matImg , CV_BGR2HSV);
	float fx= 400/matImgcopy.rows;
	float fy= 400/matImgcopy.cols;
	x = (x*400)/matImgcopy.cols;
	y = (y*400)/matImgcopy.rows;
	// x = (int)(fx*x);
	// y = (int)(fy*y);
	cout<<y<<":"<<x<<endl;
	Vec3b min = matImg.at<Vec3b>(y, x);
	Vec3b max = matImg.at<Vec3b>(y, x);
	Vec3b prev = matImg.at<Vec3b>(y, x);
	recurse( y , x , min , max , prev);
	cout<<(int)min.val[0]<<":"<<(int)min.val[1]<<":"<<(int)min.val[2]<<endl;
	cout<<(int)max.val[0]<<":"<<(int)max.val[1]<<":"<<(int)max.val[2]<<endl;
	IplImage frame2 = matImg;
	cvNamedWindow("mywindow" , CV_WINDOW_AUTOSIZE);
	cvShowImage("mywindow" , &frame2);
	cvWaitKey(0);
	cvDestroyWindow( "mywindow" );
}


int main(int argc , char ** argv){
	IplImage * image = cvLoadImage("image.jpg" , CV_LOAD_IMAGE_COLOR);
	/*Mat oldmat(image);
	Mat newmat( 400 , 400 , CV_8UC3);
	IplImage c = newmat;
	resize(oldmat , newmat , newmat.size() , 0 , 0 , INTER_CUBIC);*/
/*	cvNamedWindow("mywindow" , CV_WINDOW_AUTOSIZE);		
	cvShowImage("mywindow" , destination);
	cvWaitKey(0);
	cvDestroyWindow( "mywindow" );*/
	int x = 100;
	int y = 100;
	findObject(image , y , x);
	//CvCapture* capture = cvCaptureFromCAM( CV_CAP_ANY);
/*	cvNamedWindow("mywindow" , CV_WINDOW_AUTOSIZE);
	int i; 
*/	/*cout<<image->imageSize<<endl;
	cout<<image->width<<endl;
	cout<<image->height<<endl;*/
/*	uchar * data =(uchar *)image->imageData;
	for (i = 0; i < image->imageSize; i+=1){ 
		if(i%3 == 0) 
   			data[i] = 255;
   		else if(i%3 == 1)
   			data[i] = 0;
   		else
   			data[i] = 0;
   	}
	cvShowImage("mywindow" , image);
	cvWaitKey(0);
	cvDestroyWindow( "mywindow" );
*/
	return 0;
}