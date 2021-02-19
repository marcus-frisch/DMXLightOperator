#pragma once

#include "ofMain.h"



class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

    void uipanel(int i);
    
    

    vector<string> cpActions = {"store", "", "setup", "delete", "rename", "panels", "move", "blackout", "", "", "", "", "", "clear", "off"};
    
    
    //mouse variables
    int red = 255;
    int green = 255;
    int blue = 255;
    vector<int> mouseColors = {255,255,255,0}; // Color data the mouse is carrying. Red, Green, Blue, White
    int mouseBrightness = 0;
    int level = 0;  // 0 grid, 1 panel, 2 control panel
    string mode = "";

    
    void simulatedFixture(int fixAddress);
    bool clickLeft(int x, int y, int w, int h);
    void addFixButton();
    int gX(int x);  //function to generate X coords for element on grid
    int gY(int y);  //function to generate Y coords for element on grid
    
    void colorsPanel(int i);
    void rgbMixer(int i);
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
    ofTrueTypeFont fixText;
    ofTrueTypeFont panelType;
    
    ofImage colorsIcon;
    


};



class dmxFixtureclass{   //dmx fixture
public:
    string name = "fixture";
    bool simShow = false;
    int x;      //simulator x
    int y;      //simulator y
    int w;      //simulator width
    int h;      //simulatir height
    int universe;   //dmx universe
    int address;    //dmx start channel
    int channelCount;
    vector<int> channelData; //dmx channel data
    //int channelcount;   //dmx channel count
    
    
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

class panelClass{   //fixture simulator panel.
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
//    int dataIden; // (data_Identification) as its possible to have multiple panels of the same type, we ultimately want them to store their own data. to do this we add a new object to what ever that panel(types) vector is and store which parameter
    
    vector<fixColor> savedColors = {};
    vector<int> fadeData = {100,100,100}; // if a panel has faders and the faders are generated using a FOR loop, data may be stored in here.
    
    //lookup oop instantiation / constructor - method that runs when you make a new object
    
//    int getName(){
//        if name ia not null:
//        retrurn name;
//    }
//    }
 

};



