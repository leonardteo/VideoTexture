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
    cout << "Hello, OpenCV!\n";

    //Name of the file
    string filename = "/Users/leonardteo/Movies/testmovie2.mov";
    string cachefile = "/Users/leonardteo/Desktop/Temp/frameDiffCache.txt";
    
    VideoTexture *videoTexture; 
    
    try {
        //Create the new video texture
        videoTexture = new VideoTexture(filename, 0.5f);   
        
        //Load the cache file
        videoTexture->loadFrameDiffMatrix(cachefile);
        //videoTexture->showDistanceGraph();
        
        //Generate the cache file
        //videoTexture->generateFrameDistanceMatrix(cachefile);

        //Debug the matrix
        //videoTexture->printFrameDifferenceMatrix();
        
        videoTexture->generateProbabilityMatrix();
        //videoTexture->printProbabilityMatrix();
        
        videoTexture->showProbabilityGraph();
        
        
    } catch (string e) {
        cout << e << endl;
        return 1;
    }
    return 0;
        
        
}

