#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofSetupOpenGL(1920,1035,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}


/*
 To do:
 
 *Fix background guide grid (important) - right now it only works properly at a window size of 1920, 1035 and a cell size of 70 with 15 gap.
 
 *show simulated fixtures automatically. - remember not all fixtures have the same channel count.
 
 * make text centered in fixtures and cells
 
 */
