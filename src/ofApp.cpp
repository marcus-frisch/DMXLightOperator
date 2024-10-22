// Import required libraries
#include "ofApp.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdio>

#include <stdio.h>      /* printf */
#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */

// To understand data each object holds view ofApp.h and check the class definitions

vector<panelClass> panels;  //stores panel objects within the showfile
panelClass panelObj;    // Panel object
vector<dmxFixtureclass> fixturesDef;   //stores all DMX fixture definitions within the showfile
dmxFixtureclass fixObj; //DMX fixture definition object

vector<dmxPatchclass> dmxPatch; // Stores all showFile DMX patches
dmxPatchclass dmxPatchObj;  // Represents each individual DMX patch within the showfile

fixColor colorObj;  // Color object for each cell within color storage panel
storedBright brightObj; // Brightness object for each cell within brightness storage panel
storedPos posObj; // Position object for each cell within position storage panel

dmxChannel dmxChannelObj;   // DMX fixture channel object

knownChannelClass knownChannelObj;  // Known default DMX channel purpose object

warningPopup popupObj;  // Popup overlay object

simulateFixtures panSimFixObj;  // Simulated Fixture object for simulated fixtures panel


vector<showFileFile> showsList; // Vector for found showfiles from a directory
vector<showFileFile> showsListAlt; // Used as a alt to the main to store showfiles. e.g used when removing a showfile index from the main array
showFileFile showFileObject;    // Object representing a showfile

knownPanel knownPanelObj;   // Default/known panels data object

strInputvalues strInputObj;

vector<dmxFixtureclass> showFixtures;


string ocol = ""; // Represents showfile data as a string

int showCardstartheight;
int showPatchstartheight;


int ofApp::gX(int x){ //function to generate X coords for element on grid
    return (x * defCellSize)+defCellGap;
}

int ofApp::gY(int Y){ //function to generate Y coords for element on grid
    return (Y * defCellSize)+defCellGap;
}

bool ofApp::waitTime(int time){
    if (ofGetElapsedTimeMillis() > lastInteraction + time){
        return true;
    } else {
        return false;
    }
}

void ofApp::setLi(){    // Update the users last interaction
    lastInteraction = ofGetElapsedTimeMillis();
}

void ofApp::genShowFixtures(){  // Generate vector with the fixture objects the showfile will actually be using (DMX enabled fixtures)  -- Should be called when "update DMX" button is pushed
    showFixtures = {};
    mouseFixtures = {};
    
    vector<int> validPatches = {};
    
    for (int i = 0; i < dmxPatch.size(); i++){
        if (dmxPatch[i].verified){
            validPatches.push_back(i);
        }
    }
    
    for (int i = 0; i < validPatches.size(); i++){  // For the amount of valid User defined DMX patches take their starting DMX channel and DMX quantity and add the DMX fixtures to the showFixtures vector with the correct DMX addresses
        for (int n = 0; n < dmxPatch[validPatches[i]].quantity; n++){
            int numChannels = fixturesDef[dmxPatch[validPatches[i]].fixtureID].channels.size();
            int theStartChannel = dmxPatch[validPatches[i]].channel + (n * numChannels);
            if (theStartChannel < 512){ // Ensure channel is within the DMX512 protocol
                showFixtures.push_back(fixObj);
                showFixtures[showFixtures.size()-1].startChannel = theStartChannel;
                showFixtures[showFixtures.size()-1].patchID = i;
                showFixtures[showFixtures.size()-1].universe = dmxPatch[validPatches[i]].universe;
                showFixtures[showFixtures.size()-1].fixtureID = dmxPatch[validPatches[i]].fixtureID;
                for (int u = 0; u < fixturesDef[dmxPatch[validPatches[i]].fixtureID].channels.size(); u++){
                    showFixtures[showFixtures.size()-1].channels.push_back(dmxChannelObj);
                    showFixtures[showFixtures.size()-1].channels[showFixtures[showFixtures.size()-1].channels.size()-1].purpose = fixturesDef[dmxPatch[validPatches[i]].fixtureID].channels[u].purpose;
                    showFixtures[showFixtures.size()-1].channels[showFixtures[showFixtures.size()-1].channels.size()-1].effect = false;
                    showFixtures[showFixtures.size()-1].channels[showFixtures[showFixtures.size()-1].channels.size()-1].value = 0;
                }
            }
        }

    }
    
}

string ofApp::genDMX(int u){  // Generate the DMX values for the specified universe
    vector<int> dmxData = {};
    for (int i = 1; i < 512; i++){  // Set all DMX channels to 0
        dmxData.push_back(0);
    }
    
    if (!blackOut){
        vector<int> uniFixtures = {};   // Store the index of all showFixture fixtures that are on the specified DMX universe
        for (int i = 0; i < showFixtures.size(); i++){
            if (showFixtures[i].universe == u){
                uniFixtures.push_back(i);
            }
        }
        
        for (int i = 0; i < uniFixtures.size(); i++){   // Get the value of the DMX channels of each fixture and set those values in the DMX data with their correct position based off each fixtures starting channel DMX address
            for (int c = 0; c < showFixtures[uniFixtures[i]].channels.size(); c++){
                dmxData[showFixtures[uniFixtures[i]].startChannel+c] = showFixtures[uniFixtures[i]].channels[c].value;
            }
        }
    }

    string dmxString = "";
    for (int i = 1 ; i < 512; i++){
        if (i != 511){
            dmxString = dmxString + to_string(dmxData[i]) + ",";
        } else {
            dmxString = dmxString + to_string(dmxData[i]);
        }
        
    }
    
    return dmxString;
}

void ofApp::updateSelectedFixtures(){   // Update User selected showfixtures DMX channel values with the mouse values (selected values e.g colors, brightness e.t.c)
    for (int i = 0; i < mouseFixtures.size(); i++){
        for (int u = 0; u < showFixtures[mouseFixtures[i]].channels.size(); u++){
            if (showFixtures[mouseFixtures[i]].channels[u].purpose == "dimmer" && brightnessChanged){
                showFixtures[mouseFixtures[i]].channels[u].value = mouseBrightness;
            } else if (showFixtures[mouseFixtures[i]].channels[u].purpose == "red" && colorsChanged){
                showFixtures[mouseFixtures[i]].channels[u].value = mouseColors[0];
            } else if (showFixtures[mouseFixtures[i]].channels[u].purpose == "green" && colorsChanged){
                showFixtures[mouseFixtures[i]].channels[u].value = mouseColors[1];
            } else if (showFixtures[mouseFixtures[i]].channels[u].purpose == "blue" && colorsChanged){
                showFixtures[mouseFixtures[i]].channels[u].value = mouseColors[2];
            } else if (showFixtures[mouseFixtures[i]].channels[u].purpose == "pan" && posChanged){
                showFixtures[mouseFixtures[i]].channels[u].value = mousePosition[0];
            } else if (showFixtures[mouseFixtures[i]].channels[u].purpose == "tilt" && posChanged){
                showFixtures[mouseFixtures[i]].channels[u].value = mousePosition[1];
            } else if (showFixtures[mouseFixtures[i]].channels[u].purpose == "white" && wauvChanged){
                showFixtures[mouseFixtures[i]].channels[u].value = mouseColors[3];
            } else if (showFixtures[mouseFixtures[i]].channels[u].purpose == "amber" && wauvChanged){
                showFixtures[mouseFixtures[i]].channels[u].value = mouseColors[4];
            } else if (showFixtures[mouseFixtures[i]].channels[u].purpose == "uv" && wauvChanged){
                showFixtures[mouseFixtures[i]].channels[u].value = mouseColors[5];
            }
            
        }
    }
}


void ofApp::simulateFixPanel(int i){    // Simulated fixtures panel
    int x = gX(panels[i].x);
    int y = gY(panels[i].y);
    int wi = panels[i].wi*defCellSize;
    int hi = panels[i].hi*defCellSize;
    int r = panels[i].r;
    int cellSize = panels[i].cellSize;
    int defSpace = panels[i].defSpace;
    string type = panels[i].type;
    string name = panels[i].name;
    int cellC = panels[i].x * panels[i].y;   //number of cells panel can fit (or resolution of cells)
    
    overFixture = false;
    
    if (mouseX > x && mouseX < x+wi && mouseY > y && mouseY < y+hi){
        level = 1;
        overPanel = true;
    }
    
    ofFill();
    ofSetColor(22, 22, 22);
    ofDrawRectRounded(x, y, wi, hi,r);
    ofSetColor(255, 255, 255);
    panelName.drawString(name, x + 30, y+defCellGap);
        
    vector<simulateFixtures> panelSimFixTemp = panels[i].panelSimFixtures;  //The following algorithm quickly ensures based off the fixture name and starting channel if previously saved fixtures still exist and if not remove them.
    panels[i].panelSimFixtures = {};
    for (int u = 0; u < panelSimFixTemp.size(); u++){
        string findName = panelSimFixTemp[u].name;
        int findChannel = panelSimFixTemp[u].startChannel;

        
        for (int m = 0; m < showFixtures.size(); m++){  // Checking for fixtures as mentioned in previous comment
            if (showFixtures[m].name == findName && showFixtures[m].startChannel == findChannel){
                panels[i].panelSimFixtures.push_back(panSimFixObj);
                panels[i].panelSimFixtures[panels[i].panelSimFixtures.size()-1] = panelSimFixTemp[u];
            }
        }
    }
    
    
    for (int n = 0; n < panels[i].panelSimFixtures.size(); n++){
        int fixtureIndex;
        for (int l = 0; l < showFixtures.size(); l++){
            if (showFixtures[l].name == panels[i].panelSimFixtures[n].name && showFixtures[l].startChannel == panels[i].panelSimFixtures[n].startChannel){
                fixtureIndex = l;   // Get the index of the showFixture the panel is trying to simulate based of previously stored fixture name and start channel
            }
        }
        ofFill();
        ofSetLineWidth(0);
        bool foundDimmer = false;
        int foundColors = 0;
        int dimmerValue = 0;
        
        vector<int> fixtureColor = {0,0,0};
        
        for (int c = 0; c < showFixtures[fixtureIndex].channels.size(); c++){   // Check if the fixture has data that the panel can effectively display visually such as brightness and color
            if (showFixtures[fixtureIndex].channels[c].purpose == "dimmer"){
                foundDimmer = true;
                dimmerValue = showFixtures[fixtureIndex].channels[c].value;
            } else if (showFixtures[fixtureIndex].channels[c].purpose == "red"){
                foundColors++;
                fixtureColor[0] = showFixtures[fixtureIndex].channels[c].value;
            } else if (showFixtures[fixtureIndex].channels[c].purpose == "green"){
                foundColors++;
                fixtureColor[1] = showFixtures[fixtureIndex].channels[c].value;
            } else if (showFixtures[fixtureIndex].channels[c].purpose == "blue"){
                foundColors++;
                fixtureColor[2] = showFixtures[fixtureIndex].channels[c].value;
            }
            
            
            
        }
//        if (foundDimmer){   // As mentioned in previous comment if the fixture contains data that can be shown visually, render it for the user to see
//            ofSetColor(255,255,255,255);
//            ofFill();
//            ofSetColor(255);
//            ofDrawRectangle(panels[i].panelSimFixtures[n].x, panels[i].panelSimFixtures[n].y, panels[i].panelSimFixtures[n].wi, panels[i].panelSimFixtures[n].hi);
//        }
        
        if (foundColors == 3){
            ofSetColor(fixtureColor[0],fixtureColor[1],fixtureColor[2]);
            ofDrawRectangle(panels[i].panelSimFixtures[n].x, panels[i].panelSimFixtures[n].y, panels[i].panelSimFixtures[n].wi, panels[i].panelSimFixtures[n].hi);
        }
        
        if (foundColors == 3 && foundDimmer){
            ofSetColor(0,0,0,ofMap(dimmerValue,0,255,255,0));
            ofDrawRectangle(panels[i].panelSimFixtures[n].x, panels[i].panelSimFixtures[n].y, panels[i].panelSimFixtures[n].wi, panels[i].panelSimFixtures[n].hi);
        }
        
        ofSetColor(0,0,0,127);
        ofFill();
        ofDrawRectangle(panels[i].panelSimFixtures[n].x + panels[i].panelSimFixtures[n].wi/4, panels[i].panelSimFixtures[n].y + panels[i].panelSimFixtures[n].hi/4, panels[i].panelSimFixtures[n].wi/2, panels[i].panelSimFixtures[n].hi/2);
        ofSetColor(255);
        panelName.drawString(to_string(dimmerValue), panels[i].panelSimFixtures[n].x + panels[i].panelSimFixtures[n].wi/4, panels[i].panelSimFixtures[n].y + panels[i].panelSimFixtures[n].hi/2);
        
        ofNoFill();
        ofSetLineWidth(2);
        ofSetColor(255,255,255,255);
        ofDrawRectangle(panels[i].panelSimFixtures[n].x, panels[i].panelSimFixtures[n].y, panels[i].panelSimFixtures[n].wi, panels[i].panelSimFixtures[n].hi);
        
        bool isSelected = false;
        for (int j = 0; j < mouseFixtures.size(); j++){
            if (mouseFixtures[j] == fixtureIndex){
                isSelected = true;
            }
        }
        
        if (isSelected){   // As mentioned in previous comment if the fixture contains data that can be shown visually, render it for the user to see
            ofNoFill();
            ofSetLineWidth(2);
            ofSetColor(0);
            ofDrawRectangle(panels[i].panelSimFixtures[n].x+2, panels[i].panelSimFixtures[n].y+2, panels[i].panelSimFixtures[n].wi-4, panels[i].panelSimFixtures[n].hi-4);
            ofSetColor(0,255,0);
            ofDrawRectangle(panels[i].panelSimFixtures[n].x, panels[i].panelSimFixtures[n].y, panels[i].panelSimFixtures[n].wi, panels[i].panelSimFixtures[n].hi);
        }
        
        
        // mouseX > x +cellSize && mouseX < x+wi-cellSize && mouseY > y + cellSize && mouseY < y+hi-cellSize
        if (overlay < 1){
        if (mouseX > panels[i].panelSimFixtures[n].x && mouseX < panels[i].panelSimFixtures[n].x + panels[i].panelSimFixtures[n].wi && mouseY > panels[i].panelSimFixtures[n].y && mouseY < panels[i].panelSimFixtures[n].y + panels[i].panelSimFixtures[n].hi){    // Check if mouse is over fixture
            overFixture = true;
            if (mouseExe == 1){
                bool fixtureAlreadySelected = false;
                for (int j = 0; j < mouseFixtures.size(); j++){ // ensure the fixture isn't appended if it is already selected
                    if (mouseFixtures[j] == fixtureIndex){
                        fixtureAlreadySelected = true;
                    }
                }
                if (fixtureAlreadySelected == false){
                    mouseFixtures.push_back(fixtureIndex);
                }
                
                if (mode == "off"){
                    mode = "";
                    for (int j = 0; j < showFixtures[fixtureIndex].channels.size(); j++){
                        showFixtures[fixtureIndex].channels[j].value = 0;
                    }
                    
                    vector<int> tempMouseFix = mouseFixtures;
                    mouseFixtures = {};
                    for (int j = 0; j < tempMouseFix.size(); j++){
                        if (tempMouseFix[j] != fixtureIndex){
                            mouseFixtures.push_back(tempMouseFix[j]);
                        }
                    }
                    
                }
                
            }
        }
    }
        
    

    
    }
    
    if (mode == "store" && mouseFixtures.size() == 1){
        ofSetColor(255, 0, 0, 255);
        ofNoFill();
//        ofDrawRectRounded(x, y, wi, cellSize, 5);
//        ofDrawRectRounded(x, y+hi-cellSize, wi, cellSize, 5);
        
        ofDrawRectRounded(x + cellSize, y + cellSize, wi - (cellSize*2), hi - (cellSize * 2), 5);
    }
    
        if (clickLeft(x, y, x+wi, y+hi) && overFixture == false && mode == "store" && mouseX > x +cellSize && mouseX < x+wi-cellSize && mouseY > y + cellSize && mouseY < y+hi-cellSize){   // Allow user to store a fixture in the simulation panel
            mode = "";
            if (mouseFixtures.size() == 1){
                panels[i].panelSimFixtures.push_back(panSimFixObj);
                panels[i].panelSimFixtures[panels[i].panelSimFixtures.size()-1].name = showFixtures[mouseFixtures[0]].name;
                
                panels[i].panelSimFixtures[panels[i].panelSimFixtures.size()-1].startChannel = showFixtures[mouseFixtures[0]].startChannel;
                panels[i].panelSimFixtures[panels[i].panelSimFixtures.size()-1].wi = cellSize;
                panels[i].panelSimFixtures[panels[i].panelSimFixtures.size()-1].hi = cellSize;
                
                    panels[i].panelSimFixtures[panels[i].panelSimFixtures.size()-1].x = mouseX-cellSize/2;
                    panels[i].panelSimFixtures[panels[i].panelSimFixtures.size()-1].y = mouseY-cellSize/2;
                
            }
        }

}


void ofApp::popUp(){    // Popup message
    level = 3;
    overlay = 1;
    ofSetColor(0, 0, 0,220);
    ofFill();
    ofDrawRectangle(0,0,ofGetWindowWidth(), ofGetWindowHeight());
    int headerHeight = defCellGap*3;
    int width = 7*defCellSize;
    int height = 4*defCellSize;
    
    ofSetColor(80, 80, 80);
    ofDrawRectRounded(ofGetWidth()/2-width/2, ofGetHeight()/2-height/2, width, height, defRounded);  // overlay body
    ofSetColor(popupObj.cR, popupObj.cG, popupObj.cB);
    ofRectangle header; // Define header variables
    header.x = ofGetWidth()/2-width/2;
    header.y = ofGetHeight()/2-height/2;
    header.width = width;
    header.height = headerHeight;
    ofDrawRectRounded(header, 5,5,0,0); // draw header
    ofNoFill();
    ofSetLineWidth(2);
    ofDrawRectRounded(ofGetWidth()/2-width/2, ofGetHeight()/2-height/2, width, height, defRounded);  // overlay body
    
    ofSetColor(255);
    panelType.drawString(popupObj.title, ofGetWidth()/2-(defCellSize), ofGetHeight()/2-height/2 + defCellGap*2);
    overlayBody.drawString(popupObj.msg, ofGetWidth()/2-(width/2)+defCellGap, ofGetHeight()/2-height/2 + defCellGap*6);
}

void ofApp::genShowFileDir(){   // Generates / checks the existance of the applications default data directory
    
    bool doesExist = ofDirectory::doesDirectoryExist(showFilesDir); // Check if the directory exists for the specificed directory.
                                                                    //(User defined functionality to be implemented in the future)
    
    if (doesExist == false){
        cout << "lucentShowFiles not found, creating directory now" << endl;
        bool createDirectory = ofDirectory::createDirectory(showFilesDir);
        if (createDirectory == true){
            cout << "Directory created successfully" << endl;
        } else {
            cout << "Error creating directory" << endl;
        }
    } else {
        cout << "lucentShowFiles already exists" << endl;
    }
}

vector<vector<vector<string>>> newPanels {{{}}};    // each index represents a panel object with its attributes as the child
string output;


int ofApp::intParseAttribute(int panelID, int attriID){  //  responsible for parsing interger attribute values from showData string
    vector<string> allChars;    // Stores the characters of each value.. this is required as a integer was converted to a string and will need to have each character converted back again. e.g: number 24 = 50, 52 (ascii codes)
    output = "";
    for (int a = 0; a < newPanels[panelID][attriID].size(); a++){

         allChars.push_back(newPanels[panelID][attriID][a]);
    }
    output = "";
    for (int l = 0; l < allChars.size(); l++){
        output = output + char(stoi(allChars[l]));
    }
    
    
    if (output != "*"){
        return(stoi(output));
    }
}

string ofApp::strParseAttribute(int panelID, int attriID){  //  responsible for parsing interger attribute values from showData string
    vector<string> allChars;    // Stores the characters of each value.. this is required as a integer was converted to a string and will need to have each character converted back again. e.g: number 24 = 50, 52 (ascii codes)
    output = "";
    for (int a = 0; a < newPanels[panelID][attriID].size(); a++){
     
         allChars.push_back(newPanels[panelID][attriID][a]);
    }
    output = "";
    for (int l = 0; l < allChars.size(); l++){
        output = output + char(stoi(allChars[l]));
    }

    
    if (output != "*"){
        return(output);
    }
}

vector<int> ofApp::vectParseAttribute(int panelID, int attriID){  //  responsible for parsing interger attribute values from showData string
    vector<string> allChars;    // Stores the characters of each value.. this is required as a integer was converted to a string and will need to have each character converted back again. e.g: number 24 = 50, 52 (ascii codes)
    vector<int> returnV = {};
    output = "";
    string indexItem;
    for (int a = 0; a < newPanels[panelID][attriID].size(); a++){
       
         allChars.push_back(newPanels[panelID][attriID][a]);
    }
    
    output = "";
    for (int l = 0; l < allChars.size(); l++){
        if (allChars[l] != "/"){
            indexItem = indexItem + allChars[l];
            
        } else {
            cout << "NEW ITEM" << endl;
            if (indexItem != ""){
                returnV.push_back(stoi(indexItem));
                indexItem = "";
            }
        }
        
    }
    
    cout << "OUT VECT: " << output << endl;

    if (returnV.size()>0){
        return(returnV);
    }
}


vector<storedBright> vectBriParseAttribute(int panelID, int attriID){  //  responsible for parsing interger attribute values from showData string
    vector<string> allChars;    // Stores the characters of each value.. this is required as a integer was converted to a string and will need to have each character converted back again. e.g: number 24 = 50, 52 (ascii codes)
    vector<int> tempArray = {};
    vector<storedBright> returnV = {};
    output = "";
    string indexItem;
    for (int a = 0; a < newPanels[panelID][attriID].size(); a++){
        cout << "SIZE: " << to_string(newPanels[panelID][attriID].size()) << endl;
         allChars.push_back(newPanels[panelID][attriID][a]);
    }
    
    output = "";
    for (int l = 0; l < allChars.size(); l++){
        if (allChars[l] != "/"){
            indexItem = indexItem + allChars[l];
            
        } else {
            cout << "NEW ITEM" << endl;
            if (indexItem != ""){
                tempArray.push_back(stoi(indexItem));
                indexItem = "";
            }
        }
    }
    
    if (tempArray.size() > 0){
        for (int j = 0; j < tempArray.size(); j+3){
            returnV.push_back(brightObj);
            returnV[returnV.size()-1].iden = tempArray[j];
            returnV[returnV.size()-1].set = tempArray[j+1];
            returnV[returnV.size()-1].value = tempArray[2];
            
        }
    }
    

    if (tempArray.size()>0){
        return(returnV);
    }
}


// Most common position for Cross button (typically User field Input overlay)
// X value = ofGetWindowWidth()/2+(defCellSize*4)
// Y value = ofGetWindowHeight()/2-(defCellSize/2)-defCellGap-defCellSize

void ofApp::crossButton(int x, int y, int gotoscreen, bool fill, bool checkOverlay){  // Draw "Cross" ("X") button, if gotoscreen = -1 then don't change the current screen
    if (fill == false){
        ofSetColor(255, 0, 0);    // draw "cross" exit button
        ofNoFill();
    } else {
        ofSetColor(180, 80, 80);
        ofFill();
    }
    
    
    ofSetLineWidth(2);
    ofDrawRectRounded(x, y, defCellSize, defCellSize,defRounded);
    ofSetLineWidth(5);
    if (fill == true){
        ofSetColor(30, 30, 30);
    }
    ofDrawLine(x+defCellGap, y+defCellGap, x+defCellGap+defCellSize-defCellGap*2, y + defCellSize - defCellGap);
    ofDrawLine(x+defCellGap, y+defCellSize-defCellGap, x+defCellGap+defCellSize-defCellGap*2, y + defCellGap);
    if (checkOverlay == true){
        if (overlay == 0){
            if (clickLeft(x, y, defCellSize, defCellSize)){
                setLi();
                strInputObj.fieldInputState = 0;
                overlay = 0;
                mode = "";
                lastInteraction = ofGetElapsedTimeMillis();
                patchSelect = 0;
                addPanelStage = 0;
                if (gotoscreen != -1){
                    screen = gotoscreen;
                }
        }
        }
    } else {
        if (clickLeft(x, y, defCellSize, defCellSize)){
            setLi();
            strInputObj.fieldInputState = 0;
            overlay = 0;
            mode = "";
            lastInteraction = ofGetElapsedTimeMillis();
            patchSelect = 0;
            addPanelStage = 0;
            if (gotoscreen != -1){
                screen = gotoscreen;
            }
    }
    }
    
}


void ofApp::strInput(string current, int max){  // UI element for allowing a user to input a string
    level = 3;
    overlay = 1;
    maxCharacterCount = max;
    
    
        ofSetColor(0, 0, 0,220);
        ofFill();
        ofDrawRectangle(0,0,ofGetWindowWidth(), ofGetWindowHeight());
        ofSetColor(171, 122, 50);
        ofDrawRectRounded(ofGetWindowWidth()/2-(defCellSize*5),ofGetWindowHeight()/2-(defCellSize/2),defCellSize*10, defCellSize,defRounded);
        ofSetColor(255, 255, 255);
        ofSetLineWidth(2);
        ofNoFill();
        usrInput.drawString(strInputObj.textValue, ofGetWindowWidth()/2-(defCellSize*5) + defCellGap, ofGetWindowHeight()/2-(defCellSize/2) + (defCellGap * 3 + 5));
        panelName.drawString(to_string(strInputObj.textValue.size()) + " / " + to_string(max), ofGetWindowWidth()/2-(defCellSize*5), ofGetWindowHeight()/2+(defCellSize/2)+defCellGap);
        pageMain.drawString(strInputHeading, ofGetWindowWidth()/2-(defCellSize*5), ofGetWindowHeight()/2-(defCellSize/2)-defCellGap);
        crossButton(ofGetWindowWidth()/2+(defCellSize*4), ofGetWindowHeight()/2-(defCellSize/2)-defCellGap-defCellSize, -1, false, false);

    if (strInputObj.search == 1){   // Search known Channel Purposes for fixture definitions and render results
        int heightOfCard = defCellSize;
        
        vector<int> foundDefID = {};        // Vector that stores the index position of Indexes that match the User Input
        if (strInputObj.textValue != ""){   // Ensure search algorithm only runs if the User provides an input
            for (int d = 0; d < knownChannels.size(); d++){     // Search through all indexes and check if they match User Input
                string partOfPurpose = "";  // Reset the current index search chars (new index appending to previous search)
                for (int l = 0; l < strInputObj.textValue.size(); l++){     // Append same number of characters as user has entered in
                    partOfPurpose = partOfPurpose + knownChannels[d].purpose[l];  // For the amount of characters the user has input, add the same number of characters from the current index under search
                }
                if (strInputObj.textValue == partOfPurpose){    // Check if the current part of the index under search matches what user has input
                    foundDefID.push_back(d);    // Append the Indexes position to the foundDefID vector.
                }
            }
        }
        
        
        if (foundDefID.size() > 0){ // Check if there are channel purposes found with regards to user input
            for (int i = 0; i < foundDefID.size(); i++){
                ofSetLineWidth(0);
                ofSetColor(80);
                ofFill();
                ofDrawRectRounded(ofGetWindowWidth()/2-(defCellSize*5),ofGetWindowHeight()/2-(defCellSize/2)+defCellSize+defCellGap*2,defCellSize*4+defCellGap, defCellSize/2,defRounded);
                ofSetColor(255);
                cardMain.drawString("Suggested Purpose:", ofGetWindowWidth()/2-(defCellSize*5)+defCellGap, ofGetWindowHeight()/2-(defCellSize/2)+defCellSize+defCellGap*4);
                ofSetColor(80);
                ofDrawRectRounded(ofGetWindowWidth()/2-(defCellSize*5),ofGetWindowHeight()/2-(defCellSize/2)+defCellSize+defCellGap*2+(defCellSize/2)+defCellGap + (i*(defCellGap+defCellSize)),defCellSize*10, heightOfCard,defRounded);
                ofSetColor(255);
                cardMain.drawString(knownChannels[foundDefID[i]].purpose, ofGetWindowWidth()/2-(defCellSize*5)+defCellGap, ofGetWindowHeight()/2-(defCellSize/2)+defCellSize+defCellGap*2+(defCellSize/2)+defCellGap + + (i*(defCellGap+defCellSize)) + defCellGap*2);
                fixMain.drawString(knownChannels[foundDefID[i]].desc, ofGetWindowWidth()/2-(defCellSize*5)+defCellGap, ofGetWindowHeight()/2-(defCellSize/2)+defCellSize+defCellGap*2+(defCellSize/2)+defCellGap + + (i*(defCellGap+defCellSize)) + defCellGap*4);
            }
        } else if (strInputObj.textValue.size() == 0){  // If no user input list all default channel purposes
            for (int i = 0; i < knownChannels.size(); i++){
                ofSetLineWidth(0);
                ofSetColor(80);
                ofFill();
                ofDrawRectRounded(ofGetWindowWidth()/2-(defCellSize*5),ofGetWindowHeight()/2-(defCellSize/2)+defCellSize+defCellGap*2,defCellSize*4+defCellGap, defCellSize/2,defRounded);
                ofSetColor(255);
                cardMain.drawString("Default Purposes:", ofGetWindowWidth()/2-(defCellSize*5)+defCellGap, ofGetWindowHeight()/2-(defCellSize/2)+defCellSize+defCellGap*4);
                ofSetColor(80);
                ofDrawRectRounded(ofGetWindowWidth()/2-(defCellSize*5),ofGetWindowHeight()/2-(defCellSize/2)+defCellSize+defCellGap*2+(defCellSize/2)+defCellGap + (i*(defCellGap+defCellSize)),defCellSize*10, heightOfCard,defRounded);
                ofSetColor(255);
                cardMain.drawString(knownChannels[i].purpose, ofGetWindowWidth()/2-(defCellSize*5)+defCellGap, ofGetWindowHeight()/2-(defCellSize/2)+defCellSize+defCellGap*2+(defCellSize/2)+defCellGap + + (i*(defCellGap+defCellSize)) + defCellGap*2);
                fixMain.drawString(knownChannels[i].desc, ofGetWindowWidth()/2-(defCellSize*5)+defCellGap, ofGetWindowHeight()/2-(defCellSize/2)+defCellSize+defCellGap*2+(defCellSize/2)+defCellGap + + (i*(defCellGap+defCellSize)) + defCellGap*4);
            }
        } else {    // If there are no channel purposes found with similar user input
            ofSetLineWidth(0);
            ofSetColor(80);
            ofFill();
            ofDrawRectRounded(ofGetWindowWidth()/2-(defCellSize*5),ofGetWindowHeight()/2-(defCellSize/2)+defCellSize+defCellGap*2,defCellSize*10, defCellSize/2,defRounded);
            ofSetColor(255);
            cardMain.drawString("No similar default purposes found", ofGetWindowWidth()/2-(defCellSize*5)+defCellGap*2+defCellSize, ofGetWindowHeight()/2-(defCellSize/2)+defCellSize+defCellGap*4);
        }
        
        
    
    }
    
}

void ofApp::demoShow(){ // Update all showFile values to hard-coded pre-defined values for demonstration purposes
    increaseFixtureCount = 0;
    dmxPatch.push_back(dmxPatchObj);
    dmxPatch[dmxPatch.size()-1].patchID = dmxPatch.size()-1;
    dmxPatch[dmxPatch.size()-1].channel = 1;
    dmxPatch[dmxPatch.size()-1].fixtureID = 0;
    dmxPatch[dmxPatch.size()-1].quantity = 7;
    dmxPatch[dmxPatch.size()-1].universe = 1;
    dmxPatch[dmxPatch.size()-1].verified = true;
    
    fixturesDef.push_back(fixObj);  // Add a fixture object
    fixturesDef[fixturesDef.size()-1].name = "AVE Cobra Wash 100";
    fixturesDef[fixturesDef.size()-1].simShow = true;
    fixturesDef[fixturesDef.size()-1].fixtureID = increaseFixtureCount;
    fixturesDef[fixturesDef.size()-1].universe = 1;
    fixturesDef[fixturesDef.size()-1].startChannel = 1;
    fixturesDef[fixturesDef.size()-1].channels.push_back(dmxChannelObj);
    fixturesDef[fixturesDef.size()-1].channels[fixturesDef[fixturesDef.size()-1].channels.size()-1].purpose = "pan";
    fixturesDef[fixturesDef.size()-1].channels.push_back(dmxChannelObj);
    fixturesDef[fixturesDef.size()-1].channels[fixturesDef[fixturesDef.size()-1].channels.size()-1].purpose = "tilt";
    fixturesDef[fixturesDef.size()-1].channels.push_back(dmxChannelObj);
    fixturesDef[fixturesDef.size()-1].channels[fixturesDef[fixturesDef.size()-1].channels.size()-1].purpose = "dimmer";
    fixturesDef[fixturesDef.size()-1].channels.push_back(dmxChannelObj);
    fixturesDef[fixturesDef.size()-1].channels[fixturesDef[fixturesDef.size()-1].channels.size()-1].purpose = "red";
    fixturesDef[fixturesDef.size()-1].channels.push_back(dmxChannelObj);
    fixturesDef[fixturesDef.size()-1].channels[fixturesDef[fixturesDef.size()-1].channels.size()-1].purpose = "green";
    fixturesDef[fixturesDef.size()-1].channels.push_back(dmxChannelObj);
    fixturesDef[fixturesDef.size()-1].channels[fixturesDef[fixturesDef.size()-1].channels.size()-1].purpose = "blue";
    fixturesDef[fixturesDef.size()-1].channels.push_back(dmxChannelObj);
    fixturesDef[fixturesDef.size()-1].channels[fixturesDef[fixturesDef.size()-1].channels.size()-1].purpose = "white";
    fixturesDef[fixturesDef.size()-1].channels.push_back(dmxChannelObj);
    fixturesDef[fixturesDef.size()-1].channels[fixturesDef[fixturesDef.size()-1].channels.size()-1].purpose = "amber";
    fixturesDef[fixturesDef.size()-1].channels.push_back(dmxChannelObj);
    fixturesDef[fixturesDef.size()-1].channels[fixturesDef[fixturesDef.size()-1].channels.size()-1].purpose = "uv";
    increaseFixtureCount++;
    
    fixturesDef.push_back(fixObj);  // Append a fixture definition to the Defined Fixtures vector (fixturesDef)
    fixturesDef[fixturesDef.size()-1].name = "The Lemmon Fixture"; // Give the defined Fixture a name
    fixturesDef[fixturesDef.size()-1].fixtureID = increaseFixtureCount; // Ensure the fixtureID is unique by setting it to the global count variable increaseFixtureCount
    increaseFixtureCount++; // Prepare the increaseFixtureCount global count variable for the next fixture definition
    fixturesDef[fixturesDef.size()-1].channels.push_back(dmxChannelObj);    // Add a channel to the fixture definition
    fixturesDef[fixturesDef.size()-1].channels[fixturesDef[fixturesDef.size()-1].channels.size()-1].purpose = "strobe"; // Give the last appended channel a purpose for functionality later on
    fixturesDef[fixturesDef.size()-1].channels.push_back(dmxChannelObj);
    fixturesDef[fixturesDef.size()-1].channels[fixturesDef[fixturesDef.size()-1].channels.size()-1].purpose = "smoke";
    fixturesDef[fixturesDef.size()-1].channels.push_back(dmxChannelObj);
    fixturesDef[fixturesDef.size()-1].channels[fixturesDef[fixturesDef.size()-1].channels.size()-1].purpose = "Red";
    fixturesDef[fixturesDef.size()-1].channels.push_back(dmxChannelObj);
    fixturesDef[fixturesDef.size()-1].channels[fixturesDef[fixturesDef.size()-1].channels.size()-1].purpose = "Green";
    fixturesDef[fixturesDef.size()-1].channels.push_back(dmxChannelObj);
    fixturesDef[fixturesDef.size()-1].channels[fixturesDef[fixturesDef.size()-1].channels.size()-1].purpose = "Blue";

    // Draw some UI panels
    panels.push_back(panelObj);
    panels[panels.size()-1].type = "fixtures";
    panels[panels.size()-1].name = "Fixtures";
    panels[panels.size()-1].x = 19;
    panels[panels.size()-1].y = 4;
    panels[panels.size()-1].wi = 4;
    panels[panels.size()-1].hi = 7;
    panels[panels.size()-1].cellSize = defCellSize;
    
    panels.push_back(panelObj);
    panels[panels.size()-1].type = "fixsimulation";
    panels[panels.size()-1].name = "Fixture Simulation";
    panels[panels.size()-1].x = 0;
    panels[panels.size()-1].y = 0;
    panels[panels.size()-1].wi = 17;
    panels[panels.size()-1].hi = 3;
    panels[panels.size()-1].cellSize = defCellSize*0.75;
    
    panels.push_back(panelObj);
    panels[panels.size()-1].type = "controlpanel";
    panels[panels.size()-1].name = "Control Panel";
    panels[panels.size()-1].x = 20;
    panels[panels.size()-1].y = 0;
    panels[panels.size()-1].wi = 5;
    panels[panels.size()-1].hi = 3;
    panels[panels.size()-1].cellSize = 40;
    
    panels.push_back(panelObj);
    panels[panels.size()-1].type = "controlpanel";
    panels[panels.size()-1].name = "Control Panel";
    panels[panels.size()-1].x = 8;
    panels[panels.size()-1].y = 11;
    panels[panels.size()-1].wi = 5;
    panels[panels.size()-1].hi = 3;
    panels[panels.size()-1].cellSize = 40;
    
    panels.push_back(panelObj);
    panels[panels.size()-1].type = "rgbmixer";
    panels[panels.size()-1].name = "RGB mixer";
    panels[panels.size()-1].x = 0;
    panels[panels.size()-1].y = 4;
    panels[panels.size()-1].wi = 5;
    panels[panels.size()-1].hi = 7;
    panels[panels.size()-1].cellSize = 40;
    
    panels.push_back(panelObj);
    panels[panels.size()-1].type = "brimixer";
    panels[panels.size()-1].name = "Bri Mix";
    panels[panels.size()-1].x = 6;
    panels[panels.size()-1].y = 4;
    panels[panels.size()-1].wi = 1;
    panels[panels.size()-1].hi = 7;
    panels[panels.size()-1].cellSize = 40;
    
    panels.push_back(panelObj);
    panels[panels.size()-1].type = "wauv";
    panels[panels.size()-1].name = "WAUV Mix";
    panels[panels.size()-1].x = 8;
    panels[panels.size()-1].y = 4;
    panels[panels.size()-1].wi = 5;
    panels[panels.size()-1].hi = 7;
    panels[panels.size()-1].cellSize = 40;
    
    panels.push_back(panelObj);
    panels[panels.size()-1].type = "posMixer";
    panels[panels.size()-1].name = "Pos Mixer";
    panels[panels.size()-1].x = 14;
    panels[panels.size()-1].y = 4;
    panels[panels.size()-1].wi = 4;
    panels[panels.size()-1].hi = 7;
    panels[panels.size()-1].cellSize = 40;
    
    panels.push_back(panelObj);
    panels[panels.size()-1].type = "colorstor";
    panels[panels.size()-1].name = "Colors";
    panels[panels.size()-1].x = 0;
    panels[panels.size()-1].y = 11;
    panels[panels.size()-1].wi = 5;
    panels[panels.size()-1].hi = 3;
    panels[panels.size()-1].cellSize = 40;
    
    panels.push_back(panelObj);
    panels[panels.size()-1].type = "bristor";
    panels[panels.size()-1].name = "Stored\nBrightness";
    panels[panels.size()-1].x = 5;
    panels[panels.size()-1].y = 11;
    panels[panels.size()-1].wi = 3;
    panels[panels.size()-1].hi = 3;
    panels[panels.size()-1].cellSize = 40;
    
    panels.push_back(panelObj);
    panels[panels.size()-1].type = "posStor";
    panels[panels.size()-1].name = "Positions";
    panels[panels.size()-1].x = 14;
    panels[panels.size()-1].y = 11;
    panels[panels.size()-1].wi = 4;
    panels[panels.size()-1].hi = 3;
    panels[panels.size()-1].cellSize = 40;
    
    genShowFixtures();  // Update showFixtures vector with patches
    setLi();    // Set last interaction
    showSplash = true;  // Show the splashscreen
}



void ofApp::delOverlay(){   // Render prompt if user wants to follow through with the deletion of data
    level = 3;
    overlay = 1;
    ofSetColor(0, 0, 0,220);
    ofFill();
    ofDrawRectangle(0,0,ofGetWindowWidth(), ofGetWindowHeight());
    int headerHeight = defCellGap*3;
    int width = 7*defCellSize;
    int height = 4*defCellSize;
    
    int buttonWidth = defCellSize*2;
    int buttonHeight = defCellSize;
    
    ofSetColor(80, 80, 80);
    ofDrawRectRounded(ofGetWidth()/2-width/2, ofGetHeight()/2-height/2, width, height,defRounded);  // overlay body
    ofSetColor(235, 64, 52);
    ofRectangle header; // Define header variables
    header.x = ofGetWidth()/2-width/2;
    header.y = ofGetHeight()/2-height/2;
    header.width = width;
    header.height = headerHeight;
    ofDrawRectRounded(header, 5,5,0,0); // draw header
    ofNoFill();
    ofSetLineWidth(2);
    ofDrawRectRounded(ofGetWidth()/2-width/2, ofGetHeight()/2-height/2, width, height, defRounded);  // overlay body
    
    ofSetColor(255);
    panelType.drawString("DELETE/REMOVE", ofGetWidth()/2-(defCellSize), ofGetHeight()/2-height/2 + defCellGap*2);
    overlayBody.drawString("Are you sure you want to\n delete " + delData2, ofGetWidth()/2-(width/2)+defCellGap, ofGetHeight()/2-height/2 + defCellGap*6);
    
    // YES button
    ofSetColor(235, 64, 52);
    ofDrawRectRounded(ofGetWidth()/2 - defCellGap - buttonWidth, ofGetHeight()/2+height/2 - defCellGap - buttonHeight, buttonWidth, buttonHeight, defRounded);
    if (clickLeft(ofGetWidth()/2 - defCellGap - buttonWidth, ofGetHeight()/2+height/2 - defCellGap - buttonHeight, buttonWidth, buttonHeight)){
        setLi();
        overlay = 0;
        ofDirectory::removeDirectory(delData1, true, false);
        showsListAlt = {};
        for (int i = 0; i < showsList.size(); i++){
            if (showsList[i].name != delData2){
                showsListAlt.push_back(showFileObject);
                showsListAlt[showsListAlt.size()-1].name = showsList[i].name;
                showsListAlt[showsListAlt.size()-1].modified = showsList[i].modified;
                showsListAlt[showsListAlt.size()-1].path = showsList[i].path;
                showsListAlt[showsListAlt.size()-1].freshStart = showsList[i].freshStart;
                showsListAlt[showsListAlt.size()-1].active = showsList[i].active;
            
            }
        }
        showsList = {};

        for (int i = 0; i < showsListAlt.size(); i++){
            showsList.push_back(showFileObject);
            showsList[showsList.size()-1].name = showsListAlt[i].name;
            showsList[showsList.size()-1].modified = showsListAlt[i].modified;
            showsList[showsList.size()-1].path = showsListAlt[i].path;
            showsList[showsList.size()-1].freshStart = showsListAlt[i].freshStart;
            showsList[showsList.size()-1].active = showsListAlt[i].active;
        }
        //cout << showsList.size() << endl;
      
        delData1 = "";
        delData2 = "";
        showDel = false;
        showsListAlt = {};
    }
    
    
    
    // NO button
    ofSetColor(255);
    ofDrawRectRounded(ofGetWidth()/2 + defCellGap, ofGetHeight()/2+height/2 - defCellGap - buttonHeight, buttonWidth, buttonHeight, defRounded);
    if (clickLeft(ofGetWidth()/2 + defCellGap, ofGetHeight()/2+height/2 - defCellGap - buttonHeight, buttonWidth, buttonHeight)){
        setLi();
        showDel = false;
        delData1 = "";
        delData2 = "";
        overlay = 0;
    }
    
    
    
    cardMain.drawString("YES", ofGetWidth()/2 - buttonWidth + defCellGap, ofGetHeight()/2+height/2 + defCellGap*2 - buttonHeight);
    cardMain.drawString("NO", ofGetWidth()/2 + defCellGap*2, ofGetHeight()/2+height/2 + defCellGap*2 - buttonHeight);
    
    
}

void ofApp::uiButton(int x, int y, string text, bool active, int width, int height){    // Draws a UI button (e.g next page)
    ofFill();
    ofSetColor(80,80,80);
    ofDrawRectangle(x,y,width, height);
    ofNoFill();
    ofSetLineWidth(2);
    if (active){
        ofSetColor(252, 186, 3);
    } else {
        ofSetColor(30);
    }
    
    ofDrawRectangle(x,y,width, height);
    ofSetColor(255);
    fixMain.drawString(text, x+defCellGap*0.8, y+defCellGap*1.6);
}

void ofApp::miniButton(int x, int y, string cha, bool active){  // Draws a "mini" UI button. (Usually used for small actions e.g Add/Remove channel)
    ofFill();
    ofSetLineWidth(2);
    if (active){
        ofSetColor(80,80,80);
        ofDrawRectangle(x,y,defCellSize/2, defCellSize/2);
        ofNoFill();
        ofSetColor(252, 186, 3);
    } else {
        ofSetColor(40);
        ofDrawRectangle(x,y,defCellSize/2, defCellSize/2);
        ofNoFill();
        ofSetColor(30);
    }
    
    ofDrawRectangle(x,y,defCellSize/2, defCellSize/2);
    ofSetColor(255);
    fixMain.drawString(cha, x+defCellGap*0.8, y+defCellGap*1.6);
}

void ofApp::fixtureConfig(){   // Showfile screen (load, save, delete and configuration)
    int showPatchHeight = defCellSize/2;
    int showPatchWidth = 8*defCellSize+defCellGap;
    
    int showFixtureHeight = defCellSize*2.5;
    int showFixtureWidth = 6*defCellSize;
    
    int totalHeightofFixtureDefs = 0;   // Total height of all previous fixture cards including defCellGap and page header. (allows prior cards to draw all channels with following cards to not draw over the top)

    if (mouseY < defCellGap + defCellSize*3){
        level = 1;
    }
    
    ofFill();
    
    for (int i = 0; i < fixturesDef.size(); i++){   // List all fixture definitions
        ofSetColor(80, 80, 80);
        ofFill();
        ofSetLineWidth(0);
        ofDrawRectRounded(showPatchWidth+defCellSize, showFixturestartheight+totalHeightofFixtureDefs, showFixtureWidth, showFixtureHeight + (fixturesDef[i].channels.size()*showPatchHeight),defRounded);
        if (selectedFixDef == fixturesDef[i].fixtureID){
            ofSetColor(64, 181, 43);
            ofNoFill();
            ofSetLineWidth(4);
            ofDrawRectRounded(showPatchWidth+defCellSize, showFixturestartheight+totalHeightofFixtureDefs, showFixtureWidth, showFixtureHeight + (fixturesDef[i].channels.size()*showPatchHeight),defRounded);
            ofSetLineWidth(1);
        }
        
        ofFill();
        ofSetColor(30);
        ofDrawRectangle(showPatchWidth+defCellSize + defCellGap + defCellSize, showFixturestartheight+totalHeightofFixtureDefs + (defCellGap*2.7), showFixtureWidth-defCellSize - defCellGap*2, showPatchHeight);
        ofSetColor(255);
        ofSetLineWidth(2);
        ofNoFill();
        ofDrawRectangle(showPatchWidth+defCellSize + defCellGap + defCellSize, showFixturestartheight+totalHeightofFixtureDefs + (defCellGap*2.7), showFixtureWidth-defCellSize - defCellGap*2, showPatchHeight);

        ofFill();
        
        for (int u = 0; u < fixturesDef[i].channels.size(); u++){   // Display all channels for fixture definition
            ofFill();
            ofSetColor(30);
            ofSetLineWidth(0);
            ofDrawRectangle(showPatchWidth+(defCellSize*3) + defCellGap, showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*10.7)+(showPatchHeight*u), showFixtureWidth-(defCellGap*2)-(defCellSize*2), showPatchHeight);
            ofSetLineWidth(1);
            ofSetColor(255);
            ofNoFill();
            ofDrawLine(showPatchWidth + defCellGap + defCellSize+(defCellSize*2), showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*10.7)+(showPatchHeight*u), showPatchWidth + defCellGap + defCellSize+(defCellSize*2), showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*10.7)+(showPatchHeight*u)+showPatchHeight);
            ofDrawRectangle(showPatchWidth + defCellGap + defCellSize, showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*10.7)+(showPatchHeight*u), showFixtureWidth-(defCellGap*2), showPatchHeight);
            
            ofSetColor(255);
            fixMain.drawString("#" + to_string(u), showPatchWidth + defCellGap + defCellSize*1.8, showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*10.7)+(showPatchHeight*u) + defCellGap*1.7);
            fixMain.drawString(fixturesDef[i].channels[u].purpose, showPatchWidth + (defCellGap*2) + defCellSize*3, showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*10.7)+(showPatchHeight*u) + defCellGap*1.7);
            
            if (overlay == 0 && level == 0 && clickLeft(showPatchWidth + defCellGap + defCellSize+(defCellSize*2), showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*10.7)+(showPatchHeight*u), showFixtureWidth-(defCellGap*2) - (defCellSize*2), showPatchHeight)){
                setLi();
                ptrType = 0;
                fieldPtr = &fixturesDef[i].channels[u].purpose;
                strInputHeading = "Channel #" + to_string(u) + " purpose";
                strInputObj.fieldInputState = 1;
                strInputObj.screenType = 0;
                strInputObj.search = 1;
                strInputObj.current = fixturesDef[i].channels[u].purpose;
                strInputObj.maxCount = 18;
                strInputObj.textValue = fixturesDef[i].channels[u].purpose;
                selectedFixDef = fixturesDef[i].fixtureID;
                
            }
            
        }
        
        ofSetColor(255);
        fixMain.drawString("FixtureID #" + to_string(fixturesDef[i].fixtureID), showPatchWidth+defCellSize + defCellGap, showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*2));
        fixMain.drawString("Name:    " + fixturesDef[i].name, showPatchWidth+defCellSize + defCellGap, showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*4.7));
        fixMain.drawString("Channel Count:    " + to_string(fixturesDef[i].channels.size()), showPatchWidth+defCellSize + defCellGap, showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*7.7));
        if (fixturesDef[i].channels.size() > 0){
            miniButton(showPatchWidth+(defCellSize*4) + defCellGap, showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*6), "-", true);
            if (waitTime(defWaitTime) && overlay == 0 && level == 0 && clickLeft(showPatchWidth+(defCellSize*4) + defCellGap, showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*6), defMiniButton, defMiniButton)){
                setLi();
                fixturesDef[i].channels.pop_back();
                selectedFixDef = fixturesDef[i].fixtureID;
            }
        }
        
        if (fixturesDef[i].channels.size() < 511){
            miniButton(showPatchWidth+(defCellSize*4) + (defCellGap*2) + (defCellSize/2), showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*6), "+", true);
            if (waitTime(defWaitTime) && overlay == 0 && level == 0 && clickLeft(showPatchWidth+(defCellSize*4) + (defCellGap*2) + (defCellSize/2), showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*6), defMiniButton, defMiniButton)){
                setLi();
                fixturesDef[i].channels.push_back(dmxChannelObj);
                selectedFixDef = fixturesDef[i].fixtureID;
            }
        }
        
        fixMain.drawString("Channel", showPatchWidth+defCellSize + defCellGap, showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*10));
        fixMain.drawString("Purpose", showPatchWidth+(defCellSize*3) + defCellGap, showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*10));
        
        
        if (overlay == 0 && level == 0 && clickLeft(showPatchWidth+defCellSize + defCellGap + defCellSize, showFixturestartheight+totalHeightofFixtureDefs + (defCellGap*2.7), showFixtureWidth-defCellSize - defCellGap*2, showPatchHeight)){
            setLi();
            ptrType = 0;
            fieldPtr = &fixturesDef[i].name;
            strInputHeading = "Fixture #" + to_string(fixturesDef[i].fixtureID) + " name";
            strInputObj.fieldInputState = 1;
            strInputObj.screenType = 0;
            strInputObj.search = 0;
            strInputObj.current = fixturesDef[i].name;
            strInputObj.maxCount = 24;
            strInputObj.textValue = fixturesDef[i].name;
            selectedFixDef = fixturesDef[i].fixtureID;
        }
        
        
        if (overlay == 0 && level == 0 && clickLeft(showPatchWidth+defCellSize, showFixturestartheight+totalHeightofFixtureDefs, showFixtureWidth, showFixtureHeight + (fixturesDef[i].channels.size()*showPatchHeight))){
            setLi();
            selectedFixDef = fixturesDef[i].fixtureID;

        }
        
        totalHeightofFixtureDefs = totalHeightofFixtureDefs + defCellGap + (fixturesDef[i].channels.size() * showPatchHeight) + showFixtureHeight;
    }
    
    scrollButtons(defCellGap + 15*defCellSize, defCellGap + 3*defCellSize, defCellSize, (ofGetHeight()-defCellGap-defCellSize*4), showFixturestartheight, &showFixturestartheight, totalHeightofFixtureDefs, (defCellGap*2)+(3*defCellSize), ofGetHeight()-defCellSize);

    // scrollButtons(int x, int y, int width, int height, int scrollValue, int *scrollValuePtr, int scrollHeightTotal, int scrollStartHeight, int scrollEndHeight);
    
    for (int i = 0; i < dmxPatch.size(); i++){  // List all DMX Patches
        ofSetColor(30, 30, 30);
        ofFill();
        ofDrawRectangle(0, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), showPatchWidth, showPatchHeight);
        int patchPassScore = 0; // if pass score reaches 4 then the patch may be verified and used for valid Show Fixtures and DMX output
        dmxPatch[i].verified = false;
        ofFill();
        
        int patchFixID = dmxPatch[i].fixtureID;
        bool patchFixIDFound = false;
        for (int m = 0; m < fixturesDef.size(); m++){   // Check if the fixture definitions referenced exists
            if (patchFixID == fixturesDef[m].fixtureID){
                patchFixIDFound = true;
            }
        }
        
        if (patchFixIDFound){
            patchPassScore++;
        }
        

        if (dmxPatch[i].universe != 1){ // LucentOP only allows one DMX universe currently
            ofSetColor(92, 19, 19);
            ofDrawRectangle(defCellGap+defCellSize*2, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellSize*2, showPatchHeight);
        } else {
            patchPassScore++;
        }
        
        if (dmxPatch[i].channel < 1 || dmxPatch[i].channel > 512){  // Check if channel is complient with the DMX512 protocol
            ofSetColor(92, 19, 19);
            ofDrawRectangle(defCellGap+defCellSize*4, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellSize*2, showPatchHeight);
        } else {
            patchPassScore++;
        }
        
        if (dmxPatch[i].quantity < 1 || dmxPatch[i].quantity > 512){
            ofSetColor(92, 19, 19);
            ofDrawRectangle(defCellGap+defCellSize*6, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellSize*2, showPatchHeight);
        } else {
            patchPassScore++;
        }
        
        if (patchSelect != NULL){
            if (patchSelect == &dmxPatch[i].fixtureID){ // Check if DMX patch is selected
                if (flashOn == true){
                    ofSetLineWidth(0);
                    ofFill();
                    ofSetColor(171, 122, 50);
                    ofDrawRectangle(defCellGap, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellSize*2, showPatchHeight);   // Beige flash
                }
                ofSetColor(66, 132, 245);
                ofSetLineWidth(4);
                ofNoFill();
                ofDrawRectangle(defCellGap, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellSize*2, showPatchHeight);   // Selected blue boarder

            } else if (patchSelect == &dmxPatch[i].universe){
                if (flashOn == true){
                    ofSetLineWidth(0);
                    ofFill();
                    ofSetColor(171, 122, 50);
                    ofDrawRectangle(defCellGap+defCellSize*2, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellSize*2, showPatchHeight);   // Beige flash
                }
                ofSetColor(66, 132, 245);
                ofSetLineWidth(4);
                ofNoFill();
                ofDrawRectangle(defCellGap+defCellSize*2, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellSize*2, showPatchHeight);
            } else if (patchSelect == &dmxPatch[i].channel){
                if (flashOn == true){
                    ofSetLineWidth(0);
                    ofFill();
                    ofSetColor(171, 122, 50);
                    ofDrawRectangle(defCellGap+defCellSize*4, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellSize*2, showPatchHeight);   // Beige flash
                }
                ofSetColor(66, 132, 245);
                ofSetLineWidth(4);
                ofNoFill();
                ofDrawRectangle(defCellGap+defCellSize*4, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellSize*2, showPatchHeight);
            } else if (patchSelect == &dmxPatch[i].quantity){
                if (flashOn == true){
                    ofSetLineWidth(0);
                    ofFill();
                    ofSetColor(171, 122, 50);
                    ofDrawRectangle(defCellGap+defCellSize*6, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellSize*2, showPatchHeight);   // Beige flash
                }
                ofSetColor(66, 132, 245);
                ofSetLineWidth(4);
                ofNoFill();
                ofDrawRectangle(defCellGap+defCellSize*6, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellSize*2, showPatchHeight);
            }
            ofSetLineWidth(1);
            ofSetColor(30, 30, 30);
            
            if (ofGetElapsedTimeMillis() > lastFlash + defFlashTime){
                if (flashOn == false){
                    
                    flashOn = true;
                } else {
                    flashOn = false;
                }
                lastFlash = ofGetElapsedTimeMillis();
            }
        }
        
        ofFill();

        ofSetColor(255);
        fixMain.drawString(to_string(dmxPatch[i].fixtureID), defCellGap*2+defCellSize*0.7, showPatchstartheight+(i*showPatchHeight)+defCellGap*2.8+(3*defCellSize));
        fixMain.drawString(to_string(dmxPatch[i].universe), defCellGap*2+defCellSize*0.7+defCellSize*2, showPatchstartheight+(i*showPatchHeight)+defCellGap*2.8+(3*defCellSize));
        if (overlay == 0 && level == 0 && clickLeft(defCellGap, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellSize*2, showPatchHeight)){
            setLi();
            patchSelect = &dmxPatch[i].fixtureID;
            strInputHeading = "fixtureID";
            strInputObj.fieldInputState = 1;
            strInputObj.screenType = 1;
            strInputObj.search = 0;
            strInputObj.current = to_string(dmxPatch[i].fixtureID);
            strInputObj.maxCount = 3;
            strInputObj.textValue = to_string(dmxPatch[i].fixtureID);
            ptrType = 1;
            fieldPtrInt = &dmxPatch[i].fixtureID;
            lastFlash = ofGetElapsedTimeMillis();
        } else if (overlay == 0 && level == 0 && clickLeft(defCellGap+defCellSize*2, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellSize*2, showPatchHeight)){
            setLi();
            patchSelect = &dmxPatch[i].universe;
            strInputHeading = "universe";
            strInputObj.fieldInputState = 1;
            strInputObj.screenType = 1;
            strInputObj.search = 0;
            strInputObj.current = to_string(dmxPatch[i].universe);
            strInputObj.maxCount = 3;
            strInputObj.textValue = to_string(dmxPatch[i].universe);
            ptrType = 1;
            fieldPtrInt = &dmxPatch[i].universe;
            lastFlash = ofGetElapsedTimeMillis();
        } else if (overlay == 0 && level == 0 && clickLeft(defCellGap+defCellSize*4, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellSize*2, showPatchHeight)){
            setLi();
            patchSelect = &dmxPatch[i].channel;
            strInputHeading = "channel";
            strInputObj.fieldInputState = 1;
            strInputObj.screenType = 1;
            strInputObj.search = 0;
            strInputObj.current = to_string(dmxPatch[i].channel);
            strInputObj.maxCount = 3;
            strInputObj.textValue = to_string(dmxPatch[i].channel);
            ptrType = 1;
            fieldPtrInt = &dmxPatch[i].channel;
            lastFlash = ofGetElapsedTimeMillis();
        } else if (overlay == 0 && level == 0 && clickLeft(defCellGap+defCellSize*6, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellSize*2, showPatchHeight)){
            setLi();
            patchSelect = &dmxPatch[i].quantity;
            strInputHeading = "quantity";
            strInputObj.fieldInputState = 1;
            strInputObj.screenType = 1;
            strInputObj.search = 0;
            strInputObj.current = to_string(dmxPatch[i].quantity);
            strInputObj.maxCount = 3;
            strInputObj.textValue = to_string(dmxPatch[i].quantity);
            ptrType = 1;
            fieldPtrInt = &dmxPatch[i].quantity;
            lastFlash = ofGetElapsedTimeMillis();
        }
        fixMain.drawString(to_string(dmxPatch[i].channel), defCellGap*2+defCellSize*0.7+defCellSize*4, showPatchstartheight+(i*showPatchHeight)+defCellGap*2.8+(3*defCellSize));
        fixMain.drawString(to_string(dmxPatch[i].quantity), defCellGap*2+defCellSize*0.7+defCellSize*6, showPatchstartheight+(i*showPatchHeight)+defCellGap*2.8+(3*defCellSize));
        
        if (patchPassScore == 4){   // If patch meets required score, verify it
            ofSetColor(80, 255, 80);
            dmxPatch[i].verified = true;
        } else {
            ofSetColor(255, 80, 80);
        }
        
        ofDrawRectangle(0, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellGap, showPatchHeight);
        ofSetColor(80, 80, 80);
        ofDrawLine(defCellGap+defCellSize*2, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellGap+defCellSize*2, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize)+showPatchHeight);
        ofDrawLine(defCellGap+defCellSize*4, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellGap+defCellSize*4, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize)+showPatchHeight);
        ofDrawLine(defCellGap+defCellSize*6, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellGap+defCellSize*6, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize)+showPatchHeight);
        ofSetLineWidth(1);
        ofDrawLine(0, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize)+showPatchHeight, showPatchWidth, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize)+showPatchHeight);
        
        ofSetColor(30, 30, 30);
    }

    ofFill();
    ofSetColor(80, 80, 80);
    ofDrawRectangle(0, 0, ofGetWidth(), defCellGap + defCellSize*3);
    
    ofNoFill();
    ofSetColor(255);
    ofSetLineWidth(2);
    ofSetColor(255);
    pageMain.drawString("Fixtures & Patching", defCellGap*2, defCellSize);
    pageSub.drawString("Edit definitions and patch to DMX channels", defCellGap*2, defCellSize*1.7);
    
    fixMain.drawString("DMX patching", defCellGap*2, defCellSize*2 + defCellGap);
    if (dmxPatch.size() > 0){
    miniButton(defCellGap*2+defCellSize*2, defCellSize*2 - defCellGap, "-", true);
    if(overlay == 0 && waitTime(defWaitTime) && clickLeft(defCellGap*2+defCellSize*2, defCellSize*2 - defCellGap, defCellSize/2, defCellSize/2)){
        setLi();
        dmxPatch.pop_back();
        
    }
    
    } else {
        miniButton(defCellGap*2+defCellSize*2, defCellSize*2 - defCellGap, "-", false);
    }
    
    miniButton(defCellGap*3+defCellSize*2+defCellSize/2, defCellSize*2 - defCellGap, "+", true);
    if(overlay == 0 && waitTime(defWaitTime) && clickLeft(defCellGap*3+defCellSize*2+defCellSize/2, defCellSize*2 - defCellGap, defCellSize/2, defCellSize/2)){
        setLi();
        dmxPatch.push_back(dmxPatchObj);
        dmxPatch[dmxPatch.size()-1].patchID = dmxPatch.size()-1;
        
        if (dmxPatch.size() == 1){
            dmxPatch[dmxPatch.size()-1].channel = 0;
        } else if (dmxPatch.size() > 1){
            
            int patchFixID = dmxPatch[dmxPatch.size()-2].fixtureID;
            bool patchFixIDFound = false;
            for (int m = 0; m < fixturesDef.size(); m++){
                if (patchFixID == fixturesDef[m].fixtureID){
                    patchFixIDFound = true;
                }
            }
            
            if (patchFixIDFound){
                dmxPatch[dmxPatch.size()-1].channel = fixturesDef[dmxPatch[dmxPatch.size()-2].fixtureID].channels.size() * dmxPatch[dmxPatch.size()-2].quantity + dmxPatch[dmxPatch.size()-2].channel;
            } else {
                dmxPatch[dmxPatch.size()-1].channel = dmxPatch[dmxPatch.size()-2].channel + 1;
            }
        }
        
        
        dmxPatch[dmxPatch.size()-1].universe = 1;
        dmxPatch[dmxPatch.size()-1].quantity = 0;
        dmxPatch[dmxPatch.size()-1].fixtureID = 0;
        
    }

    fixMain.drawString("Fixture dictionary", defCellGap*2+defCellSize*9, defCellSize*2 + defCellGap);
    
    if (selectedFixDef != -1){
        miniButton(defCellGap*2+defCellSize*11.5, defCellSize*2 - defCellGap, "-", true);
        if(overlay == 0 && waitTime(defWaitTime) && clickLeft(defCellGap*2+defCellSize*11.5, defCellSize*2 - defCellGap, defCellSize/2, defCellSize/2)){
            setLi();
            int delFixID = selectedFixDef;
            vector<dmxFixtureclass> fixturesTemp = {};
            fixturesTemp = fixturesDef;
            fixturesDef = {};
            for (int u = 0; u < fixturesTemp.size(); u++){
                if (fixturesTemp[u].fixtureID != delFixID){
                    fixturesDef.push_back(fixturesTemp[u]);
                }
            }
            selectedFixDef = -1;
            
        }
    } else {
        miniButton(defCellGap*2+defCellSize*11.5, defCellSize*2 - defCellGap, "-", false);
    }
    
    
    miniButton(defCellGap*2+defCellSize*11.5+defCellGap+defCellSize/2, defCellSize*2 - defCellGap, "+", true);
    if(overlay == 0 && waitTime(defWaitTime) && clickLeft(defCellGap*2+defCellSize*11.5+defCellGap+defCellSize/2, defCellSize*2 - defCellGap, defCellSize/2, defCellSize/2)){
        setLi();
        fixturesDef.push_back(fixObj);
        fixturesDef[fixturesDef.size()-1].fixtureID = increaseFixtureCount;
        fixturesDef[fixturesDef.size()-1].name = "fixture #" +  to_string(increaseFixtureCount);
        increaseFixtureCount++;
    }
    
    
    
    ofDrawLine(0, defCellSize*2.5, ofGetWidth(), defCellSize*2.5);
    fixMain.drawString("FixtureID", defCellGap*2, defCellSize*3);
    ofDrawLine(defCellSize*2+defCellGap, defCellSize*2.5, defCellSize*2+defCellGap, defCellSize*3+defCellGap);
    fixMain.drawString("Universe", defCellGap*3+defCellSize*2, defCellSize*3);
    ofDrawLine(defCellSize*4+defCellGap, defCellSize*2.5, defCellSize*4+defCellGap, defCellSize*3+defCellGap);
    fixMain.drawString("Channel", defCellGap*3+defCellSize*4, defCellSize*3);
    ofDrawLine(defCellSize*6+defCellGap, defCellSize*2.5, defCellSize*6+defCellGap, defCellSize*3+defCellGap);
    fixMain.drawString("Quantity", defCellGap*3+defCellSize*6, defCellSize*3);
    ofDrawLine(defCellSize*8+defCellGap, defCellSize*2.5, defCellSize*8+defCellGap, defCellSize*3+defCellGap);
    //fixMain.drawString("Modified", defCellGap*2 + (defCellSize*10), defCellSize*3);

    
    crossButton(ofGetWidth()-defCellGap-defCellSize, defCellGap, 0, true, true);
    
    ofSetColor(160);  // Showfiles button
    ofSetLineWidth(0);
    ofDrawRectangle(ofGetWidth()-(defCellGap*2)-(defCellSize*3)-defCellSize, defCellGap, defCellSize*3, defCellSize);
    ofSetColor(255, 255, 255, 255);
    ofNoFill();
    ofSetLineWidth(1);
    ofDrawRectangle(ofGetWidth()-(defCellGap*2)-(defCellSize*3)-defCellSize, defCellGap, defCellSize*3, defCellSize);
    panelType.drawString("Showfiles", ofGetWidth()-(defCellGap*2)-(defCellSize*3)-(defCellSize*0.5), defCellGap*4);
    if (overlay == 0 && clickLeft(ofGetWidth()-(defCellGap*2)-(defCellSize*3)-defCellSize, defCellGap, defCellSize*3, defCellSize)){
        setLi();
        if (showsList[0].active == true){
            if (showsList[0].freshStart == true){
                showsList = {};
                showsList.push_back(showFileObject);
                showsList[0].name = ofGetTimestampString("(New) ShowFile %F %R");
                showsList[0].freshStart = true;
            } else {
                showsList = {};
                showsList.push_back(showFileObject);
                showsList[0].name = currentShowName;
            }
            showsList[0].modified = "-";
            showsList[0].active = true;
        
        } else {
            showsList = {};
        }
        
        ofDirectory dir(showFilesDir);  // Checks directory for saved showfiles and adds them to the vector
        dir.allowExt("luct");   //  The next few lines of functionality are incomplete
        dir.listDir();
        dir.sortByDate();
        
        
        
        for (int i = 0; i < dir.size(); i++){
            if (dir.getName(i) != currentShowName + ".luct"){   // If user saves a fresh show it stops the showfile being listed as it is found in the directory (on next directory listing)
                showsList.push_back(showFileObject);
                string theName;
                for (int u = 0; u < dir.getName(i).size() - 5; u++){    //  removes the ".luct" file extention from the name
                    theName = theName + dir.getName(i)[u];
                }
                showsList[showsList.size()-1].name = theName;
                showsList[showsList.size()-1].freshStart = false;
                showsList[showsList.size()-1].active = false;
                showsList[showsList.size()-1].path = dir.getPath(i);
                //long number = boost::filesystem::last_write_time(dir.getPath(i));
                showsList[showsList.size()-1].modified = "-";
                //cout << gmtime(number) << endl;
            // to_string(boost::filesystem::last_write_time(dir.getPath(i)));
            // std::asctime(std::localtime(&result))
            }
        }
        
        showsList.push_back(showFileObject);
        showsList[showsList.size()-1].name = "[INBUILT DEMO]";
        showsList[showsList.size()-1].freshStart = false;
        showsList[showsList.size()-1].active = false;
        showsList[showsList.size()-1].path = "";
        showsList[showsList.size()-1].isDemo = true;
        screen = 1;
    }
    
    ofFill();
    ofSetColor(160);  // Update DMX button
    ofSetLineWidth(0);
    ofDrawRectangle(ofGetWidth()-(defCellGap*3)-(defCellSize*6)-defCellSize, defCellGap, defCellSize*3, defCellSize);
    ofSetColor(255, 255, 255, 255);
    ofNoFill();
    ofSetLineWidth(1);
    ofDrawRectangle(ofGetWidth()-(defCellGap*3)-(defCellSize*6)-defCellSize, defCellGap, defCellSize*3, defCellSize);
    panelType.drawString("Update DMX", ofGetWidth()-(defCellGap*3)-(defCellSize*6)-(defCellSize*0.5), defCellGap*4);
    if (overlay == 0 && clickLeft(ofGetWidth()-(defCellGap*3)-(defCellSize*6)-defCellSize, defCellGap, defCellSize*3, defCellSize)){
        setLi();
        genShowFixtures();
        screen = 0;
        popupObj.show = true;
        popupObj.title = "WARNING";
        popupObj.msg = "DMX functionality updated.\nDMX defined flaws may\ncause unexpected outputs";
        popupObj.cR = 48;
        popupObj.cG = 201;
        popupObj.cB = 53;
        popupObj.title = 3000;
        
    }
}

void ofApp::showFileConfig(){   // Showfile screen (load, save, delete and configuration)
    int showCardHeight = 2.5*defCellSize;
    int showCardWidth = 12*defCellSize;

   
    if (mouseY < defCellGap + defCellSize*3){
        level = 1;
    }
    
    ofFill();
    for (int i = 0; i < showsList.size(); i++){ // Draw a card for each of the showfiles found
        ofSetLineWidth(1);
        ofSetColor(80, 80, 80);
        ofDrawRectRounded(defCellGap, showCardstartheight+(i*showCardHeight)+defCellGap+(i*defCellGap), showCardWidth, showCardHeight,defRounded);
        ofSetColor(255);
        ofDrawLine(defCellSize*10, showCardstartheight+(i*showCardHeight)+defCellGap+(i*defCellGap), defCellSize*10, showCardstartheight+(i*showCardHeight)+(defCellSize*0.7)+(i*defCellGap));
        cardMain.drawString(showsList[i].name, defCellGap*2, showCardstartheight+(i*showCardHeight)+(defCellGap*3)+(i*defCellGap));
        fixMain.drawString(showsList[i].modified, defCellSize*10 + (defCellGap * 3), showCardstartheight+(i*showCardHeight)+(defCellGap*3)+(i*defCellGap));

        if (showsList[i].active == true){
            ofSetColor(155, 222, 78);
            fixMain.drawString("Active", defCellGap*2, showCardstartheight+(i*showCardHeight)+defCellGap+(i*defCellGap)+(defCellSize*2.2));
        } else if (showsList[i].isDemo == true){
            ofSetColor(245, 173, 100);
            fixMain.drawString("Internal demo", defCellGap*2, showCardstartheight+(i*showCardHeight)+defCellGap+(i*defCellGap)+(defCellSize*2.2));
            int u = 0;
            string text = "load";
            ofSetColor(255);
            fixMain.drawString(text, showCardWidth - defCellSize - (defCellGap/2) - ((u*defCellSize)+defCellGap*u), showCardstartheight+(i*showCardHeight)+(defCellGap)+(i*defCellGap)+(showCardHeight-defCellSize+defCellGap));
            ofNoFill();
            ofSetLineWidth(2);
            ofDrawRectangle(showCardWidth - defCellSize - defCellGap - ((u*defCellSize)+defCellGap*u), showCardstartheight+(i*showCardHeight)+(defCellGap)+(i*defCellGap)+(showCardHeight-defCellSize-defCellGap), defCellSize, defCellSize);
            if (clickLeft(showCardWidth - defCellSize - defCellGap - ((u*defCellSize)+defCellGap*u), showCardstartheight+(i*showCardHeight)+(defCellGap)+(i*defCellGap)+(showCardHeight-defCellSize-defCellGap), defCellSize, defCellSize)){    // Update variables for loading of show demonstration (clear current showfile values)
                setLi();
                mode = "";
                panels = {};
                brightnessChanged = false;
                colorsChanged = false;
                wauvChanged = false;
                posChanged = false;
                red = 255;
                green = 255;
                blue = 255;
                clearValues();
                demoShow();
                
                screen = 0;
                
            }
        }
        
        // Buttons for each card
        ofNoFill();
        ofSetColor(255);
        ofSetLineWidth(2);
        
        if (showsList[i].freshStart == false && showsList[i].isDemo == false){  // Showfiles found within the defined directory
            
            for (int u = 0; u < showFileButtons.size(); u++){
                string text = showFileButtons[u];
                fixMain.drawString(text, showCardWidth - defCellSize - (defCellGap/2) - ((u*defCellSize)+defCellGap*u), showCardstartheight+(i*showCardHeight)+(defCellGap)+(i*defCellGap)+(showCardHeight-defCellSize+defCellGap));
                ofDrawRectangle(showCardWidth - defCellSize - defCellGap - ((u*defCellSize)+defCellGap*u), showCardstartheight+(i*showCardHeight)+(defCellGap)+(i*defCellGap)+(showCardHeight-defCellSize-defCellGap), defCellSize, defCellSize);
                
                if (clickLeft(showCardWidth - defCellSize - defCellGap - ((u*defCellSize)+defCellGap*u), showCardstartheight+(i*showCardHeight)+(defCellGap)+(i*defCellGap)+(showCardHeight-defCellSize-defCellGap), defCellSize, defCellSize)){
                    setLi();
                    if (text == "delete"){
                        showDel = true;
                        delType = 0;
                        delData1 = showsList[i].path;
                        delData2 = showsList[i].name;
                        
                    }
                    
                    else if (text == "save as"){
                        strInputHeading = "Save as";
                        strInputObj.fieldInputState = 1;
                        strInputObj.screenType = 0;
                        strInputObj.search = 0;
                        strInputObj.maxCount = 32;
                        strInputObj.current = showsList[i].name + "copy";
                        strInputObj.textValue = showsList[i].name + "copy";
                        ptrType = 0;
                        fieldPtr = &fileSaveAsName;
                        fileSaveAsPath = showsList[i].path;
                        
                    } else if (text == "save") {
                        genShowFileDir();
                        ofstream out(showFilesDir + "/" + showsList[i].name + ".luct" , fstream::out | fstream::trunc);
                        if (out.is_open()){
                            genShowFileStr();
                            out << ocol;
                            out.flush();
                            out.close(); // Closes file
                            showsList[i].freshStart = false;
                        } else {
                            cout << "Error with file" << endl;
                        }
                    } else if (text == "load"){
                        lastInteraction = ofGetElapsedTimeMillis();
                        popupObj.show = true;
                        popupObj.title = "WARNING";
                        popupObj.msg = "The developer has removed Load\nfunctionality in this build.";
                        popupObj.cR = 235;
                        popupObj.cG = 64;
                        popupObj.cB = 52;
                        popupObj.time = 2000;
                        
/*
 
 Below is redundant code in regards to loading a showfile.
 The below code never worked and has since become outdated by far as new functionality has been
 added to the application involving in objects and data structure being changed.
 
 The comment below is temporary and serves as a explanation of the idea for loading/parsing a showfile data string at the time.
 
*/
                        

//                        string line;
//                        fstream myfile;
//                        myfile.open(showsList[i].path, fstream::in);
//                        while (getline(myfile, line)){
//                            showFileData = line;
//                        }
//                        myfile.close();
//
//                        for (int u = 0; u < showsList.size(); u++){
//                            if (showsList[u].active == true){
//                                showsList[u].active = false;
//                            }
//                        }
//                        if (showFileData != ""){
//                            currentShowName = showsList[i].name;
//                            showsList[i].active = true;
//                            panels = {};
//                            fixtures = {};
//
//
//
//                            if (showFileData == ""){
//                                cout << "Blank File" << endl;
//
//                            } else {                                // Parse showfile data
//                                cout << "parsing" << endl;
//
//                                string panelSplit = "^";
//                                string attributeSplit = "$";
//
//                                // ImportedPanels --> Panel object --> panel attributes --> panelAttributePanelData --> dataAttributes
//
//                                //vector<vector<vector<string>>> newPanels {{{}}};    // each index represents a panel object with its attributes as the child
//
//
//                                int currentPanel = 0;
//                                int currentAttribute = 0;   //  storage data found at 9, 10, 11, 12
//
//                                for (int c = 0; c < showFileData.size(); c++){   // split panels up into their own vector index --> newPanels[]
//                                        if (showFileData[c] != '^'){
//                                            if (showFileData[c] != '$'){
//                                                newPanels[currentPanel][currentAttribute].push_back(to_string(showFileData[c]));
//                                            } else {
//                                                currentAttribute++;
//                                                newPanels[currentPanel].push_back({});
//                                            }
//
//                                        } else {
//                                            newPanels.push_back({{}});
//                                            currentPanel++;
//                                            currentAttribute = 0;
//                                        }
//                                    }
//
//                                for (int p = 0; p < newPanels.size()-1; p++){
//                                    panels.push_back(panelObj);
//
//
//                                    panels[p].x = intParseAttribute(p,0);
//                                    panels[p].y = intParseAttribute(p,1);
//                                    panels[p].wi = intParseAttribute(p,2);
//                                    panels[p].hi = intParseAttribute(p,3);
//                                    panels[p].r = intParseAttribute(p,4);
//                                    panels[p].cellSize = intParseAttribute(p,5);
//                                    panels[p].defSpace = intParseAttribute(p,6);
//                                    panels[p].type = strParseAttribute(p,7);
//                                    panels[p].name = strParseAttribute(p,8);
//
//                                    int panelID = p;
//                                    int attriID = 9;
//
//                                    vector<int> rawData;
//                                    string quickPrint;
//                                    string tempIndex;
//
//                                    cout << "crazy" << endl;
//
//                                    //cout << newPanels[panelID][attriID] << endl;
//
//                                    for (int c = 1; c < newPanels[panelID][attriID].size(); c++){
//                                        if (stoi(newPanels[panelID][attriID][c]) != 47){    // '/'
//                                            quickPrint = quickPrint + newPanels[panelID][attriID][c];
//                                            int asciiInt = stoi(newPanels[panelID][attriID][c]);
//                                            char theChar = char(asciiInt);
//                                            cout << "ASCII INT: " << asciiInt << endl;
//                                            tempIndex.push_back(theChar);
//
//                                            //rawData.push_back(stoi(to_string(asciiInt-48)));
//                                            //rawData[rawData.size()-1]=rawData[rawData.size()-1]*10+asciiInt-48;
//
//                                        }
//                                        else{
//                                            cout << "NEW SLASH" << endl;
////                                            rawData.push_back(0);
//                                            if (tempIndex != ""){
//                                                rawData.push_back(stoi(tempIndex));
//
//                                            }
//                                            tempIndex = "";
//                                        }
//
//
//
//                                    }
//                                    cout << "THE STRING OUTPUT: " << quickPrint << endl;
//                                    //out << rawData[0] << endl;
//
//
//                                    //panels[p].savedBrightness = returnV;
//
//                                    //cout << "VVVVV: " << to_string(returnV.size()) << endl;
//
//                                    //cout << to_string(panels[p].savedBrightness[0].value);
//                                    //panels[p].fadeData = vectBriParseAttribute(p,9);
//
//                                    //vectBriParseAttribute(p,9);
//
//
//                                    //  dodo
//
//                                    //cout << "EEEEEEE: "<< panels[0].x << endl;
//
//
//                                }
//
//                                vector<string> attriList = {"x", "y", "wi", "hi", "r", "cellSize", "defSpace", "type", "name", "Brightness", "Positions", "Colors", "Faders"};
//
//                                // print out each vector index
//                                for (int i = 0; i < newPanels.size()-1; i++){     // each panel object
//
//                                    for (int a = 0; a < newPanels[i].size(); a++){
//                                        string panelWhole;
//                                                for (int c = 0; c < newPanels[i][a].size(); c++){
//                                                    int ascii = stoi(newPanels[i][a][c]);
//
//
//                                                    panelWhole = panelWhole + char(ascii);
//                                                }
//                                        cout << attriList[a] << ": " << panelWhole << endl;
//
//                                    }
//                                    cout << "---" << endl;
//                                }
//
//
//
//                            }
//
//
//                        }
                        
/*
 
 End of showfile parse code
 
 */
                    }
                    
                }
                
                if (strInputObj.fieldInputState == 3){
                   
                    genShowFileDir();
//                    cout << "O PATH: " << fileSaveAsPath << endl;
//                    cout << "N PATH:" << showFilesDir << "/" << fileSaveAsName << ".luct" << endl;
//                    cout << "NAME: " << fileSaveAsName << endl;
                    
                    if (fileSaveAsPath != ""){  // Check saving as file has a path (could be a new file not yet saved?)
                        ofFile fileToRead(ofToDataPath(fileSaveAsPath)); // a file that exists
                            fileToRead.copyTo(showFilesDir + "/" + fileSaveAsName + ".luct");   // Causing Errors yet works [BugLog FS01]
                    } else {    // If file does not have a path, save as a new file
                        genShowFileDir();
                        ofstream out(showFilesDir + "/" + fileSaveAsName + ".luct", fstream::out | fstream::trunc);
                        if (out.is_open()){
                            genShowFileStr();
                            out << ocol;
                            out.flush();
                            out.close(); // Closes file
                        } else {
                            cout << "Error with file" << endl;
                        }
                    }
                    
                }
                
                if (u == 2 && showsList[i].active == false){    // Don't show Save button unless showfile is active
                    u++;
                }
            }
        } else  if (showsList[i].isDemo == false){    // Fresh default showfile
        
            fixMain.drawString("save", showCardWidth - defCellSize - (defCellGap/2) - ((0*defCellSize)+defCellGap*0), showCardstartheight+(i*showCardHeight)+(defCellGap)+(i*defCellGap)+(showCardHeight-defCellSize+defCellGap));
            ofDrawRectangle(showCardWidth - defCellSize - defCellGap - ((0*defCellSize)+defCellGap*0), showCardstartheight+(i*showCardHeight)+(defCellGap)+(i*defCellGap)+(showCardHeight-defCellSize-defCellGap), defCellSize, defCellSize);
            if (clickLeft(showCardWidth - defCellSize - defCellGap - ((0*defCellSize)+defCellGap*0), showCardstartheight+(i*showCardHeight)+(defCellGap)+(i*defCellGap)+(showCardHeight-defCellSize-defCellGap), defCellSize, defCellSize) && level == 0){
                setLi();
                strInputHeading = "Save";
                strInputObj.fieldInputState = 1;
                strInputObj.screenType = 0;
                strInputObj.search = 0;
                strInputObj.maxCount = 32;
                strInputObj.current = ofGetTimestampString("ShowFile %F %R");
                strInputObj.textValue = ofGetTimestampString("ShowFile %F %R");
                ptrType = 0;
                fieldPtr = &showsList[i].name;
            }
            if (strInputObj.fieldInputState == 3){
                genShowFileDir();
                ofstream out(showFilesDir + "/" + showsList[i].name + ".luct", fstream::out | fstream::trunc);
                currentShowName = showsList[i].name;
                if (out.is_open()){
                    genShowFileStr();
                    out << ocol;
                    out.flush();
                    out.close(); // Closes file
                    showsList[i].freshStart = false;
                } else {
                    cout << "Error with file" << endl;
                }
            }
        }
        
        ofFill();
    }
    ofFill();
    ofSetColor(80, 80, 80);
    ofDrawRectangle(0, 0, ofGetWidth(), defCellGap + defCellSize*3);
    
    ofSetColor(255, 255, 255);
    pageMain.drawString("Showfiles", defCellGap*2, defCellSize);
    pageSub.drawString("Save, Delete and configure showfiles", defCellGap*2, defCellSize*1.7);
    ofDrawLine(0, defCellSize*2.5, ofGetWidth(), defCellSize*2.5);
    fixMain.drawString("Showfile name", defCellGap*2, defCellSize*3);
    fixMain.drawString("Modified", defCellGap*2 + (defCellSize*10), defCellSize*3);
    ofDrawLine(defCellSize*10 , defCellSize*2.5, defCellSize*10, defCellSize*3+defCellGap);
    
    ofSetColor(180, 80, 80);    // draw "cross" exit button
    ofSetLineWidth(0);
    ofDrawRectangle(ofGetWidth()-defCellGap-defCellSize, defCellGap, defCellSize, defCellSize);
    ofSetColor(80,80,80);
    ofSetLineWidth(5);
    ofDrawLine(ofGetWidth()-defCellSize, defCellGap*2, ofGetWidth()-(defCellGap*2), defCellSize);
    ofDrawLine(ofGetWidth()-defCellSize, defCellSize, ofGetWidth()-(defCellGap*2), defCellGap*2);
    if (overlay == 0){
        if (clickLeft(ofGetWidth()-defCellGap-defCellSize, defCellGap, defCellSize, defCellSize)){
            setLi();
            screen = 0;
            
        }
    }
    
    ofSetColor(255, 255, 255,100);  // fixtures button
    ofSetLineWidth(0);
    ofDrawRectangle(ofGetWidth()-(defCellGap*2)-(defCellSize*3)-defCellSize, defCellGap, defCellSize*3, defCellSize);
    
    ofSetColor(255, 255, 255, 255);
    ofNoFill();
    ofSetLineWidth(1);
    ofDrawRectangle(ofGetWidth()-(defCellGap*2)-(defCellSize*3)-defCellSize, defCellGap, defCellSize*3, defCellSize);
    panelType.drawString("Patching", ofGetWidth()-(defCellGap*2)-(defCellSize*3)-(defCellSize*0.5), defCellGap*4);
    if (clickLeft(ofGetWidth()-(defCellGap*2)-(defCellSize*3)-defCellSize, defCellGap, defCellSize*3, defCellSize)){
        setLi();
        screen = 2;
        selectedFixDef = -1;
    }
    
    //  scroll buttons
    
    if (showCardstartheight >= 3*defCellSize+defCellGap){   // up button
        ofFill();
        ofSetColor(40,40,40);
        ofDrawRectRounded(defCellGap*2+showCardWidth, 3*defCellSize+(defCellGap*2), defCellSize/2, (ofGetHeight()-(3*defCellSize+defCellGap)-(defCellGap*4)-defCellGap/2)/2-defCellGap/2, defRounded);   // UP button
    } else {
        ofFill();
        ofSetColor(80, 80, 80);
        ofDrawRectRounded(defCellGap*2+showCardWidth, 3*defCellSize+(defCellGap*2), defCellSize/2, (ofGetHeight()-(3*defCellSize+defCellGap)-(defCellGap*4)-defCellGap/2)/2-defCellGap/2, defRounded);   // UP button
        ofSetColor(252, 186, 3);
        ofSetLineWidth(2);
        ofNoFill();
        ofDrawRectRounded(defCellGap*2+showCardWidth, 3*defCellSize+(defCellGap*2), defCellSize/2, (ofGetHeight()-(3*defCellSize+defCellGap)-(defCellGap*4)-defCellGap/2)/2-defCellGap/2, defRounded);   // UP button
        if (clickLeft(defCellGap*2+showCardWidth, 3*defCellSize+(defCellGap*2), defCellSize/2, (ofGetHeight()-(3*defCellSize+defCellGap)-(defCellGap*4)-defCellGap/2)/2-defCellGap/2) && ofGetElapsedTimeMillis() > lastInteraction + defWaitTime && overlay == 0){
            setLi();
            showCardstartheight = showCardstartheight + defCellSize;
        }
    }
    
    if (showCardstartheight+(showsList.size()*showCardHeight)+defCellGap+(showsList.size()*defCellGap)+showCardHeight <= ofGetHeight()-defCellGap){ // down button
        ofFill();
        ofSetColor(40,40,40);
        ofDrawRectRounded(defCellGap*2+showCardWidth, 3*defCellSize+(defCellGap*2) + (ofGetHeight()-(3*defCellSize+defCellGap)-(defCellGap*4)-defCellGap/2)/2-defCellGap/2 + defCellGap, defCellSize/2, (ofGetHeight()-(3*defCellSize+defCellGap)-(defCellGap*4)-defCellGap/2)/2-defCellGap/2, defRounded);  // DOWN button
    } else {
        ofFill();
        ofSetColor(80, 80, 80);
        ofDrawRectRounded(defCellGap*2+showCardWidth, 3*defCellSize+(defCellGap*2) + (ofGetHeight()-(3*defCellSize+defCellGap)-(defCellGap*4)-defCellGap/2)/2-defCellGap/2 + defCellGap, defCellSize/2, (ofGetHeight()-(3*defCellSize+defCellGap)-(defCellGap*4)-defCellGap/2)/2-defCellGap/2, defRounded);  // DOWN button
        ofSetColor(252, 186, 3);
        ofSetLineWidth(2);
        ofNoFill();
        ofDrawRectRounded(defCellGap*2+showCardWidth, 3*defCellSize+(defCellGap*2) + (ofGetHeight()-(3*defCellSize+defCellGap)-(defCellGap*4)-defCellGap/2)/2-defCellGap/2 + defCellGap, defCellSize/2, (ofGetHeight()-(3*defCellSize+defCellGap)-(defCellGap*4)-defCellGap/2)/2-defCellGap/2, defRounded);  // DOWN button
        if (clickLeft(defCellGap*2+showCardWidth, 3*defCellSize+(defCellGap*2) + (ofGetHeight()-(3*defCellSize+defCellGap)-(defCellGap*4)-defCellGap/2)/2-defCellGap/2 + defCellGap, defCellSize/2, (ofGetHeight()-(3*defCellSize+defCellGap)-(defCellGap*4)-defCellGap/2)/2-defCellGap/2) && ofGetElapsedTimeMillis() > lastInteraction + defWaitTime && overlay == 0){
            setLi();
            showCardstartheight = showCardstartheight - defCellSize;
        }
    }
    
    ofSetColor(255);
    fixText.drawString("up", defCellGap*2+showCardWidth+((defCellSize/2)/2), 3*defCellSize+(defCellGap*2) + ((ofGetHeight()-(3*defCellSize+defCellGap)-(defCellGap*4)-defCellGap/2)/2-defCellGap/2)/2);
    fixText.drawString("dn", defCellGap*2+showCardWidth+((defCellSize/2)/2), 3*defCellSize+(defCellGap*2) + ((ofGetHeight()-(3*defCellSize+defCellGap)-(defCellGap*4)-defCellGap/2)/2-defCellGap/2)/2 + ((ofGetHeight()-(3*defCellSize+defCellGap)-(defCellGap*4)-defCellGap/2)/2-defCellGap/2 + defCellGap));
    
}

void ofApp::genShowFileStr(){       // Parser - Generates string data representing showfile
    
    /*
     
     Please note this function is old and needs to be updated to accomodate new LucentOP functionality. This function is still active just for concept demonstration and will be updated in the future
     
     */
    
    // delimiter key: '^' = new panel, '$' new class attribute, '/' new (secondary) class attribute (savedData example storedBright), '|' new vector index, '*' no storedData
    
    ocol = "";
    for (int i = 0; i < panels.size(); i++){
        ocol = ocol + to_string(panels[i].x) + "$" + to_string(panels[i].y) + "$" + to_string(panels[i].wi) + "$" + to_string(panels[i].hi) + "$" + to_string(panels[i].r) + "$" + to_string(panels[i].cellSize) + "$" + to_string(panels[i].defSpace) + "$" + panels[i].type + "$" + panels[i].name + "$";
        string savedBrightness;
        if (panels[i].savedBrightness.size() > 0){
            for (int u = 0; u < panels[i].savedBrightness.size(); u++){
                savedBrightness = savedBrightness + "/" + to_string(panels[i].savedBrightness[u].iden) + "/" + to_string(panels[i].savedBrightness[u].set) + "/" + to_string(panels[i].savedBrightness[u].value);
            }
        } else {
            savedBrightness = "*";  // means no stored values
        }
        ocol = ocol + savedBrightness + "$";
        
        string savedPositions;
        if (panels[i].savedPositions.size() > 0){
            for (int u = 0; u < panels[i].savedPositions.size(); u++){
                savedPositions = savedPositions + "/" + to_string(panels[i].savedPositions[u].iden) + "/" + to_string(panels[i].savedPositions[u].set) + "/";
                
                if (panels[i].savedPositions[u].name == ""){
                    savedPositions = savedPositions + "`";
                } else {
                    savedPositions = savedPositions + panels[i].savedPositions[u].name;
                }
                savedPositions = savedPositions + "/";
                
                if (panels[i].savedPositions[u].position.size() > 0){
                    for (int p = 0; p < panels[i].savedPositions[u].position.size(); p++){
                        savedPositions = savedPositions + to_string(panels[i].savedPositions[u].position[p]) + "|";
                    }
                } else {
                    savedPositions = savedPositions + "*";
                }
                
                
            }
        } else {
            savedPositions = "*";  // means no stored values
        }
        ocol = ocol + savedPositions + "$";
        
        string savedColors;
        if (panels[i].savedColors.size() > 0){
            for (int u = 0; u < panels[i].savedColors.size(); u++){
                savedColors = savedColors + "/" + to_string(panels[i].savedColors[u].iden) + "/" + to_string(panels[i].savedColors[u].set) + "/";
                
                if (panels[i].savedColors[u].name == ""){
                    savedColors = savedColors + "`";
                } else {
                    savedColors = savedColors + panels[i].savedColors[u].name;
                }
                savedColors = savedColors + "/" + to_string(panels[i].savedColors[u].r) + "/" + to_string(panels[i].savedColors[u].g) + "/" + to_string(panels[i].savedColors[u].b) + "/" + to_string(panels[i].savedColors[u].w);
            }
        } else {
            savedColors = "*";  // means no stored values
        }
        ocol = ocol + savedColors + "$";
        
        if (panels[i].fadeData.size() > 0){
            for (int p = 0; p < panels[i].fadeData.size(); p++){
                ocol = ocol + to_string(panels[i].fadeData[p]) + "|";
            }
        } else {
            ocol = ocol + "*";
        }
        
        
        ocol = ocol + "^";
        
    }

}


void ofApp::testFunction(){ // Random code in here and can be called anytime to check if code works --> Stub used for detecting errors and assiting in development
    cout << "running test function" << endl;
    genShowFileDir();
    ofstream out(showFilesDir + "/Showfiles" + ofGetTimestampString("ShowFile %F %R") + ".luct", fstream::out | fstream::trunc);

    if (out.is_open()){
        genShowFileStr();
        out << ocol;
        out.flush();
        out.close(); // Closes file
    } else {
        cout << "Error with file" << endl;
    }
        
}



void ofApp::drawPanel(){    // Checks the UIpanel object and determines which ui panel function to call depending on panel type
    level = 3;  //  prevent users engaging with elements, when drawing
    overlay = 0;

    ofFill();
    ofSetColor(80, 80, 80);
    if (overPanel == false){
        if (definedStart == false){
            ofDrawRectangle(defCellGap + int(floor(((ofGetMouseX()-defCellGap)/defCellSize)))*defCellSize, defCellGap + int(floor(((ofGetMouseY()-defCellGap)/defCellSize)))*defCellSize, defCellSize, defCellSize);
            if (waitTime(300)){
                if (clickLeft(defCellGap + int(floor(((ofGetMouseX()-defCellGap)/defCellSize)))*defCellSize, defCellGap + int(floor(((ofGetMouseY()-defCellGap)/defCellSize)))*defCellSize, defCellSize, defCellSize)){
                    startX = (int(floor(((ofGetMouseX()-defCellGap)/defCellSize)))*defCellSize) + defCellGap;
                    startY = (int(floor(((ofGetMouseY()-defCellGap)/defCellSize)))*defCellSize) + defCellGap;
                    definedStart = true;
                    setLi();
    //                cout << "EE " << to_string(startX) << " " << to_string(startY) << endl;
                }
            }
           
        } else {
            theDiffX = ((int(floor(((ofGetMouseX()-defCellGap)/defCellSize)))*defCellSize) + defCellGap)+defCellSize - startX;
            theDiffY = ((int(floor(((ofGetMouseY()-defCellGap)/defCellSize)))*defCellSize) + defCellGap)+defCellSize - startY;
            if (theDiffX < 0){
                theDiffX = theDiffX - defCellSize;  // Draw panel near user clicked
            }
            if (theDiffY < 0){
                theDiffY = theDiffY - defCellSize;
            }
            if (theDiffX == 0){
                theDiffX = -defCellSize;
            }
            if (theDiffY == 0){
                theDiffY = -defCellSize;
            }
           
            ofFill();
            if (theDiffX < knownPanelType[panelToDraw].minWidth*defCellSize || theDiffY < knownPanelType[panelToDraw].minHeight*defCellSize){   // set color of panel according to if the dimensions meet minimum requirements
                if (theDiffX < knownPanelType[panelToDraw].minWidth*defCellSize*-1 || theDiffY < knownPanelType[panelToDraw].minHeight*defCellSize*-1){
                    ofSetColor(0, 255, 0,255);
                } else {
                    ofSetColor(255, 0, 0,255);
                }
            } else {
                ofSetColor(0, 255, 0,255);
            }
            ofDrawRectangle(startX, startY, theDiffX, theDiffY);
            if (ofGetElapsedTimeMillis() > lastInteraction + 300){
                if (clickLeft(defCellGap + int(floor(((ofGetMouseX()-defCellGap)/defCellSize)))*defCellSize, defCellGap + int(floor(((ofGetMouseY()-defCellGap)/defCellSize)))*defCellSize, defCellSize, defCellSize)){
                    setLi();
                    addPanelStage = 0;
                    overlay = 0;
                    definedStart = false;
                    panels.push_back(panelObj);
                    panels[panels.size()-1].type = knownPanelType[panelToDraw].type;
                    panels[panels.size()-1].name = knownPanelType[panelToDraw].name;
                    if (theDiffX<0){
                        startX = startX - theDiffX;
                        theDiffX = theDiffX*-1;
                    }
                    if (theDiffY<0){
                        startY = startY - theDiffY;
                        theDiffY = theDiffY*-1;
                    }
                    
                    panels[panels.size()-1].x = (startX-defCellGap)/defCellSize;
                    panels[panels.size()-1].y = (startY-defCellGap)/defCellSize;
                    panels[panels.size()-1].wi = int(theDiffX/defCellSize);
                    panels[panels.size()-1].hi = int(theDiffY/defCellSize);
                    panels[panels.size()-1].cellSize = knownPanelType[panelToDraw].cellSize;
                
                }
            }
        }
    }
}

void ofApp::addPanelOverlay(){  // Overlay allowing user to select which panel to draw
    level = 3;
    overlay = 1;
    ofSetColor(0, 0, 0,220);
    int buttonWidth = (defCellSize*2.5)-(defCellGap*2);
    int buttonHeight = (defCellSize*1.5)-(defCellGap*2);
    int buttonRows = 3;
    int buttonCols = 4;
    int width = buttonWidth * buttonCols + (defCellGap * buttonCols) + defCellGap;
    int height = buttonHeight * buttonRows + (defCellGap * buttonRows) + defCellGap;
    ofFill();
    ofDrawRectangle(0,0,ofGetWindowWidth(), ofGetWindowHeight());
    ofSetColor(255,255,255,255);
    pageMain.drawString("Add Panel", (ofGetWidth()/2)-(defCellSize*4)-(defCellGap*3), (ofGetHeight()/2)-(defCellSize*2)-(defCellGap*3) - defCellGap);
    ofSetColor(70, 70, 70);
    ofDrawRectRounded(ofGetWidth()/2 - width/2, ofGetHeight()/2 - height/2, width, height, defRounded);
    ofNoFill();

    crossButton(ofGetWidth()/2 +(((ofGetWidth()/2)-(defCellSize*4)-(defCellGap*3))-defCellSize)/2-defCellGap*2, (ofGetHeight()/2)-(defCellSize*2)-(defCellGap*3)-defCellGap*3, 0, false, false);
    
    ofSetLineWidth(2);
    ofSetColor(255, 255, 255);
    
    int theX = 0;
    int theY = 0;
    

    int panelIndex = 0;
    for (int r = 0; r < 4; r++){
        for (int c = 0; c < 4; c++){
            if (panelIndex < knownPanelType.size()){
                ofDrawRectRounded(ofGetWidth()/2 - width/2 + defCellGap + (buttonWidth * c) + (defCellGap * c), ofGetHeight()/2 - height/2 + defCellGap + (buttonHeight * r) + (defCellGap * r), buttonWidth, buttonHeight, defRounded);
                fixMain.drawString(knownPanelType[panelIndex].name, ofGetWidth()/2 - width/2 + defCellGap + (buttonWidth * c) + (defCellGap * c) + defCellGap, ofGetHeight()/2 - height/2 + defCellGap + (buttonHeight * r) + (defCellGap * r) + (defCellGap*2));
                theX = ofGetWidth()/2 - width/2 + defCellGap + (buttonWidth * c) + (defCellGap * c);
                theY = ofGetHeight()/2 - height/2 + defCellGap + (buttonHeight * r) + (defCellGap * r);
                if (mouseX > ofGetWidth()/2 - width/2 + defCellGap + (buttonWidth * c) + (defCellGap * c) && mouseX < ofGetWidth()/2 - width/2 + defCellGap + (buttonWidth * c) + (defCellGap * c) + buttonWidth && mouseY > ofGetHeight()/2 - height/2 + defCellGap + (buttonHeight * r) + (defCellGap * r) && mouseY < ofGetHeight()/2 - height/2 + defCellGap + (buttonHeight * r) + (defCellGap * r) + buttonHeight && mouseExe == 1){
                    mouseExe = 0;
                    panelToDraw = panelIndex;
                    addPanelStage = 2;
                    lastInteraction = ofGetElapsedTimeMillis();
                
                }
            }
            panelIndex++;
        }
    }

}

void ofApp::clearValues(){
    brightnessChanged = false;
    colorsChanged = false;
    wauvChanged = false;
    posChanged = false;
    mouseFixtures = {};  // array stores index array of fixtures from showFixtures
    mouseColors = {0,0,0,0,0,0};
    mousePosition = {0,0};
    mouseFixtures = {};
    mouseBrightness = 0;
}

void ofApp::controlPanel(int i){    // function responsible for drawing the control panel
    int x = gX(panels[i].x);
    int y = gY(panels[i].y);
    int wi = panels[i].wi*defCellSize;
    int hi = panels[i].hi*defCellSize;
    int r = panels[i].r;
    int cellSize = panels[i].cellSize;
    int defSpace = panels[i].defSpace;
    string type = panels[i].type;
    string name = panels[i].name;
    int cellC = panels[i].x * panels[i].y;   //number of cells panel can fit (or resolution of cells)
    int wid = panels[i].wi;
    int hei = panels[i].hi;
    int multi = wid * hei;
    if (mouseX > x && mouseX < x+wi && mouseY > y && mouseY < y+hi){
        level = 2;
    }
            ofFill();
            ofSetColor(77, 77, 77);
            ofDrawRectRounded(x, y, wi, hi,r);
            ofSetColor(255, 255, 255);
            panelName.drawString(name, x + 30, y+10);

    int indexAction = 0;
    for (int r = 0; r < hei; r++){          //for the amount of rows and columns, display cells with their corresponding action value in cpActions. If it's blank ("") don't do anything
        for (int c = 0; c < wid; c++){
            if (indexAction < cpActions.size()){
                if (cpActions[indexAction] != ""){
                    ofSetColor(70, 70, 70);
                    ofFill();
                    ofDrawRectangle(x+(defCellGap/2) + (defCellSize*c), y+(defCellGap/2) + (defCellSize*r), defCellSize-(defCellGap), defCellSize-(defCellGap));
                    ofSetColor(255, 255, 255);
                    fixText.drawString(cpActions[indexAction],x+12 + (defCellSize*c),y+defCellGap + (defCellSize*r));
                    ofNoFill();
                    ofSetColor(252, 186, 3);
                    ofSetLineWidth(2);
                    ofDrawRectangle(x+(defCellGap/2) + (defCellSize*c), y+(defCellGap/2) + (defCellSize*r), defCellSize-(defCellGap), defCellSize-(defCellGap));
                    if (overlay == 0 && waitTime(defWaitTime)){
                        if (clickLeft(x+(defCellGap/2) + (defCellSize*c), y+(defCellGap/2) + (defCellSize*r), defCellSize-(defCellGap), defCellSize-(defCellGap))){
                            setLi();
                            mode = cpActions[indexAction];
                            if (cpActions[indexAction] == "showfile"){
                                screen = 1;
                                mode = "";
                                
                                if (showsList[0].active == true){
                                    if (showsList[0].freshStart == true){
                                        showsList = {};
                                        showsList.push_back(showFileObject);
                                        showsList[0].name = ofGetTimestampString("(New) ShowFile %F %R");
                                        showsList[0].freshStart = true;
                                    } else {
                                        showsList = {};
                                        showsList.push_back(showFileObject);
                                        showsList[0].name = currentShowName;
                                    }
                                    showsList[0].modified = "-";
                                    showsList[0].active = true;
                                
                                } else {
                                    showsList = {};
                                }
                                
                                ofDirectory dir(showFilesDir);  // Checks directory for saved showfiles and adds them to the vector
                                dir.allowExt("luct");   //  The next few lines of functionality are incomplete
                                dir.listDir();
                                dir.sortByDate();
                                
                                
                                
                                for (int i = 0; i < dir.size(); i++){
                                    if (dir.getName(i) != currentShowName + ".luct"){   // If user saves a fresh show it stops the showfile being listed as it is found in the directory (on next directory listing)
                                        showsList.push_back(showFileObject);
                                        string theName;
                                        for (int u = 0; u < dir.getName(i).size() - 5; u++){    //  removes the ".luct" file extention from the name
                                            theName = theName + dir.getName(i)[u];
                                        }
                                        showsList[showsList.size()-1].name = theName;
                                        showsList[showsList.size()-1].freshStart = false;
                                        showsList[showsList.size()-1].active = false;
                                        showsList[showsList.size()-1].path = dir.getPath(i);
                                        //long number = boost::filesystem::last_write_time(dir.getPath(i));
                                        showsList[showsList.size()-1].modified = "-";
                                        //cout << gmtime(number) << endl;
                                    // to_string(boost::filesystem::last_write_time(dir.getPath(i)));
                                    // std::asctime(std::localtime(&result))
                                    }
                                }
                                
                                showsList.push_back(showFileObject);
                                showsList[showsList.size()-1].name = "[INBUILT DEMO]";
                                showsList[showsList.size()-1].freshStart = false;
                                showsList[showsList.size()-1].active = false;
                                showsList[showsList.size()-1].path = "";
                                showsList[showsList.size()-1].isDemo = true;
                               
                            }
                            if (cpActions[indexAction] == "panels"){
                                addPanelStage = 1;
                            } else if (cpActions[indexAction] == "setup"){
                                screen = 2;
                                mode = "";
                                
//                                string dmxC = "ola_streaming_client -u 1 -d 50,50";
//                                const char *comm = dmxC.c_str();
//                                cout << "click" << endl;
//                                cout << dmxC << endl;
////                                system(comm);
//
//                                system("ola_streaming_client -u 1 -d 50,50");
                                
                            } else if (cpActions[indexAction] == "blackout"){
                                mode = "";
                                cout << "B clicked" << endl;
                                
                                if (blackOut){
                                    blackOut = false;
                                } else {
                                    blackOut = true;
                                }
                                
                            } else if (cpActions[indexAction] == "clear") {
                                mode = "";
                                clearValues();
                            }
                        }
                    }
                }
            }
            indexAction++;
        }
    }

}


void ofApp::briMixer(int i){  // brightness mixer UI panel
    int x = gX(panels[i].x);
    int y = gY(panels[i].y);
    int wi = panels[i].wi*defCellSize;
    int hi = panels[i].hi*defCellSize;
    int r = panels[i].r;
    int cellSize = panels[i].cellSize;
    int defSpace = panels[i].defSpace;
    string type = panels[i].type;
    string name = panels[i].name;
    int cellC = panels[i].x * panels[i].y;   //number of cells panel can fit (or resolution of cells)
    int wid = panels[i].wi;
    int hei = panels[i].hi;
    int multi = wid * hei;
    //vector<int> fadata = panels[i].fadeData;
    ofFill();
    ofSetColor(77, 90, 77);
    ofDrawRectRounded(x, y, wi, hi,r);
    ofNoFill();
    ofSetColor(114, 196, 114);
    ofSetLineWidth(1);
    ofDrawRectRounded(x, y, wi, hi,r);
    ofSetColor(255, 255, 255);
    panelName.drawString(name, x+15, y+15);
    
    if (mode == "delete"){
        if (clickLeft(x, y, wi, hi)){
            delPanel(i);
            return;
        }
    }

    if (hei < 4){    //check if panel is too small
        fixText.drawString("Panel\ntoo small", x+80, y+(hi/2)-15);

    } else {
        int f = 0;
            ofNoFill();
            ofSetLineWidth(2);
            int c1 = 255;
            int c2 = 255;
            int c3 = 255;
        
        ofSetColor(c1, c2, c3);
        ofDrawRectRounded(x + defSpace + (defCellSize * f), y + defCellSize + (defCellSize/2), wi - (defSpace * 2), hi-defCellSize*2,r);    // outside rectangle
        ofFill();
        if (mouseX > x + defSpace + (defCellSize * f) && mouseX < x + defSpace + wi - (defSpace * 2) + (defCellSize * f) && mouseY >  y + defCellSize + (defCellSize/2) && mouseY < y + defCellSize + (defCellSize/2) + hi-defCellSize*2 && mouseExe == 1 && level < 2 && overlay == 0 && waitTime(defWaitTime)){   // check if mouse is in fader coordinates and map values.
            mouseBrightness = ofMap(mouseY, y + defCellSize + (defCellSize/2) + hi-defCellSize*2-1, y + defCellSize + (defCellSize/2)+1, 0, 255);
            brightnessChanged = true;
        }
        ofDrawRectRounded(x + defSpace + 3 + (defCellSize * f), y + defCellSize + (defCellSize/2) + hi-defCellSize*2 -3, wi - (defSpace * 2)-6,ofMap(mouseBrightness, 0, 255, 0, ((hi-defCellSize*2)-6)*-1),r);  //  draw mapped value
        ofSetColor(255, 255, 255);
        ofDrawRectRounded(x + defSpace + (defCellSize * f), y + defCellSize, wi - (defSpace * 2), (defCellSize/2)-10,r);  // MAX
        ofSetColor(0, 0, 0);
        fixText.drawString("MAX", x + defSpace + (wi/2)-20 + (defCellSize * f), y + defCellSize+ 20);
        if (mouseX > x + defSpace + (defCellSize * f) && mouseX < x + defSpace +  wi - (defSpace * 2) + (defCellSize * f) && mouseY > y + defCellSize && mouseY < y + defCellSize + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0 && waitTime(defWaitTime)){  // check mouse position for MAX button
            mouseBrightness = 255;
            brightnessChanged = true;
        }
        ofSetColor(255, 255, 255);
        ofDrawRectRounded(x + defSpace + (defCellSize * f), y + hi - (defCellSize/2) + 4, wi - (defSpace * 2), (defCellSize/2)-10,r);  // MIN
        ofSetColor(0, 0, 0);
        fixText.drawString("MIN", x + defSpace + (wi/2)-20 + (defCellSize * f),  y + hi - (defCellSize/2) + 20);
        if (mouseX > x + defSpace + (defCellSize * f) && mouseX < x + defSpace +  wi - (defSpace * 2) + (defCellSize * f) && mouseY > y + hi - (defCellSize/2) + 4 && mouseY <  y + hi - (defCellSize/2) + 4 + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0 && waitTime(defWaitTime)){   // check mouse position for MIN button
            mouseBrightness = 0;
            brightnessChanged = true;
        }

        ofSetColor(255, 255, 255);
        fixText.drawString(to_string(mouseBrightness), x + (wi/2) - 15, y+42);   //  Value in text

        ofFill();
        ofSetColor(190, 190, 190);

    }
    ofFill();
    
}


void ofApp::rgbMixer(int i){       // RGB mixer UI panel
    int x = gX(panels[i].x);
    int y = gY(panels[i].y);
    int wi = panels[i].wi*defCellSize;
    int hi = panels[i].hi*defCellSize;
    int r = panels[i].r;
    int cellSize = panels[i].cellSize;
    int defSpace = panels[i].defSpace;
    string type = panels[i].type;
    string name = panels[i].name;
    int cellC = panels[i].x * panels[i].y;   //number of cells panel can fit (or resolution of cells)
    int wid = panels[i].wi;
    int hei = panels[i].hi;
    int multi = wid * hei;
    
    if (mode == "delete"){
        if (clickLeft(x, y, wi, hi)){
            delPanel(i);
            return;
        }
    }
    //vector<int> fadata = panels[i].fadeData;
    ofFill();
    ofSetColor(77, 90, 77);
    ofDrawRectRounded(x, y, wi, hi,r);
    ofNoFill();
    ofSetColor(114, 196, 114);
    ofSetLineWidth(1);
    ofDrawRectRounded(x, y, wi, hi,r);
    ofSetColor(255, 255, 255);
    panelName.drawString(name, x+15, y+15);
    
    if (wid < 3 || hei < 4){    //check if panel is too small
        fixText.drawString("Panel\ntoo small", x+80, y+(hi/2)-15);
        
    } else {
        
        for (int f = 0; f < 3; f++){   // draw the three seperate faders
            ofNoFill();
            ofSetLineWidth(2);
            int c1 = 0;
            int c2 = 0;
            int c3 = 0;
            if (f == 0){
                c1 = 255;
            } else if (f == 1){
                c2 = 255;
            } else {
                c3 = 255;
            }
            ofSetColor(c1, c2, c3);
            ofDrawRectRounded(x + defSpace + ((wi/3) * f), y + (wi/3) + ((wi/3)/2), (wi/3) - (defSpace * 2), hi-(wi/3)*2,r);    // outside rectangle
            ofFill();
            if (mouseX > x + defSpace + ((wi/3) * f) && mouseX < x + defSpace + (wi/3) - (defSpace * 2) + ((wi/3) * f) && mouseY >  y + (wi/3) + ((wi/3)/2) && mouseY < y + (wi/3) + ((wi/3)/2) + hi-(wi/3)*2 && mouseExe == 1 && level < 2 && overlay == 0 && waitTime(defWaitTime)){   // check if mouse is in position and map values.
                mouseColors[f] = ofMap(mouseY, y + (wi/3) + ((wi/3)/2) + hi-(wi/3)*2-1, y + (wi/3) + ((wi/3)/2)+1, 0, 255);
                colorsChanged = true;
            }
            ofDrawRectRounded(x + defSpace + 3 + ((wi/3) * f), y + (wi/3) + ((wi/3)/2) + hi-(wi/3)*2 -3, (wi/3) - (defSpace * 2)-6,ofMap(mouseColors[f], 0, 255, 0, ((hi-(wi/3)*2)-6)*-1),r);  //  draw mapped value
            ofSetColor(255, 255, 255);
            ofDrawRectRounded(x + defSpace + ((wi/3) * f), y + (wi/3), (wi/3) - (defSpace * 2), (defCellSize/2)-10,r);  // MAX
            ofSetColor(0, 0, 0);
            fixText.drawString("MAX", x + defSpace + ((wi/3)/2)-20 + ((wi/3) * f), y + (wi/3)+ 20);
            if (mouseX > x + defSpace + ((wi/3) * f) && mouseX < x + defSpace +  (wi/3) - (defSpace * 2) + ((wi/3) * f) && mouseY > y + (wi/3) && mouseY < y + (wi/3) + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0 && waitTime(defWaitTime)){
                mouseColors[f] = 255;
                colorsChanged = true;
            }
            ofSetColor(255, 255, 255);
            ofDrawRectRounded(x + defSpace + ((wi/3) * f), y + hi - (defCellSize/2) + 4, (wi/3) - (defSpace * 2), (defCellSize/2)-10,r);  // MIN
            ofSetColor(0, 0, 0);
            fixText.drawString("MIN", x + defSpace + ((wi/3)/2)-20 + ((wi/3) * f),  y + hi - (defCellSize/2) + 20);
            if (mouseX > x + defSpace + ((wi/3) * f) && mouseX < x + defSpace +  (wi/3) - (defSpace * 2) + ((wi/3) * f) && mouseY > y + hi - (defCellSize/2) + 4 && mouseY <  y + hi - (defCellSize/2) + 4 + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0 && waitTime(defWaitTime)){
                mouseColors[f] = 0;
                colorsChanged = true;
            }

            ofSetColor(255, 255, 255);
            fixText.drawString(to_string(mouseColors[f]), x + defSpace+(wi/3)-((wi/3)/2)-15 + (wi/3 * f), y+42);   //  Value in text
            }

   
        ofFill();
        ofSetColor(190, 190, 190);
        
    }
    ofFill();
    
}


void ofApp::wauv(int i){       // White Amber UV fader mixer UI panel
    int x = gX(panels[i].x);
    int y = gY(panels[i].y);
    int wi = panels[i].wi*defCellSize;
    int hi = panels[i].hi*defCellSize;
    int r = panels[i].r;
    int cellSize = panels[i].cellSize;
    int defSpace = panels[i].defSpace;
    string type = panels[i].type;
    string name = panels[i].name;
    int cellC = panels[i].x * panels[i].y;   //number of cells panel can fit (or resolution of cells)
    int wid = panels[i].wi;
    int hei = panels[i].hi;
    int multi = wid * hei;
    
    if (mode == "delete"){
        if (clickLeft(x, y, wi, hi)){
            delPanel(i);
            return;
        }
    }
    
    //vector<int> fadata = panels[i].fadeData;
    ofFill();
    ofSetColor(77, 90, 77);
    ofDrawRectRounded(x, y, wi, hi,r);
    ofNoFill();
    ofSetColor(114, 196, 114);
    ofSetLineWidth(1);
    ofDrawRectRounded(x, y, wi, hi,r);
    ofSetColor(255, 255, 255);
    panelName.drawString(name, x+15, y+15);
    
    if (wid < 3 || hei < 4){    //check if panel is too small
        fixText.drawString("Panel\ntoo small", x+80, y+(hi/2)-15);
        
    } else {
        
        for (int f = 0; f < 3; f++){   // draw the three seperate faders
                    ofNoFill();
                    ofSetLineWidth(2);
                    if (f == 0){  //number of fader
                        ofSetColor(255, 255, 255);   // first fader is white channel so set fill colour to white
                    } else if (f == 1){
                        ofSetColor(255, 185, 130);   // set fill to orange (amber)
                    } else {
                      ofSetColor(131, 98, 240);     // set fill to violet (uv)
                    }
                    
                    ofDrawRectRounded(x + defSpace + ((wi/3) * f), y + (wi/3) + ((wi/3)/2), (wi/3) - (defSpace * 2), hi-(wi/3)*2,r);    // outside rectangle
                    ofFill();
                    if (mouseX > x + defSpace + ((wi/3) * f) && mouseX < x + defSpace + (wi/3) - (defSpace * 2) + ((wi/3) * f) && mouseY >  y + (wi/3) + ((wi/3)/2) && mouseY < y + (wi/3) + ((wi/3)/2) + hi-(wi/3)*2 && mouseExe == 1 && level < 2 && overlay == 0 && waitTime(defWaitTime)){   // check if mouse is in position and map values.
                        mouseColors[3+f] = ofMap(mouseY, y + (wi/3) + ((wi/3)/2) + hi-(wi/3)*2-1, y + (wi/3) + ((wi/3)/2)+1, 0, 255);
                        wauvChanged = true;
                        
                    }
                    ofDrawRectRounded(x + defSpace + 3 + ((wi/3) * f), y + (wi/3) + ((wi/3)/2) + hi-(wi/3)*2 -3, (wi/3) - (defSpace * 2)-6,ofMap(mouseColors[3+f], 0, 255, 0, ((hi-(wi/3)*2)-6)*-1),r);  //  draw mapped value
                    ofSetColor(255, 255, 255);
                    ofDrawRectRounded(x + defSpace + ((wi/3) * f), y + (wi/3), (wi/3) - (defSpace * 2), (defCellSize/2)-10,r);  // MAX
                    ofSetColor(0, 0, 0);
                    fixText.drawString("MAX", x + defSpace + ((wi/3)/2)-20 + ((wi/3) * f), y + (wi/3)+ 20);
                    if (mouseX > x + defSpace + ((wi/3) * f) && mouseX < x + defSpace +  (wi/3) - (defSpace * 2) + ((wi/3) * f) && mouseY > y + (wi/3) && mouseY < y + (wi/3) + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0 && waitTime(defWaitTime)){
                        mouseColors[3+f] = 255;
                        wauvChanged = true;
                    }
                    ofSetColor(255, 255, 255);
                    ofDrawRectRounded(x + defSpace + ((wi/3) * f), y + hi - (defCellSize/2) + 4, (wi/3) - (defSpace * 2), (defCellSize/2)-10,r);  // MIN
                    ofSetColor(0, 0, 0);
                    fixText.drawString("MIN", x + defSpace + ((wi/3)/2)-20 + ((wi/3) * f),  y + hi - (defCellSize/2) + 20);
                    if (mouseX > x + defSpace + ((wi/3) * f) && mouseX < x + defSpace +  (wi/3) - (defSpace * 2) + ((wi/3) * f) && mouseY > y + hi - (defCellSize/2) + 4 && mouseY <  y + hi - (defCellSize/2) + 4 + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0 && waitTime(defWaitTime)){
                        mouseColors[3+f] = 0;
                        wauvChanged = true;
                    }

                    ofSetColor(255, 255, 255);
                    fixText.drawString(to_string(mouseColors[3+f]), x + defSpace+(wi/3)-((wi/3)/2)-15 + (wi/3 * f), y+42);   //  Value in text
                    }
        ofFill();
        ofSetColor(190, 190, 190);
    }
    ofFill();
}

void ofApp::posMixer(int i){       // White Amber UV fader mixer UI panel
    int x = gX(panels[i].x);
    int y = gY(panels[i].y);
    int wi = panels[i].wi*defCellSize;
    int hi = panels[i].hi*defCellSize;
    int r = panels[i].r;
    int cellSize = panels[i].cellSize;
    int defSpace = panels[i].defSpace;
    string type = panels[i].type;
    string name = panels[i].name;
    int cellC = panels[i].x * panels[i].y;   //number of cells panel can fit (or resolution of cells)
    int wid = panels[i].wi;
    int hei = panels[i].hi;
    int multi = wid * hei;
    
    if (mode == "delete"){
        if (clickLeft(x, y, wi, hi)){
            delPanel(i);
            return;
        }
    }
    
    //vector<int> fadata = panels[i].fadeData;
    ofFill();
    ofSetColor(77, 90, 77);
    ofDrawRectRounded(x, y, wi, hi,r);
    ofNoFill();
    ofSetColor(114, 196, 114);
    ofSetLineWidth(1);
    ofDrawRectRounded(x, y, wi, hi,r);
    ofSetColor(255, 255, 255);
    panelName.drawString(name, x+15, y+15);
    if (wid < 2 || hei < 4){    //check if panel is too small
        fixText.drawString("Panel\ntoo small", x+80, y+(hi/2)-15);
    } else {
        for (int f = 0; f < 2; f++){   // draw the two seperate faders
                    ofNoFill();
                    ofSetLineWidth(2);
                    ofSetColor(255,255,255);
                    ofDrawRectRounded(x + defSpace + ((wi/2) * f), y + (wi/2) + ((wi/2)/2), (wi/2) - (defSpace * 2), hi-(wi/2)*2,r);    // outside rectangle
                    ofFill();
                    if (mouseX > x + defSpace + ((wi/2) * f) && mouseX < x + defSpace + (wi/2) - (defSpace * 2) + ((wi/2) * f) && mouseY >  y + (wi/2) + ((wi/2)/2) && mouseY < y + (wi/2) + ((wi/2)/2) + hi-(wi/2)*2 && mouseExe == 1 && level < 2 && overlay == 0 && waitTime(defWaitTime)){   // check if mouse is in position and map values.
                        mousePosition[f] = ofMap(mouseY, y + (wi/2) + ((wi/2)/2) + hi-(wi/2)*2-1, y + (wi/2) + ((wi/2)/2)+1, 0, 255);
                        posChanged = true;
                    }
                    ofDrawRectRounded(x + defSpace + 3 + ((wi/2) * f), y + (wi/2) + ((wi/2)/2) + hi-(wi/2)*2 -3, (wi/2) - (defSpace * 2)-6,ofMap(mousePosition[f], 0, 255, 0, ((hi-(wi/2)*2)-6)*-1),r);  //  draw mapped value
                    ofSetColor(255, 255, 255);
                    ofDrawRectRounded(x + defSpace + ((wi/2) * f), y + (wi/2), (wi/2) - (defSpace * 2), (defCellSize/2)-10,r);  // MAX
                    ofSetColor(0, 0, 0);
                    fixText.drawString("MAX", x + defSpace + ((wi/2)/2)-20 + ((wi/2) * f), y + (wi/2)+ 20);
                    if (mouseX > x + defSpace + ((wi/2) * f) && mouseX < x + defSpace +  (wi/2) - (defSpace * 2) + ((wi/2) * f) && mouseY > y + (wi/2) && mouseY < y + (wi/2) + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0 && waitTime(defWaitTime)){
                        mousePosition[f] = 255;
                        posChanged = true;
                    }
                    ofSetColor(255, 255, 255);
                    ofDrawRectRounded(x + defSpace + ((wi/2) * f), y + hi - (defCellSize/2) + 4, (wi/2) - (defSpace * 2), (defCellSize/2)-10,r);  // MIN
                    ofSetColor(0, 0, 0);
                    fixText.drawString("MIN", x + defSpace + ((wi/2)/2)-20 + ((wi/2) * f),  y + hi - (defCellSize/2) + 20);
                    if (mouseX > x + defSpace + ((wi/2) * f) && mouseX < x + defSpace +  (wi/2) - (defSpace * 2) + ((wi/2) * f) && mouseY > y + hi - (defCellSize/2) + 4 && mouseY <  y + hi - (defCellSize/2) + 4 + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0 && waitTime(defWaitTime)){
                        mousePosition[f] = 0;
                        posChanged = true;
                    }

                    ofSetColor(255, 255, 255);
                    fixText.drawString(to_string(mousePosition[f]), x + defSpace+(wi/2)-((wi/2)/2)-15 + (wi/2 * f), y+42);   //  Value in text
                    }
        ofFill();
        ofSetColor(190, 190, 190);
        
    }
    ofFill();
    
}

void ofApp::colorsPanel(int i){     // UI panel for user storing / defining colours
    int x = gX(panels[i].x);
    int y = gY(panels[i].y);
    int wi = panels[i].wi*defCellSize;
    int hi = panels[i].hi*defCellSize;
    int r = panels[i].r;
    int cellSize = panels[i].cellSize;
    int defSpace = panels[i].defSpace;
    string type = panels[i].type;
    string name = panels[i].name;
    int cellC = panels[i].x * panels[i].y;   //number of cells panel can fit (or resolution of cells)
    int wid = panels[i].wi;
    int hei = panels[i].hi;
    int multi = wid * hei;
    if (panels[i].savedColors.size() == 0){     // initially - the vector needs to be defined so data can be stored later and the vector not to be cleared everytime the function is called. So random values are generated initially. Initial colours not shown as class "set" attribute is set to false.
                for (int u = 0; u < multi; u++){
                    panels[i].savedColors.push_back(colorObj);
                    panels[i].savedColors[u].iden = u;
                    panels[i].savedColors[u].r = rand() % 255;
                    panels[i].savedColors[u].g = rand() % 255;
                    panels[i].savedColors[u].b = rand() % 255;
                    
                }
    }
    ofFill();       // draw background rectangle
    ofSetColor(48, 36, 54);
    ofDrawRectRounded(x, y, wi, hi,r);
    ofSetColor(217, 128, 255);
    ofNoFill();
    ofSetLineWidth(1);
    ofDrawRectRounded(x, y, wi, hi,r);
    int getCellByIden = 0;
    
        for (int r = 0; r < hei; r++){      //draw rows
            for (int c = 0; c < wid; c++){  //draw columns
                if (panels[i].savedColors[getCellByIden].iden != 0){
                    if (panels[i].savedColors[getCellByIden].set != false){
                        ofSetColor(panels[i].savedColors[getCellByIden].r, panels[i].savedColors[getCellByIden].g, panels[i].savedColors[getCellByIden].b);   //set colour of cell
                        ofFill();
                        ofDrawRectRounded(x + (c * defCellSize), y + (r*defCellSize), defCellSize, defCellSize,10);
                        ofSetLineWidth(1);
                        ofSetColor(40,40,40);
                        ofRectangle topGrey;
                        topGrey.x = x + (c * defCellSize);
                        topGrey.y = y + (r*defCellSize);
                        topGrey.width = defCellSize;
                        topGrey.height = 25;
                        ofDrawRectRounded(topGrey,10,10,0,0);   //abstract shape top of cell for label
                        ofDrawTriangle(x+(c*defCellSize), y+(r*defCellSize)+25, x+defCellSize+(c*defCellSize), y+(r*defCellSize)+25, x+(c*defCellSize), y+(r*defCellSize)+40);
                        ofSetColor(255, 255, 255);
                        fixText.drawString(to_string(panels[i].savedColors[getCellByIden].iden),x+ 10 + (c * defCellSize),y + 17 + (r*defCellSize));
                        fixMain.drawString(panels[i].savedColors[getCellByIden].name,x+ 10 + (c * defCellSize),y + 45 + (r*defCellSize));
                        ofNoFill();
                        
                        
                        
                    } else {
                        ofSetColor(217, 128, 255);   //set colour of cell
                        ofNoFill();
                        ofDrawRectRounded(x + (c * defCellSize), y + (r*defCellSize), defCellSize, defCellSize,10);
                        ofSetLineWidth(1);
                        fixText.drawString(to_string(panels[i].savedColors[getCellByIden].iden),x+ 10 + (c * defCellSize),y + 17 + (r*defCellSize));
                        ofNoFill();
                    }
                    if (overlay == 0 && waitTime(defWaitTime)){
                        if (clickLeft(x + (c * defCellSize), y + (r*defCellSize), defCellSize, defCellSize) && level < 2){   //if user clicks on a cell. store a color from color data mouse has.
                            setLi();
                            
                            if (panels[i].savedColors[getCellByIden].set && mode != "store"){
                                mouseColors[0] = panels[i].savedColors[getCellByIden].r;
                                mouseColors[1] = panels[i].savedColors[getCellByIden].g;
                                mouseColors[2] = panels[i].savedColors[getCellByIden].b;
                                mouseColors[3] = panels[i].savedColors[getCellByIden].w;
                                mouseColors[4] = panels[i].savedColors[getCellByIden].a;
                                mouseColors[5] = panels[i].savedColors[getCellByIden].uv;
                                colorsChanged = true;
                            }
                            
                            if (mode == "store"){
                                mode = "";
                                panels[i].savedColors[getCellByIden].r = mouseColors[0];
                                panels[i].savedColors[getCellByIden].g = mouseColors[1];
                                panels[i].savedColors[getCellByIden].b = mouseColors[2];
                                panels[i].savedColors[getCellByIden].name = "Color";
                                panels[i].savedColors[getCellByIden].set = true;
                            } else if (mode == "delete") {
                                mode = "";
                                panels[i].savedColors[getCellByIden].r = 255;
                                panels[i].savedColors[getCellByIden].g = 255;
                                panels[i].savedColors[getCellByIden].b = 255;
                                panels[i].savedColors[getCellByIden].name = "";
                                panels[i].savedColors[getCellByIden].set = false;
                            } else if (mode == "rename" && strInputObj.fieldInputState == 0){   // incomplete renaming function
                                strInputObj.fieldInputState = 1;
                                strInputObj.screenType = 0;
                                strInputObj.search = 0;
                                strInputObj.maxCount = 6;
                                strInputObj.current = panels[i].savedColors[getCellByIden].name;
                                strInputObj.textValue = panels[i].savedColors[getCellByIden].name;
                                ptrType = 0;
                                fieldPtr = &panels[i].savedColors[getCellByIden].name;
                    
                            }  else if (mode == ""){   // Update mouse colors
                                if (panels[i].savedColors[getCellByIden].set){
                                    colorsChanged = true;
                                    mouseColors[0] = panels[i].savedColors[getCellByIden].r;
                                    mouseColors[1] = panels[i].savedColors[getCellByIden].g;
                                    mouseColors[2] = panels[i].savedColors[getCellByIden].b;
                                    mouseColors[3] = panels[i].savedColors[getCellByIden].w;
                                    mouseColors[4] = panels[i].savedColors[getCellByIden].a;
                                    mouseColors[5] = panels[i].savedColors[getCellByIden].uv;
                                }
                            }
                        
                    }
                    }
                    
                    getCellByIden++;
                } else {
                    ofSetColor(217, 128, 255);   //set colour of cell
                    ofFill();
                    ofRectangle rectShape;
                    rectShape.x = x + (c * defCellSize);
                    rectShape.y = y + (r*defCellSize);
                    rectShape.width = defCellSize;
                    rectShape.height = defCellSize;
                    ofDrawRectRounded(rectShape,r,10,10,10);
                    ofSetColor(255, 255, 255);
                    panelType.drawString("Colors",x+ defCellGap/2 + (c * defCellSize),y + defCellGap + (r*defCellSize));
                    colorsIcon.draw(x+12 + (c * defCellSize),y+15 + (r*defCellSize),iconSize,iconSize);
                    ofNoFill();
                    ofSetLineWidth(2);
                    //ofDrawRectangle(x, y, defCellSize, defCellSize);
                    getCellByIden++;
                    if (clickLeft(x, y, defCellSize, defCellSize) && mode == "delete"){
                        delPanel(i);
                        return;
                    }
                    
                }
            }
        }
}

void ofApp::delPanel(int i){
    vector<panelClass> panelsTemp;
    panelsTemp = panels;
    panels = {};
    
    for (int u = 0; u < panelsTemp.size(); u++){
        if (u != i){
            panels.push_back(panelObj);
            panels[panels.size()-1] = panelsTemp[u];
        }
    }
    
}


void ofApp::brightnessPanel(int i){ // UI panel for user stored brightness values
    int x = gX(panels[i].x);
    int y = gY(panels[i].y);
    int wi = panels[i].wi*defCellSize;
    int hi = panels[i].hi*defCellSize;
    int r = panels[i].r;
    int cellSize = panels[i].cellSize;
    int defSpace = panels[i].defSpace;
    string type = panels[i].type;
    string name = panels[i].name;
    int cellC = panels[i].x * panels[i].y;   //number of cells panel can fit (or resolution of cells)
    int wid = panels[i].wi;
    int hei = panels[i].hi;
    int multi = wid * hei;
    
    if (panels[i].savedBrightness.size() == 0){
                for (int u = 0; u < multi; u++){
                    panels[i].savedBrightness.push_back(brightObj);
                    panels[i].savedBrightness[u].iden = u;
                }
    }
    
    ofFill();
    ofSetColor(36, 54, 52);
    ofDrawRectRounded(x, y, wi, hi,r);
    ofSetColor(128, 238, 255);
    ofNoFill();
    ofSetLineWidth(1);
    ofDrawRectRounded(x, y, wi, hi,r);

    int getCellByIden = 0;
    
        for (int r = 0; r < hei; r++){      //draw rows
            for (int c = 0; c < wid; c++){  //draw columns
                if (panels[i].savedBrightness[getCellByIden].iden != 0){
                    if (panels[i].savedBrightness[getCellByIden].set != false){
                        
                        ofFill();
                        ofSetColor(90,90,90);
                        ofDrawRectRounded(x + (c * defCellSize), y + (r*defCellSize), defCellSize, defCellSize,10);
                        ofSetLineWidth(1);
                        ofSetColor(40,40,40);
                        ofRectangle topGrey;
                        topGrey.x = x + (c * defCellSize);
                        topGrey.y = y + (r*defCellSize);
                        topGrey.width = defCellSize;
                        topGrey.height = 25;
                        ofDrawRectRounded(topGrey,10,10,0,0);   //abstract shape top of cell for label
                        ofDrawTriangle(x+(c*defCellSize), y+(r*defCellSize)+25, x+defCellSize+(c*defCellSize), y+(r*defCellSize)+25, x+(c*defCellSize), y+(r*defCellSize)+40);
                        ofSetColor(255, 255, 255);
                        fixText.drawString(to_string(panels[i].savedBrightness[getCellByIden].iden),x+ 10 + (c * defCellSize),y + 17 + (r*defCellSize));
                        panelType.drawString(to_string(int(ofMap(panels[i].savedBrightness[getCellByIden].value,0,255,0,100))) + "%",x+ 15 + (c * defCellSize),y + 44 + (r*defCellSize));
                        ofNoFill();
                    } else {
                        ofSetColor(128, 238, 255);   //set colour of cell
                        ofNoFill();
                        ofDrawRectRounded(x + (c * defCellSize), y + (r*defCellSize), defCellSize, defCellSize,10);
                        ofSetLineWidth(1);
                        fixText.drawString(to_string(panels[i].savedBrightness[getCellByIden].iden),x+ 10 + (c * defCellSize),y + 17 + (r*defCellSize));
                        ofNoFill();
                        
                    }
                    if (overlay == 0 && waitTime(defWaitTime)){
                        if (clickLeft(x + (c * defCellSize), y + (r*defCellSize), defCellSize, defCellSize) && level < 2){   //if user clicks on a cell. store a color from color data mouse has.
                            setLi();
                            if (mode == "store"){
                                mode = "";
                                panels[i].savedBrightness[getCellByIden].value = mouseBrightness;
                                panels[i].savedBrightness[getCellByIden].set = true;
                            } else if (mode == "delete") {
                                mode = "";
                                panels[i].savedBrightness[getCellByIden].value = 255;
                                panels[i].savedBrightness[getCellByIden].set = false;
                            } else if (mode == ""){
                                if (panels[i].savedBrightness[getCellByIden].set){
                                    brightnessChanged = true;
                                    mouseBrightness = panels[i].savedBrightness[getCellByIden].value;
                                }
                            }
                        }
                    }
                    
                    
                    getCellByIden++;
                } else {
                    ofSetColor(128, 238, 255);   //set colour of cell
                    ofFill();
                    ofRectangle rectShape;
                    rectShape.x = x + (c * defCellSize);
                    rectShape.y = y + (r*defCellSize);
                    rectShape.width = defCellSize;
                    rectShape.height = defCellSize;
                    ofDrawRectRounded(rectShape,r,10,10,10);
                    ofSetColor(255, 255, 255);
                    panelType.drawString("Bright",x+ 5 + (c * defCellSize),y + defCellGap + (r*defCellSize));
                    colorsIcon.draw(x+12 + (c * defCellSize),y+15 + (r*defCellSize),iconSize,iconSize);
                    ofNoFill();
                    ofSetLineWidth(2);
                    //ofDrawRectangle(x, y, defCellSize, defCellSize);
                    getCellByIden++;
                    if (clickLeft(x, y, defCellSize, defCellSize) && mode == "delete"){
                        delPanel(i);
                        return;
                    }
                }
            }
        }
}

void ofApp::posPanel(int i){    // UI panel for user defined position values
    int x = gX(panels[i].x);
    int y = gY(panels[i].y);
    int wi = panels[i].wi*defCellSize;
    int hi = panels[i].hi*defCellSize;
    int r = panels[i].r;
    int cellSize = panels[i].cellSize;
    int defSpace = panels[i].defSpace;
    string type = panels[i].type;
    string name = panels[i].name;
    int cellC = panels[i].x * panels[i].y;   //number of cells panel can fit (or resolution of cells)
    int wid = panels[i].wi;
    int hei = panels[i].hi;
    int multi = wid * hei;
    
    if (panels[i].savedPositions.size() == 0){
                for (int u = 0; u < multi; u++){
                    panels[i].savedPositions.push_back(posObj);
                    panels[i].savedPositions[u].iden = u;
                }
    }
    
    ofFill();
    ofSetColor(45, 69, 48);
    ofDrawRectRounded(x, y, wi, hi,r);
    ofSetColor(59, 194, 41);
    ofNoFill();
    ofSetLineWidth(1);
    ofDrawRectRounded(x, y, wi, hi,r);

    int getCellByIden = 0;
    
        for (int r = 0; r < hei; r++){      //draw rows
            for (int c = 0; c < wid; c++){  //draw columns
                if (panels[i].savedPositions[getCellByIden].iden != 0){
                    if (panels[i].savedPositions[getCellByIden].set != false){
                        
                        ofFill();
                        ofSetColor(90,90,90);
                        ofDrawRectRounded(x + (c * defCellSize), y + (r*defCellSize), defCellSize, defCellSize,10);
                        ofSetLineWidth(1);
                        ofSetColor(40,40,40);
                        ofRectangle topGrey;
                        topGrey.x = x + (c * defCellSize);
                        topGrey.y = y + (r*defCellSize);
                        topGrey.width = defCellSize;
                        topGrey.height = 25;
                        ofDrawRectRounded(topGrey,10,10,0,0);   //abstract shape top of cell for label
                        ofDrawTriangle(x+(c*defCellSize), y+(r*defCellSize)+25, x+defCellSize+(c*defCellSize), y+(r*defCellSize)+25, x+(c*defCellSize), y+(r*defCellSize)+40);
                        ofSetColor(255, 255, 255);
                        fixText.drawString(to_string(panels[i].savedPositions[getCellByIden].iden),x+ 10 + (c * defCellSize),y + 17 + (r*defCellSize));
                        panelType.drawString(panels[i].savedPositions[getCellByIden].name,x+ 5 + (c * defCellSize),y + 34 + (r*defCellSize));
                        ofNoFill();
                    } else {
                        ofSetColor(59, 194, 41);   //set colour of cell
                        ofNoFill();
                        ofDrawRectRounded(x + (c * defCellSize), y + (r*defCellSize), defCellSize, defCellSize,defRounded*2);
                        ofSetLineWidth(1);
                        fixText.drawString(to_string(panels[i].savedPositions[getCellByIden].iden),x+ 10 + (c * defCellSize),y + 17 + (r*defCellSize));
                        ofNoFill();
                        
                    }
                    if (overlay == 0 && waitTime(defWaitTime)){
                        if (clickLeft(x + (c * defCellSize), y + (r*defCellSize), defCellSize, defCellSize) && level < 2){   //if user clicks on a cell. store a color from color data mouse has.
                            setLi();
                            if (mode == "store"){
                                mode = "";
                                panels[i].savedPositions[getCellByIden].position = mousePosition;
                                panels[i].savedPositions[getCellByIden].set = true;
                                panels[i].savedPositions[getCellByIden].name = "p #" + to_string(getCellByIden);
                            } else if (mode == "delete") {
                                mode = "";
                                panels[i].savedPositions[getCellByIden].position = {};
                                panels[i].savedPositions[getCellByIden].set = false;
                            } else if (mode == "rename" && strInputObj.fieldInputState == 0){   // incomplete renaming function
                                strInputObj.fieldInputState = 1;
                                strInputObj.maxCount = 6;
                                strInputObj.current = panels[i].savedPositions[getCellByIden].name;
                                strInputObj.textValue = panels[i].savedPositions[getCellByIden].name;
                                ptrType = 0;
                                fieldPtr = &panels[i].savedPositions[getCellByIden].name;
                            } else if (mode == "" && panels[i].savedPositions[getCellByIden].set){
                                posChanged = true;
                                mousePosition[0] = panels[i].savedPositions[getCellByIden].position[0];
                                mousePosition[1] = panels[i].savedPositions[getCellByIden].position[1];
                                
                            }
                        }
                    }
                    
                    
                    getCellByIden++;
                } else {
                    ofSetColor(59, 194, 41);   //set colour of cell
                    ofFill();
                    ofRectangle rectShape;
                    rectShape.x = x + (c * defCellSize);
                    rectShape.y = y + (r*defCellSize);
                    rectShape.width = defCellSize;
                    rectShape.height = defCellSize;
                    ofDrawRectRounded(rectShape,r,10,10,defRounded*2);
                    ofSetColor(255, 255, 255);
                    panelType.drawString("Positn",x+ 5 + (c * defCellSize),y + defCellGap + (r*defCellSize));
                    colorsIcon.draw(x+12 + (c * defCellSize),y+15 + (r*defCellSize),iconSize,iconSize);
                    ofNoFill();
                    ofSetLineWidth(2);
                    //ofDrawRectangle(x, y, defCellSize, defCellSize);
                    getCellByIden++;
                    if (clickLeft(x, y, defCellSize, defCellSize) && mode == "delete"){
                        delPanel(i);
                        return;
                    }
                }
            }
        }
}



void ofApp::drawGrid(){     // Subprogram that draws grid on screen (panel placement and UI elements) needs work --> check main.cpp comment
    ofSetLineWidth(1);
    ofSetColor(90, 90, 90);
    for (int i = 0; i <= floor((ofGetWindowWidth() - (defCellGap * 2))/ defCellSize); i++){
        int x = (defCellSize * i) + defCellGap;
        ofDrawLine(x, defCellGap, x, ofGetWindowHeight()-(defCellGap*2));
    }

    for (int i = 0; i <= floor((ofGetWindowHeight() - (defCellGap * 2))/ defCellSize); i++){
        int y = (defCellSize * i) + defCellGap;
        ofDrawLine(defCellGap, y, ofGetWindowWidth()-(defCellGap*2), y);
    }
}


bool ofApp::clickLeft(int x, int y, int w, int h){  // Function that determines if User's left Mouse button is clicked between boundary

    if (ofGetMouseX() > x && ofGetMouseX() < x + w && ofGetMouseY() > y && ofGetMouseY() < y + h && mouseExe == 1){
        
        mouseExe = 0;  //reset mouse so no endless execution occurs.
        return true;
    } else {
        return false;
    };
}

bool ofApp::pressLeft(int x, int y, int w, int h){  // Function that determines if User's left Mouse button is pressed between boundary

    if (ofGetMouseX() > x && ofGetMouseX() < x + w && ofGetMouseY() > y && ofGetMouseY() < y + h && mousePExe == 1){
        return true;
    } else {
        return false;
    };
}


//void ofApp::addFixture(string name, bool simShow, int x, int y, int w, int h, int universe, int address){ // Old function --> to be removed in future
//
//    fixturesDef.push_back(fixObj);
//    fixturesDef[fixturesDef.size()-1].name = "cobra";
//    fixturesDef[fixturesDef.size()-1].simShow = true;
//
//    fixturesDef[fixturesDef.size()-1].universe = 1;
//    fixturesDef[fixturesDef.size()-1].startChannel = fixturesDef.size() * 9 + 1;
//
//}

//void ofApp::addFixButton(){   // Old Function --> to be removed in future
//    int x = ofGetWindowWidth() - 80;
//    int y = ofGetWindowHeight() - 80;
//
//    ofSetColor(70, 70, 70);
//    ofFill();
//    ofDrawRectangle(x, y, 30, 30);
//    ofSetColor(255, 255, 255);
//    fixMain.drawString("+",x+8,y+24);
//    ofNoFill();
//    ofSetColor(252, 186, 3);
//    ofSetLineWidth(2);
//    ofDrawRectangle(x, y, 30, 30);
//
//    if (clickLeft(x, y, 30, 30)){
//
//        testFunction();
//    }
//}

void ofApp::scrollButtons(int x, int y, int width, int height, int scrollValue, int * scrollValuePtr, int scrollHeightTotal, int scrollStartHeight, int scrollEndHeight){   // Draw Scroll buttons for Y value responcible for UI elements Y position. Calculate if the elements are off the screen and only show appropriate scroll button
    if (scrollValue < scrollStartHeight){
        ofSetColor(80);
        ofFill();
        ofDrawRectRounded(x+defCellGap, y+defCellGap, width-defCellGap*2, height/2, defRounded);
        ofSetLineWidth(2);
        ofSetColor(252, 186, 3);
        ofNoFill();
        ofDrawRectRounded(x+defCellGap, y+defCellGap, width-defCellGap*2, height/2, defRounded);
        
        if (clickLeft(x+defCellGap, y+defCellGap, width-defCellGap*2, height/2) && waitTime(defWaitTime)){
            * scrollValuePtr = scrollValue + defCellSize*2;
            lastInteraction = ofGetElapsedTimeMillis();
        }
        
    } else {
        ofSetColor(40);
        ofFill();
        ofDrawRectRounded(x+defCellGap, y+defCellGap, width-defCellGap*2, height/2, defRounded);
        ofSetLineWidth(2);
        ofSetColor(30);
        ofNoFill();
        ofDrawRectRounded(x+defCellGap, y+defCellGap, width-defCellGap*2, height/2, defRounded);
        
    }
    
    if (scrollValue + scrollHeightTotal > ofGetHeight() - defCellSize){
        ofSetColor(80);
        ofFill();
        ofDrawRectRounded(x+defCellGap, y+defCellGap*2 + (ofGetHeight()-defCellGap-defCellSize*4)/2, width-defCellGap*2, height/2, defRounded);
        ofSetLineWidth(2);
        ofSetColor(252, 186, 3);
        ofNoFill();
        ofDrawRectRounded(x+defCellGap, y+defCellGap*2 + (ofGetHeight()-defCellGap-defCellSize*4)/2, width-defCellGap*2, height/2, defRounded);
        
        if (clickLeft(x+defCellGap, y+defCellGap*2 + (ofGetHeight()-defCellGap-defCellSize*4)/2, width-defCellGap*2, height/2) && waitTime(defWaitTime)){
             * scrollValuePtr = scrollValue - defCellSize*2;
            lastInteraction = ofGetElapsedTimeMillis();
        }
        
    } else {
        ofSetColor(40);
        ofFill();
        ofDrawRectRounded(x+defCellGap, y+defCellGap*2 + (ofGetHeight()-defCellGap-defCellSize*4)/2, width-defCellGap*2, height/2, defRounded);
        ofSetLineWidth(2);
        ofSetColor(30);
        ofNoFill();
        ofDrawRectRounded(x+defCellGap, y+defCellGap*2 + (ofGetHeight()-defCellGap-defCellSize*4)/2, width-defCellGap*2, height/2, defRounded);
        
    }
    
    
    
}

void ofApp::fixpatchgroup(int i){   // UI panel for displaying DMX patches [incomplete]
    int x = gX(panels[i].x);
    int y = gY(panels[i].y);
    int wi = panels[i].wi*defCellSize;
    int hi = panels[i].hi*defCellSize;
    int r = panels[i].r;
    int cellSize = panels[i].cellSize;
    int defSpace = panels[i].defSpace;
    
    string type = panels[i].type;
    string name = panels[i].name;
    int cellC = panels[i].x * panels[i].y;   //number of cells panel can fit (or resolution of cells)
    int wid = panels[i].wi;
    int hei = panels[i].hi;
    int multi = wid * hei;
    
    ofFill();
    ofSetColor(30);
    ofSetLineWidth(0);
    ofDrawRectRounded(x, y, wi, hi, r);
    ofSetColor(252, 186, 3);
    ofSetLineWidth(1);
    ofNoFill();
    ofDrawRectRounded(x, y, wi, hi, r);
    ofFill();
    ofRectangle header; // Define header variables
    header.x = x;
    header.y = y;
    header.width = wi;
    header.height = defCellSize/2;
    ofDrawRectRounded(header, 5,5,0,0); // draw header
    ofSetColor(255);
    panelType.drawString("Fix Patch Groups", x + defCellSize/2, y+defCellGap*2);
        
    vector<int> knownVerified = {};
    
    for (int u = 0; u < dmxPatch.size(); u++){
        if (dmxPatch[u].verified){
            knownVerified.push_back(u);
        }

    }
    
    int maxRows = (hi - defCellSize*2)/(defCellSize/3);
    
    
    if (knownVerified.size() > 0){  // Check if there is verified patches and if there is display them below
        ofSetColor(255);
        fixText.drawString("Uni", x+ defCellGap, y+defCellSize/2+defCellGap);
        fixText.drawString("Chan", x+ defCellGap+defCellSize/2, y+defCellSize/2+defCellGap);
        fixText.drawString("Quan", x+ defCellGap+defCellSize, y+defCellSize/2+defCellGap);
        fixText.drawString("Name", x+ defCellGap+defCellSize*1.5, y+defCellSize/2+defCellGap);
        ofSetColor(80);
        ofDrawLine(x, y+defCellSize/2+defCellGap*1.5, x+wi, y+defCellSize/2+defCellGap*1.5);
        ofDrawLine(x+defCellSize/2+defCellGap/3, y+defCellSize/2, x+defCellSize/2+defCellGap/3, y+hi-defCellSize);
        ofDrawLine(x+defCellSize/2+defCellGap/2+defCellSize/2, y+defCellSize/2, x+defCellSize/2+defCellGap/2+defCellSize/2, y+hi-defCellSize);
        ofDrawLine(x+defCellSize/2+defCellGap/2+defCellSize, y+defCellSize/2, x+defCellSize/2+defCellGap/2+defCellSize, y+hi-defCellSize);
    
        for (int p = panels[i].startIndex; p < panels[i].startIndex + maxRows; p++){
            int mappedP = ofMap(p, panels[i].startIndex, panels[i].startIndex + maxRows, 0, maxRows);
            if (p < knownVerified.size()){
            ofSetColor(255);
            panelName.drawString(to_string(dmxPatch[knownVerified[p]].universe), x + defCellGap, y+defCellSize/2+defCellGap*2.5+(defCellGap*mappedP*1.7));
            panelName.drawString(to_string(dmxPatch[knownVerified[p]].channel), x + defCellSize/2 + defCellGap*1.5, y+defCellSize/2+defCellGap*2.5+(defCellGap*mappedP*1.7));
            panelName.drawString(to_string(dmxPatch[knownVerified[p]].quantity), x + defCellSize + defCellGap*1.5, y+defCellSize/2+defCellGap*2.5+(defCellGap*mappedP*1.7));
            panelName.drawString(fixturesDef[dmxPatch[knownVerified[p]].fixtureID].name, x + defCellSize*1.5 + defCellGap*1.5, y+defCellSize/2+defCellGap*2.5+(defCellGap*mappedP*1.7));
            
            ofSetColor(80);
            ofDrawLine(x, y+defCellSize/2+defCellGap*3+(defCellGap*mappedP*1.7), x+wi, y+defCellSize/2+defCellGap*3+(defCellGap*mappedP*1.7));
            }
            }
        
        // Page scroll buttons
        
        if (panels[i].startIndex > 0){
            uiButton(x+defCellGap, y+hi-defCellSize, "<<", true, wi/2-defCellGap*2, defCellSize-defCellGap);
            if (clickLeft(x+defCellGap, y+hi-defCellSize, wi/2-defCellGap*2, defCellSize-defCellGap)){
                panels[i].startIndex = panels[i].startIndex - maxRows;
            }
        } else {
            uiButton(x+defCellGap, y+hi-defCellSize, "<<", false, wi/2-defCellGap*2, defCellSize-defCellGap);
        }

        if ((panels[i].startIndex + maxRows) < knownVerified.size()){
            uiButton(x+ (defCellGap*3) + wi/2-defCellGap*2, y+hi-defCellSize, ">>", true, wi/2-defCellGap*2, defCellSize-defCellGap);
            if (clickLeft(x+ (defCellGap*3) + wi/2-defCellGap*2, y+hi-defCellSize, wi/2-defCellGap*2, defCellSize-defCellGap)){
                panels[i].startIndex = panels[i].startIndex + maxRows;
            }
        } else {
            uiButton(x+ (defCellGap*3) + wi/2-defCellGap*2, y+hi-defCellSize, ">>", false, wi/2-defCellGap*2, defCellSize-defCellGap);
        }
        
    
    } else {
        ofSetColor(80);
        panelType.drawString("No suitable\nPatches found", x+defCellSize, y+hi/2-defCellSize/2+defCellGap);
    }
    
}





void ofApp::fixtures(int i){    // UI panel that lists all DMX fixtures within the showfile
    int x = gX(panels[i].x);
    int y = gY(panels[i].y);
    int wi = panels[i].wi*defCellSize;
    int hi = panels[i].hi*defCellSize;
    int r = panels[i].r;
    int cellSize = panels[i].cellSize;
    int defSpace = panels[i].defSpace;
    int startIndex = panels[i].startIndex;
    
    string type = panels[i].type;
    string name = panels[i].name;
    int cellC = panels[i].x * panels[i].y;   //number of cells panel can fit (or resolution of cells)
    int wid = panels[i].wi;
    int hei = panels[i].hi;
    int multi = wid * hei;
    
    ofFill();
    ofSetColor(30);
    ofSetLineWidth(0);
    ofDrawRectRounded(x, y, wi, hi, r);
    ofSetColor(252, 186, 3);
    ofSetLineWidth(1);
    ofNoFill();
    ofDrawRectRounded(x, y, wi, hi, r);
    ofFill();
    ofRectangle header; // Define header variables
    header.x = x;
    header.y = y;
    header.width = wi;
    header.height = defCellSize/2;
    ofDrawRectRounded(header, 5,5,0,0); // draw header
    ofSetColor(255);
    panelType.drawString("Fixtures", x + defCellSize/2, y+defCellGap*2);
        
    vector<int> knownVerified = {};
    
    for (int u = 0; u < dmxPatch.size(); u++){
        if (dmxPatch[u].verified){
            knownVerified.push_back(u);
        }

    }
    
    int maxRows = (hi - defCellSize*2)/(defCellSize/3);   //  defCellSize/3
    
    
    if (showFixtures.size() > 0){  // Check if there is verified patches and if there is display them below
        ofSetColor(255);
        fixText.drawString("Chan", x+ defCellGap, y+defCellSize/2+defCellGap);
        fixText.drawString("Name", x+ defCellGap+defCellSize/2, y+defCellSize/2+defCellGap);
        ofSetColor(80);
        ofDrawLine(x, y+defCellSize/2+defCellGap*1.5, x+wi, y+defCellSize/2+defCellGap*1.5);
        ofDrawLine(x+defCellSize/2+defCellGap/3, y+defCellSize/2, x+defCellSize/2+defCellGap/3, y+hi-defCellSize);
      
        for (int r = 0; r < maxRows; r++){
            if (r + startIndex < showFixtures.size()){
                
                if (mouseX > x && mouseX < x+wi && mouseY > y+defCellSize/2 + defCellSize/3 + (r*defCellSize/3) && mouseY < y+defCellSize/2 + defCellSize/3 + (r*defCellSize/3) + (defCellSize/3)){
                    ofSetColor(80);
                    ofDrawRectangle(x, y+defCellSize/2 + defCellSize/3 + (r*(defCellSize/3)), wi-1, (defCellSize/3));
                    if (clickLeft(x, y+defCellSize/2 + defCellSize/3 + (r*(defCellSize/3)), wi, (defCellSize/3))){
                        bool fixtureAlreadySelected = false;
                        for (int j = 0; j < mouseFixtures.size(); j++){ // ensure the fixture isn't appended if it is already selected
                            if (mouseFixtures[j] == r+startIndex){
                                fixtureAlreadySelected = true;
                            }
                        }
                        if (fixtureAlreadySelected == false){
                            mouseFixtures.push_back(r+startIndex);
                        }
                        
                    }
                }
                
                int index = r+startIndex;
                ofSetColor(255);
                panelName.drawString(to_string(showFixtures[index].startChannel),  x + defCellGap, y+defCellSize/2+defCellGap*2.5+(r*defCellSize/3));
                                int getFixID = showFixtures[index].fixtureID;
                                string name = "";
                                for (int m = 0; m < fixturesDef.size(); m++){
                                    if (fixturesDef[m].fixtureID == getFixID){
                                        name = fixturesDef[m].name;
                                    }
                                }
                panelName.drawString(name,  x + defCellGap*2 + defCellGap*1.5, y+defCellSize/2+defCellGap*2.5+(r*defCellSize/3));
                ofSetColor(80);
                ofDrawLine(x, y+defCellSize/2+defCellGap*3+(defCellSize/3*r), x+wi, y+defCellSize/2+defCellGap*3+(defCellSize/3*r));
                
                bool fixIsSelected = false;
                for (int w = 0; w < mouseFixtures.size(); w++){
                    if (mouseFixtures[w] == index){
                        fixIsSelected = true;
                    }
                }
                
                if (fixIsSelected){
                    ofSetColor(48, 201, 53);
                    ofDrawCircle(x+wi-defCellGap*2, y+defCellSize/2+defCellGap*2.5+(r*defCellSize/3)-defCellGap/4, defCellGap/2);
                }
                
            }
        }
        
        
        
        if (panels[i].startIndex > 0){
            uiButton(x+defCellGap, y+hi-defCellSize, "<<", true, wi/2-defCellGap*2, defCellSize-defCellGap);
            if (clickLeft(x+defCellGap, y+hi-defCellSize, wi/2-defCellGap*2, defCellSize-defCellGap)){
                panels[i].startIndex = panels[i].startIndex - maxRows;
            }
        } else {
            uiButton(x+defCellGap, y+hi-defCellSize, "<<", false, wi/2-defCellGap*2, defCellSize-defCellGap);
        }

        if ((panels[i].startIndex + maxRows) < showFixtures.size()){
            uiButton(x+ (defCellGap*3) + wi/2-defCellGap*2, y+hi-defCellSize, ">>", true, wi/2-defCellGap*2, defCellSize-defCellGap);
            if (clickLeft(x+ (defCellGap*3) + wi/2-defCellGap*2, y+hi-defCellSize, wi/2-defCellGap*2, defCellSize-defCellGap)){
                panels[i].startIndex = panels[i].startIndex + maxRows;
            }
        } else {
            uiButton(x+ (defCellGap*3) + wi/2-defCellGap*2, y+hi-defCellSize, ">>", false, wi/2-defCellGap*2, defCellSize-defCellGap);
        }
        
    
    } else {
        ofSetColor(80);
        panelType.drawString("No Fixtures found\nTry updating DMX", x+defCellSize, y+hi/2-defCellSize/2+defCellGap);
    }
    
}

void ofApp::uipanel(int i){ //draw panels ---> i is the panelObj of the panel within the panels vector
    int x = gX(panels[i].x);
    int y = gY(panels[i].y);
    int wi = panels[i].wi*defCellSize;
    int hi = panels[i].hi*defCellSize;
    int r = panels[i].r;
    int cellSize = panels[i].cellSize;
    int defSpace = panels[i].defSpace;
    string type = panels[i].type;
    string name = panels[i].name;
    int cellC = panels[i].x * panels[i].y;   //number of cells panel can fit (or resolution of cells)
    
    if (mouseX > x && mouseX < x+wi && mouseY > y && mouseY < y+hi){
        level = 1;
        overPanel = true;
    }
    
    if (type == "fixsimulation"){       //simulated fixtures panel
        simulateFixPanel(i);
    } else if (type == "controlpanel"){     //control panel
        controlPanel(i);
    } else if (type=="rgbmixer"){   //colour mixer
        rgbMixer(i);
    } else if (type=="brimixer"){   //brightness mixer
        briMixer(i);
    } else if (type =="colorstor"){ //draw panels where colour can be stored.
        colorsPanel(i);
    } else if (type == "bristor"){
        brightnessPanel(i);
    } else if (type == "wauv"){ // draw white, amber, UV fader mixer
        wauv(i);
    } else if (type == "posMixer"){ // Position mixer panel
        posMixer(i);
    } else if (type == "posStor"){
        posPanel(i);
    } else if (type == "fixpatchgroup"){
        fixpatchgroup(i);
    } else if (type == "fixtures"){
        fixtures(i);
    }
}




void ofApp::loadIconsFonts(){   // Load in fonts and scale them to the correct size
    
    // Below code seems funny as the sizes are stored in a vector before being assigned to a font but it was the only way to get on the fly font sized calculated.. Might be a problem with OpenFrameworks
    
    // fonts
    
    vector<int> fontSize = {};  // scale fonts to their correct proportions
    
    if (defCellSize > 50){
        fontSize.push_back(defCellSize*0.2143); //0
        fontSize.push_back(defCellSize*0.1429); //1
        fontSize.push_back(defCellSize*0.2143); //2
        fontSize.push_back(defCellSize*0.2143); //3
        fontSize.push_back(defCellSize*0.3143); //4
        fontSize.push_back(defCellSize*0.6429); //5
        fontSize.push_back(defCellSize*0.4286); //6
        fontSize.push_back(defCellSize*0.4286); //7
        fontSize.push_back(defCellSize*0.1);    //8
        fontSize.push_back(defCellSize*0.2857); //9
        fontSize.push_back(defCellSize*0.2857); //10
        fontSize.push_back(defCellSize*0.8); //11
    } else {
        // Horrible code practice here but added in temporarily so low res displays can at least render somewhat readable text
        fontSize.push_back(defCellSize*0.2143); //0
        fontSize.push_back(defCellSize*0.1429); //1
        fontSize.push_back(defCellSize*0.2143); //2
        fontSize.push_back(defCellSize*0.2143); //3
        fontSize.push_back(defCellSize*0.3143); //4
        fontSize.push_back(defCellSize*0.6429); //5
        fontSize.push_back(defCellSize*0.4286); //6
        fontSize.push_back(defCellSize*0.4286); //7
        fontSize.push_back(2*(defCellSize*0.1));    //8
        fontSize.push_back(defCellSize*0.2857); //9
        fontSize.push_back(defCellSize*0.2857); //10
        fontSize.push_back(defCellSize*0.8); //11
    }

    
    debugText.load("Lato-Regular.ttf", fontSize[0]);    // tie each font type, name and file to the correct size as done above
    panelName.load("Lato-Regular.ttf", fontSize[1]);
    panelType.load("Lato-Bold.ttf", fontSize[2]);
    fixMain.load("Lato-Bold.ttf", fontSize[3]);
    cardMain.load("Lato-Bold.ttf", fontSize[4]);
    pageMain.load("Lato-Bold.ttf", fontSize[5]);
    pageSub.load("Lato-regular.ttf", fontSize[6]);
    usrInput.load("Lato-Regular.ttf", fontSize[7]);
    fixText.load("Lato-Regular.ttf", fontSize[8]);
    uiIcons.load("LibraSans-Bold.ttf", fontSize[9]);
    overlayBody.load("Lato-Regular.ttf", fontSize[10]);
    splashText.load("Lato-Bold.ttf", fontSize[11]);
    
    
    // icons / images
    colorsIcon.load("panel_icons/colors.png");
}

//--------------------------------------------------------------
void ofApp::setup(){    // Code executed once on initial launch of ofApp
//    ofSetDataPathRoot("../Resources/data/");
    
    lastInteraction = ofGetElapsedTimeMillis();
    showCardstartheight = 3*defCellSize+defCellGap;
    loadIconsFonts();
    genShowFileDir();
    
    showsList.push_back(showFileObject);
    showsList[0].name = ofGetTimestampString("ShowFile %F %R");
    showsList[0].modified = "-";
    showsList[0].freshStart = true;
    showsList[0].active = true;
    
    knownChannels.push_back(knownChannelObj);
    knownChannels[knownChannels.size()-1].purpose = "pan";
    knownChannels[knownChannels.size()-1].desc = "Pan channel for Position Mixer";
    knownChannels.push_back(knownChannelObj);
    knownChannels[knownChannels.size()-1].purpose = "tilt";
    knownChannels[knownChannels.size()-1].desc = "Tilt channel for Position Mixer";
    knownChannels.push_back(knownChannelObj);
    knownChannels[knownChannels.size()-1].purpose = "dimmer";
    knownChannels[knownChannels.size()-1].desc = "Dimmer channel for Brightness Mixer";
    knownChannels.push_back(knownChannelObj);
    knownChannels[knownChannels.size()-1].purpose = "red";
    knownChannels[knownChannels.size()-1].desc = "RED channel for RGB Mixer";
    knownChannels.push_back(knownChannelObj);
    knownChannels[knownChannels.size()-1].purpose = "green";
    knownChannels[knownChannels.size()-1].desc = "GREEN channel for RGB Mixer";
    knownChannels.push_back(knownChannelObj);
    knownChannels[knownChannels.size()-1].purpose = "blue";
    knownChannels[knownChannels.size()-1].desc = "BLUE channel for RGB Mixer";
    knownChannels.push_back(knownChannelObj);
    knownChannels[knownChannels.size()-1].purpose = "white";
    knownChannels[knownChannels.size()-1].desc = "WHITE channel for WAUV Mixer";
    knownChannels.push_back(knownChannelObj);
    knownChannels[knownChannels.size()-1].purpose = "amber";
    knownChannels[knownChannels.size()-1].desc = "AMBER channel for WAUV Mixer";
    knownChannels.push_back(knownChannelObj);
    knownChannels[knownChannels.size()-1].purpose = "uv";
    knownChannels[knownChannels.size()-1].desc = "UV channel for WAUV Mixer";
    
    // prepare known panel types for user definition
    knownPanelType.push_back(knownPanelObj);
    knownPanelType[knownPanelType.size()-1].type = "fixsimulation";
    knownPanelType[knownPanelType.size()-1].name = "Fixture\nSimulation";
    knownPanelType[knownPanelType.size()-1].minWidth = 5;
    knownPanelType[knownPanelType.size()-1].minHeight = 4;
    knownPanelType[knownPanelType.size()-1].cellSize = defCellSize*0.75;
    
    knownPanelType.push_back(knownPanelObj);
    knownPanelType[knownPanelType.size()-1].type = "controlpanel";
    knownPanelType[knownPanelType.size()-1].name = "Control Panel";
    knownPanelType[knownPanelType.size()-1].minWidth = 3;
    knownPanelType[knownPanelType.size()-1].minHeight = 5;
    knownPanelType[knownPanelType.size()-1].cellSize = 40;
    
    knownPanelType.push_back(knownPanelObj);
    knownPanelType[knownPanelType.size()-1].type = "colorstor";
    knownPanelType[knownPanelType.size()-1].name = "Colors";
    knownPanelType[knownPanelType.size()-1].minWidth = 2;
    knownPanelType[knownPanelType.size()-1].minHeight = 1;
    knownPanelType[knownPanelType.size()-1].cellSize = 40;
    
    knownPanelType.push_back(knownPanelObj);
    knownPanelType[knownPanelType.size()-1].type = "rgbmixer";
    knownPanelType[knownPanelType.size()-1].name = "RGB mixer";
    knownPanelType[knownPanelType.size()-1].minWidth = 5;
    knownPanelType[knownPanelType.size()-1].minHeight = 4;
    knownPanelType[knownPanelType.size()-1].cellSize = 40;
    
    knownPanelType.push_back(knownPanelObj);
    knownPanelType[knownPanelType.size()-1].type = "brimixer";
    knownPanelType[knownPanelType.size()-1].name = "Brightness\nMix";
    knownPanelType[knownPanelType.size()-1].minWidth = 1;
    knownPanelType[knownPanelType.size()-1].minHeight = 5;
    knownPanelType[knownPanelType.size()-1].cellSize = defCellSize;   //blah
    knownPanelType[knownPanelType.size()-1].defSpace = 5;
    
    
    knownPanelType.push_back(knownPanelObj);
    knownPanelType[knownPanelType.size()-1].type = "wauv";
    knownPanelType[knownPanelType.size()-1].name = "WAUV Mix";
    knownPanelType[knownPanelType.size()-1].minWidth = 4;
    knownPanelType[knownPanelType.size()-1].minHeight = 5;
    knownPanelType[knownPanelType.size()-1].cellSize = defCellSize;
    knownPanelType[knownPanelType.size()-1].defSpace = 5;
    
    knownPanelType.push_back(knownPanelObj);
    knownPanelType[knownPanelType.size()-1].type = "posMixer";
    knownPanelType[knownPanelType.size()-1].name = "Position\nMix";
    knownPanelType[knownPanelType.size()-1].minWidth = 2;
    knownPanelType[knownPanelType.size()-1].minHeight = 4;
    knownPanelType[knownPanelType.size()-1].cellSize = defCellSize;
    knownPanelType[knownPanelType.size()-1].defSpace = 5;
    
    knownPanelType.push_back(knownPanelObj);
    knownPanelType[knownPanelType.size()-1].type = "posStor";
    knownPanelType[knownPanelType.size()-1].name = "Positions";
    knownPanelType[knownPanelType.size()-1].abbreviation = "Positn";
    knownPanelType[knownPanelType.size()-1].minWidth = 2;
    knownPanelType[knownPanelType.size()-1].minHeight = 1;
    knownPanelType[knownPanelType.size()-1].cellSize = defCellSize;
    knownPanelType[knownPanelType.size()-1].defSpace = 5;
    
    knownPanelType.push_back(knownPanelObj);
    knownPanelType[knownPanelType.size()-1].type = "bristor";
    knownPanelType[knownPanelType.size()-1].name = "Stored\nBrightness";
    knownPanelType[knownPanelType.size()-1].minWidth = 2;
    knownPanelType[knownPanelType.size()-1].minHeight = 2;
    knownPanelType[knownPanelType.size()-1].cellSize = defCellSize;
    knownPanelType[knownPanelType.size()-1].defSpace = 5;
    
//    knownPanelType.push_back(knownPanelObj);
//    knownPanelType[knownPanelType.size()-1].type = "fixpatchgroup";
//    knownPanelType[knownPanelType.size()-1].name = "Fix Patch\nGroups";
//    knownPanelType[knownPanelType.size()-1].minWidth = 3;
//    knownPanelType[knownPanelType.size()-1].minHeight = 4;
//    knownPanelType[knownPanelType.size()-1].cellSize = defCellSize;
//    knownPanelType[knownPanelType.size()-1].defSpace = 5;
    
    knownPanelType.push_back(knownPanelObj);
    knownPanelType[knownPanelType.size()-1].type = "fixtures";
    knownPanelType[knownPanelType.size()-1].name = "Fixtures";
    knownPanelType[knownPanelType.size()-1].minWidth = 4;
    knownPanelType[knownPanelType.size()-1].minHeight = 3;
    knownPanelType[knownPanelType.size()-1].cellSize = defCellSize;
    knownPanelType[knownPanelType.size()-1].defSpace = 5;
    

    panels.push_back(panelObj);
    panels[panels.size()-1].type = "controlpanel";
    panels[panels.size()-1].name = "Control Panel";
    panels[panels.size()-1].x = 24;
    panels[panels.size()-1].y = 8;
    panels[panels.size()-1].wi = 3;
    panels[panels.size()-1].hi = 5;
    panels[panels.size()-1].cellSize = 40;
    
    //demoShow();
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

void ofApp::drawSplash(int wait, int fade){ // Draw splash screen with wait time for how long it is shown for and then how long the fade duration is after the wait
    int alpha = ofMap(ofGetElapsedTimeMillis(),lastInteraction + wait, lastInteraction + wait + fade, 255, 0);
    ofFill();
    ofSetColor(60,60,60,alpha);
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    ofSetColor(40,40,40,alpha);
    splashText.drawString("Lucent\nOperator", ofGetWidth()/2-defCellSize*2, ofGetHeight()/2);
    pageSub.drawString("Project by Marcus Frischknecht 2021", ofGetWidth()/2-defCellSize*4-defCellGap*2, ofGetHeight()/2+defCellSize*2.4);
    
    if (ofGetElapsedTimeMillis() > lastInteraction + wait + fade){
        showSplash = false;
    }
}

//--------------------------------------------------------------

void ofApp::draw(){ // Code repeatedly looped until program close
    
    level = 0;  // Set mouse level to 0 when just on grid
    overPanel = false;  // set mouse to not hovering over a panel when just on grid
    
    if (showGrid == true){
        drawGrid();
    }
    
    
    if (screen == 0){
        strInputHeading = "Rename";
        
        updateSelectedFixtures();
        
    if (panels.size() > 0){
        for (int i = 0; i < panels.size(); i++){
            uipanel(i);
        }
    }

    ofSetColor(255, 255, 255);
    
    if (strInputObj.fieldInputState == 1){
        strInput("#", strInputObj.maxCount);
    }
    
        showGrid = false;
    if (addPanelStage == 1){
        addPanelOverlay();
    } else if (addPanelStage == 2) {
        drawPanel();
        showGrid = true;
    }
    
    } else if (screen == 1){
        drawGrid();
        showFileConfig();
        if (strInputObj.fieldInputState == 1){
            strInput("#", strInputObj.maxCount);
        }
    } else if (screen == 2){
        drawGrid();
        fixtureConfig();
        if (strInputObj.fieldInputState == 1){
            
            strInput("#", strInputObj.maxCount);
        }
    }

    
    if (showDel){
        delOverlay();
    }
    
    if (popupObj.show){
        popUp();
        if (ofGetElapsedTimeMillis() > lastInteraction + popupObj.time){
            popupObj.show = false;
            overlay = 0;
            
        }
    }


    ofSetColor(255);
    debugText.drawString("Incorrect scaling? Push '~'\nLucent Developer | " + to_string(ofGetMouseX()) + " " + to_string(ofGetMouseY()) + " | F" + to_string(fixturesDef.size()) + " | W" + to_string(ofGetWindowWidth()) + " H" + to_string(ofGetWindowHeight()) + " | GR" + to_string(int(floor(((ofGetMouseX()-defCellGap)/defCellSize)))) + " " + to_string(int(floor(((ofGetMouseY()-defCellGap)/defCellSize)))) + " | m" + to_string(level) + " " + mode + " | o" + to_string(overlay) + " | op " + to_string(overPanel) + " | mf " + to_string(mouseFixtures.size()), 20, ofGetWindowHeight() - defCellGap*3); //Debug text
    mouseExe = 0;   //allows user to click on something, drag mouse away and stop execution of that function from occuring. (Like accidently clicking something)
    if (strInputObj.fieldInputState == 3){
        strInputObj.fieldInputState = 0;
    }
    
    if (showSplash == true){
        drawSplash(1000, 1000);
    }
    
    if (ofGetElapsedTimeMillis() > lastDMXOutput + waitBetweenDMX){
//        cout << "\n\n\n\n\n\n\n\n\n" << endl;
//        cout << genDMX(1) << endl;
        lastDMXOutput = ofGetElapsedTimeMillis();
        
//        cout << "DMX" << endl;
        //ofSystem("export PATH=$PATH:~/opt/bin; ola_dev_info");
        if (enableDMXoutput){
            ofSystem("/opt/local/bin/ola_streaming_client  -u 1 -d " + genDMX(1));
        }
    }
    
}

void ofApp::updateScaling(){    // Update all variables responcible for application UI proportion scaling
    if (defCellSize == 70){
        defCellSize = 140;
    } else if (defCellSize == 140){
        defCellSize = 47;
    } else if (defCellSize == 47){
        defCellSize = 70;
    }
    
    defCellGap = defCellSize*0.2142;
    defRounded = defCellSize*0.0714;
    defMiniButton = defCellSize/2;
    showFixturestartheight = (defCellGap*2)+(3*defCellSize);
    showCardstartheight = 3*defCellSize+defCellGap;
    iconSize = defCellSize*0.5; //size to draw icons
    
    loadIconsFonts();
    showSplash = true;
    lastInteraction = ofGetElapsedTimeMillis();
}


//--------------------------------------------------------------

void ofApp::keyPressed(int key){    // Check what key the user is pressing on their ASCII keyboard
    
    char userKey = key;
    
    
    if (strInputObj.fieldInputState == 0 && key == '~'){
        updateScaling();
    }
    
    if (key > 31 && key < 126 && strInputObj.textValue.length() < maxCharacterCount){
        if (strInputObj.screenType == 0){   // Allow char's that are not ints
            char myChar = key;
            
            
            if (myChar != '!' && myChar != '@' && myChar != '#' && myChar != '$' && myChar != '%' && myChar != '^' && myChar != '&' && myChar != '*' && myChar != '(' && myChar != ')' && myChar != '/' && myChar != '|' && myChar != '`'){
                strInputObj.textValue = strInputObj.textValue + myChar;
            }
        
        } else if (strInputObj.screenType == 1){    // allow only chars that represent integer numbers
            char myChar = key;
            if (myChar != '!' && myChar != '@' && myChar != '#' && myChar != '$' && myChar != '%' && myChar != '^' && myChar != '&' && myChar != '*' && myChar != '(' && myChar != ')' && myChar != '/' && myChar != '|' && myChar != '`'){
                if (key > 47 && key < 58){
                    strInputObj.textValue = strInputObj.textValue + myChar; // Only allow characters that represent numbers
                }
            }
        }

    }

    if (key == OF_KEY_BACKSPACE && strInputObj.textValue.size() > 0){
        string original = strInputObj.textValue;
        strInputObj.textValue = "";
        for (int i = 0; i < original.size()-1; i++){
            strInputObj.textValue = strInputObj.textValue + original[i];
        }
    }
    
    if (key == OF_KEY_RETURN && strInputObj.textValue.size() > 0){
        strInputObj.fieldInputState = 3;
        if (ptrType == 0){
            *fieldPtr = strInputObj.textValue;
        } else if (ptrType == 1){
            *fieldPtrInt = stoi(strInputObj.textValue);
        }
        
        mode = "";
        overlay = 0;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){ // Called when the mouse is being dragged
    if (button == 0){
        mousePExe = 1;
        mouseExe = 1;
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){ // Called when the mouse is being pressed
    if (button == 0){
        mousePExe = 1;
    } else {
        mousePExe = 0;
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){    // Called when a mouse button is released
    // left 0, center 1, right 2 <--- default values
    // mouseExe determines if there has been a click. This value changes
    //back to 0 (don't do anything) so it doesn't continually call a function
    //or IF statement to complete an action. (1 Left, 2 Middle, 3 Right)
    
    if (button == 0){
        mouseExe = 1;
    }
    

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}



