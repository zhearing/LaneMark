#include <iostream>
#include<fstream>
#include <time.h>
#include "stdio.h"
#include "math.h"
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>

using namespace std;
using namespace cv;

// Global variables
Mat img_original, img_drawing;
Point quad[4];
//Point quad_2[4];
int quadout[4];
//the value of pointNum is between 0~4
static int pointNum = 0;
//the  value of LineBegin_y, LineEnd_y
static double linebegin_y = 100;
static double lineend_y = 320;
//0 present not drawing
bool drawing_line = 0;
/*************************************************
// Method: help
// Description: describe the usage
// Author: Zeyu Zhong
// Date: 2017/07/10
// Returns: void
// History:
*************************************************/
static void help()
{
	cout << "This program designed for labeling video \n"
		"Only if you press the 'n' the present quadrilateral data will be written into txt file\n";


	cout << "Hot keys: \n"
		"\tESC - quit the program\n"
		"\tn - next frame of the video\n"
		"\tz - undo the last label point \n"
		"\tc - clear all the labels\n"
		<< endl;
}
/*************************************************
// Method: drawQuadri
// Description:
// Author: Zeyu Zhong
// Date: 2017/07/10
// Returns: void
// Parameter: quad the point of Point array
// History:
*************************************************/
static void drawQuadri(Point * quad) {
	//for (int i = 0; i < 4; i++)
	//{
	//	line(img_drawing, quad[i], quad[(i + 1)], Scalar(0, 255, 0), 1, 8, 0);
	//	line(img_drawing, quad[i], quad[(i + 1) % 4], Scalar(0, 255, 0), 1, 8, 0);
	//}
	line(img_drawing, quad[0], quad[1], Scalar(0, 255, 0), 2, 8, 0);
	line(img_drawing, quad[2], quad[3], Scalar(0, 255, 0), 2, 8, 0);
}

/*************************************************
 //Method: onMouse
 //Description: do the actions after onMouse event is called
 // Author: Zeyu Zhong
 // Date: 2017/07/10
 //Returns: void
 //Parameter: event
 //Parameter: x Mouse's coordinate
 //Parameter: y
 //History:
*************************************************/
static void onMouse(int event, int x, int y, int, void*)
{


	switch (event)
	{
	case CV_EVENT_LBUTTONDOWN:
		if (pointNum == 0 || pointNum == 2)
		{
			drawing_line = true;
		}
		else if (pointNum == 1 || pointNum == 3)
		{
			drawing_line = false;
		}
		quad[pointNum % 4].x = x;
		quad[pointNum % 4].y = y;
		cout << "x = " << x << " y = " << y << endl;
		pointNum++;
		

		break;
	case CV_EVENT_LBUTTONUP:
		//finish drawing the rect (use color green for finish)

		circle(img_drawing, cvPoint(x, y), 1, Scalar(0, 255, 0), 2, 8, 0);

		if (pointNum == 4)
		{
			pointNum = 0;

			cout << "draw quadri line" << endl;
			drawQuadri(quad);
		}

		break;
	case CV_EVENT_MOUSEMOVE:
		if (drawing_line)
		{
			if (pointNum == 1)
			{
				img_original.copyTo(img_drawing);
				circle(img_drawing, quad[0], 1, Scalar(0, 255, 0), 2, 8, 0);
				line(img_drawing, quad[0], cvPoint(x, y), Scalar(0, 255, 0), 2, 8, 0);
			}
			else if (pointNum == 3)
			{
				img_original.copyTo(img_drawing);
				circle(img_drawing, quad[0], 1, Scalar(0, 255, 0), 2, 8, 0);
				line(img_drawing, quad[0], quad[1], Scalar(0, 255, 0), 2, 8, 0);
				circle(img_drawing, quad[1], 1, Scalar(0, 255, 0), 2, 8, 0);
				circle(img_drawing, quad[2], 1, Scalar(0, 255, 0), 2, 8, 0);
				line(img_drawing, quad[2], cvPoint(x, y), Scalar(0, 255, 0), 2, 8, 0);
			}
		}

		break;
	}

	imshow("Video", img_drawing);
	return;
}

/*************************************************
// Method: isempty
// Description: check the quad is empty
// Author: Zeyu Zhong
// Date: 2017/07/10
// Returns: int
// Parameter: quad
// History:
*************************************************/
int isempty(Point * quad)
{
	for (int i = 0; i < 4; i++)
	{
		if (quad[i].x != 0 || quad[i].y != 0)
		{
			return 0;
		}
	}
	return 1;
}
int main(){
	namedWindow("Video");
	ofstream outfile("1.txt");
	help();
	VideoCapture capture("1.avi");
	VideoWriter video;
	video.open("out.avi", CV_FOURCC('X', 'V', 'I', 'D'),
		capture.get(CV_CAP_PROP_FPS), cvSize(capture.get(CV_CAP_PROP_FRAME_WIDTH),
		capture.get(CV_CAP_PROP_FRAME_HEIGHT)));
	
	capture >> img_original;
	img_original.copyTo(img_drawing);
	imshow("Video", img_original);
	setMouseCallback("Video", onMouse, 0);
	int frame_counter = 0;
	while (1){
		int c = waitKey(0);
		if ((c & 255) == 27)
		{
			cout << "Exiting ...\n";
			break;
		}
		quadout[0] = int(double(linebegin_y - (quad[1].x * quad[0].y - quad[0].x * quad[1].y) / double(quad[1].x - quad[0].x)) / (double(quad[1].y - quad[0].y) / double(quad[1].x - quad[0].x)));
		quadout[1] = int(double(lineend_y - (quad[1].x * quad[0].y - quad[0].x * quad[1].y) / double(quad[1].x - quad[0].x)) / (double(quad[1].y - quad[0].y) / double(quad[1].x - quad[0].x)));
		quadout[2] = int(double(linebegin_y - (quad[3].x * quad[2].y - quad[2].x * quad[3].y) / double(quad[3].x - quad[2].x)) / (double(quad[3].y - quad[2].y) / double(quad[3].x - quad[2].x)));
		quadout[3] = int(double(lineend_y - (quad[3].x * quad[2].y - quad[2].x * quad[3].y) / double(quad[3].x - quad[2].x)) / (double(quad[3].y - quad[2].y) / double(quad[3].x - quad[2].x)));	
		quad[0].x = quadout[0], quad[0].y = int(linebegin_y);
		quad[1].x = quadout[1], quad[1].y = int(lineend_y);
		quad[2].x = quadout[2], quad[2].y = int(linebegin_y);
		quad[3].x = quadout[3], quad[3].y = int(lineend_y);

		switch ((char)c)
		{
		case 'n':
			//read the next frame
			++frame_counter;
			capture >> img_original;
			if (img_original.empty()){
				cout << "\nVideo Finished!" << endl;
				cvDestroyWindow("Video");
				return 0;
			}

			img_original.copyTo(img_drawing);

			if (!isempty(quad))
			{
				drawQuadri(quad);
				//memset(quad, 0, 4 * sizeof(Point));

				outfile << frame_counter << " " << quadout[0] << " " << linebegin_y << " "
					<< quadout[1] << " " << lineend_y << " "
					<< quadout[2] << " " << linebegin_y << " "
					<< quadout[3] << " " << lineend_y << " " << endl;

				video << img_drawing;
				
				//line(img_drawing, quad_2[0], quad_2[1], Scalar(0, 255, 0), 1, 8, 0);
				//line(img_drawing, quad_2[2], quad_2[3], Scalar(0, 255, 0), 1, 8, 0);

			//outfile << frame_counter << " " << quad[0].x << " " << quad[0].y << " "
			//		<< quad[1].x << " " << quad[1].y << " "
			//		<< quad[2].x << " " << quad[2].y << " "
			//		<< quad[3].x << " " << quad[3].y << " " << endl;
				
			}

			break;
		case 'z':
			//undo the latest labeling point
			if (pointNum == 0)
			{
				cout << "if you want to clear the existent quad please press 'c'" << endl;
				break;
			}
			pointNum--;
			quad[pointNum].x = 0;
			quad[pointNum].y = 0;
			img_original.copyTo(img_drawing);
			for (int i = 0; i < pointNum; i++)
			{
				circle(img_drawing, quad[i], 1, Scalar(0, 255, 0), 2, 8, 0);
				if (pointNum == 2)
					line(img_drawing, quad[0], quad[1], Scalar(0, 255, 0), 2, 8, 0);
			}

			break;
		case 'c':
			//clear quad array
			memset(quad, 0, 4 * sizeof(Point));
			img_original.copyTo(img_drawing);
		}
		imshow("Video", img_drawing);
	}
	//waitKey(0);
	return 0;
}
