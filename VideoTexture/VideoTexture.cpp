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
#include <algorithm>
#include "VideoTexture.h"
#include "core.hpp"
#include "highgui.hpp"
#include "imgproc.hpp"

/**
 * Constructor
 */
VideoTexture::VideoTexture(string file, double sigma)
{
    //Initialize random seed
    srand ( time(NULL) );
    
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
    
    cout << "Wrote frame distance matrix to: " << file << endl;
    
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
 * Generate the weighted frame distance matrix
 */
void VideoTexture::generateWeightedFrameDistanceMatrix()
{
    //Initialize the weighted matrix
    this->weightedFrameDistanceMatrix = new double*[this->frameCount];
    for (int i=0; i<this->frameCount; i++)
    {
        this->weightedFrameDistanceMatrix[i] = new double[this->frameCount];
        
        //Preset all values to values from the non weighted version
        for (int j=0; j<this->frameCount; j++)
        {
            this->weightedFrameDistanceMatrix[i][j] = this->frameDistanceMatrix[i][j];
        }
    }        
    
    
    //Algorithm from Schodl et al
    int m = 2;  //2 tap filter  (paper says you can make it 1)
    double w[] = {0.25f, 0.75f, 0.75f, 0.25f};  //4 tap weighted kernels
    
    for (int row=m; row<(this->frameCount - (m-1)); row++)
    {
        for (int col=m; col<(this->frameCount - (m-1)); col++)
        {
            //Calculate D'[i,j]
            double sum = 0.0f;
            
            int w_index = 0;    //I can't figure out a better way to access w
            
            for (int k=-m; k<m; k++)
            {
                sum = sum + (w[w_index] * this->frameDistanceMatrix[row+k][col+k]);
                w_index++;
            }
            
            this->weightedFrameDistanceMatrix[row][col] = sum;
        }
    }
    
    //Normalize the distances between 0-1
    for (int row=0; row < this->frameCount; row++)
    {
        
        double max = 0.0f;
        
        //First get the max
        for (int col=0; col < this->frameCount; col++)
        {
            if (this->weightedFrameDistanceMatrix[row][col] > max)
            {
                max = this->weightedFrameDistanceMatrix[row][col];
            }
        }
        
        for (int col=0; col < this->frameCount; col++)
        { 
            this->weightedFrameDistanceMatrix[row][col] /= max;
        }
    }
    
}

/**
 * Generate weighted probability matrix
 */
void VideoTexture::generateWeightedProbabilityMatrix()
{
    //Initialize probability matrix
    this->weightedFrameProbabilityMatrix = new double*[this->frameCount];
    for (int i=0; i<this->frameCount; i++)
    {
        this->weightedFrameProbabilityMatrix[i] = new double[this->frameCount];
        
        //Preset all values to 0
        for (int j=0; j<this->frameCount; j++)
        {
            this->weightedFrameProbabilityMatrix[i][j] = 0.0f;
        }
    }    
    
    //Calculate the probability for each frame
    //probability[i,j] = exp(-D[i+1, j]/sigma
    
    for (int row=0; row < this->frameCount - 1; row++)
    {
        for (int col=0; col < this->frameCount; col++)
        {
            this->weightedFrameProbabilityMatrix[row][col] = exp(-this->weightedFrameDistanceMatrix[row+1][col]/this->sigma);
        }
    }
    
    
    //Normalize the matrix so that each row adds up to 1
    for (int row=0; row < this->frameCount - 1; row++)
    {
        double sum = 0.0f;
        
        //First loop calculates the sum
        for (int col=0; col < this->frameCount; col++)
        {
            sum += this->weightedFrameProbabilityMatrix[row][col];
        }
        
        //Second loop normalizes each value
        for (int col=0; col < this->frameCount; col++)
        {
            this->weightedFrameProbabilityMatrix[row][col] /= sum;
        }
        
    }    
    
}


/**
 * Anticipate future cost using Shodl et al
 * @param double p
 * @param double alpha
 * @param int passes Number of times you want this to run
 */
void VideoTexture::generateAnticipateFutureCostMatrix(double p, double alpha, int passes)
{
    //First, initialize the new anticipated future cost matrix of D'' 
    this->anticipatedFutureCostMatrix = new double*[this->frameCount];
    for (int i=0; i<this->frameCount; i++)
    {
        this->anticipatedFutureCostMatrix[i] = new double[this->frameCount];
        
        //Initialize default values of D''ij as D'ij^p  (D'ij is the weighted distance matrix)
        for (int j=0; j<this->frameCount; j++)
        {
            this->anticipatedFutureCostMatrix[i][j] = pow(this->weightedFrameDistanceMatrix[i][j], p);
        }
    } 
    
    //Loop through passes
    for (int pass=0; pass < passes; pass++)
    {
        //Initialize array m, which holds the minimum for each row
        double *m = new double[this->frameCount];
        
        cout << "Pass: " << pass << endl;
        
        //Step 1 - Find the minimum distance for each row  min_k D''jk
        for (int j = 0; j < this->frameCount; j++)
        {
            //Find m[j], the cost of the best transition
            m[j] = this->anticipatedFutureCostMatrix[j][0];   //Set to the first number
            int index_min_k = 0;
            for (int k=0; k<this->frameCount; k++)
            {
                if (this->anticipatedFutureCostMatrix[j][k] < m[j] && j != k)
                {
                    m[j] = this->anticipatedFutureCostMatrix[j][k];
                    index_min_k = k;
                }
            }
            cout << "Value min_k(D''jk) min cost of transition from j= " << j << " is: " << m[j] << ", k= " << index_min_k << endl;;
        }
        
        //Step 2 Calculate new D''ij
        for (int i=this->frameCount-1; i>=0; i--)
        {
            for (int j=0; j<this->frameCount; j++)
            {
                this->anticipatedFutureCostMatrix[i][j] = pow(this->weightedFrameDistanceMatrix[i][j], p) + (alpha * m[j]);
                
            }
        }
    }
    
    //Normalize the distances between 0-1
    for (int row=0; row < this->frameCount; row++)
    {
        
        double max = 0.0f;
        
        //First get the max
        for (int col=0; col < this->frameCount; col++)
        {
            if (this->anticipatedFutureCostMatrix[row][col] > max)
            {
                max = this->anticipatedFutureCostMatrix[row][col];
            }
        }
        
        for (int col=0; col < this->frameCount; col++)
        { 
            this->anticipatedFutureCostMatrix[row][col] /= max;
        }
    }


    
    //Calculate probability matrix based on anticipated future cost matrix
    //Initialize probability matrix
    this->anticipatedFutureCostProbabilityMatrix = new double*[this->frameCount];
    for (int i=0; i<this->frameCount; i++)
    {
        this->anticipatedFutureCostProbabilityMatrix[i] = new double[this->frameCount];
        
        //Preset all values to 0
        for (int j=0; j<this->frameCount; j++)
        {
            this->anticipatedFutureCostProbabilityMatrix[i][j] = 0.0f;
        }
    }    
    
    //Calculate the probability for each frame
    //probability[i,j] = exp(-D[i+1, j]/sigma
    
    for (int row=0; row < this->frameCount - 1; row++)
    {
        for (int col=0; col < this->frameCount; col++)
        {
            this->anticipatedFutureCostProbabilityMatrix[row][col] = exp(-this->anticipatedFutureCostMatrix[row+1][col]/this->sigma);
        }
    }
    
    //Normalize the distances between 0-1
    for (int row=0; row < this->frameCount; row++)
    {
        
        double max = 0.0f;
        
        //First get the max
        for (int col=0; col < this->frameCount; col++)
        {
            if (this->anticipatedFutureCostProbabilityMatrix[row][col] > max)
            {
                max = this->anticipatedFutureCostProbabilityMatrix[row][col];
            }
        }
        
        for (int col=0; col < this->frameCount; col++)
        { 
            if (max > 0.0f)
                this->anticipatedFutureCostProbabilityMatrix[row][col] /= max;
            else
                this->anticipatedFutureCostProbabilityMatrix[row][col] = 0.0f;
        }
    } 
}


/**
 * Gets the highest probability next frame
 * @param int currentFrame - the current frame
 * @param double** matrix - the matrix to read from
 */
int VideoTexture::getNextFrameStochastically(int currentFrame, double** matrix)
{
    int size = 0; 
    
    //For each col at row[currentframe] add all the numbers together
    for (int col=0; col<this->frameCount-1; col++)
    {
        size += round(matrix[currentFrame][col] * 1000.0f);
    }
    
    //Create an array of size. This is our selection array
    int selectArray[size];
    
    int position = 0;   //This tracks our position as we fill the array

    //Fill the select array
    for (int col=0; col<this->frameCount-1; col++)
    {
        //Localsize is the size that the probability gives us
        int localSize = round(matrix[currentFrame][col] * 1000.0f);
        
        //Fill the selectArray with the current col index based on localsize
        for (int i=position; i<position+localSize; i++)
        {
            selectArray[i] = col;
        }
        
        position += localSize;
    }
    
    //Test
    /*
    cout << "Size: " << size << endl;
    for (int i=0; i<size; i++)
    {
        cout << selectArray[i] << " ";
    }
     */
    
    return selectArray[rand() % size];
    
}

/**
 * Plays the video texture
 * @param double** matrix to play from
 */
void VideoTexture::randomPlay(double** matrix)
{
    bool stop = false;
    int delay = 1000 / this->frameRate;
    
    //Open a window
    cv::namedWindow("Video Texture");
    
    int currentFrame = rand() % this->frameCount;
    
    while (!stop)
    {
        //Display the current frame
        cv::imshow("Video Texture", this->frames[currentFrame]);
        
        cout << "Playing frame: " << currentFrame << endl;

        //Get the highest probability next transition
        currentFrame = this->getNextFrameStochastically(currentFrame, matrix);
        
        //Wait for key or delay
        if (cv::waitKey(delay) >= 0)
        {
            stop = true;
        }
    }
}

/**
 * Generic debug method for printing the values of a matrix
 * @param double** matrix
 * @param int size
 */
void VideoTexture::printMatrix(double **matrix, int size)
{
    for (int row=0; row<size; row++)
    {
        for (int col=0; col<size; col++)
        {
            cout << matrix[row][col] << " ";
        }
        cout << endl;
    }
}

/**
 * show a matrix as an image
 * @param string name
 * @param double** matrix
 * @param int size
 * @param bool invert
 * @param int scale
 */
void VideoTexture::showMatrix(string name, double** matrix, int size, bool invert, int scale)
{
    //First normalize the matrix for viewing. The matrix needs to be between 0-1. 
    double max = 0.0f;
    for (int row=0; row<size; row++)
    {
        for (int col=0; col<size; col++)
        {
            if (matrix[row][col] > max)
            {
                max = matrix[row][col];
            }
        }
    }
    
    //Now create a new matrix to view and normalize the data
    double **viewMatrix = new double*[size];
    for (int row=0; row<size; row++)
    {
        viewMatrix[row] = new double[size];
        for (int col=0; col<size; col++)
        {
            viewMatrix[row][col] = matrix[row][col]/max;
        }
    }
    
    
    //Create a new nxn image
    int n = size;  //Create a dimension
    cv::Mat image(cv::Size(n, n), CV_8UC1);
    
    //Plot the matrix
    for (int y=0; y<n; y++)
    {
        for (int x=0; x<n; x++)
        {
            double color = viewMatrix[x][y];
            
            //Invert the color
            if (invert)
            {   
                color -= 0.5f;
                if (color < 0.0f)
                {
                    color += 1.0f;
                }            
            }
            
            color *= 255.0f;
            
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
        cv::imshow(name, displayImage);
        
        //Wait for key or delay
        if (cv::waitKey())
        {
            stop = true;
        }
    }
}
