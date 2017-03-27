#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <ctime>
#include <cmath>
#include <fstream>



using namespace cv;
using namespace std;

/* functions */
void menu(void);
static void onTrackbar_tunning(int, void*);
void xyPosition(double *x, double *y, int x_pixel, int r_pixel);
void generateMfile(void);

/* variables */
Mat frame, gray_img, blured_img, edge_img, hsv_img;
Mat R_img, R_img2, G_img, B_img;


int medianBlurstep = 6, cannyStep = 50, HC_maxr = 500, HC_minr = 30, HC_accthreshold = 24, HC_dist = 325, save = 0; // used in tunnig
int avrg_H, avrg_S, avrg_V; // used in color detection


vector<Vec3f> circles, Rcircles, Gcircles, Bcircles;

int main(int argc, char* argv[])
{
//	generateMfile();
//	system("pause");
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
			while (1)
			{
				Mat frame;
				i++;
				sprintf(cesta, "../Images/%s_%d.bmp", nazov, i);
				frame = imread(cesta, CV_LOAD_IMAGE_COLOR);
				if (!frame.data)
				{
					cout << "End of Replay mode" << endl;
					destroyWindow("Orginal Image");
					break;
				}
				
				imshow("Original Image", frame);
				/* Convert original RGB image to HSV image */
				cvtColor(frame, hsv_img, COLOR_BGR2HSV);


				inRange(hsv_img, Scalar(150, 100, 20), Scalar(180, 255, 255), R_img);
				inRange(hsv_img, Scalar(0, 100, 20), Scalar(20, 255, 255), R_img2);
				inRange(hsv_img, Scalar(35, 100, 20), Scalar(90, 255, 255), G_img);
				inRange(hsv_img, Scalar(90, 100, 20), Scalar(150, 255, 255), B_img);
				addWeighted(R_img,1, R_img2, 1, 0, R_img);

				GaussianBlur(R_img, R_img, Size(5, 5), 3, 3);
				GaussianBlur(G_img, G_img, Size(5, 5), 3, 3);
				GaussianBlur(B_img, B_img, Size(5, 5), 3, 3);

				//imshow("R_img", R_img);
				//imshow("G_img", G_img);
				//imshow("B_img", B_img);
	
				HoughCircles(R_img, Rcircles, HOUGH_GRADIENT, 1, 100, 50, 30, 50, 400);
				HoughCircles(G_img, Gcircles, HOUGH_GRADIENT, 1, 100, 50, 30, 50, 400);
				HoughCircles(B_img, Bcircles, HOUGH_GRADIENT, 1, 100, 50, 30, 50, 400);
				//HoughCircles(B_img, Bcircles, HOUGH_GRADIENT, 2, HC_dist, 200, HC_accthreshold, HC_minr, HC_maxr);


				for (int c = 0; c < 3; c++)
				{
					vector<Vec3f>  current_colour;
					Scalar circle_color;
					char text_color[10];

					switch (c)
					{
					case 0:
						current_colour = Rcircles;
						circle_color = Scalar(0, 0, 255);
						sprintf(text_color, "Red");
						break;
					case 1:
						current_colour = Gcircles;
						circle_color = Scalar(0, 255, 0);
						sprintf(text_color, "Green");
						break;
					case 2:
						current_colour = Bcircles;
						circle_color = Scalar(255, 0, 0);
						sprintf(text_color, "Blue");
						break;
					}

					for (size_t i = 0; i < current_colour.size(); i++)
					{
						Vec3i c = current_colour[i];
						double x_cm, y_cm;
						char text_xy[25];

						/* If circle is inside of the image */
						if (c[1] + c[2] <= frame.rows && c[1] - c[2] >= 0 &&
							c[0] + c[2] <= frame.cols && c[0] - c[2] >= 0)
						{
							/* Calculate x, y coordinates of the object in real units */
							xyPosition(&x_cm, &y_cm, c[0], c[2]);
							/* Show result in original image*/
							sprintf(text_xy, "X=%4.2lf cm, Y=%4.2lf cm", x_cm, y_cm);
							circle(frame, Point(c[0], c[1]), c[2], circle_color, 3, LINE_AA);
							circle(frame, Point(c[0], c[1]), 2, Scalar(255, 0, 0), 3, LINE_AA);
							putText(frame, text_color, Point(c[0], c[1] - 20), FONT_HERSHEY_COMPLEX, .4, Scalar(0, 0, 0));
							putText(frame, text_xy, Point(c[0] - c[2] + 5, c[1] + 18), FONT_HERSHEY_COMPLEX, .4, Scalar(0, 0, 0));
						}
					}
				}
				imshow("Original Image", frame);

				if (waitKey(50) == 27) break;

			}
			break;

		case 4:
			if (!cap.isOpened())  // if not success, exit program
			{
				cout << "Cannot open the video cam" << endl;
				return -1;
			}

			while (1)
			{
				Mat frame;
				if (!cap.read(frame)) //if not success, break loop
				{
					cout << "Cannot read a frame from video stream" << endl;
					break;
				}
				imshow("Original Image", frame);
				/* Convert original RGB image to HSV image */
				cvtColor(frame, hsv_img, COLOR_BGR2HSV);


				inRange(hsv_img, Scalar(150, 100, 20), Scalar(180, 255, 255), R_img);
				inRange(hsv_img, Scalar(0, 100, 20), Scalar(20, 255, 255), R_img2);
				inRange(hsv_img, Scalar(35, 100, 20), Scalar(90, 255, 255), G_img);
				inRange(hsv_img, Scalar(90, 100, 20), Scalar(150, 255, 255), B_img);
				addWeighted(R_img, 1, R_img2, 1, 0, R_img);

				GaussianBlur(R_img, R_img, Size(5, 5), 4, 3);
				GaussianBlur(G_img, G_img, Size(5, 5), 4, 3);
				GaussianBlur(B_img, B_img, Size(5, 5), 4, 3);

				//imshow("R_img", R_img);
				//imshow("G_img", G_img);
				//imshow("B_img", B_img);

				HoughCircles(R_img, Rcircles, HOUGH_GRADIENT, 1, 100, 50, 30, 50, 400);
				HoughCircles(G_img, Gcircles, HOUGH_GRADIENT, 1, 100, 50, 30, 50, 400);
				HoughCircles(B_img, Bcircles, HOUGH_GRADIENT, 1, 100, 50, 30, 50, 400);
				//HoughCircles(B_img, Bcircles, HOUGH_GRADIENT, 2, HC_dist, 200, HC_accthreshold, HC_minr, HC_maxr);


				for (int c = 0; c < 3; c++)
				{
					vector<Vec3f>  current_colour;
					Scalar circle_color;
					char text_color[10];

					switch (c)
					{
					case 0:
						current_colour = Rcircles;
						circle_color = Scalar(0, 0, 255);
						sprintf(text_color, "Red");
						break;
					case 1:
						current_colour = Gcircles;
						circle_color = Scalar(0, 255, 0);
						sprintf(text_color, "Green");
						break;
					case 2:
						current_colour = Bcircles;
						circle_color = Scalar(255, 0, 0);
						sprintf(text_color, "Blue");
						break;
					}

					for (size_t i = 0; i < current_colour.size(); i++)
					{
						Vec3i c = current_colour[i];
						double x_cm, y_cm;
						char text_xy[25];

						/* If circle is inside of the image */
						if (c[1] + c[2] <= frame.rows && c[1] - c[2] >= 0 &&
							c[0] + c[2] <= frame.cols && c[0] - c[2] >= 0)
						{
							/* Calculate x, y coordinates of the object in real units */
							xyPosition(&x_cm, &y_cm, c[0], c[2]);
							/* Show result in original image*/
							sprintf(text_xy, "X=%4.2lf cm, Y=%4.2lf cm", x_cm, y_cm);
							circle(frame, Point(c[0], c[1]), c[2], circle_color, 3, LINE_AA);
							circle(frame, Point(c[0], c[1]), 2, Scalar(255, 0, 0), 3, LINE_AA);
							putText(frame, text_color, Point(c[0], c[1] - 20), FONT_HERSHEY_COMPLEX, .4, Scalar(0, 0, 0));
							putText(frame, text_xy, Point(c[0] - c[2] + 5, c[1] + 18), FONT_HERSHEY_COMPLEX, .4, Scalar(0, 0, 0));
						}
					}
				}
				imshow("Original Image", frame);

				if (waitKey(50) == 27) break;

			}
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

void generateMfile(void)
{
	Mat frame;
	Mat frame2;
	Mat frame3;
	char cesta[25];
	char nazov[10];
	int i = 1;
	ofstream file;
	file.open("hodnotyPreZavislost.m");
	file << "% 1.hodnota je x druha y a tretia priemer\n";
	file << "hodnoty = [";

	namedWindow("Obrazok", CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
	namedWindow("Tunning", CV_WINDOW_AUTOSIZE);
	
	createTrackbar("Median", "Tunning", &medianBlurstep, 20, onTrackbar_tunning);
	createTrackbar("Canny", "Tunning", &cannyStep, 100, onTrackbar_tunning);
	createTrackbar("HC_dist", "Tunning", &HC_dist, 1080, onTrackbar_tunning);
	createTrackbar("HC_ACC", "Tunning", &HC_accthreshold, 100, onTrackbar_tunning);
	createTrackbar("HC_maxr", "Tunning", &HC_maxr, 1000, onTrackbar_tunning);
	createTrackbar("HC_minr", "Tunning", &HC_minr, 1000, onTrackbar_tunning);
	createTrackbar("SAVE", "Tunning", &save, 1, onTrackbar_tunning);
while (1) {
	sprintf(cesta, "../Images/meranie_%d.bmp", i++);
	frame = imread(cesta, CV_LOAD_IMAGE_COLOR);
	if (frame.data == NULL)
	{
		cout << "End" << endl;
		destroyWindow("Obrazok");
		destroyWindow("Tunning");
		destroyWindow("Median Blur"); 
		file << "];";
		file.close();
		break;
	}


	cvtColor(frame, hsv_img, COLOR_BGR2HSV);
	//inRange(hsv_img, Scalar(40, 50, 50), Scalar(90, 255, 255), blured_img);
	/* Separation of HSV image into H, S, V planes */
	vector<Mat> hsv_planes;
	split(hsv_img, hsv_planes);
	Mat s = hsv_planes[1];
					
	/* Choosing Saturation plane as grayscale image for next steps */
	gray_img = s;
					
	/* Apply median blur */
	medianBlur(gray_img, blured_img, 6 * 2 - 1);
	imshow("Median Blur", blured_img);
	HoughCircles(blured_img, circles, HOUGH_GRADIENT, 2, HC_dist, 200, HC_accthreshold, HC_minr, HC_maxr);
//	HoughCircles(blured_img, circles, HOUGH_GRADIENT, 2, 325, 200, 24, 30, 500);
	Vec3i out = NULL;
	for (size_t i = 0; i < 1; i++)
		{
			Vec3i c = circles[i];
			double x_cm, y_cm;
			out = c;
			/* If circle is inside of the image */
			if (c[1] + c[2] <= frame.rows && c[1] - c[2] >= 0 &&
				c[0] + c[2] <= frame.cols && c[0] - c[2] >= 0)
			{
				/* Compute mean HSV values */
//				meanHSV(c);

				/* Define RGB color based on HSV values */
//				colorRange();

				/* Calculate x, y coordinates of the object in real units */
				
				xyPosition(&x_cm, &y_cm, c[0], c[2]);

				char text_xy[25];

				sprintf(text_xy, "X=%4.2lf cm, Y=%4.2lf cm", x_cm, y_cm);

				/* Show result in original image*/
		//		circle(frame, Point(c[0], c[1]), c[2], Scalar(0,0,255), 3, LINE_AA);
		////		circle(frame, Point(c[0], c[1]), 2, Scalar(255, 0, 0), 3, LINE_AA);
		//		putText(frame, "K", Point(c[0], c[1] - 20), FONT_HERSHEY_COMPLEX, .4, Scalar(0, 0, 0));
		//		putText(frame, text_xy, Point(c[0] - c[2] + 5, c[1] + 18), FONT_HERSHEY_COMPLEX, .4, Scalar(0, 0, 0));
			}
		}
		imshow("Obrazok", frame);
		while (1)
		{
			if (waitKey(1) == 'c')
			{
				file << out[0] <<" " << out[1] << " " << out[2] << "\n";
				break;
			}
		}

	}

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
	printf("%c Detection from video...........................4 %c\n", 186, 186);
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

/* Function is used to calculate x, y coordinates of the object in real world units [cm] */
void xyPosition(double *x, double *y, int x_pixel, int r_pixel)
{
	double p_y[] = { -0.0000751, 0.02944, -3.932, 211.2};
	double px[] = { 5.597 , -0.01148, 0.3461, -0.001268, 0.0003492 };

	*y = p_y[0]*pow( r_pixel,3) + p_y[1]*pow(r_pixel,2) + p_y[2]* r_pixel + p_y[3];

	// minimum distance between object and camera to compute "x_cm" is 40 cm 
	if (*y > 20.0)	*x = px[0] + px[1] * x_pixel + px[2] * *y + px[3] * *y * x_pixel + px[4] * pow(*y,2);
	else *x = 0;
}