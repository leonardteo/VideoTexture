//
//  main.cpp
//  UnitTests
//
//  Created by Leonard Teo on 11-11-09.
//  Copyright 2011 Leonard Teo. All rights reserved.
//

#include <iostream>
#include "Transition.h"
#include "VideoLoop.h"

using namespace std;


bool assertTrue(bool test)
{
    if (!test)
    {
        cout << "Boolean assert failed" << endl;
        return false;
    }
    return true;
}

bool assertIntEquals(int a, int b)
{
    if (a != b)
    {
        cout << "Integer assert failed: " << a << " != " << b << endl;
        return false;
    }
    return true;
}


/**
 * Magically creates a compound loop or returns NULL
 */
VideoLoop* createCompoundLoop(VideoLoop*** table, int numRows, int numColumns, int currentRow, int currentColumn)
{
    
    int targetFrames = currentRow + 1;  //Target number of frames is the current row that we're in.
    
    //examine all compound loops of
    //shorter length in that same column, and tries to combine them with
    //compound loops from columns whose primitive loops have ranges
    //that overlap that of the column being considered.    
    
    vector<VideoLoop*>* matches = new vector<VideoLoop*>();
    
    //Step 1 - examine all compound loops of shorter length in the same column
    for (int row=0; row < currentRow; row++)
    {
        if (table[row][currentColumn]->length() > 0)
        {
            matches->push_back(table[row][currentColumn]);    //Add it to the list of matches
        }
    }
    
    //Step 2 - try to combine each compound loop with other compound loops from columns that overlap and that add up to the target number of frames
    //For each match
    vector<VideoLoop*>* newCompoundLoops = new vector<VideoLoop*>();
    
    for (int i=0; i<matches->size(); i++)
    {
        //Loop through the table
        for (int row=0; row < currentRow; row++)
        {
            for (int col=0; col < numColumns; col++)
            {
                //If the compound loop overlaps...
                if (matches->at(i)->overlaps(table[row][col]))
                {
                    //And if the compound loop adds up to the target number of frames, Create a new compound loop and return it
                    if (matches->at(i)->length() + table[row][col]->length() == targetFrames)
                    {
                        VideoLoop* newLoop = VideoLoop::create(matches->at(i));
                        newLoop->addLoops(table[row][col]);
                        newCompoundLoops->push_back(newLoop);
                    }
                }
            }
        }
    }
    
    //If we're empty, return null
    if (newCompoundLoops->size() == 0)
    {
        return NULL;
    }
    
    //Otherwise calculate the lowest cost compound loop
    double max = 0.0f;
    int max_i = 0;
    for (int i=0; i<newCompoundLoops->size(); i++)
    {
        if (newCompoundLoops->at(i)->totalCost() > max)
        {
            max = newCompoundLoops->at(i)->totalCost();
            max_i = i;
        }
    }
    double min = max;
    int min_i = max_i;
    for (int i=0; i<newCompoundLoops->size(); i++)
    {
        if (newCompoundLoops->at(i)->totalCost() < min)
        {
            min = newCompoundLoops->at(i)->totalCost();
            min_i = i;
        }
    }
    
    return newCompoundLoops->at(min_i);
    
}




/**
 * Test the dynamic programming table from the paper
 * To see if we can get the same results
 */
void testTable()
{
    //Create our initial transitions
    Transition* A = Transition::create(4, 9, 2, 'A');
    Transition* B = Transition::create(7, 8, 3, 'B');
    Transition* C = Transition::create(2, 5, 4, 'C');
    Transition* D = Transition::create(1, 3, 5, 'D');
    
    //Put it into a container
    vector<Transition*>* transitions = new vector<Transition*>();
    
    transitions->push_back(A);
    transitions->push_back(B);
    transitions->push_back(C);
    transitions->push_back(D);
    
    //Construct our beautiful table
    int maxFrames = 6;
    int n = (int) transitions->size();
    
    VideoLoop*** table = new VideoLoop**[maxFrames];
    for (int row=0; row < maxFrames; row++)
    {
        table[row] = new VideoLoop*[n];
        
        //Initialize table
        for (int col=0; col < n; col++)
        {
            table[row][col] = new VideoLoop();
        }
    }
    
    //-----------------------------------------
    
    //Generate table
    for (int row=0; row < maxFrames; row++)
    {
        int currentNumberOfFramesToTarget = row + 1;
        
        for (int col=0; col < n; col++)
        {
            //Check the transition
            //If the length of the transition exceeds the number of frames, don't bother
            if (transitions->at(col)->length > currentNumberOfFramesToTarget)
            {
                continue;
            }
            
            //If the length of the transition is the number of frames, set it as a simple loop
            if (transitions->at(col)->length == currentNumberOfFramesToTarget)
            {
                table[row][col]->addLoop(transitions->at(col));
                continue;
            }
            
            //Use uber procedure
            VideoLoop* newLoop = createCompoundLoop(table, maxFrames, n, row, col);
            if (newLoop != NULL)
            {
                table[row][col] = newLoop;
            }
            
        }
    }
    
    //-----------------------------------------
    
    //Debug our table
    cout << endl << "Debugging Transitions Table" << endl;
    
    for (int row=0; row<maxFrames; row++) {
        
        int currentNumberOfFramesToTarget = row + 1;
        
        cout << currentNumberOfFramesToTarget << ": ";  //Echo out the current number of frames we are targeting
        
        //For each transition being considered
        for (int col=0; col<n; col++)
        {
            cout << table[row][col]->numLoops() << " ";
        }   
        cout << endl;
    }
    
    //Debug the compound loops
    for (int row=0; row<maxFrames; row++) {
        
        int currentNumberOfFramesToTarget = row + 1;
        
        //For each transition being considered
        for (int col=0; col<n; col++)
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



int main (int argc, const char * argv[])
{
    std::cout << "Running unit tests\n";
    
    //Test that two primitive loops overlap
    Transition* transition1 = Transition::create(0, 5, 1);
    Transition* transition2 = Transition::create(4, 10, 1);
    
    bool test = transition1->overlaps(transition2);
    assertTrue(test);
    
    //Test that two primitive loops DON'T overlap
    transition1 = Transition::create(0, 2, 1);
    transition2 = Transition::create(3, 4, 1);
    
    test = transition1->overlaps(transition2);
    assertTrue(!test);
    
    //Test that one primitive loop inside another overlaps
    transition1 = Transition::create(0, 10, 1);
    transition2 = Transition::create(1, 5, 1);
    
    test = transition1->overlaps(transition2);
    assertTrue(test);
    
    //Test a compound video loop and try to overlap it with a single transition
    Transition* testTransition = Transition::create(0, 5, 1);
    VideoLoop* videoLoop = VideoLoop::create(testTransition);
    Transition* testTransition2 = Transition::create(4, 10, 1);
    
    test = videoLoop->overlaps(testTransition2);
    assertTrue(test);
    
    //Test that they DON'T overlap
    testTransition2 = Transition::create(6, 10, 1);
    test = videoLoop->overlaps(testTransition2);
    assertTrue(!test);
    
    //Test that one primitive loop is within the compound loop
    testTransition2 = Transition::create(1, 4, 1);
    test = videoLoop->overlaps(testTransition2);
    assertTrue(test);
    
    //Add to the compound loop
    testTransition = Transition::create(0, 5, 1);
    testTransition2 = Transition::create(4, 10, 1);
    videoLoop = VideoLoop::create(testTransition);
    videoLoop->addLoop(testTransition2);

    //Check the min and max frames
    assertIntEquals(0, videoLoop->minFrame);
    assertIntEquals(10, videoLoop->maxFrame);
    
    //Check overlapping videoloops
    VideoLoop* videoLoop2 = VideoLoop::create(Transition::create(8, 15, 1));
    videoLoop2->addLoop(Transition::create(14, 20, 1));
    
    Transition* overlappingTransition = videoLoop->overlaps(videoLoop2);
    
    assertTrue(overlappingTransition != NULL);
    assertIntEquals(overlappingTransition->startFrame, 8);
    
    //Check that the length of a videoloop is correct
    testTransition = Transition::create(0, 5, 1);
    assertIntEquals(5, testTransition->length);
    testTransition2 = Transition::create(4, 10, 1);
    assertIntEquals(6, testTransition2->length);
    
    videoLoop = VideoLoop::create(testTransition);
    videoLoop->addLoop(testTransition2);
    assertIntEquals(11, videoLoop->length());
    
    
    testTable();
    
    cout << "If you don't see any errors, unit tests passed!" << endl;
    
    return 0;
}

