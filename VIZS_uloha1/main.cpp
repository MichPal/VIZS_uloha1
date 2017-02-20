#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <ctime>
#include <chrono>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
	VideoCapture cap(1); // open the video camera no. 0

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the video cam" << endl;
		return -1;
	}

	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

	cout << "Frame size : " << dWidth << " x " << dHeight << endl;

	namedWindow("MyVideo", CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
	int i = 0;
	char cesta[25];
	
	while (1)
	{
		Mat frame;
		i++;
		bool bSuccess = cap.read(frame); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}


		sprintf(cesta, "../Images/image_%i.bmp", i);
		imwrite(cesta, frame);

		imshow("MyVideo", frame); //show the frame in "MyVideo" window

		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}

	}
	i = 0;

	while (1)
	{
		Mat frame;
		i++;
		sprintf(cesta, "../Images/image_%d.bmp", i);
		frame = imread(cesta,CV_LOAD_IMAGE_COLOR);
		if (!frame.data) break;

		imshow("MyVieo", frame); //show the frame in "MyVideo" window

		if (waitKey(30)==27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
		
	}


	return 0;

}
