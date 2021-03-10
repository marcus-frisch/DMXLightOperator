#pragma once

#include "ofMain.h"

class dmxChannel{   // the dmx channel(s) of each defined fixture
public:
    string purpose; // e.g "pan", "tilt", "dimmer", "red", "green", "blue", "white", "amber", "uv"
    int value;  //e.g 255
};

class dmxFixtureclass{   //dmx fixture
public:
    string name = "fixture";
    bool simShow = false;
    int iden;
    int x;      //simulator x
    int y;      //simulator y
    int w;      //simulator width
    int h;      //simulator height
    int universe;   //dmx universe
    int address;    //dmx start channel
    int channelCount;
    vector<dmxChannel> channels;
};

class mouseData{    // stores temporary data (such as data from cells the mouse clicks on
public:
    int red = 255;
    int greeen = 255;
    int blue = 255;
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

    void loadIconsFonts();
    
    // Variables regarding string input
    int maxCharacterCount;
    string textValue = "";
    
    vector<string> cpActions = {"store", "", "setup", "delete", "rename", "panels", "move", "blackout", "channels", "", "", "", "", "clear", "off"};
    
    //mouse variables
    int red = 255;
    int green = 255;
    int blue = 255;
    vector<int> mouseColors = {255,255,255,255,255,255}; // Color data the mouse is carrying. Red, Green, Blue, White, Amber, UV
    vector<int> mousePosition = {72,72};  // pan tilt values.
    vector<int> mouseFixtures;  // array stores "iden" (identifiers) of fixtures
    int mouseBrightness = 52;
    int level = 0;  // 0 grid, 1 panel, 2 control panel, 3 fieldInput
    
    int overlay = 0; // 0 no overlay, 1 fieldInput
    int fieldInputState = 0; // 0 none/finished input, 1 onScreen input, 3 completed input - ready for change
    string mode = "";

    
    void simulatedFixture(int fixAddress);
    bool clickLeft(int x, int y, int w, int h);
    void strInput(string current, int max);
    void addFixButton();
    int gX(int x);  //function to generate X coords for element on grid
    int gY(int y);  //function to generate Y coords for element on grid
    
    void uipanel(int i);
    void colorsPanel(int i);
    void brightnessPanel(int i);
    void rgbMixer(int i);
    void wauv(int i);   // White, Amber, UV fader mixer
    void posPanel(int i);
    void posMixer(int i);
    void briMixer(int i);
    void addFixture(string name, bool simShow, int x, int y, int w, int h, int universe, int address, int channelCount);
    void controlPanel(int i);
    
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    void drawGrid();
    
    int mouseExe = 0;
    
    int defCellSize = 70;
    int defCellGap = 15;
    
    int iconSize = 50; //size to draw icons
    
    ofTrueTypeFont debugText;
    ofTrueTypeFont panelName;
    ofTrueTypeFont fixMain;
    ofTrueTypeFont pageMain;
    ofTrueTypeFont fixText;
    ofTrueTypeFont panelType;
    ofTrueTypeFont usrInput;
    ofImage colorsIcon;
    


};




class fixColor{
public:
    int iden; //cell identifier
    bool set = false; //has user set colour data
    string name = ""; //name
    int r;  //red
    int g;  //green
    int b;  //blue
    int w;  //white
    
};

class storedBright{
public:
    int iden;
    bool set = false;
    int value = 255;
};

class storedPos{
public:
    int iden;
    bool set = false;
    string name = "#";
    vector<int> position = {};
};

class panelClass{
public:
    int x;        // panel X location
    int y;        // panel Y location
    int wi;       // panel width
    int hi;       // panel height
    int r = 5;    // panel corner radius
    int cellSize; // default w and h for fixtures
    int defSpace; // default seperating space size between cells
    string type;  // panel type identifier     e.g "fixsimulation", "controlpanel"
    string name;  // panel title e.g "simulation"    
    vector<storedBright> savedBrightness = {};
    vector<storedPos> savedPositions = {};
    vector<fixColor> savedColors = {};
    vector<int> fadeData = {100,100,100}; // if a panel has faders and the faders are generated using a FOR loop, data may be stored in here.
};
