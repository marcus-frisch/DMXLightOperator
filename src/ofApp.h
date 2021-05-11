#pragma once

#include "ofMain.h"

class strInputvalues{
public:
    int fieldInputState = 0; // 0 none/finished input, 1 onScreen input, 3 completed input - ready for change
    int screenType; // 0 string ascii input, 1 int only Input
    string current = "";
    int maxCount;
    string textValue = "";
};

class dmxChannel{   // the dmx channel(s) of each defined fixture
public:
    string purpose; // e.g "pan", "tilt", "dimmer", "red", "green", "blue", "white", "amber", "uv"
    int value;  //e.g 255
};

class knownChannelClass{ // Pre-known channels LucentOP's panels are designed to work with straight away. ( Check setup() to see channels )
public:
    string purpose;
    string desc;
};

class dmxFixtureclass{   //dmx fixture
public:
    string name = "fixture";
    bool simShow = false;
    int fixtureID;
    int x;      //simulator x
    int y;      //simulator y
    int w;      //simulator width
    int h;      //simulator height
    int universe;   //dmx universe
    int address;    //dmx start channel
    vector<dmxChannel> channels;
};

class dmxPatchclass{
public:
    bool verified = false;  // Checks if Patch data is complient e.g if fixtureID is a real fixture and DMX address it a true DMX address
    bool selected = false;
    int fixtureID = 0;
    int universe = 0;
    int channel = 0;
    int quantity = 0;
};

class mouseData{    // stores temporary data (such as data from cells the mouse clicks on
public:
    int red = 255;
    int greeen = 255;
    int blue = 255;
};

class knownPanel{
public:
    string type;
    string name;
    string abbreviation = "";
    int minWidth;
    int minHeight;
    int cellSize;
    int defSpace;
};

class storedBright{
public:
    int iden;
    bool set = false;
    int value = 255;
};



class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

    void loadIconsFonts();
    
    int screen = 2; // What screen should be shown on the screen: 0 = showspace, 1, showfiles, 2, fixures
    bool showGrid = false;
    
    
    string pleaseCopy;
    
    // Variables regarding string input
    int maxCharacterCount;
    
    vector<string> cpActions = {"store", "showfile", "setup", "delete", "rename", "panels", "move", "blackout", "channels", "", "", "", "", "clear", "off"};
    vector<knownPanel> knownPanelType = {}; // stores objects with Panel type, name, minimum dimensions and abbreviation - generated in setup function.
 
    vector<string> showFileButtons = {"delete", "save as", "load", "save"};
    
    vector<knownChannelClass> knownChannels = {};
    
    string showFileData;
    
    bool showBlock = false;
    string blockMsg;
    void blockFeature();
    
    void fixtureConfig();
    
    void miniButton(int x, int y, string cha);  // mini button.
    void crossButton(int x, int y, int gotoscreen, bool fill, bool checkOverlay);
    
    void showFileConfig();
    int currentShow = 0;    // Determines if current show is 0 = freshly made (not saved), 1 = loaded from showFile
    string currentShowName;
    
    int ptrType = 0; // Type of data the pointer variable is.. 0 = string, 1 = int. ( used in keyPressed() )
    string * fieldPtr;
    int * fieldPtrInt;
    int * patchSelect;
    string showFilesDir = ofFilePath::getUserHomeDir() + "/Documents/Lucent Showfiles";
    
    int selectedFixDef = -1;
    
    
    string fileSaveAsName;
    string fileSaveAsPath;
    void genShowFileStr(); // Generates showfile data as a string
    
    int intParseAttribute(int panelID, int attriID);    //  responcible for parsing interger attribute values from showData string
    string strParseAttribute(int panelID, int attriID); //  responcible for parsing string attribute values from showData string
    vector<storedBright> vectBriParseAttribute(int panelID, int attriID); //  responcible for parsing vector attribute values from showData string
    vector<int> vectParseAttribute(int panelID, int attriID); //  responcible for parsing vector attribute values from showData string
    
   
    
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
    string mode = "";
    
    bool showDel = false;
    int delType;    // Overlay that prompts user upon deleting things. type: 0 = file
    string delData1;    // If used for file: data1 represents file path data2 represents file name
    string delData2;
    void delOverlay();
    
    int lastFlash;
    int defFlashTime = 500; // Time used to wait between flashes for UI elements e.g Flashing of a text cursor.
    int flashOn = false;
    int lastInteraction;
    int defWaitTime = 100; // time to wait after user finishes an action before calling the code inside a mouse related function (time in millis)
    bool waitTime(int time);
    
    bool overPanel;
    void simulatedFixture(int fixAddress);
    bool clickLeft(int x, int y, int w, int h);
    bool pressLeft(int x, int y, int w, int h);
    
    string strInputHeading;
    void strInput(string current, int max);
    
  
    void addPanelOverlay();
    void drawPanel();
    bool definedStart; //   has the start of a new panel (being drawn) been defined yet
    int panelToDraw;
    int addPanelStage = 0; // 0 = no process, 1 = show panel overlay, 2 = drawing panel.
    int startX; // drawing panel starting coordinates
    int startY;
    int theDiffX;
    int theDiffY;
    
    
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
    void addFixture(string name, bool simShow, int x, int y, int w, int h, int universe, int address);
    void controlPanel(int i);
    
    void genShowFileDir();  // Used to generate the Showfile Directory
    //void errorScreen(string message);   // Used to display an error to the User.
    
    
    void testFunction();

    
		void mousePressed(int x, int y, int button);
    void keyPressed(int key);
            void keyReleased(int key);
            void mouseMoved(int x, int y );
            void mouseDragged(int x, int y, int button);
            
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    void drawGrid();
    
    int mouseExe = 0;   // Used for Click functionality
    int mousePExe = 0;  // Used for Pressing functionality
    
    int defCellSize = 70;  // 70
    int defCellGap = defCellSize*0.2142;
    int defRounded = defCellSize*0.0714;
    int defMiniButton = defCellSize/2;
    
    int iconSize = 50; //size to draw icons
    
    ofTrueTypeFont debugText;
    ofTrueTypeFont panelName;
    ofTrueTypeFont fixMain;
    ofTrueTypeFont cardMain;
    ofTrueTypeFont pageMain;
    ofTrueTypeFont pageSub;
    ofTrueTypeFont fixText;
    ofTrueTypeFont panelType;
    ofTrueTypeFont usrInput;
    ofTrueTypeFont uiIcons;
    ofTrueTypeFont overlayBody;
    ofImage colorsIcon;
    


};

class fixColor{
public:
    int iden; //cell identifier
    bool set = false; //has user set colour data
    string name = "*"; //name
    int r;  //red
    int g;  //green
    int b;  //blue
    int w;  //white
    
};



class storedPos{
public:
    int iden;
    bool set = false;
    string name = "*";
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
    string type = "*";  // panel type identifier     e.g "fixsimulation", "controlpanel"
    string name = "*";  // panel title e.g "simulation"
    vector<storedBright> savedBrightness = {};
    vector<storedPos> savedPositions = {};
    vector<fixColor> savedColors = {};
    vector<int> fadeData = {100,100,100}; // if a panel has faders and the faders are generated using a FOR loop, data may be stored in here.
};

class showFileFile{
public:
    string name = "";
    string modified = "";
    string path = "";
    bool freshStart = false;    // New showscreen on program start
    bool active = false;
};
