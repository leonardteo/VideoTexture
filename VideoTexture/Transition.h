//
//  Transition.h
//  VideoTexture
//
//  Created by Leonard Teo on 11-11-08.
//  Copyright 2011 Leonard Teo. All rights reserved.
//

#ifndef TRANSITION_H
#define TRANSITION_H

#include <math.h>

class Transition
{
public:
    //Object members
    int startFrame;
    int endFrame;
    int length;
    double cost;
    char ID;
    
    //Object methods
    bool overlaps(Transition* transition);
    void asc(); 
    
    //Static methods
    static Transition* create(int startFrame, int endFrame, double cost, char ID = ' ');
};

#endif