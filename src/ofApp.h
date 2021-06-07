#pragma once

#include "ofMain.h"

class strInputvalues{   // Object reponsible for user string input
public:
    int fieldInputState = 0; // 0 none/finished input, 1 onScreen input, 3 completed input - ready for change
    int screenType; // 0 string ascii input, 1 int only Input
    string current = "";
    int maxCount;
    string textValue = "";
    int search = 0; // 0 Do not search anything, 1 Search DMX Fixture Channel Purpose Pre-definitions
};

class dmxChannel{   // the dmx channel(s) of each defined fixture
public:
    string purpose; // e.g "pan", "tilt", "dimmer", "red", "green", "blue", "white", "amber", "uv"
    bool effect = false;
    int value = 0;  //e.g 255
};

class knownChannelClass{ // Pre-known channels LucentOP's panels are designed to work with straight away. ( Check setup() to see channels )
public:
    string purpose;
    string desc;
};

class dmxFixtureclass{   // DMX show fixture
public:
    string name = "fixture";
    bool simShow = false;
    int fixtureID;  // ID used to check up the fixture dictionary
    int patchID;    // Matches with the patchID of the patch that created this fixture object
    int universe;   //dmx universe
    int startChannel;    //dmx start channel address
    vector<dmxChannel> channels;
};


class dmxPatchclass{    // DMX patch within showfile
public:
    int patchID;    // Represents each Patch
    bool verified = false;  // Checks if Patch data is complient e.g if fixtureID is a real fixture and DMX address it a true DMX address
    bool selected = false;
    int fixtureID = 0;
    int universe = 0;
    int channel = 0;
    int quantity = 0;
};

class mouseData{    // stores temporary data (such as data from cells the mouse clicks on)
public:
    int red = 255;
    int greeen = 255;
    int blue = 255;
};

class knownPanel{   // "Default" known panels LucentOP knows (allowed panels the user can select)
public:
    string type;
    string name;
    string abbreviation = "";
    int minWidth;
    int minHeight;
    int cellSize;
    int defSpace;
};

class storedBright{ // Stored brightness values for brightness storage cells in bristor panel
public:
    int iden;
    bool set = false;
    int value = 255;
};

class warningPopup{ // Warning Msg popup object for application
public:
    bool show = false;
    string title = "WARNING";
    string msg;
    int cR = 235;
    int cG = 64;
    int cB = 52;
    int time = 2000;
};


class ofApp : public ofBaseApp{ // Global values within LucentOP application

    public:
    
    bool enableDMXoutput = true;
    
        void setup(); // Code executed once on initial launch of ofApp
        void update();
        void draw(); // Code repeatedly looped until program close

    void loadIconsFonts(); // Load in fonts and scale them to the correct size
    
    int screen = 0; // The screen to render: 0 = showspace, 1, showfiles, 2 fixures, 3 channels
    bool showGrid = false;  // Should the grid be rendered?

    int maxCharacterCount;  // Global value for maximum number of characters users can input for string input
    
    vector<string> cpActions = {"store", "showfile", "setup", "delete", "rename", "panels", "move", "blackout", "", "", "", "", "", "clear", "off"};    // Showfile actions to be rendered in control panel
    vector<knownPanel> knownPanelType = {}; // stores objects with Panel type, name, minimum dimensions and abbreviation - generated in setup function.
 
    vector<string> showFileButtons = {"delete", "save as", "load", "save"}; // Action buttons for when listing showfiles
    
    vector<knownChannelClass> knownChannels = {};   //Default known DMX channels (generated in setup)
    
    string showFileData;    // String representing the whole showfile
    
    
    bool showBlock = false; // Show the function blocked prompt?
    string blockMsg;    // Message to show on prompt
    void popUp(); // function blocked prompt renderer
    
    void fixtureConfig();   // fixtureConfig screen (2)
    
    void genShowFixtures(); // Update DMX fixtures within the showfile (validated active fixtures)
    
    void updateSelectedFixtures();  // Function takes mouse values and applies them to selected DMX fixtures
    
    void scrollButtons(int x, int y, int width, int height, int scrollValue, int * scrollValuePtr, int scrollHeightTotal, int scrollStartHeight, int scrollEndHeight);  // Draw Scroll buttons for Y value responcible for UI elements Y position. Calculate if the elements are off the screen and only show appropriate scroll button
    
    void uiButton(int x, int y, string text, bool active, int width, int height); // Draws a UI button (e.g next page)
    void miniButton(int x, int y, string cha, bool active);  // Draws a "mini" UI button. (Usually used for small actions e.g Add/Remove channel)
    void crossButton(int x, int y, int gotoscreen, bool fill, bool checkOverlay); // Draw "Cross" ("X") button, if gotoscreen = -1 then don't change the current screen
    
    void showFileConfig();  // Showfile config sceeen (1)
    int currentShow = 0;    // Determines if current active show is 0 = freshly made (not saved), 1 = loaded from showFile
    string currentShowName; // Current active showfile name
    
    int ptrType = 0; //  Determines which pointer value to use for user Input (depending on data type) 0 = string, 1 = int. ( used in keyPressed() )
    string * fieldPtr;  // Pointer used for string related data
    int * fieldPtrInt;  // Pointer used for integer related data
    int * patchSelect;  // Pointer used for remembering which Patch is selected for user input
    
    string showFilesDir = ofFilePath::getUserHomeDir() + "/Documents/Lucent Showfiles"; // Default PATH to LucentOP application data (configuration, showfiles e.t.c)
    
    int selectedFixDef = -1; // Selected Fixture Definition ID. (Set to -1 when no fixture definition is selected)
    
    
    string fileSaveAsName;  // String responsible for showfiles name when being saved (file name)
    string fileSaveAsPath;  // String responsible for showfiles's path when being saved
    void genShowFileStr(); // Generates showfile data as a string
    
    int intParseAttribute(int panelID, int attriID);    //  responcible for parsing interger attribute values from showData string
    string strParseAttribute(int panelID, int attriID); //  responcible for parsing string attribute values from showData string
    vector<storedBright> vectBriParseAttribute(int panelID, int attriID); //  responcible for parsing vector attribute values from showData string
    vector<int> vectParseAttribute(int panelID, int attriID); //  responcible for parsing vector attribute values from showData string
    
   
    
    //mouse variables (values the user has selected --> used to interact with UI elements and selectedFixtures)
    int red = 255;  // RAW Mouse Colors [Old code and will be removed in future as mouseColors vector is replacement]
    int green = 255;
    int blue = 255;
    bool colorsChanged = false; // Has the User interacted with any Color values?
    bool wauvChanged = false;
    bool posChanged = false;
    vector<int> mouseColors = {0,0,0,0,0,0}; // Color data the mouse is carrying. Red, Green, Blue, White, Amber, UV
    vector<int> mousePosition = {0,0};  // fixture pan tilt values.
    vector<int> mouseFixtures = {};  // array stores index array of fixtures from showFixtures
    bool brightnessChanged = false; // Has the User interacted with any Brightness values?
    int mouseBrightness = 0;  // Brightness the mouse (user) has selected
    int level = 0;  // 0 grid, 1 panel, 2 control panel, 3 fieldInput (used to prevent overlapping UI elements both being functional at once)
    bool overFixture = false;   // Is the mouse currently over a simulated fixture?
    
    int increaseFixtureCount = 0;   // Ensures all fixtures throughout the program have a unique ID to prevent a fixture being deleted and a new one to be added resulting in a duplicate fixtureID.
    
    
    int overlay = 0; // 0 no overlay, 1 fieldInput
    string mode = "";   // What action button has the user clicked on? What mode is the mouse currently in?
    
    bool showDel = false;   // Show Delete Warning MSG popup prompt?
    int delType;    // Overlay that prompts user upon deleting things. type: 0 = file
    string delData1;    // If used for file: data1 represents file path data2 represents file name
    string delData2;
    void delOverlay();  // Delete Warning MSG popup prompt renderer
    
    int lastFlash;  // Elapsed Millis of application running when a flash last occured (flash for UI element being edited)
    int defFlashTime = 500; // Time used to wait between flashes for UI elements e.g Flashing of a text cursor.
    int flashOn = false;    // Is flash currently being rendered?
    
    void clearValues();
    
    int lastDMXOutput = 0;
    int waitBetweenDMX = 2;
    bool blackOut = false;
    int lastInteraction;    // Store a copy of the Elapsed Millis the application has been running when the user has last interacted with a UI element
    int defWaitTime = 100; // time to wait after user finishes an action before calling the code inside a mouse related function (time in millis) - prevents function being called again too soon
    bool waitTime(int time);   // Has the time passed for the funciton to be called again?? (uses lastInteraction and wait time specified - usually defWaitTime)
    void setLi();   //sets lastInteraction to ofGetElapsedTimeMillis()
    
    bool showSplash = true; // Should the splashScreen be rendered?
    void drawSplash(int wait, int fade);    // Draw splash screen with wait time for how long it is shown for and then how long the fade duration is after the wait
    
    bool overPanel; // Is the mouse over a UI panel?
    bool clickLeft(int x, int y, int w, int h); // Function that determines if User's left Mouse button is clicked between boundary
    bool pressLeft(int x, int y, int w, int h); // Function that determines if User's left Mouse button is pressed between boundary
    
    string strInputHeading; // Main/title/heading text to be rendered on user string input overlay
    void strInput(string current, int max); // UI element for allowing a user to input a string
    
    void addPanelOverlay(); // Overlay allowing user to select which panel to draw
    void drawPanel(); // Checks the UIpanel object and determines which ui panel function to call depending on panel type
    bool definedStart; //   has the start of a new panel (being drawn) been defined yet
    int panelToDraw;    // Index of knownPanelType
    int addPanelStage = 0; // 0 = no process, 1 = show panel overlay, 2 = drawing panel.
    int startX; // drawing panel starting coordinates
    int startY;
    int theDiffX;   // Difference in X values of panel dimensions
    int theDiffY;   // Difference in Y values of panel dimensions
    
    void demoShow();    // Update all showFile values to hard-coded pre-defined values for demonstration purposes
    
    
    int gX(int x);  //function to generate X coords for element on grid
    int gY(int y);  //function to generate Y coords for element on grid
    
    
    void uipanel(int i); //draw panels ---> i is the panelObj of the panel within the panels vector
    void colorsPanel(int i); // UI panel for user storing / defining colours
    void brightnessPanel(int i); // UI panel for user stored brightness values
    void rgbMixer(int i); // RGB mixer UI panel
    void wauv(int i);   // White, Amber, UV fader mixer
    void posPanel(int i); // UI panel for user defined position values
    void posMixer(int i); // White Amber UV fader mixer UI panel
    void briMixer(int i); // brightness mixer UI panel
    void fixpatchgroup(int i); // UI panel for displaying DMX patches [incomplete]
    void fixtures(int i); // UI panel that lists all DMX fixtures within the showfile
    void controlPanel(int i); // function responsible for drawing the control panel
    void simulateFixPanel(int i); // Simulated fixtures panel
    
    void delPanel(int i);   // Function used to delete UI panels
    
    string genDMX(int u); // Generate the DMX values for the specified universe
    void outputDMX(); // Output the existing DMX values
    
    //void scrollBar(int x, int y, );
    
    void genShowFileDir();  // Used to generate the Showfile Directory
    //void errorScreen(string message);   // Used to display an error to the User.
    
    
    void testFunction(); // Random code in here and can be called anytime to check if code works --> Stub used for detecting errors and assiting in development

    
    void mousePressed(int x, int y, int button); // Called when the mouse is being pressed
    void keyPressed(int key); // Check what key the user is pressing on their ASCII keyboard
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button); // Called when the mouse is being dragged
        
    void mouseReleased(int x, int y, int button); // Called when a mouse button is released
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void drawGrid(); // Subprogram that draws grid on screen (panel placement and UI elements) needs work --> check main.cpp comment
    
    int mouseExe = 0;   // Used for Click functionality
    int mousePExe = 0;  // Used for Pressing functionality
    
    void updateScaling(); // Update all variables responcible for application UI proportion scaling
    
    int defCellSize = 140;  // Value that is responsible for the whole UI proportion scaling within the LucentOP application. Represents one cell's square dimensions on the grid. Should be big enough to accomoodate a single finger on a touchscreen 70 or 140 (retina)
    int defCellGap = defCellSize*0.2142;    // Gap generally used to space out two or more very close UI elements next to each other
    int defRounded = defCellSize*0.0714;    // Radius for rounded corners of rounded UI elements
    int defMiniButton = defCellSize/2;      // Value represented for smaller UI actions (not commonly used throughout application use - such as adding or removing channels from a fixture definition)
    
    int showFixturestartheight = (defCellGap*2)+(3*defCellSize);    //  Global variable representing the Y start value for drawing FixtureDefinitions on screen 2
    
    int iconSize = defCellSize*0.5; //size to draw icons
    
    // Font variables
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
    ofTrueTypeFont splashText;
    ofImage colorsIcon;
    


};

class fixColor{ // Class used for User defined Color data (stored in each cell in colorstor panel)
public:
    int iden; //cell identifier
    bool set = false; //has user set colour data
    string name = "*"; //name
    int r;  //red
    int g;  //green
    int b;  //blue
    int w;  //white
    int a;  //amber
    int uv; //uv
    
};

class storedPos{ // Class used for User defined position data (stored in each cell in posStor panel)
public:
    int iden;
    bool set = false;
    string name = "*";
    vector<int> position = {};
};

class simulateFixtures{ // Data representing which fixture and where to render inside a simulate fixture panel
public:
    int startChannel;   // starting channel and fixture name is used to identify a showfiles fixture instead of a fixtureID incase other fixtures are added and deleted and the vector index changes. - Not the best solution but currently works till something else is figured out
    string name;
    int x;
    int y;
    int wi; // Width and height to render that simulated fixture
    int hi;
};

class panelClass{   // Class representing a UI panel
public:
    int x;        // panel X location
    int y;        // panel Y location
    int wi;       // panel width
    int hi;       // panel height
    int r = 5;    // panel corner radius
    int cellSize; // default w and h for fixtures
    int defSpace; // default seperating space size between cells
    int startIndex = 0; // Can be considered the row or index at which the panel starts displaying from, from another vector or array. (Typically used for "pages" if the panel cannot display the whole array.
    string type = "*";  // panel type identifier     e.g "fixsimulation", "controlpanel"
    string name = "*";  // panel title e.g "simulation"
    //Data the panel stores (most vectors will be empty as a panel usually only revolves around one data purpose e.g colors)
    vector<storedBright> savedBrightness = {};
    vector<storedPos> savedPositions = {};
    vector<fixColor> savedColors = {};
    vector<int> fadeData = {100,100,100}; // if a panel has faders and the faders are generated using a FOR loop, data may be stored in here.
    vector<simulateFixtures> panelSimFixtures = {};
};

class showFileFile{ // Class representing showfiles (on startup, files found in directory, demos, e.t.c)
public:
    string name = "";
    string modified = "";
    string path = "";
    bool freshStart = false;    // New showscreen on program start
    bool active = false;
    bool isDemo = false;
};

