//
//  VideoLoop.h
//  VideoTexture
//
//  Created by Leonard Teo on 11-11-08.
//  Copyright 2011 Leonard Teo. All rights reserved.
//

#include <iostream>
#include <vector>
#include "Transition.h"

#ifndef VIDEOLOOP_H
#define VIDEOLOOP_H

using namespace std;

/**
 * Class for simple or compound video loops
 */
class VideoLoop
{
public:
    //Object members
    vector<Transition*>* loops;
    int minFrame;   //The minimum frame that this loop covers
    int maxFrame;   //The maxima frame that this loop covers
    
    //Static methods
    static VideoLoop* create(Transition* transition);  //Must create with at least one transition
    static VideoLoop* create(VideoLoop* videoLoop); //COPIES the values from the old video loop over so that we're not accidentally writing to the same videoloop object
    
    //Object methods
    VideoLoop();    //Constructor
    
    void addLoop(Transition* transition);   //Adds a loop and sets the minima and maxima
    void addLoops(VideoLoop* compoundLoop); //Adds compound loops to this object
    bool overlaps(Transition* transition);  //Checks if a loop overlaps with the current compound video loop
    Transition* overlaps(VideoLoop* videoLoop);    //Checks if a compound loop overlaps another compound loop's primitive loops
    int numLoops();
    int length();
    void printLoops();
    double totalCost();
    
    int getMaxima();    //Get the loop with the last frame
    Transition* popMaxima();    //Pop the maxima out of the list
    vector<VideoLoop*>* getRanges();    //Get ranges from a video loop
    Transition* popFirst();
    
};

#endif