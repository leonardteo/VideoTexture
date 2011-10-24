//
//  VideoTexture.cpp
//  VideoTexture
//
//  Created by Leonard Teo on 11-10-23.
//  Copyright 2011 Leonard Teo. All rights reserved.
//

#include <exception>
#include <math.h>
#include <fstream>
#include "VideoTexture.h"
#include "core.hpp"
#include "highgui.hpp"
#include "imgproc.hpp"

/**
 * Constructor
 */
VideoTexture::VideoTexture(string file, double sigma)
{
    //Pre-initialize class properties
    this->frameCount = 0;
    this->frameRate = 0.0f;
    this->width = 0;
    this->height = 0;
    
    this->sigma = sigma;
    
    //Load the video
    try {
        this->loadVideo(file);
    } catch (string e)
    {
        throw e;    //Duck!
    }
}

/**
 * Loads a video file
 * @param string file
 */
void VideoTexture::loadVideo(string file)
{
    cout << "Loading video: " << file << endl;
    
    //Read the video
    cv::VideoCapture capture(file);
    if (!capture.isOpened())
    {
        throw string("Couldn't open file");
    }
    
    //Analyze the video
    this->frameRate = capture.get(CV_CAP_PROP_FPS);
    this->width = (int) capture.get(CV_CAP_PROP_FRAME_WIDTH);
    this->height = (int) capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    
    cout << "Framerate: " << this->frameRate << endl;
    cout << "Width: " << this->width << endl;
    cout << "Height: " << this->height << endl;    
    
    //Run through the frames once to get the real number of frames
    cv::Mat frame;  //Temp frame
    while (capture.read(frame))
    {
        this->frameCount++;
    }
    capture.release();
    
    cout << "Number of real frames in sequence: " << this->frameCount << endl;
    
    //Load the frames into the frame array
    this->frames = new cv::Mat[this->frameCount];
    
    capture.open(file);
    if (!capture.isOpened())
    {
        throw string ("Couldn't open file");
    }
    
    for (int i=0; i<frameCount; i++)
    {
        capture.read(frame);
        frame.copyTo(this->frames[i]);  //Need to copy the actual frame
    }
    capture.release();
    
}

/**
 * Play the loaded video
 */
void VideoTexture::playVideo()
{
    cout << "Playing video straight through" << endl;
    
    bool stop = false;
    int delay = 1000 / this->frameRate;
    int currentFrame = 0;
    
    //Open a window
    cv::namedWindow("Video");
    
    while (!stop)
    {
        //Display the current frame
        cv::imshow("Video", this->frames[currentFrame]);
        
        currentFrame++;
        
        //Wait for key or delay
        if (cv::waitKey(delay) >= 0)
        {
            stop = true;
        }
    }
}

/**
 * Play the greyscale video
 */
void VideoTexture::playGreyscaleVideo()
{
    cout << "Playing greyscale video straight through" << endl;
    
    bool stop = false;
    int delay = 1000 / this->frameRate;
    int currentFrame = 0;
    
    //Open a window
    cv::namedWindow("Video");
    
    while (!stop)
    {
        //Display the current frame
        cv::imshow("Video", this->greyscaleFrames[currentFrame]);

        currentFrame++;
        
        //Wait for key or delay
        if (cv::waitKey(delay) >= 0)
        {
            stop = true;
        }
    }
}

/**
 * Calculate the L2 distance between two frames
 * Requires both frames to have 1 channel
 */
double VideoTexture::getDistanceBetweenFrames(const cv::Mat& image1, const cv::Mat& image2)
{
    double sum = 0.0f;
    
    //Analyze each pixel
    for (int y=0; y<this->height; y++)
    {
        for (int x=0; x<this->width; x++)
        {

            uchar pixelValue1 = image1.at<uchar>(y, x);
            uchar pixelValue2 = image2.at<uchar>(y, x);

            if (pixelValue1 != pixelValue2)
            {
                double diff = ((double) pixelValue1/255.0f) - ( (double) pixelValue2/255.0f);
                sum = sum + (diff*diff);    //Distance squared
            }
        }
    }
    
    return sqrt(sum);
}


/**
 * Generates greyscale frames for analysis
 */
void VideoTexture::generateGreyscaleFrames()
{
    cout << "Generating greyscale frames" << endl;
    
    this->greyscaleFrames = new cv::Mat[this->frameCount];
    
    for (int i=0; i<this->frameCount; i++)
    {
        //Generate the greyscale frame
        cv::cvtColor(this->frames[i], this->greyscaleFrames[i], CV_RGB2GRAY);
    }
}


/**
 * Generates the frame diff matrix and writes to a cache file
 * @param string file
 */
void VideoTexture::generateFrameDistanceMatrix(string file)
{
    //First convert all frames to greyscale
    this->generateGreyscaleFrames();
    
    //Initialize the arrays
    this->frameDistanceMatrix = new double*[this->frameCount];
    for (int i=0; i<this->frameCount; i++)
    {
        this->frameDistanceMatrix[i] = new double[this->frameCount];
        
        //Preset all values to 0
        for (int j=0; j<this->frameCount; j++)
        {
            this->frameDistanceMatrix[i][j] = 0.0f;
        }
    }

    //Calculate the distance between each frame
    for (int row=0; row < this->frameCount; row++)
    {
        cout << "Calculating distances for frame: " << row << endl;
        for (int col=0; col < this->frameCount; col++)
        {
            double diff = this->getDistanceBetweenFrames(this->greyscaleFrames[row], this->greyscaleFrames[col]);
            this->frameDistanceMatrix[row][col] = diff;
            //cout << "Distance between frames " << row << " and " << col << ": " << diff << endl;
        }
    }
    
    cout << "Normalizing frame distances..." << endl;
    
    //Normalize the distances between 0-1
    for (int row=0; row < this->frameCount; row++)
    {
        
        double max = 0.0f;
        
        //First get the max
        for (int col=0; col < this->frameCount; col++)
        {
            if (this->frameDistanceMatrix[row][col] > max)
            {
                max = this->frameDistanceMatrix[row][col];
            }
        }
        
        for (int col=0; col < this->frameCount; col++)
        { 
            this->frameDistanceMatrix[row][col] /= max;
            //cout << "Normalized distance between frames " << row << " and " << col << ": " << this->frameDistanceMatrix[row][col] << endl;
        }
    }
    
    //Open the file handler to write
    fstream outfile;
    outfile.open(file.c_str(), ios::out);
    
    if (!outfile.is_open())
    {
        throw string("Could not open " + file);
    }
    
    //Write distances to file
    for (int row=0; row < this->frameCount; row++)
    {
        
        for (int col=0; col < this->frameCount; col++)
        { 
            outfile << this->frameDistanceMatrix[row][col] << endl;
        }
    }    
    
    //Close the file handler
    outfile.close();
    
}


/**
 * Loads the frame diff matrix from a file
 * @param string file
 */
void VideoTexture::loadFrameDiffMatrix(string file)
{
    //Initialize matrix
    this->frameDistanceMatrix = new double*[this->frameCount];
    for (int i=0; i<this->frameCount; i++)
    {
        this->frameDistanceMatrix[i] = new double[this->frameCount];
        
        //Preset all values to 0
        for (int j=0; j<this->frameCount; j++)
        {
            this->frameDistanceMatrix[i][j] = 0.0f;
        }
    }    
    
    //open the file
    fstream infile;
    infile.open(file.c_str(), ios::in);
    if (!infile.is_open())
    {
        throw string("Couldn't open file " + file);
    }
    
    string line;
    
    for (int row=0; row < this->frameCount; row++)
    {
        for (int col=0; col < this->frameCount; col++)
        {
            if (infile.good())
            {
                getline(infile, line);
                this->frameDistanceMatrix[row][col] = atof(line.c_str());
            }
        }
    }
}


/**
 * Debug a frame
 */
void VideoTexture::debugFrame(int frameNum)
{
    //Show the current frame
    bool stop = false;
    
    //Open a window
    cv::namedWindow("Image");
    
    cv::Mat frame = this->greyscaleFrames[frameNum];
    
    int width = frame.cols;
    int height = frame.rows;
    cout << "Frame width: " << width << endl;
    cout << "Frame height: " << height << endl;
    
    for (int y=0; y<height; y++)
    {
        for (int x=0; x<width; x++)
        {
            uchar value = frame.at<uchar>(y, x);
            double valuef = (double)value/255.0f;
            cout << "Pixel (" << x << ", " << y << "): " << valuef << endl;
        }
    }
    
    while (!stop)
    {
        //Display the current frame
        cv::imshow("Image", frame);
        
        //Wait for key or delay
        if (cv::waitKey())
        {
            stop = true;
        }
    }
}

/**
 * Debug the frame distance matrix by printing all values to cout
 */
void VideoTexture::printFrameDistanceMatrix()
{
    for (int row=0; row < this->frameCount; row++)
    {
        for (int col=0; col < this->frameCount; col++)
        {
            cout << this->frameDistanceMatrix[row][col] << " ";
        }
        
        cout << endl;
    }
}

/**
 * Generate the probability matrix
 */
void VideoTexture::generateProbabilityMatrix()
{
    //Initialize probability matrix
    this->frameProbabilityMatrix = new double*[this->frameCount];
    for (int i=0; i<this->frameCount; i++)
    {
        this->frameProbabilityMatrix[i] = new double[this->frameCount];
        
        //Preset all values to 0
        for (int j=0; j<this->frameCount; j++)
        {
            this->frameProbabilityMatrix[i][j] = 0.0f;
        }
    }    
    
    //Calculate the probability for each frame
    //probability[i,j] = exp(-D[i+1, j]/sigma
    
    for (int row=0; row < this->frameCount - 1; row++)
    {
        for (int col=0; col < this->frameCount; col++)
        {
            this->frameProbabilityMatrix[row][col] = exp(-this->frameDistanceMatrix[row+1][col]/this->sigma);
        }
    }
    
    //Normalize the matrix so that each row adds up to 1
    /*
    for (int row=0; row < this->frameCount - 1; row++)
    {
        double sum = 0.0f;
        
        //First loop calculates the sum
        for (int col=0; col < this->frameCount; col++)
        {
            sum += this->frameProbabilityMatrix[row][col];
        }
        
        //Second loop normalizes each value
        for (int col=0; col < this->frameCount; col++)
        {
            this->frameProbabilityMatrix[row][col] /= sum;
        }
        
    }
     */
    
}


/**
 * prints out the probability matrix
 */
void VideoTexture::printProbabilityMatrix()
{
    for (int row=0; row < this->frameCount; row++)
    {
        for (int col=0; col < this->frameCount; col++)
        {
            cout << this->frameProbabilityMatrix[row][col] << " ";
        }
        
        cout << endl;
    }    
}


/**
 * Show the probability graph
 */
void VideoTexture::showProbabilityGraph(int scale)
{
    //Create a new nxn image
    int n = this->frameCount;  //Create a dimension
    cv::Mat image(cv::Size(n, n), CV_8UC1);
    
    //Plot the probabilities
    for (int y=0; y<n; y++)
    {
        for (int x=0; x<n; x++)
        {
            float color = this->frameProbabilityMatrix[x][y] * 255.0f;    //Figure out the color based on the probability
            image.at<uchar>(y, x) = (uchar)color;
        }
    }
    
    cv::Mat displayImage;
    cv::resize(image, displayImage, cv::Size(n*scale, n*scale));
    
    //Display the image
    bool stop = false;
    
    while (!stop)
    {
        //Display the current frame
        cv::imshow("Frame Probability Matrix", displayImage);
        
        //Wait for key or delay
        if (cv::waitKey())
        {
            stop = true;
        }
    }
    
}


/**
 * Show the distance graph
 */
void VideoTexture::showDistanceGraph(int scale)
{
    //Create a new nxn image
    int n = this->frameCount;  //Create a dimension
    cv::Mat image(cv::Size(n, n), CV_8UC1);
    
    //Plot the distances
    for (int y=0; y<n; y++)
    {
        //Get the maximum x value
        double max = 0.0f;
        for (int x=0; x<n; x++)
        {
            if (this->frameDistanceMatrix[x][y] > max)
            {
                max = this->frameDistanceMatrix[x][y];
            }
        }
        
        //Loop through again and get the lerp
        for (int x=0; x<n; x++)
        {
            //Invert the color
            double color = (this->frameDistanceMatrix[x][y]/max) - 0.5f;
            if (color < 0.0f)
            {
                color += 1.0f;
            }
            color *= 255.0f;    //Make it in terms of 0-255
            image.at<uchar>(y, x) = (uchar)color;
        }
    }
    
    cv::Mat displayImage;
    cv::resize(image, displayImage, cv::Size(n*scale, n*scale));
    
    //Display the image
    bool stop = false;
    
    while (!stop)
    {
        //Display the current frame
        cv::imshow("Frame Distance Matrix", displayImage);
        
        //Wait for key or delay
        if (cv::waitKey())
        {
            stop = true;
        }
    }
    
}



/**
 * Get average Distance
 */
double VideoTexture::getAverageDistance()
{
    double sum = 0.0f;
    for (int row=0; row<this->frameCount; row++)
    {
        for (int col=0; col<this->frameCount; col++)
        {
            sum += this->frameDistanceMatrix[row][col];
        }
    }
    return sum / (this->frameCount * this->frameCount);
}


/**
 * Preserve dynamics by applying a weighted kernel
 */
void VideoTexture::preserveDynamics()
{
    //Initialize distance matrix
    this->weightedFrameDistanceMatrix = new double*[this->frameCount];
    for (int i=0; i<this->frameCount; i++)
    {
        this->weightedFrameDistanceMatrix[i] = new double[this->frameCount];
        
        //Preset all values to 0
        for (int j=0; j<this->frameCount; j++)
        {
            this->weightedFrameDistanceMatrix[i][j] = 0.0f;
        }
    }
    
}



