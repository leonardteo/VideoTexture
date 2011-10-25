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

//VideoTexture
#include "VideoTexture.h"

using namespace std;

/**
 * Program entry point
 */
int main (int argc, const char * argv[])
{
    
    //Name of the file
    string filename = "/Users/leonardteo/Movies/dead_end.mov";
    string cachefile = "/Users/leonardteo/Desktop/Temp/frameDiffCache.txt";
    
    VideoTexture *videoTexture; 
    
    try {
        
        //Create the new video texture
        videoTexture = new VideoTexture(filename, 0.1f);

        //Generate the cache file if needed
        //videoTexture->generateFrameDistanceMatrix(cachefile);        
        
        //Load the cache file
        videoTexture->loadFrameDiffMatrix(cachefile);
        
        //Show the distance matrix
        videoTexture->showMatrix("Distance Matrix", videoTexture->frameDistanceMatrix, videoTexture->frameCount);

        //Generate the probability matrix
        videoTexture->generateProbabilityMatrix();
        
        //Show the probability matrix
        videoTexture->showMatrix("Probability Matrix", videoTexture->frameProbabilityMatrix, videoTexture->frameCount);
        
        //Generate the weighted distance matrix
        videoTexture->generateWeightedFrameDistanceMatrix();
        
        //Show the weighted distance matrix
        videoTexture->showMatrix("Weighted Distance Matrix", videoTexture->weightedFrameDistanceMatrix, videoTexture->frameCount);
        
        //Generate the weighted probability matrix
        videoTexture->generateWeightedProbabilityMatrix();
        
        //Show the weighted probability matrix
        videoTexture->showMatrix("Weighted Probability Matrix", videoTexture->weightedFrameProbabilityMatrix, videoTexture->frameCount);
        
        //Generate the anticipated future cost matrix
        videoTexture->generateAnticipateFutureCostMatrix(1, 0.995, 5);
        
        //Show the anticipated future cost matrices ###THIS IS WHERE YOU ARE
        videoTexture->showMatrix("Anticipated future cost probability matrix", videoTexture->anticipatedFutureCostProbabilityMatrix, videoTexture->frameCount);
        videoTexture->printMatrix(videoTexture->anticipatedFutureCostProbabilityMatrix, videoTexture->frameCount);
        
        //videoTexture->randomPlay(videoTexture->anticipatedFutureCostProbabilityMatrix);
        videoTexture->randomPlay(videoTexture->weightedFrameProbabilityMatrix);
        
        //Debug the weighted probability matrix
        //videoTexture->printMatrix(videoTexture->weightedFrameProbabilityMatrix, videoTexture->frameCount);
        /*
        for (int row=0; row<videoTexture->frameCount; row++)
        {
            double sum = 0.0f;
            for (int col=0; col<videoTexture->frameCount; col++)
            {
                sum += videoTexture->weightedFrameProbabilityMatrix[row][col];
            }
            cout << "Sum: " << sum << endl;
        }
         */
        
        
        //videoTexture->randomPlay();
        
    } catch (string e) {
        cout << e << endl;
        return 1;
    }
    return 0;
        
        
}

