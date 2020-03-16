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
    Rect roi;
    int offset_x = 20;
    int offset_y = 20;

    VideoCapture cap;
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API
    cap.open(deviceID + apiID);
    // check if we succeeded
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
        return -1;
    }
    cap.set(CAP_PROP_FRAME_WIDTH,1000);
    cap.set(CAP_PROP_FRAME_HEIGHT,1000);
    cout << cap.get(CAP_PROP_FRAME_WIDTH) << endl;
    cout << cap.get(CAP_PROP_FRAME_HEIGHT) << endl;

    /*Mat background;
    for(int i=0;i<30;i++)
    {
    cap >> background;
    }*/
    Mat background(720,960, CV_8UC3, Scalar(0,255,0));
  
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

        roi.x = offset_x;
        roi.y = offset_y;
        roi.width = frame.size().width -(offset_x*2);
        roi.height = frame.size().height -(offset_y*2);

        Mat crop = frame(roi);

        cvtColor(frame,hsv,COLOR_BGR2HSV);
        Mat mask1, mask2;
        inRange(hsv, Scalar(0,180,70),Scalar(5,255,255),mask1);
        inRange(hsv, Scalar(175,180,70),Scalar(180,255,255),mask2);
        mask1=mask1+mask2;
        //imshow("mask", mask1);
        Mat kernel = Mat::ones(1,1, CV_32F);
        morphologyEx(mask1,mask1,cv::MORPH_OPEN,kernel);
        //imshow("MORPH_OPEN", mask1);
        morphologyEx(mask1,mask1,cv::MORPH_DILATE,kernel);
        //imshow("MORPH_DILATE", mask1);
        // creating an inverted mask to segment out the cloth from the frame
        bitwise_not(mask1,mask2);
        Mat res1, res2, final_output;
        //imshow("bitwise_not", mask2);
        // Segmenting the cloth out of the frame using bitwise and with the inverted mask
        bitwise_and(frame,frame,res1,mask1);
        res1=res1*1.5;
        Mat kernel2 = Mat::ones(9,9, CV_32F);
        imshow("bitwise_and", res1);
        morphologyEx(res1,res1,cv::MORPH_DILATE,kernel2);
        imshow("dilate", res1);
        // creating image showing static background frame pixels only for the masked region
        bitwise_and(background,background,res2,mask1);

        // Generating the final augmented output.
        addWeighted(res1,1,frame,1,0,final_output);
        imshow("magic", final_output);
        //imshow("crop", crop);

        // show live and wait for a key with timeout long enough to show images
        //imshow("Live", frame);
        if (waitKey(5) >= 0)
            break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}