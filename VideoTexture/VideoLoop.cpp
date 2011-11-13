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

/**
 * Get the index of the loop with the highest end frame
 */
int VideoLoop::getMaxima()
{
    //If length is 0, fail
    if (this->numLoops() == 0)
    {
        return -1;
    }
    
    //If length is 1, return the first loop
    if (this->numLoops() == 1)
    {
        return 0;
    }
    
    int maxFrame = 0;
    int max_i = 0;
    for (int i=0; i<this->numLoops(); i++)
    {
        if (this->loops->at(i)->endFrame > maxFrame)
        {
            maxFrame = this->loops->at(i)->endFrame;
            max_i = i;
        }
    }
    return max_i; 
}

/**
 * Pops the maxima out of the list
 */
Transition* VideoLoop::popMaxima()
{
    int maxima = this->getMaxima();
    if (maxima < 0)
    {
        return NULL;    //fail
    }
    Transition* transition = this->loops->at(maxima);
    this->loops->erase(this->loops->begin() + maxima);
    
    //Set the endFrame again
    int maxFrame = 0;
    for (int i=0; i<this->numLoops(); i++)
    {
        if (this->loops->at(i)->endFrame > maxFrame)
        {
            maxFrame = this->loops->at(i)->endFrame;
        }
    }
    this->maxFrame = maxFrame;
    
    return transition;
    
}

/**
 * Pops the first Loop off
 */
Transition* VideoLoop::popFirst()
{
    if (this->loops->size() == 0)
    {
        return NULL;
    }
    
    Transition* transition = this->loops->front();
    this->loops->erase(this->loops->begin());
    
    //Set the start and endFrame again
    int maxFrame = 0;
    for (int i=0; i<this->numLoops(); i++)
    {
        if (this->loops->at(i)->endFrame > maxFrame)
        {
            maxFrame = this->loops->at(i)->endFrame;
        }
    }
    this->maxFrame = maxFrame;
    
    int minFrame = maxFrame;
    for (int i=0; i<this->numLoops(); i++)
    {
        if (this->loops->at(i)->startFrame < minFrame)
        {
            minFrame = this->loops->at(i)->startFrame;
        }
    }
    this->minFrame = minFrame;
    
    return transition;
}

/**
 * Get ranges from a video loop that has had its maxima popped (sounds rude!)
 */
vector<VideoLoop*>* VideoLoop::getRanges()
{
    //Check if there's anything to do
    if (this->numLoops() == 0)
    {
        return NULL;
    }
    
    //Create a new set of ranges
    vector<VideoLoop*>* ranges = new vector<VideoLoop*>();
    
    VideoLoop* currentRange;
    
    //If there's only 1 just deal with it
    if (this->numLoops() == 1)
    {
        currentRange = VideoLoop::create(this->loops->at(0));
        ranges->push_back(currentRange);
        return ranges;
    }
    
    for (int i=0; i<this->numLoops(); i++)
    {
        //If this is the first loop
        if (i == 0)
        {
            currentRange = VideoLoop::create(this->loops->at(i));
            continue;
        }
        
        //Check if it overlaps with the current range
        if (currentRange->overlaps(this->loops->at(i)))
        {
            currentRange->addLoop(this->loops->at(i));
        } else {
            //Add the range
            ranges->push_back(currentRange);
            
            //Create a new range
            currentRange = VideoLoop::create(this->loops->at(i));
        }
        
        //Check if there are more loops to consider
        if (this->numLoops() - i == 1)
        {
            ranges->push_back(currentRange);
        }
    }
    
    //Sort the ranges so that they are continuous
    //Bubble sort algorithm btw
    for (int i=(int)ranges->size(); i>=0; i--)
    {
        for (int j=1; j<i; j++) //Start at 1 so we can swap backwards
        {
            //If previous is greater than current, swap them
            if (ranges->at(j-1)->maxFrame > ranges->at(j)->maxFrame)
            {
                //Swap
                VideoLoop* tempLoop = ranges->at(j-1);
                ranges->at(j-1) = ranges->at(j);
                ranges->at(j) = tempLoop;
            }
        }
    }
    
    return ranges;
    
}




