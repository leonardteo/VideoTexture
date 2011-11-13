//
//  main.cpp
//  VideoTexturePlayground
//
//  Created by Leonard Teo on 11-10-25.
//  Copyright 2011 Leonard Teo. All rights reserved.
//

#include <iostream>

//OpenCV libraries
#include "core.hpp"
#include "highgui.hpp"
#include "imgproc.hpp"

#include "VideoTexture.h"

using namespace std;


//Show an image
void showImage(const cv::Mat& frame)
{
    bool close = false;
    
    while (!close)
    {
        cv::imshow("Frame", frame);
        
        if (cv::waitKey())
        {
            close = true;
        }
    }
}


//Program entry point
int main (int argc, const char * argv[])
{

    // insert code here...
    std::cout << "Hello, OpenCV!\n";
    
    string filename = "/Users/leonardteo/Movies/dead_end.mov";
    
    
    cout << "Loading video: " << filename << endl;
    
    
    //Read the video
    cv::VideoCapture capture(filename);
    if (!capture.isOpened())
    {
        cout << "Couldn't open file" << endl;
        exit(1);
    }
    
    //Analyze the video
    double frameRate = capture.get(CV_CAP_PROP_FPS);
    int width = (int) capture.get(CV_CAP_PROP_FRAME_WIDTH);
    int height = (int) capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    
    union {
        int value;
        char code[4];
    } codec_union;
    
    codec_union.value = (int) capture.get(CV_CAP_PROP_FOURCC);
    char codec[4];
    codec[0] = codec_union.code[0];
    codec[1] = codec_union.code[1];
    codec[2] = codec_union.code[2];    
    codec[3] = codec_union.code[3];
    cout << "Codec: " << codec[0] << codec[1] << codec[2] << codec[3] << endl;
    
    cout << "Framerate: " << frameRate << endl;
    cout << "Width: " << width << endl;
    cout << "Height: " << height << endl;    
    
    //Run through the frames once to get the real number of frames
    cv::Mat frame;  //Temp frame
    int frameCount = 0;
    
    while (capture.read(frame))
    {
        frameCount++;
    }
    capture.release();
    
    cout << "Number of real frames in sequence: " << frameCount << endl;
    
    //Load the frames into the frame array
    cv::Mat* frames = new cv::Mat[frameCount];
    
    capture.open(filename);
    if (!capture.isOpened())
    {
        cout << "Couldn't open " << filename << endl;
        exit(1);
    }

    string fileout = "/Users/leonardteo/Movies/dead_end_out.mov";
    cv::VideoWriter writer(fileout, CV_FOURCC('j', 'p', 'e', 'g'), frameRate, cvSize(width, height));       
    
    for (int i=0; i<frameCount; i++)
    {
        capture.read(frame);
        frame.copyTo(frames[i]);  //Need to copy the actual frame
        writer.write(frame);
    }
    capture.release();
    
        
    
   

    
    
    //Get the first frame
    //cv::Mat frameFrom = vt->frames[0];
    
    //Get the 10th frame
    //cv::Mat frameTo = vt->frames[10];
    
    //cv::Mat image = vt->createCrossFadeFrame(frameFrom, frameTo);
    
    //showImage(image);
    
    //Show first frame
    //showImage(frame1);
    
    //showImage(frame10);
    
    return 0;
}

