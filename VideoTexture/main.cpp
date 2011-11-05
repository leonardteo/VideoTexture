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

struct FileSetting
{
    string filename;
    double sigma;
    int scale;
    double pruneThreshold;
};

FileSetting candle, catseyes, deadend, fireplace, fireplace2, flag, puppy, vtclock, vtclock_deadend, southpark1, southpark2;

/**
 * Initialize a bunch of examples
 */
void initFiles()
{
    candle.filename = "candle.mov";
    candle.sigma = 0.01f;
    candle.scale = 1;
    candle.pruneThreshold = 0.0000001f;
    
    catseyes.filename = "catseyes.mov";
    catseyes.sigma = 0.01f;
    catseyes.scale = 1;
    catseyes.pruneThreshold = 0.00000001f;
    
    deadend.filename = "dead_end.mov";
    deadend.sigma = 0.03f;
    deadend.scale = 10;
    deadend.pruneThreshold = 0.00000001f;
    
    fireplace.filename = "fireplace.mov";
    fireplace.sigma = 0.05f;
    fireplace.scale = 5;
    fireplace.pruneThreshold = 0.00000001f;;
    
    fireplace2.filename = "fireplace2.mov";
    fireplace2.sigma = 0.05f;
    fireplace2.scale = 2;
    fireplace2.pruneThreshold = 0.00000001f;;
    
    flag.filename = "flag.mov";
    flag.sigma = 0.025f;
    flag.scale = 1;
    flag.pruneThreshold = 0.00000001f;
    
    puppy.filename = "puppy.mov";
    puppy.sigma = 0.01f;
    puppy.scale = 1;
    puppy.pruneThreshold = 0.00000001f;

    vtclock.filename = "vtclk1a.mpg";
    vtclock.sigma = 0.01f;
    vtclock.scale = 10;
    vtclock.pruneThreshold = 0.00000001f;
    
    vtclock_deadend.filename = "vtclk1a.mpg";
    vtclock_deadend.sigma = 0.02f;
    vtclock_deadend.scale = 10;
    vtclock_deadend.pruneThreshold = 0.00000001f;
    
    southpark1.filename = "southpark1.mp4";
    southpark1.sigma = 0.1f;
    southpark1.scale = 1;
    southpark1.pruneThreshold = 0.0f;

    southpark2.filename = "southpark2.mp4";
    southpark2.sigma = 0.04f;
    southpark2.scale = 1;
    southpark2.pruneThreshold = 0.0f;    
}


/**
 * Program entry point
 */
int main (int argc, const char * argv[])
{

    initFiles();
    
    string videoPath = "/Users/leonardteo/Movies/";
    string cachePath = "/Users/leonardteo/Desktop/Temp/videotexture_cache/";
    
    FileSetting fileSetting = deadend;
    
    int image_scale = fileSetting.scale;
    double sigma = fileSetting.sigma;
    double pruneThreshold = fileSetting.pruneThreshold;
    
    string filename = videoPath + fileSetting.filename;
    string cachefile = cachePath + fileSetting.filename + ".txt";
    
    VideoTexture *videoTexture; 
    
    try {
        
        //Create the new video texture
        videoTexture = new VideoTexture(filename, sigma);  
        
        //Generate the cache file if needed
        //videoTexture->generateFrameDistanceMatrix(cachefile);        
        
        //Load the cache file
        videoTexture->loadFrameDiffMatrix(cachefile);
        
        //Show the distance matrix
        videoTexture->showMatrix("Distance Matrix", videoTexture->frameDistanceMatrix, videoTexture->frameCount, false, image_scale);

        //Generate the probability matrix
        videoTexture->generateProbabilityMatrix();
        
        //Show the probability matrix
        videoTexture->showMatrix("Probability Matrix", videoTexture->frameProbabilityMatrix, videoTexture->frameCount, false, image_scale);
        
        //Generate the weighted distance matrix
        videoTexture->generateWeightedFrameDistanceMatrix();
        
        //Show the weighted distance matrix
        videoTexture->showMatrix("Weighted Distance Matrix", videoTexture->weightedFrameDistanceMatrix, videoTexture->frameCount, false, image_scale);
        
        //Generate the weighted probability matrix
        videoTexture->generateWeightedProbabilityMatrix();
        
        //Show the weighted probability matrix
        videoTexture->showMatrix("Weighted Probability Matrix", videoTexture->weightedFrameProbabilityMatrix, videoTexture->frameCount, false, image_scale);
        
        //Generate the anticipated future cost matrix
        videoTexture->generateAnticipatedFutureCostMatrix(1.0f, 0.995f, 0.001f);
        
        //Show the anticipated future cost matrices
        videoTexture->showMatrix("Anticipated future cost probability matrix", videoTexture->anticipatedFutureCostProbabilityMatrix, videoTexture->frameCount, false, image_scale);
        
        //videoTexture->randomPlay(videoTexture->anticipatedFutureCostProbabilityMatrix, pruneThreshold);
        
        videoTexture->findTransitions(videoTexture->anticipatedFutureCostMatrix, 50);

        
    } catch (string e) {
        cout << e << endl;
        return 1;
    }
    return 0;
        
        
}

