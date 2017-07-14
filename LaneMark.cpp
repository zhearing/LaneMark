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
Point lanePoint[4];
static int pointOutX[4];
//the value of pointNum is between 0~4
static int pointNum = 0;
//the value of lineBeginY, lineEndY could be set in advance between 0~480
static double lineBeginY = 100;
static double lineEndY = 320;
//0 present not drawing
static bool drawingLine = 0;
//0 present no lane on the road
static bool allEmptyFlag = 0;
static bool leftEmptyFlag = 0;
static bool rightEmptyFlag = 0;
//number of the marked lanes
static int leftNum = 0;
static int rightNum = 0;
enum
{
	LANE_LEFT = 1,
	LANE_RIGHT = 2,
	LANE_ALL = 3
};

/*************************************************
// Method: help
// Description: describe the usage
// Author: Zeyu Zhong
// Date: 2017/07/13
// Returns: void
// History:
*************************************************/
static void help()
{
	cout << "This program designed for labeling video \n"
		"Only if you press the 'n' the present lane lines data will be written into txt file\n";


	cout << "Hot keys: \n"
		"\tESC - quit the program\n"
		"\tn - next frame of the video\n"
		"\tz - undo the last label point\n"
		"\tc - clear all the labels\n"
		"\tl - no left lane on the road\n"
		"\tr - no right lane on the road\n"
		"\tx - no lane on the road\n\n"
		<< endl;
}

/*************************************************
// Method: drawLaneLine
// Description:
// Author: Zeyu Zhong
// Date: 2017/07/10
// Returns: void
// Parameter: connect the point of Point array
// History:
*************************************************/
static void drawLaneLine(Point * lanePoint)
{
	line(img_drawing, lanePoint[0], lanePoint[1], Scalar(0, 255, 0), 2, 8, 0);
	line(img_drawing, lanePoint[2], lanePoint[3], Scalar(0, 255, 0), 2, 8, 0);
}

/*************************************************
 //Method: onMouse
 //Description: do the actions after onMouse event is called
 // Author: Zeyu Zhong
 // Date: 2017/07/13
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
			drawingLine = true;
		else if (pointNum == 1 || pointNum == 3)
			drawingLine = false;
		lanePoint[pointNum % 4].x = x;
		lanePoint[pointNum % 4].y = y;
		//cout << "x = " << x << " y = " << y << endl;
		pointNum++;

		break;
	case CV_EVENT_LBUTTONUP:
		//finish drawing the rect (use color green for finish)
		circle(img_drawing, cvPoint(x, y), 1, Scalar(0, 255, 0), 2, 8, 0);
		if (pointNum == 4)
		{
			pointNum = 0;
			//cout << "draw lane line! " << endl;
			drawLaneLine(lanePoint);
		}

		break;
	case CV_EVENT_MOUSEMOVE:
		if (drawingLine)
		{
			if (pointNum == 1)
			{
				img_original.copyTo(img_drawing);
				circle(img_drawing, lanePoint[0], 1, Scalar(0, 255, 0), 2, 8, 0);
				line(img_drawing, lanePoint[0], cvPoint(x, y), Scalar(0, 255, 0), 2, 8, 0);
			}
			else if (pointNum == 3)
			{
				img_original.copyTo(img_drawing);
				circle(img_drawing, lanePoint[0], 1, Scalar(0, 255, 0), 2, 8, 0);
				circle(img_drawing, lanePoint[1], 1, Scalar(0, 255, 0), 2, 8, 0);
				line(img_drawing, lanePoint[0], lanePoint[1], Scalar(0, 255, 0), 2, 8, 0);
				circle(img_drawing, lanePoint[2], 1, Scalar(0, 255, 0), 2, 8, 0);
				line(img_drawing, lanePoint[2], cvPoint(x, y), Scalar(0, 255, 0), 2, 8, 0);
			}
		}

		break;
	}

	imshow("Video", img_drawing);
	return;
}

/*************************************************
// Method: isempty
// Description: check the lanePoint is empty
// Author: Zeyu Zhong
// Date: 2017/07/13
// Returns: void
// Parameter: lanePoint
// History:
*************************************************/
void isempty(Point * lanePoint)
{
	for (int i = 0; i < 2; i++)
	{
		if (lanePoint[i].x == 0 && lanePoint[i].y == 0)
		{
			leftEmptyFlag = true;
			pointNum += 2;
		}
	}
	for (int i = 2; i < 4; i++)
	{
		if (lanePoint[i].x == 0 && lanePoint[i].y == 0)
		{
			rightEmptyFlag = true;
			pointNum = 0;
		}
	}
	if (leftEmptyFlag && rightEmptyFlag)
	{
		allEmptyFlag = true;
		leftEmptyFlag = rightEmptyFlag = false;
		pointNum = 0;
	}
	return;
}

/*************************************************
// Method: calculateCoordinate
// Description: calculate the coordinates of the mark points
// Author: Zeyu Zhong
// Date: 2017/07/14
// Returns: void
// Parameter: type
// History:
*************************************************/
static void calculateCoordinate(int type)
{
	switch (type)
	{
	case LANE_LEFT:
		pointOutX[0] = int(double(lineBeginY - (lanePoint[1].x * lanePoint[0].y - lanePoint[0].x * lanePoint[1].y) / double(lanePoint[1].x - lanePoint[0].x)) / (double(lanePoint[1].y - lanePoint[0].y) / double(lanePoint[1].x - lanePoint[0].x)));
		pointOutX[1] = int(double(lineEndY - (lanePoint[1].x * lanePoint[0].y - lanePoint[0].x * lanePoint[1].y) / double(lanePoint[1].x - lanePoint[0].x)) / (double(lanePoint[1].y - lanePoint[0].y) / double(lanePoint[1].x - lanePoint[0].x)));
		lanePoint[0].x = pointOutX[0], lanePoint[0].y = int(lineBeginY);
		lanePoint[1].x = pointOutX[1], lanePoint[1].y = int(lineEndY);

		break;
	case LANE_RIGHT:
		pointOutX[2] = int(double(lineBeginY - (lanePoint[3].x * lanePoint[2].y - lanePoint[2].x * lanePoint[3].y) / double(lanePoint[3].x - lanePoint[2].x)) / (double(lanePoint[3].y - lanePoint[2].y) / double(lanePoint[3].x - lanePoint[2].x)));
		pointOutX[3] = int(double(lineEndY - (lanePoint[3].x * lanePoint[2].y - lanePoint[2].x * lanePoint[3].y) / double(lanePoint[3].x - lanePoint[2].x)) / (double(lanePoint[3].y - lanePoint[2].y) / double(lanePoint[3].x - lanePoint[2].x)));
		lanePoint[2].x = pointOutX[2], lanePoint[2].y = int(lineBeginY);
		lanePoint[3].x = pointOutX[3], lanePoint[3].y = int(lineEndY);

		break;
	case LANE_ALL:
		pointOutX[0] = int(double(lineBeginY - (lanePoint[1].x * lanePoint[0].y - lanePoint[0].x * lanePoint[1].y) / double(lanePoint[1].x - lanePoint[0].x)) / (double(lanePoint[1].y - lanePoint[0].y) / double(lanePoint[1].x - lanePoint[0].x)));
		pointOutX[1] = int(double(lineEndY - (lanePoint[1].x * lanePoint[0].y - lanePoint[0].x * lanePoint[1].y) / double(lanePoint[1].x - lanePoint[0].x)) / (double(lanePoint[1].y - lanePoint[0].y) / double(lanePoint[1].x - lanePoint[0].x)));
		lanePoint[0].x = pointOutX[0], lanePoint[0].y = int(lineBeginY);
		lanePoint[1].x = pointOutX[1], lanePoint[1].y = int(lineEndY);
		pointOutX[2] = int(double(lineBeginY - (lanePoint[3].x * lanePoint[2].y - lanePoint[2].x * lanePoint[3].y) / double(lanePoint[3].x - lanePoint[2].x)) / (double(lanePoint[3].y - lanePoint[2].y) / double(lanePoint[3].x - lanePoint[2].x)));
		pointOutX[3] = int(double(lineEndY - (lanePoint[3].x * lanePoint[2].y - lanePoint[2].x * lanePoint[3].y) / double(lanePoint[3].x - lanePoint[2].x)) / (double(lanePoint[3].y - lanePoint[2].y) / double(lanePoint[3].x - lanePoint[2].x)));
		lanePoint[2].x = pointOutX[2], lanePoint[2].y = int(lineBeginY);
		lanePoint[3].x = pointOutX[3], lanePoint[3].y = int(lineEndY);

		break;
	}
	return;
}

int main()
{
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
	int frameCounter = 0;

	while (1)
	{
		int c = waitKey(0);
		if ((c & 255) == 27)
		{
			cout << "Exiting ...\n";
			break;
		}

		switch ((char)c)
		{
		case 'n':
			//read the next frame
			++frameCounter;
			capture >> img_original;
			if (img_original.empty())
			{
				outfile << frameCounter << " " << leftNum << " " << rightNum << " " << endl;

				cout << "\nVideo Finished!" << endl;
				cvDestroyWindow("Video");
				return 0;
			}
			img_original.copyTo(img_drawing);
			isempty(lanePoint);
			if (!allEmptyFlag)
			{
				//if (frameCounter != 1)
				//{
				if (!leftEmptyFlag)
				{
					calculateCoordinate(LANE_LEFT);
					leftNum++;
				}
				if (!rightEmptyFlag)
				{
					calculateCoordinate(LANE_RIGHT);
					rightNum++;
				}
				//}
				//else if (frameCounter == 1)
				//{
				//	if (leftEmptyFlag)
				//		calculateCoordinate(LANE_RIGHT);
				//	else if (rightEmptyFlag)
				//		calculateCoordinate(LANE_LEFT);
				//	else
				//	{
				//		calculateCoordinate(LANE_ALL);
				//		leftNum++;
				//		rightNum++;
				//	}
				//}
			}
			else
				//include the first frame has no lane line
				memset(lanePoint, 0, 4 * sizeof(Point));

			outfile << frameCounter << " " << lanePoint[0].x << " " << lanePoint[0].y << " "
				<< lanePoint[1].x << " " << lanePoint[1].y << " "
				<< lanePoint[2].x << " " << lanePoint[2].y << " "
				<< lanePoint[3].x << " " << lanePoint[3].y << " " << endl;

			drawLaneLine(lanePoint);
			video << img_drawing;
			//memset(lanePoint, 0, 4 * sizeof(Point));	//Whether to clear the coordinates of the previous frame after marking	
			//clear all flags
			allEmptyFlag = 0;
			leftEmptyFlag = 0;
			rightEmptyFlag = 0;
			cout << "left mark: " << leftNum << " right mark: " << rightNum << endl;

			break;
		case 'z':
			//undo the latest labeling point
			if (pointNum == 0)
			{
				cout << "if you want to clear the existent lanePoint please press 'c'" << endl;
				break;
			}
			pointNum--;
			lanePoint[pointNum].x = 0;
			lanePoint[pointNum].y = 0;
			img_original.copyTo(img_drawing);
			for (int i = 0; i < pointNum; i++)
			{
				circle(img_drawing, lanePoint[i], 1, Scalar(0, 255, 0), 2, 8, 0);
				if (pointNum == 2)
					line(img_drawing, lanePoint[0], lanePoint[1], Scalar(0, 255, 0), 2, 8, 0);
			}

			break;
		case 'c':
			//clear lanePoint array
			memset(lanePoint, 0, 4 * sizeof(Point));
			img_original.copyTo(img_drawing);

			break;
		case 'l':
			//mark empty left lane
			for (int i = 0; i < 2; i++)
			{
				lanePoint[i].x = 0;
				lanePoint[i].y = 0;
			}
			pointNum += 2;
			leftEmptyFlag = 1;

			break;
		case 'r':
			//mark right empty lane
			for (int i = 2; i < 4; i++)
			{
				lanePoint[i].x = 0;
				lanePoint[i].y = 0;
			}
			pointNum = 0;
			rightEmptyFlag = 1;

			break;
		case 'x':
			//mark all empty lane
			memset(lanePoint, 0, 4 * sizeof(Point));
			pointNum = 0;
			allEmptyFlag = 1;

			break;
		}

		imshow("Video", img_drawing);
	}
	return 0;
}
