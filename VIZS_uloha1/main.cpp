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
void defineColor(int x, int y ,int r);
void xyPosition(double *x, double *y, int x_pixel, int r_pixel);
void drawCircles(void);

/* variables */
Mat frame, gray_img, blured_img, hsv_img;
int medianBlurstep = 4, cannyStep = 136, HC_maxr = 300, HC_minr = 39, HC_accthreshold = 167, HC_dist = 75, save = 0; // used in tunnig
double x_cm, y_cm;
char text_color[10];
Scalar circle_color;
vector<Vec3f> circles;

int main(int argc, char* argv[])
{
	VideoCapture cap(1); // open the video camera no. 0
	uint32_t i = 0;
	int numberOfFrames;
	char cesta[25];
	char nazov[10];
	char y_n;
	int mode;
	double dWidth;
	double dHeight;


	while (1)
	{
		menu();
		cin >> mode;
		switch (mode)
		{
		/************** Detection from camera input **************/
		case 1:		
			if (!cap.isOpened())  // if not success, exit program
			{
				cout << "Cannot open the video cam" << endl;
				return -1;
			}

			dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
			dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

			cout << "Frame size : " << dWidth << " x " << dHeight << endl;
			cout << "Zadaj pocet framov (-1 pre kontinualny mod)" << endl;
			cin >> numberOfFrames;

			if (numberOfFrames == -1) numberOfFrames = INT32_MAX;
			namedWindow("Image view", CV_WINDOW_AUTOSIZE);

			while (1)
			{
				if (!cap.read(frame)) //if not success, break loop
				{
					cout << "Cannot read a frame from video stream" << endl;
					break;
				}
				// convert original RGB image to HSV image
				cvtColor(frame, hsv_img, COLOR_BGR2HSV);

				// use S plane as grayscale image
				vector<Mat> hsv_planes;
				split(hsv_img, hsv_planes);
				gray_img = hsv_planes[1];

				// apply median blur
				medianBlur(gray_img, blured_img, medianBlurstep * 2 - 1);

				// detect circles using Hough transform
				HoughCircles(blured_img, circles, HOUGH_GRADIENT, 2, HC_dist, cannyStep, HC_accthreshold, HC_minr, HC_maxr);

				// output result
				drawCircles();
				imshow("blured image view", blured_img);
				imshow("Image view", frame);
				if (waitKey(50) == 27) break;

			}
			cout << "End of Capture mode" << endl;
			destroyWindow("Image view");
			destroyWindow("blured image view");
			break;

		/************* Capture mode *************/
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
				waitKey(50);
				imshow("Video Capture", frame);

				if (waitKey(1) == 50)
				{
					i++;
					sprintf(cesta, "../Images/%s_%i.bmp", nazov, i);
					imwrite(cesta, frame);
					cout << "Dalsi obrazok? (y/n)" << endl;
					cin >> y_n;
					if (y_n == 'n') break;
				}
				if (waitKey(9) == 27)
					break;
			}
			cout << "End of Capture mode" << endl;
			destroyWindow("Video Capture");
			break;

		/************* Detection from replay mode *************/
		case 3:
			i = 0;
			cout << "Zadaj nazov suboru " << endl;
			cin >> nazov;
			namedWindow("Video Replay", CV_WINDOW_AUTOSIZE); 
			while (1)
			{
				i++;
				sprintf(cesta, "../Images/%s_%d.bmp", nazov, i);
				frame = imread(cesta, CV_LOAD_IMAGE_COLOR);
				if (!frame.data)
				{
					cout << "End of Replay mode" << endl;
					destroyWindow("Video Replay");
					break;
				}

				// convert original RGB image to HSV image
				cvtColor(frame, hsv_img, COLOR_BGR2HSV);

				// use S plane as grayscale image
				vector<Mat> hsv_planes;
				split(hsv_img, hsv_planes);
				gray_img = hsv_planes[1];

				// apply median blur
				medianBlur(gray_img, blured_img, medianBlurstep * 2 - 1);

				// detect circles using Hough transform
				HoughCircles(blured_img, circles, HOUGH_GRADIENT, 2, HC_dist, cannyStep, HC_accthreshold, HC_minr, HC_maxr);

				// output result
				drawCircles();

				imshow("Video Replay", frame);
				imshow("Blured Video Replay", blured_img);

				if (waitKey(50) == 27) break;

			}
			destroyWindow("Video Replay");
			destroyWindow("Blured Video Replay");
			break;
		/************** Parameter Tunning mode **************/
		case 4:
			i = 0;
			cout << "Zadaj nazov obrazka " << endl;
			cin >> nazov;
			cout << "Zadaj cislo obrazka " << endl;
			cin >> i;
			namedWindow("Original Image", CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
			namedWindow("Tunning", CV_WINDOW_AUTOSIZE);

			createTrackbar("Median", "Tunning", &medianBlurstep, 20, onTrackbar_tunning);
			createTrackbar("Canny", "Tunning", &cannyStep, 300, onTrackbar_tunning);
			createTrackbar("HC_dist", "Tunning", &HC_dist, 1080, onTrackbar_tunning);
			createTrackbar("HC_ACC", "Tunning", &HC_accthreshold, 200, onTrackbar_tunning);
			createTrackbar("HC_maxr", "Tunning", &HC_maxr, 1000, onTrackbar_tunning);
			createTrackbar("HC_minr", "Tunning", &HC_minr, 500, onTrackbar_tunning);
			createTrackbar("SAVE", "Tunning", &save, 1, onTrackbar_tunning);
			
			while (1)
			{
				sprintf(cesta, "../Images/%s_%d.bmp", nazov, i);
				frame = imread(cesta, CV_LOAD_IMAGE_COLOR);
				if (!frame.data)
				{
					cout << "End of Replay mode" << endl;
					destroyWindow("Original Image");
					break;
				}

				// convert original RGB image to HSV image
				cvtColor(frame, hsv_img, COLOR_BGR2HSV);

				// use S plane as grayscale image
				vector<Mat> hsv_planes;
				split(hsv_img, hsv_planes);
				gray_img = hsv_planes[1];

				// apply median blur
				medianBlur(gray_img, blured_img, medianBlurstep * 2 - 1);

				// detect circles using Hough transform
				HoughCircles(blured_img, circles, HOUGH_GRADIENT, 2, HC_dist, cannyStep, HC_accthreshold, HC_minr, HC_maxr);

				// output result
				drawCircles();

				imshow("Original Image", frame);
				waitKey(50);
				if (save)
				{
					cout << "Parameters saved!" << endl;
					destroyWindow("Original Image");
					destroyWindow("Tunning");
					save = 0;
					break;
				}
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


void menu(void)
{
	char riadok[51];
	memset(riadok, 0, 51);
	memset(riadok, 0xCD, 50);
	printf("%c%s%c\n", 201, riadok, 187);
	printf("%c Detection from camera..........................1 %c\n", 186, 186);
	printf("%c Take pictures..................................2 %c\n", 186, 186);
	printf("%c Detection from replay..........................3 %c\n", 186, 186);
	printf("%c Parameter tunning..............................4 %c\n", 186, 186);
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

void defineColor(int x, int y, int r)
{
	Mat R_img, R_img2, G_img, B_img, R_mask, G_mask, B_mask;
	Scalar mean_color_r, mean_color_g, mean_color_b;
	inRange(hsv_img, Scalar(150, 50, 20), Scalar(180, 255, 255), R_img);
	inRange(hsv_img, Scalar(0, 50, 20), Scalar(25, 255, 255), R_img2);
	inRange(hsv_img, Scalar(35, 50, 20), Scalar(90, 255, 255), G_img);
	inRange(hsv_img, Scalar(91, 50, 20), Scalar(150, 255, 255), B_img);
	addWeighted(R_img, 1, R_img2, 1, 0, R_img);

	Mat circle_image(hsv_img.rows, hsv_img.cols, CV_8UC1, Scalar(0));
	circle(circle_image, Point(x, y), r, Scalar(255, 255, 255), CV_FILLED, 8, 0);

	subtract(circle_image, B_img, R_mask);
	subtract(R_mask, G_img, R_mask);

	subtract(circle_image, B_img, G_mask);
	subtract(G_mask, R_img, G_mask);

	subtract(circle_image, R_img, B_mask);
	subtract(B_mask, G_img, B_mask);

	R_mask = R_mask(Range(y - r, y + r), Range(x - r, x + r));
	G_mask = G_mask(Range(y - r, y + r), Range(x - r, x + r));
	B_mask = B_mask(Range(y - r, y + r), Range(x - r, x + r));

	mean_color_r = mean(R_mask);
	mean_color_b = mean(B_mask);
	mean_color_g = mean(G_mask);

	if (mean_color_r[0] > mean_color_b[0] && mean_color_r[0] > mean_color_g[0])
	{
		circle_color = Scalar(0, 0, 255);
		sprintf(text_color, "Red");
	}
	else if (mean_color_g[0] > mean_color_b[0] && mean_color_g[0] > mean_color_r[0])
	{
		circle_color = Scalar(0, 255, 0);
		sprintf(text_color, "Green");
	}
	else if (mean_color_b[0] > mean_color_g[0] && mean_color_b[0] > mean_color_r[0])
	{
		circle_color = Scalar(255, 0, 0);
		sprintf(text_color, "Blue");
	}
	else
	{
		circle_color = Scalar(129, 129, 129);
		sprintf(text_color, "Undefined");
	}
	
}

/* Function is used to calculate x, y coordinates of the object in real world units [cm] */
void xyPosition(double *x, double *y, int x_pixel, int r_pixel)
{
	double p_y[] = { -0.0000751, 0.02944, -3.932, 211.2 };
	double px[] = { 5.597 , -0.01148, 0.3461, -0.001268, 0.0003492 };

	*y = p_y[0] * pow(r_pixel, 3) + p_y[1] * pow(r_pixel, 2) + p_y[2] * r_pixel + p_y[3];

	// minimum distance between object and camera to compute "x_cm" is 40 cm 
	if (*y > 20.0)	*x = -(px[0] + px[1] * x_pixel + px[2] * *y + px[3] * *y * x_pixel + px[4] * pow(*y, 2));
	else *x = 0;
}


void drawCircles(void)
{
	for (size_t i = 0; i < circles.size(); i++)
	{
		Vec3i c = circles[i];

		// circles outside of frame boundaries are ignored
		if (c[1] + c[2] <= frame.rows && c[1] - c[2] >= 0 &&
			c[0] + c[2] <= frame.cols && c[0] - c[2] >= 0)
		{
			//averageHSV(c);
			defineColor(c[0], c[1], c[2]);
			xyPosition(&x_cm, &y_cm, c[0], c[2]);

			char text_xy[25];

			sprintf(text_xy, "X=%4.2lf cm, Y=%4.2lf cm", x_cm, y_cm);
			circle(frame, Point(c[0], c[1]), c[2], circle_color, 3, LINE_AA);

			circle(frame, Point(c[0], c[1]), 2, Scalar(255, 0, 0), 3, LINE_AA);
			putText(frame, text_color, Point(c[0], c[1] - 20), FONT_HERSHEY_COMPLEX, .4, Scalar(0, 0, 0));
			putText(frame, text_xy, Point(c[0] - c[2] + 5, c[1] + 18), FONT_HERSHEY_COMPLEX, .4, Scalar(0, 0, 0));
		}

	}
}