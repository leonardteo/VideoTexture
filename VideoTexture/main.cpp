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
#include "TransitionsTable.h"

using namespace std;

struct FileSetting
{
    string filename;
    double sigma;
    int scale;
    double pruneThreshold;
    string fileout;
    int frameTarget;
    int minTransitionLength;
    int numTransitions;
};

FileSetting candle, candle1, candle2, catseyes, deadend, fireplace, fireplace2, flag, puppy, vtclock, vtclock_deadend, southpark1, southpark2;

/**
 * Initialize a bunch of examples
 */
void initFiles()
{
    candle.filename = "candle.mov";
    candle.fileout = "candle_out";
    candle.sigma = 0.01f;
    candle.scale = 1;
    candle.pruneThreshold = 0.0000001f;
    candle.frameTarget = 200;
    candle.minTransitionLength = 50;
    candle.numTransitions = 10;    
    
    candle1.filename = "candle1.mov";
    candle1.fileout = "candle1_out";
    candle1.sigma = 0.025f;
    candle1.scale = 1;
    candle1.pruneThreshold = 0.0000001f;
    candle1.frameTarget = 200;
    candle1.minTransitionLength = 50;
    candle1.numTransitions = 10;
    
    candle2.filename = "candle2.mov";
    candle2.fileout = "candle2_out";
    candle2.sigma = 0.03f;
    candle2.scale = 1;
    candle2.pruneThreshold = 0.0000001f;    
    candle2.frameTarget = 600;
    candle2.minTransitionLength = 50;
    candle2.numTransitions = 20;    
    
    catseyes.filename = "catseyes.mov";
    catseyes.fileout = "catseyes_out";
    catseyes.sigma = 0.01f;
    catseyes.scale = 1;
    catseyes.pruneThreshold = 0.00000001f;
    catseyes.frameTarget = 1000;
    catseyes.minTransitionLength = 50;
    catseyes.numTransitions = 10;    
    
    deadend.filename = "dead_end.mov";
    deadend.fileout = "dead_end_out";
    deadend.sigma = 0.03f;
    deadend.scale = 10;
    deadend.pruneThreshold = 0.00000001f;
    deadend.frameTarget = 200;
    deadend.minTransitionLength = 50;
    deadend.numTransitions = 10;     
    
    fireplace.filename = "fireplace.mov";
    fireplace.fileout = "fireplace_out";
    fireplace.sigma = 0.05f;
    fireplace.scale = 5;
    fireplace.pruneThreshold = 0.00000001f;
    fireplace.frameTarget = 500;
    fireplace.minTransitionLength = 10;
    fireplace.numTransitions = 10;     
    
    fireplace2.filename = "fireplace2.mov";
    fireplace2.fileout = "fireplace2_out";
    fireplace2.sigma = 0.05f;
    fireplace2.scale = 2;
    fireplace2.pruneThreshold = 0.00000001f;
    fireplace2.frameTarget = 500;
    fireplace2.minTransitionLength = 10;
    fireplace2.numTransitions = 10;         
    
    flag.filename = "flag.mov";
    flag.fileout = "flag_out";
    flag.sigma = 0.5f;
    flag.scale = 1;
    flag.pruneThreshold = 0.00000001f;
    flag.frameTarget = 1200;
    flag.minTransitionLength = 20;
    flag.numTransitions = 10;      
    
    puppy.filename = "puppy.mov";
    puppy.fileout = "puppy_out";
    puppy.sigma = 0.01f;
    puppy.scale = 1;
    puppy.pruneThreshold = 0.00000001f;
    puppy.frameTarget = 1250;
    puppy.minTransitionLength = 50;
    puppy.numTransitions = 10;         

    vtclock.filename = "vtclk1a.mpg";
    vtclock.fileout = "vtclk1a_out";
    vtclock.sigma = 0.01f;
    vtclock.scale = 10;
    vtclock.pruneThreshold = 0.00000001f;
    vtclock.frameTarget = 100;
    vtclock.minTransitionLength = 5;
    vtclock.numTransitions = 10;         
    
    
    vtclock_deadend.filename = "vtclk2a.mpg";
    vtclock_deadend.fileout = "vtclk2a_out";
    vtclock_deadend.sigma = 0.02f;
    vtclock_deadend.scale = 10;
    vtclock_deadend.pruneThreshold = 0.00000001f;
    vtclock_deadend.frameTarget = 100;
    vtclock_deadend.minTransitionLength = 5;
    vtclock_deadend.numTransitions = 10;           
    
    southpark1.filename = "southpark1.mp4";
    southpark1.fileout = "southpark1_out";
    southpark1.sigma = 0.1f;
    southpark1.scale = 1;
    southpark1.pruneThreshold = 0.0f;
    southpark1.frameTarget = 200;
    southpark1.minTransitionLength = 50;
    southpark1.numTransitions = 10;          

    southpark2.filename = "southpark2.mp4";
    southpark2.fileout = "southpark2_out";
    southpark2.sigma = 0.04f;
    southpark2.scale = 1;
    southpark2.pruneThreshold = 0.0f;    
    southpark2.frameTarget = 200;
    southpark2.minTransitionLength = 50;
    southpark2.numTransitions = 10;              
}


/**
 * Program entry point
 */
int main (int argc, const char * argv[])
{

    initFiles();
    
    string videoPath = "/Users/leonardteo/Movies/";
    string cachePath = "/Users/leonardteo/Desktop/Temp/videotexture_cache/";
    
    //WHICH FILE TO READ AND WRITE
    FileSetting fileSetting = vtclock_deadend;
    
    int image_scale = fileSetting.scale;
    double sigma = fileSetting.sigma;
    //double pruneThreshold = fileSetting.pruneThreshold;
    
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
        
        //return 0;
        
        videoTexture->findTransitions(videoTexture->anticipatedFutureCostMatrix, fileSetting.numTransitions, fileSetting.minTransitionLength);
        TransitionsTable* transitionsTable = videoTexture->generateTransitionsTable(videoTexture->transitions, fileSetting.frameTarget);
        
        //Debug the table
        //transitionsTable->print();
        
        videoTexture->writeVideoTextures(transitionsTable, videoPath + fileSetting.fileout);
        
    } catch (string e) {
        cout << e << endl;
        return 1;
    }
    return 0;
        
        
}

