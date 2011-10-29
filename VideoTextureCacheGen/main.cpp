/**
 * This program takes an input file and generates the frame difference cache for the video texture application
 */

#include <iostream>
#include "VideoTexture.h"

using namespace std;

int main (int argc, const char* argv[])
{
    string videoPath = "/Users/leonardteo/Movies/";
    string cachePath = "/Users/leonardteo/Desktop/Temp/videotexture_cache/";
    
    string files[] = {
        "southpark1.mp4"
    };
    
    int num_files = 1;
    
    VideoTexture* videotex;
    
    //Create the new video texture
    try {
        
        for (int i=0; i<num_files; i++)
        {
            string video = videoPath + files[i];
            string cache = cachePath + files[i] + ".txt";
            
            videotex = new VideoTexture(video, 0.1f);
            videotex->generateFrameDistanceMatrix(cache);
            
            //Free the memory
            delete videotex;
        }
        
    } catch (string e)
    {
        cout << e << endl;
        return 1;
    }
    
    return 0;
}