#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <ctime>
#include <chrono>


using namespace cv;
using namespace std;

void menu(void);

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
		case 3:
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
	printf("%c Capture mode...................................1 %c\n",186,186);
	printf("%c Replay mode....................................2 %c\n", 186, 186);
	printf("%c Exit...........................................3 %c\n", 186, 186);
	printf("%c%s%c\n", 200, riadok, 188);
}