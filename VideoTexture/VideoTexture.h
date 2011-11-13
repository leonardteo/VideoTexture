//
//  VideoTexture.h
//  VideoTexture
//
//  Created by Leonard Teo on 11-10-23.
//  Copyright 2011 Leonard Teo. All rights reserved.
//

#include <iostream>
#include <exception>
#include <math.h>
#include <fstream>
#include <algorithm>

//Include OpenCV libraries
#include "core.hpp"
#include "imgproc.hpp"
#include "highgui.hpp"

//My libraries
#include "VideoLoop.h"
#include "Transition.h"
#include "TransitionsTable.h"


#ifndef VIDEOTEXTURE_H
#define VIDEOTEXTURE_H

using namespace std;


/**
 * Class definition for VideoTexture
 */
class VideoTexture {
    
public:
    
    //Frames
    cv::Mat *frames;
    cv::Mat *greyscaleFrames; //Greyscale versions of the same frames for analysis
    
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
    
    //Weighted frame distance matrix and probability matrices for preserving dynamics
    double **weightedFrameDistanceMatrix;
    double **weightedFrameProbabilityMatrix;
    
    //Anticipated future cost matrix
    double **anticipatedFutureCostMatrix;
    double **anticipatedFutureCostProbabilityMatrix;
    
    //FrameCount
    int frameCount;    
    
    //list of transitions
    vector<Transition*>* transitions;
    
    
    //METHODS
    
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
    
    //Generate the frameProbabilityMatrix
    void generateProbabilityMatrix();
    
    //Get average distance (for calculating sigma)
    double getAverageDistance();
        
    //Generate the weighted distance matrix
    void generateWeightedFrameDistanceMatrix();
    
    //Generate the weighted probability matrix
    void generateWeightedProbabilityMatrix();
    
    //Avoid dead ends
    void generateAnticipatedFutureCostMatrix(double p = 1, double alpha = 0.995, double convergenceThreshold = 0.001f);
    
    //Generic function for showing a matrix
    void printMatrix(double** matrix, int size);
    
    //Generic function for displaying a matrix graphically
    void showMatrix(string name, double** matrix, int size, bool invert = false, int scale = 10);
    
    //Stochastically get next frame based on probability
    int getNextFrameStochastically(int currentFrame, double** matrix);
    
    //Playback
    void randomPlay(double** matrix, double pruneThreshold, bool crossFade = true);
    
    //Lerp
    uchar lerp(uchar from, uchar to, float amount);
    
    //Cross fade the frame
    cv::Mat createCrossFadeFrame(cv::Mat& from, cv::Mat& to);
    
    //Generalized method for initializing a matrix
    double** initMatrix(int size);
    
    //Generalized method for normalizing a matrix
    void normalizeMatrixRows(double** matrix);
    void normalizeMatrix(double** matrix);
    
    //Check frame
    int checkFrame(double** matrix, int frame);
    
    //Prune transitions
    void pruneTransitions(double** matrix, double threshold);
    
    //Write video
    
    //Find Transitions
    void findTransitions(double** matrix, int numTransitions = 10, int min_length = 1);
    
    //Generate the transitions table
    TransitionsTable* generateTransitionsTable(vector<Transition*>* transitions, int maxFrames);
    
    static bool transitionsOverlap(Transition transition1, Transition transition2);
    
    VideoLoop* createCompoundLoop(VideoLoop*** table, int numRows, int numColumns, int currentRow, int currentColumn);

    //Write out the video
    void writeVideoTextures(TransitionsTable* transitionsTable, string filename);
    
    VideoLoop* sequenceLoop(VideoLoop* compoundLoop);
    void writeVideoTexture(VideoLoop* compoundLoop, string filename);
    
};

#endif