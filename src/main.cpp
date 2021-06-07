#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofSetupOpenGL(2880,1500,OF_WINDOW); //1920, 1035
	// this kicks off the running of the application.
	// can be OF_WINDOW or OF_FULLSCREEN

	ofRunApp(new ofApp());

}

/*
 NOTES:
 
 * The comments below are thoughts and warnings from the developer during various times of development. These comments serve as a "current build" notice
 for future development.
 
 */

/*
 KNOWN PROBLEMS (BUGS):
 
 * Funky mouse behaviour when a fixture simulation panel is drawn - seems mouse clicks are not registered when the mouse is within ~2 cell sizes proximity of panel
 
 * Occasionally runs into a "OUT OF MEMORY" error causing program to crash.
   System restart has proven to be a fix for the meanwhile. - No known set of steps can be repeated to re-produce the error.
 
 * Search for comment "FS01" - causing errors in console yet functions as I want (Not important but worth figuring out)
 
 
 */


/*
 TO DO: (simple development goals - not overall program functionality)
 
 * remove flash from editing DMX patch
 
 * check if fixture definition (FixtureID) is used in showfile upon deletion - remove all referenced to that fixture within all subprograms e.g fixture simulation
 
 * add "last modified" data to showfiles (probably involves epoch --> mac (unix) and windows epoch the same????)
 
 * add error screen popup if program fails to generate default Lucent directory
 
 * add ESC button support (prevent program just closing)
 
 * add arrow key button support for navigating UI features
 
 * fix drawing panel function so clicked on grid reference will be inlcuded if panel is drawn upward to the left. (currently initial GR is ignored)
 
 * fix background guide grid (important) - right now it only works properly at a window size of 1920, 1035 and a cell size of 70 with 15 gap.
 
 * make text centered in UI elements
 
 * allow user to export/load fixture definitions
 
 * allow user to define default data directory for application support data along with showfile and fixture definition files
 
 
 */


/*
Possible bugs:
 
 * Dealing with paths and file read/write functionality on windows systems as it was developed using the MacOS structure
 
 * lastInteraction value could lead to interger overflow if program millis gets too large [int variable type could overflow in year 2038 due to epoch elapsed millis being too great]
  
 * generating a panel (e.g stored colors) and then resizing it - could loose data if panel size decreases. (Resizing functionality yet to be built in)
 
*/


/*
 Bugs:
 
 * drawing a panel backwards (moving mouse leftward or up or both - after initial panel XY origin has been set)
 
 * drawing a panel through another defined panel
 
 
 */

/*
 CREDITS
 
 * Noto Font
 
 * Libra Sans Font Family (1001 fonts)
 
 */
