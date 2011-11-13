//
//  Transition.cpp
//  VideoTexture
//
//  Created by Leonard Teo on 11-11-08.
//  Copyright 2011 Leonard Teo. All rights reserved.
//

#include "Transition.h"

bool Transition::overlaps(Transition* transition)
{
    return (this->endFrame >= transition->startFrame && this->endFrame <= transition->endFrame) || (transition->endFrame >= this->startFrame && transition->endFrame <= this->endFrame);
}

Transition* Transition::create(int startFrame, int endFrame, double cost, char ID)
{
    Transition* transition = new Transition();
    transition->startFrame = startFrame;
    transition->endFrame = endFrame;
    transition->length = (int) fabs(startFrame - endFrame);
    transition->cost = cost;
    transition->ID = ID;
    return transition;
}

/**
 * Makes the transition ordered by asc
 */
void Transition::asc()
{
   if (this->startFrame > this->endFrame)
   {
       int temp = this->startFrame;
       this->startFrame = this->endFrame;
       this->endFrame = temp;
   }
}


void Transition::print()
{
    cout << this->ID << "[" << this->startFrame << "," << this->endFrame << "]";
}