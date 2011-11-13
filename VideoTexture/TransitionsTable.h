//
//  TransitionsTable.h
//  VideoTexture
//
//  Created by Leonard Teo on 11-11-11.
//  Copyright 2011 Leonard Teo. All rights reserved.
//


#include "VideoLoop.h"
#include <iostream>
#include <vector>

#ifndef TRANSITIONSTABLE_H
#define TRANSITIONSTABLE_H

using namespace std;

class TransitionsTable
{
public:
    VideoLoop*** table;
    int numRows;
    int numCols;
    
    TransitionsTable(VideoLoop*** table, int rows, int cols);
    void print();
    
    vector<VideoLoop*>* getLoops();
};

#endif