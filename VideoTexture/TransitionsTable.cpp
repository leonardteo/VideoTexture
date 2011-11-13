//
//  TransitionsTable.cpp
//  VideoTexture
//
//  Created by Leonard Teo on 11-11-11.
//  Copyright 2011 Leonard Teo. All rights reserved.
//

#include "TransitionsTable.h"

TransitionsTable::TransitionsTable(VideoLoop*** table, int rows, int cols)
{
    this->table = table;
    this->numRows = rows;
    this->numCols = cols;
}

/**
 * Return all the valid loops in the table
 */
vector<VideoLoop*>* TransitionsTable::getLoops()
{
    vector<VideoLoop*>* loops = new vector<VideoLoop*>();
    
    for (int row=0; row<this->numRows; row++) {
      
        //For each transition being considered
        for (int col=0; col<this->numCols; col++)
        {
            //If there is a compound/simple loop here
            if (table[row][col]->numLoops() > 0)
            {
                
                loops->push_back(table[row][col]);
            }
        }   
    }
    
    return loops;
}

void TransitionsTable::print()
{
    //Debug the table
    cout << endl << "Showing Transitions Table" << endl;
    
    for (int row=0; row<this->numRows; row++) {
        
        int currentNumberOfFramesToTarget = row + 1;
        cout << currentNumberOfFramesToTarget << ": ";  //Echo out the current number of frames we are targeting
        
        //For each transition being considered
        for (int col=0; col<numCols; col++)
        {
            cout << table[row][col]->numLoops() << " ";
        }   
        
        cout << endl;
    }
    
    //Debug the compound loops
    for (int row=0; row<this->numRows; row++) {
        
        int currentNumberOfFramesToTarget = row + 1;
        
        //For each transition being considered
        for (int col=0; col<this->numCols; col++)
        {
            //If there is a compound/simple loop here
            if (table[row][col]->numLoops() > 0)
            {
                cout << "Loop (" << currentNumberOfFramesToTarget << "): ";
                
                //Show each loop
                table[row][col]->printLoops();
                
                cout << endl;
            }
        }   
    }

}