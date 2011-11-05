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
    this->frameDistanceMatrix = this->initMatrix(this->frameCount);

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
    
    //cout << "Normalizing frame distances..." << endl;
    
    //Normalize the distances between 0-1
    //this->normalizeMatrix(this->frameDistanceMatrix);
     
    
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
    this->frameDistanceMatrix = this->initMatrix(this->frameCount);
    
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
    
    //Rescale matrix so that its maxima is 1
    this->normalizeMatrix(this->frameDistanceMatrix);
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
    this->frameProbabilityMatrix = this->initMatrix(this->frameCount);
    
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
    this->normalizeMatrixRows(this->frameProbabilityMatrix);

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
    this->weightedFrameDistanceMatrix = this->initMatrix(this->frameCount);
    for (int i=0; i<this->frameCount; i++)
    {
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
    this->normalizeMatrix(this->weightedFrameDistanceMatrix);
    
}

/**
 * Generate weighted probability matrix
 */
void VideoTexture::generateWeightedProbabilityMatrix()
{
    //Initialize probability matrix
    this->weightedFrameProbabilityMatrix = this->initMatrix(this->frameCount);
    
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
    this->normalizeMatrixRows(this->weightedFrameProbabilityMatrix);
    
}


/**
 * Anticipate future cost using Shodl et al
 * @param double p
 * @param double alpha
 * @param int passes Number of times you want this to run
 */
void VideoTexture::generateAnticipatedFutureCostMatrix(double p, double alpha, double convergenceThreshold)
{
    //First, initialize the new anticipated future cost matrix of D'' 
    this->anticipatedFutureCostMatrix = this->initMatrix(this->frameCount);
    for (int i=0; i<this->frameCount; i++)
    {       
        //Initialize default values of D''ij as D'ij^p  (D'ij is the weighted distance matrix)
        for (int j=0; j<this->frameCount; j++)
        {
            this->anticipatedFutureCostMatrix[i][j] = pow(this->weightedFrameDistanceMatrix[i][j], p);
        }
    } 

    //Initialize array m, which holds the minimum for each row
    double m[this->frameCount];
    for (int j=0; j<this->frameCount; j++)
    {
        //m[j] = this->anticipatedFutureCostMatrix[j][0]; //Initialize m[j] to D''j_0
        m[j] = 10000.0f;    //Really high number so it will change on second pass
    }
    
    bool converged = false;
    int passCount = 0;
    
    while (!converged)
    {
        passCount++;
        //cout << "Pass: " << passCount << endl;
        
        //Step 1 - Find the minimum distance for each row  min_k D''jk
        for (int j = 0; j < this->frameCount; j++)
        {
            //Set a new mj
            double m_j = 10000.0f; //set to a really high number so that it WILL change
            int index_min_k = -1;
            
            for (int k=0; k<this->frameCount; k++)
            {
                if (this->anticipatedFutureCostMatrix[j][k] < m_j && j != k)
                {
                    m_j = this->anticipatedFutureCostMatrix[j][k];
                    index_min_k = k;
                }
            }
             
            //cout << "Value min_k(D''jk) min cost of transition from j= " << j << " is: " << m_j << ", k= " << index_min_k << endl;;
            
            //Check what the difference is between the two
            double diff = abs(m[j] - m_j);
            
            if (diff < convergenceThreshold)
            {
                //cout << "MATRIX CONVERGED!!! Difference: " << diff << " < " << convergenceThreshold << endl;;
                converged = true;   
            }
            
            m[j] = m_j;
            
        }
        
        //Step 2 Calculate new D''ij
        for (int i=this->frameCount-1; i>=0; i--)
        {
            for (int j=0; j<this->frameCount; j++)
            {
                this->anticipatedFutureCostMatrix[i][j] = pow(this->weightedFrameDistanceMatrix[i][j], p) + (alpha * m[j]);
                
            }
        }
        
        //Normalize the distances between 0-1
        this->normalizeMatrix(this->anticipatedFutureCostMatrix);
        
    }

    
    //Calculate probability matrix based on anticipated future cost matrix
    //Initialize probability matrix
    this->anticipatedFutureCostProbabilityMatrix = this->initMatrix(this->frameCount);
    
    //Calculate the probability for each frame
    //probability[i,j] = exp(-D[i+1, j]/sigma
    for (int row=0; row < this->frameCount - 1; row++)
    {
        for (int col=0; col < this->frameCount; col++)
        {
            this->anticipatedFutureCostProbabilityMatrix[row][col] = exp(-this->anticipatedFutureCostMatrix[row+1][col]/this->sigma);
        }
    }
    
    //Normalize the probability matrix
    //this->normalizeMatrixRows(this->anticipatedFutureCostProbabilityMatrix);
   
}


/**
 * Initialize a matrix
 */
double** VideoTexture::initMatrix(int size)
{
    double** matrix = new double*[size];
    
    for (int i=0; i<size; i++)
    {
        //Initialize rows
        matrix[i] = new double[size];
        
        for (int j=0; j<size; j++)
        {
            //Initialize columns
            matrix[i][j] = 0.0f;
        }
    }
    return matrix;
}


/**
 * Checks if a frame is ok to use or if it will cause problems
 */
int VideoTexture::checkFrame(double** matrix, int frame)
{
    int size = 0; 
    
    //For each col at row[currentframe] add all the numbers together
    for (int col=0; col<this->frameCount-1; col++)
    {
        size += round(matrix[frame][col] * 1000.0f);
    }
    
    return size;
}

/**
 * Gets the highest probability next frame
 * @param int currentFrame - the current frame
 * @param double** matrix - the matrix to read from
 */
int VideoTexture::getNextFrameStochastically(int currentFrame, double** matrix)
{
    unsigned int size = 0; 
    unsigned int power = 3;
    double multiplication_factor = 1.0f;
    
    //Avoid divide by zero
    while (size == 0)
    {
        power++;
        multiplication_factor = pow((double)10.0f, (double)power);
        for (int col=0; col<this->frameCount-1; col++)
        {
            double foo = matrix[currentFrame][col] * multiplication_factor;
            //cout << "Foo: " << round(foo) << endl;
            size += round(foo);
            //cout << "Size: " << size << endl;
        }
        
    }
    
    
    
    if (size == 0)
    {
        //This is a hack, bail out of here because we're in a bad place
        cout << "Caught critical error. Sum of all columns on probability matrix is 0!" << endl;
        double sum = 0.0f;
        for (int col=0; col<this->frameCount-1; col++)
        {
            sum += (matrix[currentFrame][col] * multiplication_factor);
            cout << matrix[currentFrame][col] << " ";
        }
        cout << "Sum: " << sum << endl;
        throw "Divide by zero error pending!";
    }
    
    //Create an array of size. This is our selection array
    int selectArray[size];
    
    int position = 0;   //This tracks our position as we fill the array

    //Fill the select array
    for (int col=0; col<this->frameCount-1; col++)
    {
        //Localsize is the size that the probability gives us
        int localSize = round(matrix[currentFrame][col] * multiplication_factor);
        
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
    
    int nextFrame = selectArray[rand() % size];  
    return nextFrame;
    
}

/**
 * Plays the video texture
 * @param double** matrix to play from
 */
void VideoTexture::randomPlay(double** matrix, double pruneThreshold, bool crossFade)
{
    bool stop = false;
    int delay = 1000 / this->frameRate;
    
    //Open a window
    cv::namedWindow("Video Texture");
    
    //Copy the matrix and normalize each row it so we don't get divide by zero errors
    double** playMatrix = this->initMatrix(this->frameCount);
    for (int i=0; i<this->frameCount; i++)
    {
        for (int j=0; j<this->frameCount; j++)
        {
            playMatrix[i][j] = matrix[i][j];
        }
    }
    this->normalizeMatrixRows(playMatrix);
    
    //Prune transitions
    this->pruneTransitions(playMatrix, pruneThreshold);
    
    //Check if this matrix is actually playable
    for (int i=0; i<this->frameCount - 1; i++)
    {
        int countNumbersAboveZero = 0;
        for (int j=0; j<this->frameCount; j++)
        {
            if (playMatrix[i][j] > 0.0f)
                countNumbersAboveZero++;
        }
        if (countNumbersAboveZero <= 1)
        {
            this->printMatrix(playMatrix, this->frameCount);
            cout << "Error with transitions at frame: " << i << ". There no transitions out of here.";
            throw "Error";
        }
    }

    
    //Debug the matrix
    //this->printMatrix(playMatrix, this->frameCount);
    
    int currentFrame = this->getNextFrameStochastically(0, playMatrix);
    
    while (!stop)
    {
        //Display the current frame
        cv::imshow("Video Texture", this->frames[currentFrame]);
        
        cout << "Playing frame: " << currentFrame << endl;

        //Get the highest probability next transition
        int nextFrame = this->getNextFrameStochastically(currentFrame, playMatrix);
        
        if (crossFade)
        {
            if (abs(nextFrame - currentFrame) > 1)
            {
                cv::Mat fadeFrame = this->createCrossFadeFrame(this->frames[currentFrame], this->frames[nextFrame]);
                if (cv::waitKey(delay) >= 0)
                {
                    stop = true;
                }
                cout << "Playing frame: crossfade" << endl;
                cv::imshow("Video Texture", fadeFrame);
            }            
        }
        currentFrame = nextFrame;
        
        //Wait for key or delay
        if (cv::waitKey(delay) >= 0)
        {
            stop = true;
        }
    }
}

/**
 * Prunes lousy transitions
 */
void VideoTexture::pruneTransitions(double** matrix, double threshold)
{
    for (int i=0; i<this->frameCount; i++)
    {
        for (int j=0; j<this->frameCount; j++)
        {
            if (matrix[i][j] < threshold)
            {
                matrix[i][j] = 0.0f;
            }
        }
    }
    
    this->normalizeMatrixRows(matrix);
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

/**
 * Linear interpolation
 */
uchar VideoTexture::lerp(uchar from, uchar to, float amount)
{
    //Get the difference between from and to
    return from + (amount * (to - from));
    
}


/**
 * Create a cross fade frame
 */
cv::Mat VideoTexture::createCrossFadeFrame(cv::Mat& from, cv::Mat& to)
{
    
    cv::Mat result;
    from.copyTo(result);
    
    for (int y = 0; y<from.rows; y++)
    {
        for (int x = 0; x<from.cols; x++)
        {              
            //Do the linear interpolation
            result.at<cv::Vec3b>(y,x)[0] = lerp(from.at<cv::Vec3b>(y,x)[0], to.at<cv::Vec3b>(y,x)[0], 0.5f);; 
            result.at<cv::Vec3b>(y,x)[1] = lerp(from.at<cv::Vec3b>(y,x)[1], to.at<cv::Vec3b>(y,x)[1], 0.5f);;
            result.at<cv::Vec3b>(y,x)[2] = lerp(from.at<cv::Vec3b>(y,x)[2], to.at<cv::Vec3b>(y,x)[2], 0.5f);;
            
            //Debug
            //cout << "(" << (int)redFrom << "," << (int)greenFrom << "," << (int)blueFrom << ") to (" << (int)redTo << "," << (int)greenTo << "," << (int)blueTo << ") = (" << (int)redResult << "," << (int)greenResult << "," << (int)blueResult << ")" << endl;
            
        }
    }
    
    return result;
    
}

/**
 * Normalize a matrix
 */
/*
void VideoTexture::normalizeMatrix(double **matrix)
{
    //Normalize the ENTIRE matrix, not just each row
    double max = 0.0f;  //First pass calculates the maximum in the entire matrix
    for (int row=0; row < this->frameCount; row++)
    {
        //First get the max
        for (int col=0; col < this->frameCount; col++)
        {
            if (matrix[row][col] > max)
            {
                max = matrix[row][col];
            }
        }
    }
    
    //Second pass normalizes each item
    for (int row=0; row < this->frameCount; row++)
    {
        for (int col=0; col < this->frameCount; col++)
        { 
            matrix[row][col] /= max;
        }
    }
    
    
}*/

/**
 * Normalizes each row so that it adds up to 1
 */
void VideoTexture::normalizeMatrixRows(double **matrix)
{
    //For each row
    for (int row=0; row < this->frameCount; row++)
    {
        double sum = 0.0f;
        
        //First get the max
        for (int col=0; col < this->frameCount; col++)
        {
            sum += matrix[row][col];
        }
        
        //Divide the current value by the max for the row
        for (int col=0; col < this->frameCount; col++)
        { 
            if (sum > 0.0f)
                matrix[row][col] /= sum;
            else 
                matrix[row][col] = 0.0f;
        }
    }
}


/**
 * Normalizes the entire matrix so that its maxima is 1
 * @todo rename this function.
 */
void VideoTexture::normalizeMatrix(double **matrix)
{
    //double sum = 0.0f;
    double max = 0.0f;
    
    for (int row=0; row < this->frameCount; row++)
    {
        for (int col=0; col < this->frameCount; col++)
        {
            if (matrix[row][col] > max)
            {
                max = matrix[row][col];
            }
        }
    }
    
    for (int row=0; row < this->frameCount; row++)
    {        
        for (int col=0; col < this->frameCount; col++)
        { 
            matrix[row][col] /= max;
        }
    }    
}


/**
 * Find the best transitions in a video
 */
void VideoTexture::findTransitions(double** matrix, int numTransitions)
{
    vector<Transition> transitions;
    
    //Find the minimum numTransitions transitions
    int count = 0;
    
    
    //First calculate the global max
    double max = 0.0f;
    for (int i=0; i<this->frameCount; i++)
    {
        for (int j=0; j<i; j++)
        {
            if (matrix[i][j] > max)
            {
                max = matrix[i][j];
            }
        }
    }
    
    double global_min = 0.0f;   //Var that controls the minimum transition to check against
    
    while (count < numTransitions)
    {
        //Find the best minimum cost transition where i>=j
        
        //Calculate min
        double min = max;
        int min_i = 0;
        int min_j = 0;
        for (int i=0; i<this->frameCount; i++)
        {
            for (int j=0; j<i; j++)
            {
                if (matrix[i][j] < min && matrix[i][j] > global_min)
                {
                    min = matrix[i][j];
                    min_i = i;
                    min_j = j;
                }
            }
        }
        
        global_min = min;
        
        Transition transition;
        transition.cost = min;
        transition.startFrame = min_i;
        transition.endFrame = min_j;
        transition.length = min_i - min_j;
        
        transitions.push_back(transition);
        
        //cout << "Best transition: " << transition.startFrame << " to " << transition.endFrame << " costing: " << transition.cost << " length: " << transition.length << endl;
        
        count++;
    }
    
    for (int i=0; i<transitions.size(); i++)
    {
        Transition transition = transitions[i];
        cout << i << ". " << transition.startFrame << " to " << transition.endFrame << " costing: " << transition.cost << " length: " << transition.length << endl;
    }
    
}




