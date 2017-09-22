#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/videoio/videoio.hpp" // pour ios je suppose, mettre opencv2/video/video.hpp
#include "opencv2/imgcodecs/imgcodecs.hpp"
#include <iostream>
#include <stdio.h>
#include <array>

using namespace cv;
using namespace std;


// INFO : Dans le makefile, ajouter -lopencv_videoio OU -lopencv_video 

    
int main( int argc, char* argv[] ){
    
    CvCapture* capture = cvCaptureFromCAM( 0 );
    cvNamedWindow( "Afficher une camera", CV_WINDOW_AUTOSIZE );
    IplImage* frame;
    while(1){
        frame = cvQueryFrame( capture );
        cvShowImage( "Afficher une camera", frame );
        char c = cvWaitKey(33);
        if( c == 27 ) break;
    }
    cvReleaseCapture( &capture );
    cvDestroyWindow( "Afficher une camera" );
    
}

