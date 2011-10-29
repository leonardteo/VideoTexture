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
    
    VideoTexture* vt = new VideoTexture(filename, 1.0f);
    
    //Get the first frame
    cv::Mat frameFrom = vt->frames[0];
    
    //Get the 10th frame
    cv::Mat frameTo = vt->frames[10];
    
    cv::Mat image = vt->createCrossFadeFrame(frameFrom, frameTo);
    
    showImage(image);
    
    //Show first frame
    //showImage(frame1);
    
    //showImage(frame10);
    
    return 0;
}

