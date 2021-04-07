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
 PROBLEMS:
 
 * Occasionally run into a "OUT OF MEMORY" error causing program to crash.
   System restart has proven to be a fix for the meanwhil
 
 
 */


/*
 VERY IMPORTANT
 
 *FIX LINES WITH //$%$%% COMMENT
 
 */


/*
 To do:
 
 
 
 * add ESC button support
 
 * fix drawing panel function so clicked on cell will be inlcuded if panel is drawn upward to the left
 
 * make panelOverlay responive and change size according to the number of known panels. (gets bigger if the known panel type exceeds avaliable space
 
 * level variable could become redundant. (may be removed in future)
 
 * fix background guide grid (important) - right now it only works properly at a window size of 1920, 1035 and a cell size of 70 with 15 gap.
 
 * show simulated fixtures automatically. - remember not all fixtures have the same channel count.
 
 * make text centered in fixtures and cells
 
 * make font files load with size corresponding to cell size
 
 */


/*
Possible bugs:
 
 * last interaction value could lead to interger overflow if program millis gets too large
  
 * generating a panel (eg lights) and then resizing it.
 
 * potential bug located in line "if (panels[i].savedPositions[u].position.size() > 0){   //  could cause bugs" when loading a panel with zero savedPosition data
 
*/


/*
 Bugs:
 
 * drawing a panel backwards
 
 * drawing a panel through another defined panel
 
 */
