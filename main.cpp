#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
//#include <opencv2/opencv.hpp> // wieder entfernen
#include <iostream>
#include <stdio.h>
using namespace cv;
using namespace std;
int main(int, char**)
{
    Mat frame;
    Mat hsv;
    //Mat background;
    //--- INITIALIZE VIDEOCAPTURE
    VideoCapture cap;
    // open the default camera using default API
    // cap.open(0);
    // OR advance usage: select any API backend
    int deviceID = 2;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API
    // open selected camera using selected API
    cap.open(deviceID + apiID);
    // check if we succeeded
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
        return -1;
    }
    cap.set(CAP_PROP_FRAME_WIDTH,360);
    cap.set(CAP_PROP_FRAME_HEIGHT,240);

    Mat background;
    for(int i=0;i<30;i++)
    {
    cap >> background;
    }

    //Laterally invert the image / flip the image.
    //flip(background,background,1);
    //--- GRAB AND WRITE LOOP
    cout << "Start grabbing" << endl
        << "Press any key to terminate" << endl;
    for (;;)
    {
        // wait for a new frame from camera and store it into 'frame'
        cap.read(frame);
        // check if we succeeded
        if (frame.empty()) {
            cerr << "ERROR! blank frame grabbed\n";
            break;
        }
        cvtColor(frame,hsv,COLOR_BGR2HSV);
        Mat mask1, mask2;
        inRange(hsv, Scalar(0,120,70),Scalar(10,255,255),mask1);
        inRange(hsv, Scalar(170,120,70),Scalar(180,255,255),mask2);
        mask1=mask1+mask2;

        Mat kernel = Mat::ones(3,3, CV_32F);
        morphologyEx(mask1,mask1,cv::MORPH_OPEN,kernel);
        morphologyEx(mask1,mask1,cv::MORPH_DILATE,kernel);

        // creating an inverted mask to segment out the cloth from the frame
        bitwise_not(mask1,mask2);
        Mat res1, res2, final_output;

        // Segmenting the cloth out of the frame using bitwise and with the inverted mask
        bitwise_and(frame,frame,res1,mask2);

        // creating image showing static background frame pixels only for the masked region
        bitwise_and(background,background,res2,mask1);

        // Generating the final augmented output.
        addWeighted(res1,1,res2,1,0,final_output);
        imshow("magic", final_output);

        // show live and wait for a key with timeout long enough to show images
        //imshow("Live", frame);
        if (waitKey(5) >= 0)
            break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}