#include "ofApp.h"
#include <math.h>


vector<panelClass> panels;  //stores panel object
panelClass panelObj;
vector<dmxFixtureclass> fixtures;   //stores all dmxFixture objects
dmxFixtureclass fixObj;
fixColor colorObj;
storedBright brightObj;
storedPos posObj;
dmxChannel dmxChannelObj;   // DMX fixture channel object

knownPanel knownPanelObj;

strInputvalues strInputObj;


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

void ofApp::strInput(string current, int max){
    level = 3;
    overlay = 1;
    maxCharacterCount = max;
    ofSetColor(0, 0, 0,220);
    ofFill();
    ofDrawRectangle(0,0,ofGetWindowWidth(), ofGetWindowHeight());
    ofSetColor(171, 122, 50);
    ofDrawRectRounded(ofGetWindowWidth()/2-(defCellSize*5),ofGetWindowHeight()/2-(defCellSize/2),defCellSize*10, defCellSize,5);
    ofSetColor(255, 255, 255);
    ofSetLineWidth(2);
    ofNoFill();

    usrInput.drawString(strInputObj.textValue, ofGetWindowWidth()/2-(defCellSize*5) + defCellGap, ofGetWindowHeight()/2-(defCellSize/2) + (defCellGap * 3 + 5));
    panelName.drawString(to_string(strInputObj.textValue.size()) + " / " + to_string(max), ofGetWindowWidth()/2-(defCellSize*5), ofGetWindowHeight()/2+(defCellSize/2)+defCellGap);
    pageMain.drawString("Rename", ofGetWindowWidth()/2-(defCellSize*5), ofGetWindowHeight()/2-(defCellSize/2)-defCellGap);
}

void ofApp::showFileConfig(){
    ofFill();
    ofSetColor(80, 80, 80);
    ofDrawRectangle(0, 0, ofGetWidth(), defCellGap + defCellSize*2.5);
    ofSetColor(255, 255, 255);
    pageMain.drawString("Showfiles", 25, 75);
    pageSub.drawString("Save, Delete and configure showfiles", 25, 125);
    
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
            
            //  output colPanel string test
            
            // delimiter key: '^' = new panel, '$' new class attribute, '/' new (secondary) class attribute (savedData example storedBright), '|' new vector index, '*' no storedData, '`' blank textField
            
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
                        
                        for (int p = 0; p < panels[i].savedPositions[u].position.size(); p++){
                            savedPositions = savedPositions + to_string(panels[i].savedPositions[u].position[p]) + "|";
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
    }
    
    ofSetColor(255, 255, 255,100);  // fixtures button
    ofSetLineWidth(0);
    ofDrawRectangle(ofGetWidth()-(defCellGap*2)-(defCellSize*3)-defCellSize, defCellGap, defCellSize*3, defCellSize);
    ofSetColor(255, 255, 255, 255);
    ofNoFill();
    ofSetLineWidth(1);
    ofDrawRectangle(ofGetWidth()-(defCellGap*2)-(defCellSize*3)-defCellSize, defCellGap, defCellSize*3, defCellSize);
    panelType.drawString("Patching", ofGetWidth()-(defCellGap*2)-(defCellSize*3)-(defCellSize*0.5), defCellGap*4);
    
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
    ofDrawRectRounded(ofGetWidth()/2 - width/2, ofGetHeight()/2 - height/2, width, height, 5);
    ofNoFill();
    ofSetColor(255, 0, 0);
    ofDrawRectangle(ofGetWidth()/2 +(((ofGetWidth()/2)-(defCellSize*4)-(defCellGap*3))-defCellSize)/2, (ofGetHeight()/2)-(defCellSize*2)-(defCellGap*3)-defCellGap, defCellSize/2, -defCellSize/2);
    fixText.drawString("EXT", ofGetWidth()/2 +(((ofGetWidth()/2)-(defCellSize*4)-(defCellGap*3))-defCellSize)/2+(defCellGap), (ofGetHeight()/2)-(defCellSize*2)-(defCellGap*3)-(defCellGap*2));
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
                ofDrawRectRounded(ofGetWidth()/2 - width/2 + defCellGap + (buttonWidth * c) + (defCellGap * c), ofGetHeight()/2 - height/2 + defCellGap + (buttonHeight * r) + (defCellGap * r), buttonWidth, buttonHeight, 5);
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
    if (clickLeft(x, y, 30, 30)){
        cout << "new fixture" << endl;
        addFixture("cobra", true, 0, 0, 0, 0, 0, 0, 0);
    }
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
                                panels[i].savedColors[getCellByIden].name = "C #" + to_string(getCellByIden);
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
                                strInputObj.maxCount = 6;
                                strInputObj.current = panels[i].savedColors[getCellByIden].name;
                                strInputObj.textValue = panels[i].savedColors[getCellByIden].name;
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
                        ofDrawRectRounded(x + (c * defCellSize), y + (r*defCellSize), defCellSize, defCellSize,10);
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
                    ofDrawRectRounded(rectShape,r,10,10,10);
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
//    if (ofGetMouseX() > x && ofGetMouseX() < x + w && ofGetMouseY() > y && ofGetMouseY() < y + h){
//        cout << gamer << endl;
//    }
    if (ofGetMouseX() > x && ofGetMouseX() < x + w && ofGetMouseY() > y && ofGetMouseY() < y + h && mouseExe == 1){
        
        mouseExe = 0;  //reset mouse so no endless execution occurs.
        return true;
    } else {
        return false;
    };
}

void ofApp::addFixture(string name, bool simShow, int x, int y, int w, int h, int universe, int address, int channelCount){
    
    fixtures.push_back(fixObj);
    fixtures[fixtures.size()-1].name = "cobra";
    fixtures[fixtures.size()-1].simShow = true;
    fixtures[fixtures.size()-1].x = rand() % int((ofGetWindowWidth()-(defCellGap*2))/defCellSize);
    fixtures[fixtures.size()-1].y = rand() % int((ofGetWindowHeight()-(defCellGap*2))/defCellSize);
    fixtures[fixtures.size()-1].w = defCellSize;
    fixtures[fixtures.size()-1].h = defCellSize;
    fixtures[fixtures.size()-1].universe = 1;
    fixtures[fixtures.size()-1].channelCount = 9;
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
        cout << "new fixture" << endl;
        addFixture("cobra", true, 0, 0, 0, 0, 0, 0, 0);
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
    debugText.load("Lato-Regular.ttf", 15);
    panelName.load("Lato-Regular.ttf", 10);
    panelType.load("Lato-Bold.ttf", 15);
    fixMain.load("Lato-Bold.ttf", 15);
    pageMain.load("Lato-Bold.ttf", 45);
    pageSub.load("Lato-regular.ttf", 30);
    usrInput.load("Lato-Regular.ttf", 30);
    fixText.load("Lato-Regular.ttf", 7);
    
    // icons / images
    colorsIcon.load("panel_icons/colors.png");
}

//--------------------------------------------------------------
void ofApp::setup(){
    lastInteraction = ofGetElapsedTimeMillis();
    loadIconsFonts();
    
    // prepare known panel types for user definition
    knownPanelType.push_back(knownPanelObj);
    knownPanelType[knownPanelType.size()-1].type = "fixsimulation";
    knownPanelType[knownPanelType.size()-1].name = "Fixture Simulation";
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
    knownPanelType[knownPanelType.size()-1].name = "Brightness Mix";
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
    knownPanelType[knownPanelType.size()-1].name = "Position Mix";
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
    knownPanelType[knownPanelType.size()-1].name = "Stored Brightness";
    knownPanelType[knownPanelType.size()-1].minWidth = 2;
    knownPanelType[knownPanelType.size()-1].minHeight = 2;
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
    }


    ofSetColor(255);
    debugText.drawString("Lucent Developer | " + to_string(ofGetMouseX()) + " " + to_string(ofGetMouseY()) + " | F" + to_string(fixtures.size()) + " | W" + to_string(ofGetWindowWidth()) + " H" + to_string(ofGetWindowHeight()) + " | GR" + to_string(int(floor(((ofGetMouseX()-defCellGap)/defCellSize)))) + " " + to_string(int(floor(((ofGetMouseY()-defCellGap)/defCellSize)))) + " | m" + to_string(level) + " " + mode + " | o" + to_string(overlay) + " | op " + to_string(overPanel), 20, ofGetWindowHeight() - 20); //Debug text
    mouseExe = 0;   //allows user to click on something, drag mouse away and stop execution of that function from occuring. (Like accidently clicking something)
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key > 31 && key < 126 && strInputObj.textValue.length() < maxCharacterCount){
        char myChar = key;
        if (myChar != '!' && myChar != '@' && myChar != '#' && myChar != '$' && myChar != '%' && myChar != '^' && myChar != '&' && myChar != '*' && myChar != '(' && myChar != ')' && myChar != '/' && myChar != '|' && myChar != '`'){
            strInputObj.textValue = strInputObj.textValue + myChar;
        }
        
//        cout << "TEXT: " << strInputObj.textValue << endl;
//        cout << "KEY: " << myChar << endl;
//        cout << "NUM: " << to_string(key) << endl;
        
        vector<string> myVect = {};
        
//        cout << strInputObj.textValue[0] << endl;
    }

    if (key == OF_KEY_BACKSPACE && strInputObj.textValue.size() > 0){
        string original = strInputObj.textValue;
        strInputObj.textValue = "";
        for (int i = 0; i < original.size()-1; i++){
            strInputObj.textValue = strInputObj.textValue + original[i];
        }
    }
    
    if (key == OF_KEY_RETURN && strInputObj.textValue.size() > 0){
        strInputObj.fieldInputState = 0;
        *fieldPtr = strInputObj.textValue;
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
    mouseExe = 1;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
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

