//
//  VideoTexture.h
//  VideoTexture
//
//  Created by Leonard Teo on 11-10-23.
//  Copyright 2011 Leonard Teo. All rights reserved.
//

#include <iostream>

//Include OpenCV libraries
#include "core.hpp"
#include "imgproc.hpp"

#ifndef VIDEOTEXTURE_H
#define VIDEOTEXTURE_H

using namespace std;

/**
 * Class definition for VideoTexture
 */
class VideoTexture {
    
private:
    
    //Frames
    cv::Mat *frames;
    cv::Mat *greyscaleFrames; //Greyscale versions of the same frames for analysis
    
    //FrameCount
    int frameCount;
    
    //Framerate
    double frameRate;
    
    //Width and height of frames
    int width;
    int height;
    
    //Frame distance matrix
    double **frameDistanceMatrix;
    
    //Frame probability matrix
    double **frameProbabilityMatrix;
    
    //Sigma - magic number that controls probabilities
    double sigma;
    
    //Weighted frame distance matric and probability matrices for preserving dynamics
    double **weightedFrameDistanceMatrix;
    double **weightedFrameProbabilityMatrix;
    
public:
    
    //Constructor
    VideoTexture(string file, double sigma);
    
    //Load a video
    void loadVideo(string file);
    
    //Generates the frameDiffMatrix and writes to a cache file
    void generateFrameDistanceMatrix(string file);
    
    //Loads the frameDiffMatrix to a file
    void loadFrameDiffMatrix(string file);
    
    //Play the video
    void playVideo();
    
    //Calculate the sum squared distance between two frames
    double getDistanceBetweenFrames(const cv::Mat& image1, const cv::Mat& image2);
    
    //Generates greyscale frames for analysis
    void generateGreyscaleFrames();
    
    //Plays back the greyscale version of the video
    void playGreyscaleVideo();
    
    //Debug an individual frame
    void debugFrame(int frame);
    
    //Print the frame distance matrix
    void printFrameDistanceMatrix();
    
    //Generate the frameProbabilityMatrix
    void generateProbabilityMatrix();
    
    //Print the frame probability matrix
    void printProbabilityMatrix();
    
    //Show probability graph
    void showProbabilityGraph(int scale = 10);
    
    //Show the distance graph
    void showDistanceGraph(int scale = 10);
    
    //Get average distance (for calculating sigma)
    double getAverageDistance();
    
    //Preserve dynamics by applying weighted kernel
    void preserveDynamics();
    
};

#endif