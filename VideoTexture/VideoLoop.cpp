//
//  VideoLoop.cpp
//  VideoTexture
//
//  Created by Leonard Teo on 11-11-08.
//  Copyright 2011 Leonard Teo. All rights reserved.
//

#include "VideoLoop.h"

/**
 * Constructor
 */
VideoLoop::VideoLoop()
{
    //Create loops vector
    this->loops = new vector<Transition*>();
    this->minFrame = -1;
    this->maxFrame = -1;
}

/**
 * Creates and returns a VideoLoop object
 */
VideoLoop* VideoLoop::create(Transition* transition)
{
    VideoLoop* videoLoop = new VideoLoop();
    videoLoop->addLoop(transition);
    return videoLoop;
}

/**
 * Creates and returns a totally new VideoLoop object
 */
VideoLoop* VideoLoop::create(VideoLoop* videoLoop)
{
    VideoLoop* newLoop = new VideoLoop();
    for (int i=0; i<videoLoop->loops->size(); i++)
    {
        newLoop->addLoop(videoLoop->loops->at(i));
    }
    return newLoop;
}

/**
 * Adds a loop to the current video loop
 */
void VideoLoop::addLoop(Transition *transition)
{
    //Check that they overlap or throw an exception
    /* We're commenting this out because the addLoops method needs to skip this check
    if (this->loops->size() > 0)
    {
        if (!VideoLoop::overlaps(transition))
        {
            char* error;
            sprintf(error, "Error: Transition [%d, %d] doesn't overlap with loop [%d, %d]", transition->startFrame, transition->endFrame, this->minFrame, this->maxFrame);
            cout << error << endl;
            exit(1);
            //throw error;
        }
    }*/
    
    this->loops->push_back(transition);
    
    //Calculate the max and min frames
    int maxFrame = 0;
    for (int i=0; i<this->loops->size(); i++)
    {
        if (this->loops->at(i)->endFrame > maxFrame)
        {
            maxFrame = this->loops->at(i)->endFrame;
        }
    }
    int minFrame = maxFrame;
    for (int i=0; i<this->loops->size(); i++)
    {
        if (this->loops->at(i)->startFrame < minFrame)
        {
            minFrame = this->loops->at(i)->startFrame;
        }
    }
    
    this->minFrame = minFrame;
    this->maxFrame = maxFrame;
    
    //Debug
    /*
    cout << "Added loop. Current loops: ";
    this->printLoops();
    cout << endl;
     */
}

/**
 * Adds a compound loop to the current compound loop
 */
void VideoLoop::addLoops(VideoLoop *compoundLoop)
{
    //Check that they overlap
    if (!this->overlaps(compoundLoop))
    {
        //throw "ERROR: Compound loops do not overlap";
        cout << "ERROR: Compound loops do not overlap" << endl; //getting desperate
        exit(1);
    }
    
    //Copy the transitions over
    for (int i=0; i<compoundLoop->loops->size(); i++)
    {
        this->addLoop(compoundLoop->loops->at(i));
    }
}

/**
 * Checks if the video loop overlaps the transition
 */
bool VideoLoop::overlaps(Transition *transition)
{
    return (this->maxFrame >= transition->startFrame && this->maxFrame <= transition->endFrame) || (transition->endFrame >= this->minFrame && transition->endFrame <= this->maxFrame);
}

/**
 * Checks if a compound loop overlaps another's primitive loops
 */
Transition* VideoLoop::overlaps(VideoLoop *videoLoop)
{
    for (int i=0; i<videoLoop->numLoops(); i++)
    {
        if (this->overlaps(videoLoop->loops->at(i)))
        {
            return videoLoop->loops->at(i);
        }
    }
    return NULL;
}

/**
 * Return the number of loops in this compound loop
 */
int VideoLoop::numLoops()
{
    return (int) this->loops->size();
}

/**
 * Gets the sum of all primitive loops
 */
int VideoLoop::length()
{
    int sum = 0;
    for (int i=0; i<this->numLoops(); i++)
    {
        sum += this->loops->at(i)->length;
    }
    return sum;
}


/**
 * Debug the loops
 */
void VideoLoop::printLoops()
{
    for (int i=0; i<this->loops->size(); i++)
    {
        Transition* transition = this->loops->at(i);
        cout << transition->ID << "[" << transition->startFrame << "," << transition->endFrame << "] ";
    }
}

/**
 * Gets the total cost of the compound loop
 */
double VideoLoop::totalCost()
{
    double sum = 0.0f;
    for (int i=0; i<this->numLoops(); i++)
    {
        sum += this->loops->at(i)->cost;
    }
    return sum;
}



