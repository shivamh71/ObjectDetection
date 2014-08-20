#include "cv.h"
#include "highgui.h"
#include "stdlib.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#define DIFF 100
using namespace cv;
using namespace std;
const int MAX_NO_OBJECT = 50;
const int MIN_OBJECT_AREA = 100;
const int MAX_OBJECT_AREA = 40000;
Mat src;
Mat src_gray;
Mat matImg( 400 , 400 , CV_8UC3);
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);
Mat drawing;
bool clicked = true;
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
		if(value.val[0] == -1 && value.val[1] == -1 && value.val[2] == -1){
			cout<<"Move Out 1"<<endl;
			return;
		}	
		matImg.at<Vec3b>(y , x) = Vec3b(-1,-1,-1);
	}
	else{
		cout<<"Move Out 2"<<endl;
		return;
	}

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
	// diff = (h-(int)prev.val[0])*(h-(int)prev.val[0]);
	// cout<<"b"<<endl;	
	if(diff <= DIFF){
		cout<<"Move On"<<endl;
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
	cout<<"Move out 3"<<endl;
	return;

}

void findObject(IplImage * frame ,int  y ,int  x){
	// cvNamedWindow("mywindow1" , CV_WINDOW_AUTOSIZE);
	// cvShowImage("mywindow1" , frame);
	// cvWaitKey(0);
	// cvDestroyWindow( "mywindow1" );
	Mat matImgcopy(frame);
	resize(matImgcopy , matImg , matImg.size() , 0 , 0 , INTER_CUBIC);
	IplImage* imgHSV = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3); 
	cvCvtColor(frame , imgHSV , CV_RGB2HSV);
	cvtColor(matImg , matImg , CV_RGB2HSV);
	x = (x*400)/matImgcopy.cols;
	y = (y*400)/matImgcopy.rows;
	// x = (int)(fx*x);
	// y = (int)(fy*y);
	cout<<x<<":"<<y<<endl;
	Vec3b min = matImg.at<Vec3b>(y, x);
	Vec3b max = matImg.at<Vec3b>(y, x);
	Vec3b prev = matImg.at<Vec3b>(y, x);
	// cout<<(int)min.val[0]<<":"<<(int)min.val[1]<<":"<<(int)min.val[2]<<endl;
	// cout<<(int)max.val[0]<<":"<<(int)max.val[1]<<":"<<(int)max.val[2]<<endl;
	recurse( y , x , min , max , prev);
	cout<<(int)min.val[0]<<":"<<(int)min.val[1]<<":"<<(int)min.val[2]<<endl;
	cout<<(int)max.val[0]<<":"<<(int)max.val[1]<<":"<<(int)max.val[2]<<endl;
	IplImage frame2 = matImg;
	// cvNamedWindow("mywindow" , CV_WINDOW_AUTOSIZE);
	// cvShowImage("mywindow" , &frame2);
	// cvWaitKey(0);
	// cvDestroyWindow( "mywindow" );
	
	// IplImage* imgThresh = GetThresholdedImage(imgHSV);
	// cvNamedWindow("mywindow1" , CV_WINDOW_AUTOSIZE);
	// cvShowImage("mywindow1" , imgThresh);
	// cvWaitKey(0);
	// cvDestroyWindow( "mywindow1" );
}




void drawObject(double x,double y,Mat &frame){
	circle(frame,Point(x,y), 20,Scalar(255,0,0), 2, 8, 0 );
}
	
void thresh_callback(int, void* ){
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Canny( src_gray, canny_output, thresh, thresh*2, 3 );
	findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
	for( int i = 0; i< contours.size(); i++ ){
		Scalar color = Scalar( 255,255,255 );
		drawContours( drawing, contours, i, color, 2, 2, hierarchy, 0, Point() );
	}
	double refArea=0;

  	if(hierarchy.size() > 0){
	    int numbOfObjects = hierarchy.size();
	    if( numbOfObjects < MAX_NO_OBJECT){
	      for (int i = 0; i >= 0; i = hierarchy[i][0])
	      {
	        Moments moment = moments(contours[i],false);
	        double area = moment.m00;
	        if(area > MIN_OBJECT_AREA && area > refArea){
	          cout<<"area of object : "<<area<<endl;
	          double x = moment.m10/area;
	          double y = moment.m01/area;
	          drawObject(x,y,drawing);
	        }

	      }
	  	}
	}

}

void traceContour(IplImage* frame){
	Mat temp(frame);
	src_gray = temp;
	blur( src_gray, src_gray, Size(3,3) );
	thresh_callback(0 , 0 );
}

// Capturing a frame on mouse click
void mouse_callback(int event, int x, int y, int flags, void* param){
	IplImage *frame = (IplImage*) param;	// Captured Frame
	if (clicked){
		switch( event ){
		case CV_EVENT_LBUTTONDOWN:	// Listener for Left Mouse Click
		    printf("x = %d\ny = %d\n",x,y);
			// cvShowImage("CapturedImage",frame);	
		    if( x <= frame-> width && y <= frame->height && x >= 0&&y>=0 ){
		    	cout<<"Please"<<x<<":"<<y<<endl;
				findObject(frame, y, x);
				clicked = false;
			}
		    break;

		case CV_EVENT_RBUTTONDOWN:	// Listener for Right Mouse Click
		    printf("x = %d\ny = %d\n",x,y);
			// cvShowImage("CapturedImage",frame);	
		    if( x <= frame-> width && y <= frame->height && x >= 0&&y>=0 ){
				findObject(frame, y, x);
				clicked = false;
			}
		    break;
		}
	}	
}

// This converts HSV image to thresholded image
IplImage* GetThresholdedImage(IplImage* imgHSV){        
	IplImage* imgThresh=cvCreateImage(cvGetSize(imgHSV),IPL_DEPTH_8U, 1);
	// cout<<minh<<":"<<mins<<":"<<minv<<endl;
	// cout<<maxh<<":"<<maxs<<":"<<maxv<<endl;
	cvInRangeS(imgHSV, cvScalar(minh,mins,minv), cvScalar(maxh,maxs,maxv), imgThresh); 
	return imgThresh;
}

// This applies convolution operators on each frame (Blur, Erode, Dilate)
IplImage* convoluteFrame(IplImage* param){
	IplImage *inputImage = param;                    	
	IplImage *black_image = cvCreateImage( cvGetSize(param), IPL_DEPTH_8U, 3 );
	cvCvtColor( inputImage, black_image, CV_RGB2HSV );
	IplImage *gray = GetThresholdedImage(black_image);
	IplImage *img = cvCreateImage( cvSize(inputImage->width,inputImage->height), 8, 3 );
	img = inputImage;
	inputImage = gray;
	IplImage *outputImage;
	IplImage *outputImage2;
	outputImage = cvCreateImage( cvSize( inputImage->width, inputImage->height ), IPL_DEPTH_8U, 1 );
	outputImage2 = cvCreateImage( cvSize( inputImage->width, inputImage->height ), IPL_DEPTH_8U, 1 );
	cvErode(inputImage, outputImage, 0, 1);
	cvDilate(outputImage, outputImage2, 0, 2);
	return outputImage2;
}

int main(int argc, char** argv){
    CvCapture* capture = cvCreateFileCapture("anim.mp4");
    IplImage* frame = NULL;
    if(!capture){
        printf("Video Not Opened\n");
        return -1;
    }
    int count = 0;
    cvNamedWindow("video1");
    cvMoveWindow("video1" , 400 , 400 );
    cvNamedWindow("video2");
    while(1){
        frame = cvQueryFrame(capture);
		count++;
        if(!frame){
            printf("Capture Finished\n");
            exit(0);
            break;
        }
        cvShowImage("video1",frame);
        if (clicked) cvSetMouseCallback("video1",mouse_callback,(void*)frame);
    	frame =  convoluteFrame(frame); // This contours the frame

    	traceContour(frame); 
    	*frame = drawing;
    	cvShowImage("video2",frame);
        cvWaitKey(33);
		if (count == 40){
			//exit(0);
		}
    }
    cvReleaseCapture(&capture);
    return 0;
}
