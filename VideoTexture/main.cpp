//
//  main.cpp
//  VideoTexture
//
//  Created by Leonard Teo on 11-10-08.
//  Copyright 2011 Leonard Teo. All rights reserved.
//

//std libraries
#include <iostream>
#include <math.h>
#include <fstream>

//OpenCV libraries
#include "core.hpp"
#include "highgui.hpp"
#include "imgproc.hpp"

#include "Histogram1D.h"
#include "colorhistogram.h"
#include "ImageComparator.h"

using namespace std;

/**
 * Global Vars
 **/

float sigma = 5.0f;    //Controls the mapping between L2 distance and relative probability of taking a given transition

//Array of all the frames we are going to store
cv::Mat *frames;

int **frameDistanceMatrix;  //nxn array of frame differences
float **frameExpMatrix;     //Probabilities

//Frame count
int frameCount = 0;

//Current frame 
int currentFrame;


/**
 * Print out the values of the frame distance matrix
 */
static void printFrameDistanceMatrix()
{
    for (int row=0; row < frameCount; row++)
    {
        for (int col=0; col < frameCount; col++)
        {
            std::cout << frameDistanceMatrix[row][col] << "\t";
        }
        std::cout << std::endl;
    }
}

/**
 * Print out values of the expMatrix
 */
static void printFrameExpMatrix()
{
    for (int row=0; row < frameCount; row++)
    {
        for (int col=0; col < frameCount; col++)
        {
            std::cout << frameExpMatrix[row][col] << "\t";
        }
        std::cout << std::endl;
    }   
}


/**
 * Program entry point
 */
int main (int argc, const char * argv[])
{
    std::cout << "Hello, OpenCV!\n";

    //Name of the file
    std::string filename = "/Users/leonardteo/Movies/testmovie2.mov";
    
    //First read the video
    cv::VideoCapture capture(filename);
    if (!capture.isOpened())
    {
        std::cout << "Could not open video";
        return 1;
    }
    
    //Analyze the video
    double rate = capture.get(CV_CAP_PROP_FPS);
    
    //Run through the video once and count the REAL number of frames because OpenCV doesn't give you the correct number of frames
    cv::Mat frame;
    while (capture.read(frame))
    {
        frameCount ++;
    }
    capture.release();
    
    std::cout << frameCount << " real number of frames in sequence\n";
    std::cout << rate << " fps\n";    
    
    //Load frames into the frame array
    frames = new cv::Mat[frameCount];
    
    capture.open(filename);
    if (!capture.isOpened())
    {
        std::cout << "Could not reopen video file\n";
        return 1;
    }
    for (int i=0; i<frameCount; i++)
    {
        capture.read(frame);
        frame.copyTo(frames[i]);    //You have to copy the frame or it won't work. Go figure.
    }
    capture.release();
    
    
    //For each frame, show the histogram
    /*
    int delay = 1000/rate;
    for (int i=0; i<frameCount; i++)
    {
        cv::Mat histogram = h.getHistogramImage(frames[i]);
        cv::imshow("Histogram", histogram);
        
        cv::waitKey(delay);
    }*/
        
    
    //Create the frame distance and probability matrices
    frameDistanceMatrix = new int*[frameCount];
    frameExpMatrix = new float*[frameCount];
    for (int i=0; i<frameCount; i++)
    {
        frameDistanceMatrix[i] = new int[frameCount];
        frameExpMatrix[i] = new float[frameCount];
    }
    
    
    //Calculate the L2 distance between each frame
    //Image comparison
    ImageComparator c;
    c.setReferenceImage(frames[0]);
    double comp = c.compare(frames[0]);
    std::cout << "Comparison with self: " << comp << std::endl;
    
    
    //Write to file
    ofstream outfile("/Users/leonardteo/Desktop/frameDiffCache.txt");    
    if (outfile.is_open())
    {  
        for (int row=0; row<frameCount; row++)    //For each row
        {
            c.setReferenceImage(frames[row]);
            
            for (int col=0; col<frameCount; col++)  //For each column
            {
                //Calculate the L2 distance between frames
                int diff = c.compare(frames[col]);
                std::cout << "Difference between frame " << row << " and " << col << ": " << diff << std::endl;
                frameDistanceMatrix[row][col] = diff;
                outfile << diff << " ";
            }
            
            outfile << endl;
        }
        
        printFrameDistanceMatrix();

        outfile.close();
        
    } else {
        cout << "Could not open cache file to write." << endl;
    }
    
        
    return 0;
    
    
    
    
    
    /* I'm obviously doing it wrong */
    
    
    
    
    

    

    
    //Map the distances to probabilities
    for (int row=0; row<frameCount; row++)    //For each row
    {
        for (int col=0; col<frameCount; col++)  //For each column
        {
            if (col == 0 || col == (frameCount - 1))
            {
                frameExpMatrix[row][col] = 0.0f;  //First and last columns are always 0
            } else {
                
                //Make sure that we stay within row = frameCount - 1
                if (row < (frameCount - 1))
                {
                    frameExpMatrix[row][col] = expf(-(float)frameDistanceMatrix[row+1][col]/sigma);
                } else {
                    frameExpMatrix[row][col] = 0.0f;    //The last row will be 0;
                }
            }
        }
    }

    
    //Debug the matrix
    //printFrameExpMatrix();
    

    
    
    //Go through each row and normalize the numbers
    for (int row=0; row<frameCount; row++)
    {
        float rowSum = 0.0f;
        for (int col=0; col<frameCount; col++)
        {
            rowSum += frameExpMatrix[row][col];
        }
        for (int col=0; col<frameCount; col++)
        {
            if (rowSum > 0.0f)
                frameExpMatrix[row][col] = frameExpMatrix[row][col] / rowSum;
        }
    }
    
    
    return 0;
   
        
}

