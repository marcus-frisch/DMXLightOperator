#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofSetupOpenGL(1920,1035,OF_WINDOW);
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
    
    
    
	ofRunApp(new ofApp());

}


/*
 To do:
 
 * fix background guide grid (important) - right now it only works properly at a window size of 1920, 1035 and a cell size of 70 with 15 gap.
 
 * show simulated fixtures automatically. - remember not all fixtures have the same channel count.
 
 * make text centered in fixtures and cells
 
 */


/*
Possible bugs:
 
 * generating a panel (eg lights) and then resizing it.
 
 
 
*/
