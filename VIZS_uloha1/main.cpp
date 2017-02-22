#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <ctime>
#include <chrono>



using namespace cv;
using namespace std;

// functions
void menu(void);
static void onTrackbar_tunning(int, void*);

// variables
Mat frame, gray_img, blured_img, edge_img;
int medianBlurstep = 5, cannyStep = 50, HC_maxr = 50, HC_minr = 49, HC_accthreshold = 50, HC_dist = 250, save = 0;
vector<Vec3f> circles;

int main(int argc, char* argv[])
{
	VideoCapture cap(0); // open the video camera no. 0
	Mat frame;
	int32_t i = 0;
	int numberOfFrames;
	char cesta[25];
	char nazov[10];
	int mode;
	double dWidth;
	double dHeight;


	while (1)
	{
		menu();
		cin >> mode;
		switch (mode)
		{
		case 1:
			if (!cap.isOpened())  // if not success, exit program
			{
				cout << "Cannot open the video cam" << endl;
				return -1;
			}

			dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
			dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

			cout << "Frame size : " << dWidth << " x " << dHeight << endl;
			cout << "Zadaj nazov suboru (max 9 znakov)" << endl;
			cin >> nazov;

			cout << "Zadaj pocet framov (-1 pre kontinualny mod)" << endl;
			cin >> numberOfFrames;
			if (numberOfFrames == -1) numberOfFrames = INT32_MAX;
			namedWindow("Video Capture", CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"

			while (i < numberOfFrames)
			{

				i++;
				if (!cap.read(frame)) //if not success, break loop
				{
					cout << "Cannot read a frame from video stream" << endl;
					break;
				}


				sprintf(cesta, "../Images/%s_%i.bmp", nazov, i);
				imwrite(cesta, frame);

				imshow("Video Capture", frame); //show the frame in "MyVideo" window

				if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
				{
					break;
				}

			}
			cout << "End of Capture mode" << endl;
			destroyWindow("Video Capture");
			break;

		case 2:
			if (!cap.isOpened())  // if not success, exit program
			{
				cout << "Cannot open the video cam" << endl;
				return -1;
			}

			dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
			dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

			cout << "Frame size : " << dWidth << " x " << dHeight << endl;
			cout << "Zadaj nazov suboru (max 9 znakov)" << endl;
			cin >> nazov;

			
			namedWindow("Video Capture", CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"

			while (1)
			{

				if (!cap.read(frame)) //if not success, break loop
				{
					cout << "Cannot read a frame from video stream" << endl;
					break;
				}
				waitKey(30);
				imshow("Video Capture", frame); //show the frame in "MyVideo" window

				if (waitKey(1) == 50) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
				{
					i++;
					sprintf(cesta, "../Images/%s_%i.bmp", nazov, i);
					imwrite(cesta, frame);
				}
				if (waitKey(9) == 27)
					break;

			}
			cout << "End of Capture mode" << endl;
			destroyWindow("Video Capture");
			break;
		case 3:
			i = 0;
			cout << "Zadaj nazov suboru " << endl;
			cin >> nazov;
			namedWindow("Video Replay", CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
			while (1)
			{
				Mat frame;
				i++;
				sprintf(cesta, "../Images/%s_%d.bmp", nazov, i);
				frame = imread(cesta, CV_LOAD_IMAGE_COLOR);
				if (!frame.data)
				{
					cout << "End of Replay mode" << endl;
					destroyWindow("Video Replay");
					break;
				}
				imshow("Video Replay", frame); //show the frame in "MyVideo" window

				if (waitKey(50) == 27) break;

			}
			break;

/*==============================================================================================================
=================================================== Detection ==================================================
================================================================================================================*/
		case 4:
			i = 0;
			cout << "Zadaj nazov suboru " << endl;
			cin >> nazov;
			namedWindow("Original Image", CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
			namedWindow("Tunning", CV_WINDOW_AUTOSIZE);

			createTrackbar("Median", "Tunning", &medianBlurstep, 10, onTrackbar_tunning);
			createTrackbar("Canny", "Tunning", &cannyStep, 100, onTrackbar_tunning);
			createTrackbar("HC_dist", "Tunning", &HC_dist, 1080, onTrackbar_tunning);
			createTrackbar("HC_ACC", "Tunning", &HC_accthreshold, 100, onTrackbar_tunning);
			createTrackbar("HC_maxr", "Tunning", &HC_maxr, 100, onTrackbar_tunning);
			createTrackbar("HC_minr", "Tunning", &HC_minr, 100, onTrackbar_tunning);
			createTrackbar("SAVE", "Tunning", &save, 1, onTrackbar_tunning);

			while (1)
			{
				
				sprintf(cesta, "../Images/%s_%d.bmp", nazov, 1);
				frame = imread(cesta, CV_LOAD_IMAGE_COLOR);
				if (!frame.data)
				{
					cout << "End of Replay mode" << endl;
					destroyWindow("Original Image");
					break;
				}
				
				imshow("Original Image", frame);
				
				// convert original RGB image to grayscale image
				cvtColor(frame, gray_img, COLOR_BGR2GRAY);

				// apply median blur
				medianBlur(gray_img, blured_img, medianBlurstep * 2 - 1);
				imshow("Median Blur", blured_img);

				// canny edge detection
				Canny(blured_img, edge_img, cannyStep, cannyStep * 3, 3);
				imshow("Canny edge", edge_img);

				// detect circles using Hough transform
				HoughCircles(edge_img, circles, HOUGH_GRADIENT, 1, HC_dist , cannyStep * 3, HC_accthreshold , HC_minr, HC_maxr);
				for (size_t i = 0; i < circles.size(); i++)
				{
					Vec3i c = circles[i];
					circle(frame, Point(c[0], c[1]), c[2], Scalar(0, 0, 255), 3, LINE_AA);
					circle(frame, Point(c[0], c[1]), 2, Scalar(0, 255, 0), 3, LINE_AA);
				}
				imshow("Original Image", frame);	
				waitKey(9);
				if (save) 
				{
					cout << "Parameters saved!" << endl;
					destroyWindow("Original Image");
					destroyWindow("Canny edge");
					destroyWindow("Median Blur");
					destroyWindow("Tunning");
					break;
				}
			}
/*==============================================================================================================
================================================ END of Detection ==============================================
================================================================================================================*/
			break;
		case 99:
			return 0;
			break;
		default:
			return -1;
			break;
		}
	}
	return -1;
}


void menu(void)
{
	char riadok[51];
	memset(riadok, 0, 51);
	memset(riadok, 0xCD, 50);
	printf("%c%s%c\n",201, riadok,187);
	printf("%c Video mode.....................................1 %c\n", 186, 186);
	printf("%c Picture mode...................................2 %c\n", 186, 186);
	printf("%c Replay mode....................................3 %c\n", 186, 186);
	printf("%c Tuning mode....................................4 %c\n", 186, 186);
	printf("%c Exit..........................................99 %c\n", 186, 186);
	printf("%c%s%c\n", 200, riadok, 188);
}


static void onTrackbar_tunning(int, void*)
{
	if (medianBlurstep == 0) medianBlurstep = 1;
	if (cannyStep == 0) cannyStep = 1;
	if (HC_dist < 1) HC_dist = 1;
	if (HC_maxr < 2) HC_maxr = 2;
	if (HC_minr == 0) HC_minr = 1;
	if (HC_accthreshold == 0) HC_accthreshold = 1;

}