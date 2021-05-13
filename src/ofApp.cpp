#include "ofApp.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdio>


vector<panelClass> panels;  //stores panel object
panelClass panelObj;
vector<dmxFixtureclass> fixtures;   //stores all dmxFixture objects
dmxFixtureclass fixObj;

vector<dmxPatchclass> dmxPatch;
dmxPatchclass dmxPatchObj;

fixColor colorObj;
storedBright brightObj;
storedPos posObj;
dmxChannel dmxChannelObj;   // DMX fixture channel object

knownChannelClass knownChannelObj;


vector<showFileFile> showsList;
vector<showFileFile> showsListAlt; // Used as a alt to the main to store showfiles. e.g used when removing a showfile index from the main array
showFileFile showFileObject;

knownPanel knownPanelObj;

strInputvalues strInputObj;


string ocol = ""; // Represents showfile data as a string

int showCardstartheight;
int showPatchstartheight;
int showFixturestartheight;

int ofApp::gX(int x){ //function to generate X coords for element on grid
    return (x * defCellSize)+defCellGap;
}

int ofApp::gY(int Y){ //function to generate Y coords for element on grid
    return (Y * defCellSize)+defCellGap;
}

bool ofApp::waitTime(int time){
    if (ofGetElapsedTimeMillis() > lastInteraction + time){
        return true;
    }
}



void ofApp::blockFeature(){
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
    panelType.drawString("WARNING", ofGetWidth()/2-(defCellSize), ofGetHeight()/2-height/2 + defCellGap*2);
    overlayBody.drawString(blockMsg, ofGetWidth()/2-(width/2)+defCellGap, ofGetHeight()/2-height/2 + defCellGap*6);
}

void ofApp::genShowFileDir(){
    
    bool doesExist = ofDirectory::doesDirectoryExist(showFilesDir);
    
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


int ofApp::intParseAttribute(int panelID, int attriID){  //  responcible for parsing interger attribute values from showData string
    vector<string> allChars;    // Stores the characters of each value.. this is required as a integer was converted to a string and will need to have each character converted back again. e.g: number 24 = 50, 52 (ascii codes)
    output = "";
    for (int a = 0; a < newPanels[panelID][attriID].size(); a++){
        //cout << "SIZE: " << to_string(newPanels[panelID][attriID].size()) << endl;
         allChars.push_back(newPanels[panelID][attriID][a]);
    }
    output = "";
    for (int l = 0; l < allChars.size(); l++){
        output = output + char(stoi(allChars[l]));
    }
    //cout << "OUT " << output << endl;
    
    if (output != "*"){
        return(stoi(output));
    }
}

string ofApp::strParseAttribute(int panelID, int attriID){  //  responcible for parsing interger attribute values from showData string
    vector<string> allChars;    // Stores the characters of each value.. this is required as a integer was converted to a string and will need to have each character converted back again. e.g: number 24 = 50, 52 (ascii codes)
    output = "";
    for (int a = 0; a < newPanels[panelID][attriID].size(); a++){
        //cout << "SIZE: " << to_string(newPanels[panelID][attriID].size()) << endl;
         allChars.push_back(newPanels[panelID][attriID][a]);
    }
    output = "";
    for (int l = 0; l < allChars.size(); l++){
        output = output + char(stoi(allChars[l]));
    }
    //cout << "OUT " << output << endl;
    
    if (output != "*"){
        return(output);
    }
}

vector<int> ofApp::vectParseAttribute(int panelID, int attriID){  //  responcible for parsing interger attribute values from showData string
    vector<string> allChars;    // Stores the characters of each value.. this is required as a integer was converted to a string and will need to have each character converted back again. e.g: number 24 = 50, 52 (ascii codes)
    vector<int> returnV = {};
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


vector<storedBright> vectBriParseAttribute(int panelID, int attriID){  //  responcible for parsing interger attribute values from showData string
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
    
    //cout << "OUT VECT: " << output << endl;

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


void ofApp::strInput(string current, int max){
    level = 3;
    overlay = 1;
    maxCharacterCount = max;
    
//    if (strInputObj.screenType == 0){
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
    
}



void ofApp::delOverlay(){
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
        showDel = false;
        delData1 = "";
        delData2 = "";
        overlay = 0;
    }
    
    
    
    cardMain.drawString("YES", ofGetWidth()/2 - buttonWidth + defCellGap, ofGetHeight()/2+height/2 + defCellGap*2 - buttonHeight);
    cardMain.drawString("NO", ofGetWidth()/2 + defCellGap*2, ofGetHeight()/2+height/2 + defCellGap*2 - buttonHeight);
    
    
}

void ofApp::miniButton(int x, int y, string cha){
    ofFill();
    ofSetColor(80,80,80);
    ofDrawRectangle(x,y,defCellSize/2, defCellSize/2);
    ofNoFill();
    ofSetLineWidth(2);
    ofSetColor(252, 186, 3);
    ofDrawRectangle(x,y,defCellSize/2, defCellSize/2);
    ofSetColor(255);
    fixMain.drawString(cha, x+defCellGap*0.8, y+defCellGap*1.6);
}

void ofApp::fixtureConfig(){   // Showfile screen (load, save, delete and configuration)
    int showPatchHeight = defCellSize/2;
    int showPatchWidth = 8*defCellSize+defCellGap;
    
    int showFixtureHeight = defCellSize*2.5;
    int showFixtureWidth = 6*defCellSize;
    
    int totalHeightofFixtureDefs = (defCellGap*2)+(3*defCellSize);   // Total height of all previous fixture cards including defCellGap and page header. (allows prior cards to draw all channels with following cards to not draw over the top)

    ofFill();
    
    for (int i = 0; i < fixtures.size(); i++){
        ofSetColor(80, 80, 80);
        ofFill();
        ofSetLineWidth(0);
        ofDrawRectRounded(showPatchWidth+defCellSize, showFixturestartheight+totalHeightofFixtureDefs, showFixtureWidth, showFixtureHeight + (fixtures[i].channels.size()*showPatchHeight),defRounded);
        if (selectedFixDef == fixtures[i].fixtureID){
            ofSetColor(64, 181, 43);
            ofNoFill();
            ofSetLineWidth(4);
            ofDrawRectRounded(showPatchWidth+defCellSize, showFixturestartheight+totalHeightofFixtureDefs, showFixtureWidth, showFixtureHeight + (fixtures[i].channels.size()*showPatchHeight),defRounded);
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
        
        for (int u = 0; u < fixtures[i].channels.size(); u++){
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
            fixMain.drawString(fixtures[i].channels[u].purpose, showPatchWidth + (defCellGap*2) + defCellSize*3, showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*10.7)+(showPatchHeight*u) + defCellGap*1.7);
            
            if (clickLeft(showPatchWidth + defCellGap + defCellSize+(defCellSize*2), showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*10.7)+(showPatchHeight*u), showFixtureWidth-(defCellGap*2) - (defCellSize*2), showPatchHeight)){
                ptrType = 0;
                fieldPtr = &fixtures[i].channels[u].purpose;
                strInputHeading = "Channel #" + to_string(u) + " purpose";
                strInputObj.fieldInputState = 1;
                strInputObj.screenType = 0;
                strInputObj.current = fixtures[i].channels[u].purpose;
                strInputObj.maxCount = 18;
                strInputObj.textValue = fixtures[i].channels[u].purpose;
                selectedFixDef = fixtures[i].fixtureID;
                
            }
            
        }
        
        ofSetColor(255);
        fixMain.drawString("FixtureID #" + to_string(fixtures[i].fixtureID), showPatchWidth+defCellSize + defCellGap, showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*2));
        fixMain.drawString("Name:    " + fixtures[i].name, showPatchWidth+defCellSize + defCellGap, showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*4.7));
        fixMain.drawString("Channel Count:    " + to_string(fixtures[i].channels.size()), showPatchWidth+defCellSize + defCellGap, showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*7.7));
        if (fixtures[i].channels.size() > 0){
            miniButton(showPatchWidth+(defCellSize*4) + defCellGap, showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*6), "-");
            if (clickLeft(showPatchWidth+(defCellSize*4) + defCellGap, showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*6), defCellSize/2, defCellSize/2)){
                fixtures[i].channels.pop_back();
                selectedFixDef = fixtures[i].fixtureID;
            }
        }
        
        if (fixtures[i].channels.size() < 511){
            miniButton(showPatchWidth+(defCellSize*4) + (defCellGap*2) + (defCellSize/2), showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*6), "+");
            if (clickLeft(showPatchWidth+(defCellSize*4) + (defCellGap*2) + (defCellSize/2), showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*6), defCellSize/2, defCellSize/2)){
                fixtures[i].channels.push_back(dmxChannelObj);
                selectedFixDef = fixtures[i].fixtureID;
            }
        }
        
        fixMain.drawString("Channel", showPatchWidth+defCellSize + defCellGap, showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*10));
        fixMain.drawString("Purpose", showPatchWidth+(defCellSize*3) + defCellGap, showFixturestartheight+totalHeightofFixtureDefs+(defCellGap*10));
        
        
        
        
        
        if (clickLeft(showPatchWidth+defCellSize + defCellGap + defCellSize, showFixturestartheight+totalHeightofFixtureDefs + (defCellGap*2.7), showFixtureWidth-defCellSize - defCellGap*2, showPatchHeight)){
            ptrType = 0;
            fieldPtr = &fixtures[i].name;
            strInputHeading = "Fixture #" + to_string(fixtures[i].fixtureID) + " name";
            strInputObj.fieldInputState = 1;
            strInputObj.screenType = 0;
            strInputObj.current = fixtures[i].name;
            strInputObj.maxCount = 24;
            strInputObj.textValue = fixtures[i].name;
            selectedFixDef = fixtures[i].fixtureID;
        }
        
        
        if (clickLeft(showPatchWidth+defCellSize, showFixturestartheight+totalHeightofFixtureDefs, showFixtureWidth, showFixtureHeight + (fixtures[0].channels.size()*showPatchHeight))){
            selectedFixDef = fixtures[i].fixtureID;
        }
        
        totalHeightofFixtureDefs = totalHeightofFixtureDefs + defCellGap + (fixtures[i].channels.size() * showPatchHeight) + showFixtureHeight;
    }

    
    for (int i = 0; i < dmxPatch.size(); i++){  // Draw Patches
        ofSetColor(30, 30, 30);
        ofFill();
        ofDrawRectangle(0, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), showPatchWidth, showPatchHeight);
        int patchPassScore = 0; // if pass score reaches 4 then the patch may be verified and used in contribution to DMX channel output
        dmxPatch[i].verified = false;
        ofFill();
        
//        if (dmxPatch[i].fixtureID != 1){  // needs work
//            ofSetColor(92, 19, 19);
//            ofDrawRectangle(defCellGap+defCellSize, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellSize*2, showPatchHeight);
//        } else {
//            patchPassScore++;
//        }
        
        int patchFixID = dmxPatch[i].fixtureID;
        bool patchFixIDFound = false;
        for (int m = 0; m < fixtures.size(); m++){
            if (patchFixID == fixtures[m].fixtureID){
                patchFixIDFound = true;
            }
        }
        
        if (patchFixIDFound){
            patchPassScore++;
        }
        
        
        
        
        if (dmxPatch[i].universe != 1){
            ofSetColor(92, 19, 19);
            ofDrawRectangle(defCellGap+defCellSize*2, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellSize*2, showPatchHeight);
        } else {
            patchPassScore++;
        }
        
        if (dmxPatch[i].channel < 1 || dmxPatch[i].channel > 512){
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
            if (patchSelect == &dmxPatch[i].fixtureID){
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
        if (clickLeft(defCellGap, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellSize*2, showPatchHeight)){
            patchSelect = &dmxPatch[i].fixtureID;
            strInputHeading = "fixtureID";
            strInputObj.fieldInputState = 1;
            strInputObj.screenType = 1;
            strInputObj.current = to_string(dmxPatch[i].fixtureID);
            strInputObj.maxCount = 3;
            strInputObj.textValue = to_string(dmxPatch[i].fixtureID);
            ptrType = 1;
            fieldPtrInt = &dmxPatch[i].fixtureID;
            lastFlash = ofGetElapsedTimeMillis();
        } else if (clickLeft(defCellGap+defCellSize*2, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellSize*2, showPatchHeight)){
            patchSelect = &dmxPatch[i].universe;
            strInputHeading = "universe";
            strInputObj.fieldInputState = 1;
            strInputObj.screenType = 1;
            strInputObj.current = to_string(dmxPatch[i].universe);
            strInputObj.maxCount = 3;
            strInputObj.textValue = to_string(dmxPatch[i].universe);
            ptrType = 1;
            fieldPtrInt = &dmxPatch[i].universe;
            lastFlash = ofGetElapsedTimeMillis();
        } else if (clickLeft(defCellGap+defCellSize*4, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellSize*2, showPatchHeight)){
            patchSelect = &dmxPatch[i].channel;
            strInputHeading = "channel";
            strInputObj.fieldInputState = 1;
            strInputObj.screenType = 1;
            strInputObj.current = to_string(dmxPatch[i].channel);
            strInputObj.maxCount = 3;
            strInputObj.textValue = to_string(dmxPatch[i].channel);
            ptrType = 1;
            fieldPtrInt = &dmxPatch[i].channel;
            lastFlash = ofGetElapsedTimeMillis();
        } else if (clickLeft(defCellGap+defCellSize*6, showPatchstartheight+(i*showPatchHeight)+defCellGap+(3*defCellSize), defCellSize*2, showPatchHeight)){
            patchSelect = &dmxPatch[i].quantity;
            strInputHeading = "quantity";
            strInputObj.fieldInputState = 1;
            strInputObj.screenType = 1;
            strInputObj.current = to_string(dmxPatch[i].quantity);
            strInputObj.maxCount = 3;
            strInputObj.textValue = to_string(dmxPatch[i].quantity);
            ptrType = 1;
            fieldPtrInt = &dmxPatch[i].quantity;
            lastFlash = ofGetElapsedTimeMillis();
        }
        fixMain.drawString(to_string(dmxPatch[i].channel), defCellGap*2+defCellSize*0.7+defCellSize*4, showPatchstartheight+(i*showPatchHeight)+defCellGap*2.8+(3*defCellSize));
        fixMain.drawString(to_string(dmxPatch[i].quantity), defCellGap*2+defCellSize*0.7+defCellSize*6, showPatchstartheight+(i*showPatchHeight)+defCellGap*2.8+(3*defCellSize));
        
        if (patchPassScore == 4){
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
    
    
    
    if (mouseY < defCellGap + defCellSize*3){
        level = 1;
    }

    ofFill();
    ofSetColor(80, 80, 80);
    ofDrawRectangle(0, 0, ofGetWidth(), defCellGap + defCellSize*3);
    
    ofNoFill();
    ofSetColor(255);
    ofSetLineWidth(2);
    ofSetColor(255);
    pageMain.drawString("Fixtures & Patching", 25, 75);
    pageSub.drawString("Edit definitions and patch to DMX channels", 25, 125);
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
    
    //miniButton(defCellGap*2 + defCellSize*2, defCellSize*3-defCellGap*2, "+");  // ADD patch
    //miniButton(defCellGap*2 + defCellSize*2+defCellGap+ defCellSize/2, defCellSize*3-defCellGap*2, "-");    // Remove patch
    
    
    
    crossButton(ofGetWidth()-defCellGap-defCellSize, defCellGap, 0, true, true);
    
    ofSetColor(255, 255, 255,100);  // fixtures button
    ofSetLineWidth(0);
    ofDrawRectangle(ofGetWidth()-(defCellGap*2)-(defCellSize*3)-defCellSize, defCellGap, defCellSize*3, defCellSize);
    
    ofSetColor(255, 255, 255, 255);
    ofNoFill();
    ofSetLineWidth(1);
    ofDrawRectangle(ofGetWidth()-(defCellGap*2)-(defCellSize*3)-defCellSize, defCellGap, defCellSize*3, defCellSize);
    panelType.drawString("Showfiles", ofGetWidth()-(defCellGap*2)-(defCellSize*3)-(defCellSize*0.5), defCellGap*4);
    if (clickLeft(ofGetWidth()-(defCellGap*2)-(defCellSize*3)-defCellSize, defCellGap, defCellSize*3, defCellSize)){
        screen = 1;
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
        }
        
        // Buttons for each card
        ofNoFill();
        ofSetColor(255);
        ofSetLineWidth(2);
        
        if (showsList[i].freshStart == false){  // Showfiles found within the defined directory
            
            for (int u = 0; u < showFileButtons.size(); u++){
                string text = showFileButtons[u];
                fixMain.drawString(text, showCardWidth - defCellSize - (defCellGap/2) - ((u*defCellSize)+defCellGap*u), showCardstartheight+(i*showCardHeight)+(defCellGap)+(i*defCellGap)+(showCardHeight-defCellSize+defCellGap));
                ofDrawRectangle(showCardWidth - defCellSize - defCellGap - ((u*defCellSize)+defCellGap*u), showCardstartheight+(i*showCardHeight)+(defCellGap)+(i*defCellGap)+(showCardHeight-defCellSize-defCellGap), defCellSize, defCellSize);
                
                if (clickLeft(showCardWidth - defCellSize - defCellGap - ((u*defCellSize)+defCellGap*u), showCardstartheight+(i*showCardHeight)+(defCellGap)+(i*defCellGap)+(showCardHeight-defCellSize-defCellGap), defCellSize, defCellSize)){
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
                        showBlock = true;
                        blockMsg = "The developer has removed Load\nfunctionality in this build.";
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
        } else {    // Fresh default showfile
        
            fixMain.drawString("save", showCardWidth - defCellSize - (defCellGap/2) - ((0*defCellSize)+defCellGap*0), showCardstartheight+(i*showCardHeight)+(defCellGap)+(i*defCellGap)+(showCardHeight-defCellSize+defCellGap));
            ofDrawRectangle(showCardWidth - defCellSize - defCellGap - ((0*defCellSize)+defCellGap*0), showCardstartheight+(i*showCardHeight)+(defCellGap)+(i*defCellGap)+(showCardHeight-defCellSize-defCellGap), defCellSize, defCellSize);
            if (clickLeft(showCardWidth - defCellSize - defCellGap - ((0*defCellSize)+defCellGap*0), showCardstartheight+(i*showCardHeight)+(defCellGap)+(i*defCellGap)+(showCardHeight-defCellSize-defCellGap), defCellSize, defCellSize) && level == 0){
               
                strInputHeading = "Save";
                strInputObj.fieldInputState = 1;
                strInputObj.screenType = 0;
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
    pageMain.drawString("Showfiles", 25, 75);
    pageSub.drawString("Save, Delete and configure showfiles", 25, 125);
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
            screen = 0;
            lastInteraction = ofGetElapsedTimeMillis();
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
            lastInteraction = ofGetElapsedTimeMillis();
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
            lastInteraction = ofGetElapsedTimeMillis();
            showCardstartheight = showCardstartheight - defCellSize;
        }
    }
    
    ofSetColor(255);
    fixText.drawString("up", defCellGap*2+showCardWidth+((defCellSize/2)/2), 3*defCellSize+(defCellGap*2) + ((ofGetHeight()-(3*defCellSize+defCellGap)-(defCellGap*4)-defCellGap/2)/2-defCellGap/2)/2);
    fixText.drawString("dn", defCellGap*2+showCardWidth+((defCellSize/2)/2), 3*defCellSize+(defCellGap*2) + ((ofGetHeight()-(3*defCellSize+defCellGap)-(defCellGap*4)-defCellGap/2)/2-defCellGap/2)/2 + ((ofGetHeight()-(3*defCellSize+defCellGap)-(defCellGap*4)-defCellGap/2)/2-defCellGap/2 + defCellGap));
    
}

void ofApp::genShowFileStr(){       // Parser - Generates string data representing showfile
    //  output colPanel string test
    
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


void ofApp::testFunction(){
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



void ofApp::drawPanel(){
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
                    lastInteraction = ofGetElapsedTimeMillis();
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

void ofApp::addPanelOverlay(){
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
//    ofSetColor(255, 0, 0);
//    ofDrawRectangle(ofGetWidth()/2 +(((ofGetWidth()/2)-(defCellSize*4)-(defCellGap*3))-defCellSize)/2, (ofGetHeight()/2)-(defCellSize*2)-(defCellGap*3)-defCellGap, defCellSize/2, -defCellSize/2);
//    fixText.drawString("EXT", ofGetWidth()/2 +(((ofGetWidth()/2)-(defCellSize*4)-(defCellGap*3))-defCellSize)/2+(defCellGap), (ofGetHeight()/2)-(defCellSize*2)-(defCellGap*3)-(defCellGap*2));
    
    crossButton(ofGetWidth()/2 +(((ofGetWidth()/2)-(defCellSize*4)-(defCellGap*3))-defCellSize)/2-defCellGap*2, (ofGetHeight()/2)-(defCellSize*2)-(defCellGap*3)-defCellGap*3, 0, false, false);
    
    ofSetLineWidth(2);
    ofSetColor(255, 255, 255);
    
    int theX = 0;
    int theY = 0;
    
//    if (clickLeft(ofGetWidth()/2 +(((ofGetWidth()/2)-(defCellSize*4)-(defCellGap*3))-defCellSize)/2, (ofGetHeight()/2)-(defCellSize*2)-(defCellGap*3)-defCellGap-(defCellSize/2), defCellSize, defCellSize)){
//        addPanelStage = 0;
//        overlay = 0;
//    }
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

void ofApp::controlPanel(int i){    // function responsible for drawing the control panel function.
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
                    fixText.drawString(cpActions[indexAction],x+12 + (defCellSize*c),y+36 + (defCellSize*r));
                    ofNoFill();
                    ofSetColor(252, 186, 3);
                    ofSetLineWidth(2);
                    ofDrawRectangle(x+(defCellGap/2) + (defCellSize*c), y+(defCellGap/2) + (defCellSize*r), defCellSize-(defCellGap), defCellSize-(defCellGap));
                    if (overlay == 0 && waitTime(defWaitTime)){
                        if (clickLeft(x+(defCellGap/2) + (defCellSize*c), y+(defCellGap/2) + (defCellSize*r), defCellSize-(defCellGap), defCellSize-(defCellGap))){
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
                               
                            }
                            if (cpActions[indexAction] == "panels"){
                                addPanelStage = 1;
                            }
                        }
                    }
                }
            }
            indexAction++;
        }
    }
//    if (clickLeft(x, y, 30, 30)){
//        cout << "new fixture" << endl;
//        //addFixture("cobra", true, 0, 0, 0, 0, 0, 0, 0);
//        testFunction();
//
//    }
}


void ofApp::briMixer(int i){  // brightness mixer.
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
        }
        ofDrawRectRounded(x + defSpace + 3 + (defCellSize * f), y + defCellSize + (defCellSize/2) + hi-defCellSize*2 -3, wi - (defSpace * 2)-6,ofMap(mouseBrightness, 0, 255, 0, ((hi-defCellSize*2)-6)*-1),r);  //  draw mapped value
        ofSetColor(255, 255, 255);
        ofDrawRectRounded(x + defSpace + (defCellSize * f), y + defCellSize, wi - (defSpace * 2), (defCellSize/2)-10,r);  // MAX
        ofSetColor(0, 0, 0);
        fixText.drawString("MAX", x + defSpace + (wi/2)-20 + (defCellSize * f), y + defCellSize+ 20);
        if (mouseX > x + defSpace + (defCellSize * f) && mouseX < x + defSpace +  wi - (defSpace * 2) + (defCellSize * f) && mouseY > y + defCellSize && mouseY < y + defCellSize + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0 && waitTime(defWaitTime)){  // check mouse position for MAX button
            mouseBrightness = 255;
        }
        ofSetColor(255, 255, 255);
        ofDrawRectRounded(x + defSpace + (defCellSize * f), y + hi - (defCellSize/2) + 4, wi - (defSpace * 2), (defCellSize/2)-10,r);  // MIN
        ofSetColor(0, 0, 0);
        fixText.drawString("MIN", x + defSpace + (wi/2)-20 + (defCellSize * f),  y + hi - (defCellSize/2) + 20);
        if (mouseX > x + defSpace + (defCellSize * f) && mouseX < x + defSpace +  wi - (defSpace * 2) + (defCellSize * f) && mouseY > y + hi - (defCellSize/2) + 4 && mouseY <  y + hi - (defCellSize/2) + 4 + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0 && waitTime(defWaitTime)){   // check mouse position for MIN button
            mouseBrightness = 0;
        }

        ofSetColor(255, 255, 255);
        fixText.drawString(to_string(mouseBrightness), x + (wi/2) - 15, y+42);   //  Value in text

        ofFill();
        ofSetColor(190, 190, 190);

    }
    ofFill();
    
}


void ofApp::rgbMixer(int i){       // RGB mixer
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
            }
            ofDrawRectRounded(x + defSpace + 3 + ((wi/3) * f), y + (wi/3) + ((wi/3)/2) + hi-(wi/3)*2 -3, (wi/3) - (defSpace * 2)-6,ofMap(mouseColors[f], 0, 255, 0, ((hi-(wi/3)*2)-6)*-1),r);  //  draw mapped value
            ofSetColor(255, 255, 255);
            ofDrawRectRounded(x + defSpace + ((wi/3) * f), y + (wi/3), (wi/3) - (defSpace * 2), (defCellSize/2)-10,r);  // MAX
            ofSetColor(0, 0, 0);
            fixText.drawString("MAX", x + defSpace + ((wi/3)/2)-20 + ((wi/3) * f), y + (wi/3)+ 20);
            if (mouseX > x + defSpace + ((wi/3) * f) && mouseX < x + defSpace +  (wi/3) - (defSpace * 2) + ((wi/3) * f) && mouseY > y + (wi/3) && mouseY < y + (wi/3) + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0 && waitTime(defWaitTime)){
                mouseColors[f] = 255;
            }
            ofSetColor(255, 255, 255);
            ofDrawRectRounded(x + defSpace + ((wi/3) * f), y + hi - (defCellSize/2) + 4, (wi/3) - (defSpace * 2), (defCellSize/2)-10,r);  // MIN
            ofSetColor(0, 0, 0);
            fixText.drawString("MIN", x + defSpace + ((wi/3)/2)-20 + ((wi/3) * f),  y + hi - (defCellSize/2) + 20);
            if (mouseX > x + defSpace + ((wi/3) * f) && mouseX < x + defSpace +  (wi/3) - (defSpace * 2) + ((wi/3) * f) && mouseY > y + hi - (defCellSize/2) + 4 && mouseY <  y + hi - (defCellSize/2) + 4 + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0 && waitTime(defWaitTime)){
                mouseColors[f] = 0;
            }

            ofSetColor(255, 255, 255);
            fixText.drawString(to_string(mouseColors[f]), x + defSpace+(wi/3)-((wi/3)/2)-15 + (wi/3 * f), y+42);   //  Value in text
            }

   
        ofFill();
        ofSetColor(190, 190, 190);
        
    }
    ofFill();
    
}


void ofApp::wauv(int i){       // White Amber UV fader mixer panel.
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
                    }
                    ofDrawRectRounded(x + defSpace + 3 + ((wi/3) * f), y + (wi/3) + ((wi/3)/2) + hi-(wi/3)*2 -3, (wi/3) - (defSpace * 2)-6,ofMap(mouseColors[3+f], 0, 255, 0, ((hi-(wi/3)*2)-6)*-1),r);  //  draw mapped value
                    ofSetColor(255, 255, 255);
                    ofDrawRectRounded(x + defSpace + ((wi/3) * f), y + (wi/3), (wi/3) - (defSpace * 2), (defCellSize/2)-10,r);  // MAX
                    ofSetColor(0, 0, 0);
                    fixText.drawString("MAX", x + defSpace + ((wi/3)/2)-20 + ((wi/3) * f), y + (wi/3)+ 20);
                    if (mouseX > x + defSpace + ((wi/3) * f) && mouseX < x + defSpace +  (wi/3) - (defSpace * 2) + ((wi/3) * f) && mouseY > y + (wi/3) && mouseY < y + (wi/3) + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0 && waitTime(defWaitTime)){
                        mouseColors[3+f] = 255;
                    }
                    ofSetColor(255, 255, 255);
                    ofDrawRectRounded(x + defSpace + ((wi/3) * f), y + hi - (defCellSize/2) + 4, (wi/3) - (defSpace * 2), (defCellSize/2)-10,r);  // MIN
                    ofSetColor(0, 0, 0);
                    fixText.drawString("MIN", x + defSpace + ((wi/3)/2)-20 + ((wi/3) * f),  y + hi - (defCellSize/2) + 20);
                    if (mouseX > x + defSpace + ((wi/3) * f) && mouseX < x + defSpace +  (wi/3) - (defSpace * 2) + ((wi/3) * f) && mouseY > y + hi - (defCellSize/2) + 4 && mouseY <  y + hi - (defCellSize/2) + 4 + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0 && waitTime(defWaitTime)){
                        mouseColors[3+f] = 0;
                    }

                    ofSetColor(255, 255, 255);
                    fixText.drawString(to_string(mouseColors[3+f]), x + defSpace+(wi/3)-((wi/3)/2)-15 + (wi/3 * f), y+42);   //  Value in text
                    }
        ofFill();
        ofSetColor(190, 190, 190);
    }
    ofFill();
}

void ofApp::posMixer(int i){       // White Amber UV fader mixer panel.
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
                    }
                    ofDrawRectRounded(x + defSpace + 3 + ((wi/2) * f), y + (wi/2) + ((wi/2)/2) + hi-(wi/2)*2 -3, (wi/2) - (defSpace * 2)-6,ofMap(mousePosition[f], 0, 255, 0, ((hi-(wi/2)*2)-6)*-1),r);  //  draw mapped value
                    ofSetColor(255, 255, 255);
                    ofDrawRectRounded(x + defSpace + ((wi/2) * f), y + (wi/2), (wi/2) - (defSpace * 2), (defCellSize/2)-10,r);  // MAX
                    ofSetColor(0, 0, 0);
                    fixText.drawString("MAX", x + defSpace + ((wi/2)/2)-20 + ((wi/2) * f), y + (wi/2)+ 20);
                    if (mouseX > x + defSpace + ((wi/2) * f) && mouseX < x + defSpace +  (wi/2) - (defSpace * 2) + ((wi/2) * f) && mouseY > y + (wi/2) && mouseY < y + (wi/2) + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0 && waitTime(defWaitTime)){
                        mousePosition[f] = 255;
                    }
                    ofSetColor(255, 255, 255);
                    ofDrawRectRounded(x + defSpace + ((wi/2) * f), y + hi - (defCellSize/2) + 4, (wi/2) - (defSpace * 2), (defCellSize/2)-10,r);  // MIN
                    ofSetColor(0, 0, 0);
                    fixText.drawString("MIN", x + defSpace + ((wi/2)/2)-20 + ((wi/2) * f),  y + hi - (defCellSize/2) + 20);
                    if (mouseX > x + defSpace + ((wi/2) * f) && mouseX < x + defSpace +  (wi/2) - (defSpace * 2) + ((wi/2) * f) && mouseY > y + hi - (defCellSize/2) + 4 && mouseY <  y + hi - (defCellSize/2) + 4 + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0 && waitTime(defWaitTime)){
                        mousePosition[f] = 0;
                    }

                    ofSetColor(255, 255, 255);
                    fixText.drawString(to_string(mousePosition[f]), x + defSpace+(wi/2)-((wi/2)/2)-15 + (wi/2 * f), y+42);   //  Value in text
                    }
        ofFill();
        ofSetColor(190, 190, 190);
        
    }
    ofFill();
    
}

void ofApp::colorsPanel(int i){     // panel for storing / defining colours
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
                                strInputObj.maxCount = 6;
                                strInputObj.current = panels[i].savedColors[getCellByIden].name;
                                strInputObj.textValue = panels[i].savedColors[getCellByIden].name;
                                ptrType = 0;
                                fieldPtr = &panels[i].savedColors[getCellByIden].name;
                    
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
                    panelType.drawString("Colors",x+ 5 + (c * defCellSize),y + 17 + (r*defCellSize));
                    colorsIcon.draw(x+12 + (c * defCellSize),y+15 + (r*defCellSize),iconSize,iconSize);
                    ofNoFill();
                    ofSetLineWidth(2);
                    //ofDrawRectangle(x, y, defCellSize, defCellSize);
                    getCellByIden++;
                }
            }
        }
}


void ofApp::brightnessPanel(int i){
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
                            if (mode == "store"){
                                mode = "";
                                panels[i].savedBrightness[getCellByIden].value = mouseBrightness;
                                panels[i].savedBrightness[getCellByIden].set = true;
                            } else if (mode == "delete") {
                                mode = "";
                                panels[i].savedBrightness[getCellByIden].value = 255;
                                panels[i].savedBrightness[getCellByIden].set = false;
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
                    panelType.drawString("Bright",x+ 5 + (c * defCellSize),y + 17 + (r*defCellSize));
                    colorsIcon.draw(x+12 + (c * defCellSize),y+15 + (r*defCellSize),iconSize,iconSize);
                    ofNoFill();
                    ofSetLineWidth(2);
                    //ofDrawRectangle(x, y, defCellSize, defCellSize);
                    getCellByIden++;
                }
            }
        }
}

void ofApp::posPanel(int i){
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
                    panelType.drawString("Positn",x+ 5 + (c * defCellSize),y + 17 + (r*defCellSize));
                    colorsIcon.draw(x+12 + (c * defCellSize),y+15 + (r*defCellSize),iconSize,iconSize);
                    ofNoFill();
                    ofSetLineWidth(2);
                    //ofDrawRectangle(x, y, defCellSize, defCellSize);
                    getCellByIden++;
                }
            }
        }
}



void ofApp::drawGrid(){     // needs work --> check main.cpp comment
    ofSetLineWidth(1);
    ofSetColor(90, 90, 90);
    for (int i = 0; i <= floor((ofGetWindowWidth() - (defCellGap * 2))/ defCellSize); i++){
        int x = defCellSize * i + defCellGap;
        ofDrawLine(x, defCellGap, x, floor(((ofGetWindowHeight())/70)*defCellSize)+defCellGap);
    }

    for (int i = 0; i <= floor((ofGetWindowHeight() - (defCellGap * 2))/ defCellSize); i++){
        int y = defCellSize * i + defCellGap;
        ofDrawLine(defCellGap, y, floor(((ofGetWindowWidth())/70)*defCellSize)+defCellGap, y);
    }
}


bool ofApp::clickLeft(int x, int y, int w, int h){

    if (ofGetMouseX() > x && ofGetMouseX() < x + w && ofGetMouseY() > y && ofGetMouseY() < y + h && mouseExe == 1){
        
        mouseExe = 0;  //reset mouse so no endless execution occurs.
        return true;
    } else {
        return false;
    };
}

bool ofApp::pressLeft(int x, int y, int w, int h){

    if (ofGetMouseX() > x && ofGetMouseX() < x + w && ofGetMouseY() > y && ofGetMouseY() < y + h && mousePExe == 1){
        return true;
    } else {
        return false;
    };
}

void ofApp::addFixture(string name, bool simShow, int x, int y, int w, int h, int universe, int address){
    
    fixtures.push_back(fixObj);
    fixtures[fixtures.size()-1].name = "cobra";
    fixtures[fixtures.size()-1].simShow = true;
    fixtures[fixtures.size()-1].x = rand() % int((ofGetWindowWidth()-(defCellGap*2))/defCellSize);
    fixtures[fixtures.size()-1].y = rand() % int((ofGetWindowHeight()-(defCellGap*2))/defCellSize);
    fixtures[fixtures.size()-1].w = defCellSize;
    fixtures[fixtures.size()-1].h = defCellSize;
    fixtures[fixtures.size()-1].universe = 1;
    fixtures[fixtures.size()-1].address = fixtures.size() * 9 + 1;
    
}

void ofApp::addFixButton(){
    int x = ofGetWindowWidth() - 80;
    int y = ofGetWindowHeight() - 80;
    
    ofSetColor(70, 70, 70);
    ofFill();
    ofDrawRectangle(x, y, 30, 30);
    ofSetColor(255, 255, 255);
    fixMain.drawString("+",x+8,y+24);
    ofNoFill();
    ofSetColor(252, 186, 3);
    ofSetLineWidth(2);
    ofDrawRectangle(x, y, 30, 30);
    
    if (clickLeft(x, y, 30, 30)){
        //cout << "new fixture" << endl;
        //addFixture("cobra", true, 0, 0, 0, 0, 0, 0, 0);
        testFunction();
    }
}

void ofApp::fixpatchgroup(int i){
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
    panelType.drawString("Patch Fixture Groups", x + defCellSize/2, y+defCellGap*2);

    
    vector<int> knownVerified = {};
    
    for (int u = 0; u < dmxPatch.size(); u++){
        if (dmxPatch[u].verified){
            knownVerified.push_back(u);
        }
    }
    
    
    
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
    
        for (int p = 0; p < knownVerified.size(); p++){
            ofSetColor(255);
            panelName.drawString(to_string(dmxPatch[p].universe), x + defCellGap, y+defCellSize/2+defCellGap*2.5+(defCellGap*p*1.7));
            panelName.drawString(to_string(dmxPatch[p].channel), x + defCellSize/2 + defCellGap*1.5, y+defCellSize/2+defCellGap*2.5+(defCellGap*p*1.7));
            panelName.drawString(to_string(dmxPatch[p].quantity), x + defCellSize + defCellGap*1.5, y+defCellSize/2+defCellGap*2.5+(defCellGap*p*1.7));
            panelName.drawString(fixtures[dmxPatch[p].fixtureID].name, x + defCellSize*1.5 + defCellGap*1.5, y+defCellSize/2+defCellGap*2.5+(defCellGap*p*1.7));
        }
        
    
    } else {
        ofSetColor(80);
        panelType.drawString("No suitable\nPatches found", x+defCellSize, y+hi/2-defCellSize/2+defCellGap);
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
        ofFill();
        ofSetColor(22, 22, 22);
        ofDrawRectRounded(x, y, wi, hi,r);
        ofSetColor(255, 255, 255);
        panelName.drawString(name, x + 30, y+10);
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
    }
}


void ofApp::simulatedFixture(int fixAddress){
    ofFill();
    int x = gX(fixtures[fixAddress].x);
    int y = gY(fixtures[fixAddress].y);
    int w = fixtures[fixAddress].w;
    int h = fixtures[fixAddress].h;
    ofDrawRectangle(x, y, w, h);
    ofSetColor(255, 255, 255);
    ofFill();
    ofSetColor(70,70,70);
    ofDrawRectangle(x, y+h - 20, w, 20);
    ofNoFill();
    ofSetColor(255, 255, 255);
    ofSetLineWidth(1);
    ofDrawRectangle(x, y + h - 20, w, 20);
    ofSetColor(255, 255, 255);
    fixText.drawString(to_string(fixtures[fixAddress].address),x+ 30,y+70 - 5);
    ofNoFill();
    ofSetLineWidth(2);
    ofDrawRectangle(x, y, w, h);
    if (clickLeft(x, y, 70,70)){  // check if fixture is clicked
        cout << "clicked!"<< endl;
    }
}

void ofApp::loadIconsFonts(){
    
    // fonts
    
    vector<int> fontSize = {};  // scale fonts to their correct proportions
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
    
    // icons / images
    colorsIcon.load("panel_icons/colors.png");
}

//--------------------------------------------------------------
void ofApp::setup(){
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
    knownChannels[knownChannels.size()-1].desc = "Fixture Pan channel for Position Mixer";
    knownChannels.push_back(knownChannelObj);
    knownChannels[knownChannels.size()-1].purpose = "tilt";
    knownChannels[knownChannels.size()-1].desc = "Fixture Tilt channel for Position Mixer";
    knownChannels.push_back(knownChannelObj);
    knownChannels[knownChannels.size()-1].purpose = "dimmer";
    knownChannels[knownChannels.size()-1].desc = "Fixture Dimmer channel for Brightness Mixer";
    knownChannels.push_back(knownChannelObj);
    knownChannels[knownChannels.size()-1].purpose = "red";
    knownChannels[knownChannels.size()-1].desc = "Fixture RED channel for RGB Mixer";
    knownChannels.push_back(knownChannelObj);
    knownChannels[knownChannels.size()-1].purpose = "green";
    knownChannels[knownChannels.size()-1].desc = "Fixture GREEN channel for RGB Mixer";
    knownChannels.push_back(knownChannelObj);
    knownChannels[knownChannels.size()-1].purpose = "blue";
    knownChannels[knownChannels.size()-1].desc = "Fixture BLUE channel for TGB Mixer";
    knownChannels.push_back(knownChannelObj);
    knownChannels[knownChannels.size()-1].purpose = "white";
    knownChannels[knownChannels.size()-1].desc = "Fixture WHITE channel for WAUV Mixer";
    knownChannels.push_back(knownChannelObj);
    knownChannels[knownChannels.size()-1].purpose = "amber";
    knownChannels[knownChannels.size()-1].desc = "Fixture AMBER channel for WAUV Mixer";
    knownChannels.push_back(knownChannelObj);
    knownChannels[knownChannels.size()-1].purpose = "uv";
    knownChannels[knownChannels.size()-1].desc = "Fixture UV channel for WAUV Mixer";
    
    
    
    
    dmxPatch.push_back(dmxPatchObj);
    dmxPatch[dmxPatch.size()-1].channel = 1;
    dmxPatch[dmxPatch.size()-1].fixtureID = 0;
    dmxPatch[dmxPatch.size()-1].quantity = 1;
    dmxPatch[dmxPatch.size()-1].universe = 1;
    dmxPatch.push_back(dmxPatchObj);
    dmxPatch.push_back(dmxPatchObj);
    
    fixtures.push_back(fixObj);
    fixtures[fixtures.size()-1].name = "AVE Cobra Wash 100";
    fixtures[fixtures.size()-1].simShow = true;
    fixtures[fixtures.size()-1].fixtureID = fixtures.size()-1;
    fixtures[fixtures.size()-1].x = 200;
    fixtures[fixtures.size()-1].y = 200;
    fixtures[fixtures.size()-1].w = 100;
    fixtures[fixtures.size()-1].h = 100;
    fixtures[fixtures.size()-1].universe = 1;
    fixtures[fixtures.size()-1].address = 1;
    fixtures[fixtures.size()-1].channels.push_back(dmxChannelObj);
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].purpose = "pan";
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].value = 0;
    fixtures[fixtures.size()-1].channels.push_back(dmxChannelObj);
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].purpose = "tilt";
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].value = 0;
    fixtures[fixtures.size()-1].channels.push_back(dmxChannelObj);
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].purpose = "dimmer";
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].value = 0;
    fixtures[fixtures.size()-1].channels.push_back(dmxChannelObj);
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].purpose = "red";
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].value = 0;
    fixtures[fixtures.size()-1].channels.push_back(dmxChannelObj);
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].purpose = "green";
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].value = 0;
    fixtures[fixtures.size()-1].channels.push_back(dmxChannelObj);
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].purpose = "blue";
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].value = 0;
    fixtures[fixtures.size()-1].channels.push_back(dmxChannelObj);
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].purpose = "white";
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].value = 0;
    fixtures[fixtures.size()-1].channels.push_back(dmxChannelObj);
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].purpose = "amber";
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].value = 0;
    fixtures[fixtures.size()-1].channels.push_back(dmxChannelObj);
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].purpose = "uv";
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].value = 0;
    
    fixtures.push_back(fixObj);
    fixtures[fixtures.size()-1].name = "Wowzers fixture";
    fixtures[fixtures.size()-1].simShow = true;
    fixtures[fixtures.size()-1].fixtureID = fixtures.size()-1;
    fixtures[fixtures.size()-1].x = 200;
    fixtures[fixtures.size()-1].y = 200;
    fixtures[fixtures.size()-1].w = 100;
    fixtures[fixtures.size()-1].h = 100;
    fixtures[fixtures.size()-1].universe = 1;
    fixtures[fixtures.size()-1].address = 1;
    fixtures[fixtures.size()-1].channels.push_back(dmxChannelObj);
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].purpose = "strobe";
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].value = 0;
    fixtures[fixtures.size()-1].channels.push_back(dmxChannelObj);
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].purpose = "smoke";
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].value = 0;
    fixtures[fixtures.size()-1].channels.push_back(dmxChannelObj);
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].purpose = "cool factor";
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].value = 0;
    fixtures[fixtures.size()-1].channels.push_back(dmxChannelObj);
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].purpose = "dedicatd purple";
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].value = 0;
    fixtures[fixtures.size()-1].channels.push_back(dmxChannelObj);
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].purpose = "beep";
    fixtures[fixtures.size()-1].channels[fixtures[fixtures.size()-1].channels.size()-1].value = 0;


    
    
    // prepare known panel types for user definition
    knownPanelType.push_back(knownPanelObj);
    knownPanelType[knownPanelType.size()-1].type = "fixsimulation";
    knownPanelType[knownPanelType.size()-1].name = "Fixture\nSimulation";
    knownPanelType[knownPanelType.size()-1].minWidth = 5;
    knownPanelType[knownPanelType.size()-1].minHeight = 4;
    knownPanelType[knownPanelType.size()-1].cellSize = 40;
    
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
    
    knownPanelType.push_back(knownPanelObj);
    knownPanelType[knownPanelType.size()-1].type = "fixpatchgroup";
    knownPanelType[knownPanelType.size()-1].name = "Fix Patch\nGroups";
    knownPanelType[knownPanelType.size()-1].minWidth = 3;
    knownPanelType[knownPanelType.size()-1].minHeight = 4;
    knownPanelType[knownPanelType.size()-1].cellSize = defCellSize;
    knownPanelType[knownPanelType.size()-1].defSpace = 5;
    
//    // pre-drawn Panels
//    panels.push_back(panelObj);
//    panels[panels.size()-1].type = "fixsimulation";
//    panels[panels.size()-1].name = "Fixture Simulation";
//    panels[panels.size()-1].x = 5;
//    panels[panels.size()-1].y = 1;
//    panels[panels.size()-1].wi = 13;
//    panels[panels.size()-1].hi = 2;
//    panels[panels.size()-1].cellSize = 40;
    
    
    panels.push_back(panelObj);
    panels[panels.size()-1].type = "controlpanel";
    panels[panels.size()-1].name = "Control Panel";
    panels[panels.size()-1].x = 24;
    panels[panels.size()-1].y = 8;
    panels[panels.size()-1].wi = 3;
    panels[panels.size()-1].hi = 5;
    panels[panels.size()-1].cellSize = 40;
    
    panels.push_back(panelObj);
    panels[panels.size()-1].type = "fixpatchgroup";
    panels[panels.size()-1].name = "Fix Patch\nGroups";
    panels[panels.size()-1].x = 12;
    panels[panels.size()-1].y = 5;
    panels[panels.size()-1].wi = 4;
    panels[panels.size()-1].hi = 7;
    panels[panels.size()-1].cellSize = defCellSize;
        
//    panels.push_back(panelObj);
//    panels[panels.size()-1].type = "rgbmixer";
//    panels[panels.size()-1].name = "Color mixer";
//    panels[panels.size()-1].x = 1;
//    panels[panels.size()-1].y = 1;
//    panels[panels.size()-1].wi = 3;
//    panels[panels.size()-1].hi = 4;
//    panels[panels.size()-1].cellSize = defCellSize;
//    panels[panels.size()-1].defSpace = 5;
//
//    panels.push_back(panelObj);
//    panels[panels.size()-1].type = "brimixer";
//    panels[panels.size()-1].name = "Brightness";
//    panels[panels.size()-1].x = 11;
//    panels[panels.size()-1].y = 5;
//    panels[panels.size()-1].wi = 2;
//    panels[panels.size()-1].hi = 6;
//    panels[panels.size()-1].cellSize = defCellSize;
//    panels[panels.size()-1].defSpace = 5;
//
//    panels.push_back(panelObj);
//    panels[panels.size()-1].type = "bristor";
//    panels[panels.size()-1].name = "Stored Brightness";
//    panels[panels.size()-1].x = 19;
//    panels[panels.size()-1].y = 2;
//    panels[panels.size()-1].wi = 6;
//    panels[panels.size()-1].hi = 2;
//    panels[panels.size()-1].cellSize = 40;
//
//    panels.push_back(panelObj);
//    panels[panels.size()-1].type = "colorstor";
//    panels[panels.size()-1].name = "Stored Brightness";
//    panels[panels.size()-1].x = 13;
//    panels[panels.size()-1].y = 4;
//    panels[panels.size()-1].wi = 5;
//    panels[panels.size()-1].hi = 6;
//    panels[panels.size()-1].cellSize = 40;
//
//    panels.push_back(panelObj);
//    panels[panels.size()-1].type = "wauv";
//    panels[panels.size()-1].name = "White Amber UV mixer";
//    panels[panels.size()-1].x = 20;
//    panels[panels.size()-1].y = 6 ;
//    panels[panels.size()-1].wi = 3;
//    panels[panels.size()-1].hi = 4;
//    panels[panels.size()-1].cellSize = defCellSize;
//    panels[panels.size()-1].defSpace = 5;
//
//    panels.push_back(panelObj);
//    panels[panels.size()-1].type = "posMixer";
//    panels[panels.size()-1].name = "Pan Tilt Mixer";
//    panels[panels.size()-1].x = 1;
//    panels[panels.size()-1].y = 6 ;
//    panels[panels.size()-1].wi = 2;
//    panels[panels.size()-1].hi = 4;
//    panels[panels.size()-1].cellSize = defCellSize;
//    panels[panels.size()-1].defSpace = 5;
//
//    panels.push_back(panelObj);
//    panels[panels.size()-1].type = "posStor";
//    panels[panels.size()-1].name = "Positn";
//    panels[panels.size()-1].x = 10;
//    panels[panels.size()-1].y = 11;
//    panels[panels.size()-1].wi = 9;
//    panels[panels.size()-1].hi = 3;
//    panels[panels.size()-1].cellSize = defCellSize;
//    panels[panels.size()-1].defSpace = 5;
//
//    fixtures.push_back(fixObj);
//    fixtures[0].name = "cobra";
//    fixtures[0].simShow = true;
//    fixtures[0].iden = 0;
//    fixtures[0].x = 6;
//    fixtures[0].y = 4;
//    fixtures[0].w = defCellSize;
//    fixtures[0].h = defCellSize;
//    fixtures[0].universe = 1;
//    fixtures[0].address = 1;
//    //$%$%% fixtures[0].channelData = {0,0,50,124,8,207,8,0,0};
//    //$%$%% fixtures[0].channels = {"pan", "tilt", "dimmer", "red", "green", "blue", "white", "amber", "uv"};    // purpose of each channel
//    fixtures[0].channelCount = fixtures[0].channels.size();
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
//    ofSetColor(20,20,20);
//    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    level = 0;
    overPanel = false;
    
    if (showGrid == true){
        drawGrid();
    }
    
    
//    for (int i = 0; i < panels.size(); i++){ //draw panels
//        uipanel(i);
//        simulatedFixture(0);
//        simulatedFixture(1);
//    }
    
//    uipanel(0);   //simulated fixture
//    uipanel(1);   //control panel
//    uipanel(2);   // color panel
//    uipanel(3);   // color panel
//    uipanel(4);   // color mixer
//    uipanel(5);   // color mixer
//    uipanel(6);
//    uipanel(7);
//    uipanel(8);
//    uipanel(9);
    
    if (screen == 0){
        strInputHeading = "Rename";
        
    
    
        
        
    if (panels.size() > 0){
        for (int i = 0; i < panels.size(); i++){
            uipanel(i);
        }
    }

    
//    simulatedFixture(0);
//    simulatedFixture(1);
    
    
    for (int i = 0; i < fixtures.size(); i++){
        simulatedFixture(i);
        
    }
    
    addFixButton();
    
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
    
    if (showBlock){
        blockFeature();
        if (ofGetElapsedTimeMillis() > lastInteraction + 2000){
            showBlock = false;
            overlay = 0;
            
        }
    }


    ofSetColor(255);
    debugText.drawString("Lucent Developer | " + to_string(ofGetMouseX()) + " " + to_string(ofGetMouseY()) + " | F" + to_string(fixtures.size()) + " | W" + to_string(ofGetWindowWidth()) + " H" + to_string(ofGetWindowHeight()) + " | GR" + to_string(int(floor(((ofGetMouseX()-defCellGap)/defCellSize)))) + " " + to_string(int(floor(((ofGetMouseY()-defCellGap)/defCellSize)))) + " | m" + to_string(level) + " " + mode + " | o" + to_string(overlay) + " | op " + to_string(overPanel), 20, ofGetWindowHeight() - 20); //Debug text
    mouseExe = 0;   //allows user to click on something, drag mouse away and stop execution of that function from occuring. (Like accidently clicking something)
    if (strInputObj.fieldInputState == 3){
        strInputObj.fieldInputState = 0;
    }
    
//    cout << fixtures[0].name;
//    cout << to_string(fixtures[0].address);

}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
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
void ofApp::mouseDragged(int x, int y, int button){
    if (button == 0){
        mousePExe = 1;
        mouseExe = 1;
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if (button == 0){
        mousePExe = 1;
    } else {
        mousePExe = 0;
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
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

