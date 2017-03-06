#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <ctime>
#include <cmath>



using namespace cv;
using namespace std;

/* functions */
void menu(void);
static void onTrackbar_tunning(int, void*);
void meanHSV(Vec3i circle);
void colorRange(void);
void xyPosition(int x_pixel, int r_pixel, int x_center);

/* variables */
Mat frame, gray_img, blured_img, edge_img, hsv_img;
int medianBlurstep = 5, cannyStep = 50, HC_maxr = 50, HC_minr = 49, HC_accthreshold = 50, HC_dist = 250, save = 0; // used in tunnig
int avrg_H, avrg_S, avrg_V; // used in color detection
double x_cm, y_cm;
const int r_cm = 10; // radius of the object in cm
char text_color[10];
Scalar circle_color;
vector<Vec3f> circles;

int main(int argc, char* argv[])
{
	VideoCapture cap(1); // open the video camera no. 0
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
				
				imshow("Original Image", frame);

				/* Convert original RGB image to HSV image */
				cvtColor(frame, hsv_img, COLOR_BGR2HSV);

				/* Separation of HSV image into H, S, V planes */
				vector<Mat> hsv_planes;
				split(hsv_img, hsv_planes);
				Mat s = hsv_planes[1];

				/* Choosing Saturation plane as grayscale image for next steps */
				gray_img = s;

				/* Apply median blur */
				medianBlur(gray_img, blured_img, medianBlurstep * 2 - 1);
				
				/* Detect circles using Hough transform */
				HoughCircles(blured_img, circles, HOUGH_GRADIENT, 2, HC_dist, 200, HC_accthreshold, HC_minr, HC_maxr);

				for (size_t i = 0; i < circles.size(); i++)
				{
					Vec3i c = circles[i];

					/* If circle is inside of the image */
					if (c[1] + c[2] <= frame.rows && c[1] - c[2] >= 0 &&
						c[0] + c[2] <= frame.cols && c[0] - c[2] >= 0)
					{
						/* Compute mean HSV values */
						meanHSV(c);

						/* Define RGB color based on HSV values */
						colorRange();

						/* Calculate x, y coordinates of the object in real units */
						xyPosition(c[0], c[2], frame.cols / 2);

						char text_xy[25];

						sprintf(text_xy, "X=%4.2lf cm, Y=%4.2lf cm", x_cm, y_cm);

						/* Show result in original image*/
						circle(frame, Point(c[0], c[1]), c[2], circle_color, 3, LINE_AA);
						circle(frame, Point(c[0], c[1]), 2, Scalar(255, 0, 0), 3, LINE_AA);
						putText(frame, text_color, Point(c[0], c[1] - 20), FONT_HERSHEY_COMPLEX, .4, Scalar(0, 0, 0));
						putText(frame, text_xy, Point(c[0] - c[2] + 5, c[1] + 18), FONT_HERSHEY_COMPLEX, .4, Scalar(0, 0, 0));
					}
				}

				imshow("Original Image", frame);

				if (waitKey(50) == 27) break;

			}
			break;

/*==============================================================================================================
=================================================== Detection ==================================================
================================================================================================================*/
		case 4:
			save = 0;
			i = 0;
			cout << "Zadaj nazov suboru " << endl;
			cin >> nazov;
			namedWindow("Original Image", CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
			namedWindow("Tunning", CV_WINDOW_AUTOSIZE);

			createTrackbar("Median", "Tunning", &medianBlurstep, 20, onTrackbar_tunning);
			createTrackbar("Canny", "Tunning", &cannyStep, 100, onTrackbar_tunning);
			createTrackbar("HC_dist", "Tunning", &HC_dist, 1080, onTrackbar_tunning);
			createTrackbar("HC_ACC", "Tunning", &HC_accthreshold, 100, onTrackbar_tunning);
			createTrackbar("HC_maxr", "Tunning", &HC_maxr, 1000, onTrackbar_tunning);
			createTrackbar("HC_minr", "Tunning", &HC_minr, 1000, onTrackbar_tunning);
			createTrackbar("SAVE", "Tunning", &save, 1, onTrackbar_tunning);
			

			while (1)
			{

				sprintf(cesta, "../Images/%s_%d.bmp",nazov,615);
				frame = imread(cesta, CV_LOAD_IMAGE_COLOR);
				if (!frame.data)
				{
					cout << "End of Replay mode" << endl;
					destroyWindow("Original Image");
					break;
				}
				
				imshow("Original Image", frame);

				/* Convert original RGB image to HSV image */
				cvtColor(frame, hsv_img, COLOR_BGR2HSV);

				/* Separation of HSV image into H, S, V planes */
				vector<Mat> hsv_planes;
				split(hsv_img, hsv_planes);
				Mat s = hsv_planes[1];
				
				/* Choosing Saturation plane as grayscale image for next steps */
				gray_img = s;
				
				/* Apply median blur */
				medianBlur(gray_img, blured_img, medianBlurstep * 2 - 1);
				imshow("Median Blur", blured_img);

				/* Detect circles using Hough transform */
				HoughCircles(blured_img, circles, HOUGH_GRADIENT, 2, HC_dist , 200, HC_accthreshold , HC_minr, HC_maxr);
				
				for (size_t i = 0; i < circles.size(); i++)
				{
					Vec3i c = circles[i];
					
					/* If circle is inside of the image */
					if (c[1] + c[2] <= frame.rows && c[1] - c[2] >= 0 &&
						c[0] + c[2] <= frame.cols && c[0] - c[2] >= 0)
					{
						/* Compute mean HSV values */
						meanHSV(c);

						/* Define RGB color based on HSV values */
						colorRange();

						/* Calculate x, y coordinates of the object in real units */
						xyPosition(c[0], c[2], frame.cols / 2);

						char text_hsv[25];
						char text_xy[25];

						//sprintf(text_hsv, "H=%d S=%d V=%d", avrg_H, avrg_S, avrg_V);
						sprintf(text_hsv, "r = %d", c[0] - frame.cols / 2);
						sprintf(text_xy, "X=%4.2lf cm, Y=%4.2lf cm", x_cm, y_cm);

						/* Show result in original image*/
						circle(frame, Point(c[0], c[1]), c[2], circle_color, 3, LINE_AA);
						circle(frame, Point(c[0], c[1]), 2, Scalar(255, 0, 0), 3, LINE_AA);
						putText(frame, text_color, Point(c[0], c[1] - 20), FONT_HERSHEY_COMPLEX, .4, Scalar(0, 0, 0));
						putText(frame, text_hsv, Point(c[0] - c[2] + 5, c[1] - 2), FONT_HERSHEY_COMPLEX, .4, Scalar(0, 0, 0));
						putText(frame, text_xy, Point(c[0] - c[2] + 5, c[1] + 18), FONT_HERSHEY_COMPLEX, .4, Scalar(0, 0, 0));
					}
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

/* Trackbar function */
static void onTrackbar_tunning(int, void*)
{
	if (medianBlurstep == 0) medianBlurstep = 1;
	if (cannyStep == 0) cannyStep = 1;
	if (HC_dist < 1) HC_dist = 1;
	if (HC_maxr < 2) HC_maxr = 2;
	if (HC_minr == 0) HC_minr = 1;
	if (HC_accthreshold == 0) HC_accthreshold = 1;

}

/* Function is used to compute mean HSV values inside detected circle */
void meanHSV(Vec3i circle)
{
	
	int a = circle[2]/sqrt(2);
	Mat roi = hsv_img(Range(circle[1] - a, circle[1] + a), Range(circle[0] - a, circle[0] + a));
	Mat1b mask(roi.rows, roi.cols);
	Scalar mean_hsv = mean(roi, mask);

	avrg_H = (int)mean_hsv[0];
	avrg_S = (int)mean_hsv[1];
	avrg_V = (int)mean_hsv[2];

}

/* Function defines RGB color from HSV ranges */
void colorRange(void)
{
	if (avrg_V <= 30)
	{
		circle_color = Scalar(0, 0, 0);
		sprintf(text_color, "Black");
	}
	else if (avrg_V >= 220 && avrg_S < 40)
	{
		circle_color = Scalar(255, 255, 255);
		sprintf(text_color, "White");
	}
	else if ((avrg_V >= 40 && avrg_S >= 40 && avrg_H >= 150) ||
		(avrg_V >= 100 && avrg_S >= 50 && avrg_H < 22))
	{
		circle_color = Scalar(0, 0, 255);
		sprintf(text_color, "Red");
	}
	else if (avrg_V >= 40 && avrg_S >= 40 && avrg_H >= 22 && avrg_H < 40)
	{
		circle_color = Scalar(0, 255, 255);
		sprintf(text_color, "Yellow");
	}
	else if (avrg_V >= 40 && avrg_S >= 40 && avrg_H >= 40 && avrg_H < 90)
	{
		circle_color = Scalar(0, 255, 0);
		sprintf(text_color, "Green");
	}
	else if (avrg_V >= 40 && avrg_S >= 40 && avrg_H >= 90 && avrg_H < 150)
	{
		circle_color = Scalar(255, 0, 0);
		sprintf(text_color, "Blue");
	}
	else if (avrg_V < 220 && avrg_S < 40)
	{
		circle_color = Scalar(128, 128, 128);
		sprintf(text_color, "Gray");
	}
}

/* Function is used to calculate x, y coordinates of the object in real world units [cm] */
void xyPosition(int x_pixel, int r_pixel, int x_center)
{
	double p[] = { -0.00037519,0.090693, -7.8964,291.98 };

	y_cm = p[0]*pow( r_pixel,3) + p[1]*pow(r_pixel,2)+ p[2]* r_pixel+ p[3];

	// minimum distance between object and camera to compute "x_cm" is 40 cm 
	if (y_cm > 40.0)
	{
		double px[] = { -0.00012175 , 0.0012297, 0.3755 };
		// dost nepresne, treba nove meranie
		x_cm = px[0] * pow((x_pixel - x_center), 2) + px[1]* y_cm * abs(x_pixel - x_center) + px[2];
	}
}