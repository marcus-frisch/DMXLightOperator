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


int ofApp::gX(int x){ //function to generate X coords for element on grid
    return (x * defCellSize)+defCellGap;
}

int ofApp::gY(int Y){ //function to generate Y coords for element on grid
    return (Y * defCellSize)+defCellGap;
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
//    ofDrawRectRounded(ofGetWindowWidth()/2+(defCellSize*4), ofGetWindowHeight()/2-(defCellSize/2), defCellSize, defCellSize, 5);
//    panelType.drawString("ENTR", ofGetWindowWidth()/2+(defCellSize*4)+10, ofGetWindowHeight()/2-(defCellSize/2)+(defCellGap*3));
//
    usrInput.drawString(textValue, ofGetWindowWidth()/2-(defCellSize*5) + defCellGap, ofGetWindowHeight()/2-(defCellSize/2) + (defCellGap * 3 + 5));
    panelName.drawString(to_string(textValue.size()) + " / " + to_string(max), ofGetWindowWidth()/2-(defCellSize*5), ofGetWindowHeight()/2+(defCellSize/2)+defCellGap);
    pageMain.drawString("Rename", ofGetWindowWidth()/2-(defCellSize*5), ofGetWindowHeight()/2-(defCellSize/2)-defCellGap);
}

void ofApp::controlPanel(int i){
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
                    if (clickLeft(x+(defCellGap/2) + (defCellSize*c), y+(defCellGap/2) + (defCellSize*r), defCellSize-(defCellGap), defCellSize-(defCellGap))){
                        mode = cpActions[indexAction];
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
        if (mouseX > x + defSpace + (defCellSize * f) && mouseX < x + defSpace + wi - (defSpace * 2) + (defCellSize * f) && mouseY >  y + defCellSize + (defCellSize/2) && mouseY < y + defCellSize + (defCellSize/2) + hi-defCellSize*2 && mouseExe == 1 && level < 2 && overlay == 0){   // check if mouse is in fader coordinates and map values.
            mouseBrightness = ofMap(mouseY, y + defCellSize + (defCellSize/2) + hi-defCellSize*2-1, y + defCellSize + (defCellSize/2)+1, 0, 255);
        }
        ofDrawRectRounded(x + defSpace + 3 + (defCellSize * f), y + defCellSize + (defCellSize/2) + hi-defCellSize*2 -3, wi - (defSpace * 2)-6,ofMap(mouseBrightness, 0, 255, 0, ((hi-defCellSize*2)-6)*-1),r);  //  draw mapped value
        ofSetColor(255, 255, 255);
        ofDrawRectRounded(x + defSpace + (defCellSize * f), y + defCellSize, wi - (defSpace * 2), (defCellSize/2)-10,r);  // MAX
        ofSetColor(0, 0, 0);
        fixText.drawString("MAX", x + defSpace + (wi/2)-20 + (defCellSize * f), y + defCellSize+ 20);
        if (mouseX > x + defSpace + (defCellSize * f) && mouseX < x + defSpace +  wi - (defSpace * 2) + (defCellSize * f) && mouseY > y + defCellSize && mouseY < y + defCellSize + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0){  // check mouse position for MAX button
            mouseBrightness = 255;
        }
        ofSetColor(255, 255, 255);
        ofDrawRectRounded(x + defSpace + (defCellSize * f), y + hi - (defCellSize/2) + 4, wi - (defSpace * 2), (defCellSize/2)-10,r);  // MIN
        ofSetColor(0, 0, 0);
        fixText.drawString("MIN", x + defSpace + (wi/2)-20 + (defCellSize * f),  y + hi - (defCellSize/2) + 20);
        if (mouseX > x + defSpace + (defCellSize * f) && mouseX < x + defSpace +  wi - (defSpace * 2) + (defCellSize * f) && mouseY > y + hi - (defCellSize/2) + 4 && mouseY <  y + hi - (defCellSize/2) + 4 + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0){   // check mouse position for MIN button
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
            if (mouseX > x + defSpace + ((wi/3) * f) && mouseX < x + defSpace + (wi/3) - (defSpace * 2) + ((wi/3) * f) && mouseY >  y + (wi/3) + ((wi/3)/2) && mouseY < y + (wi/3) + ((wi/3)/2) + hi-(wi/3)*2 && mouseExe == 1 && level < 2 && overlay == 0){   // check if mouse is in position and map values.
                mouseColors[f] = ofMap(mouseY, y + (wi/3) + ((wi/3)/2) + hi-(wi/3)*2-1, y + (wi/3) + ((wi/3)/2)+1, 0, 255);
            }
            ofDrawRectRounded(x + defSpace + 3 + ((wi/3) * f), y + (wi/3) + ((wi/3)/2) + hi-(wi/3)*2 -3, (wi/3) - (defSpace * 2)-6,ofMap(mouseColors[f], 0, 255, 0, ((hi-(wi/3)*2)-6)*-1),r);  //  draw mapped value
            ofSetColor(255, 255, 255);
            ofDrawRectRounded(x + defSpace + ((wi/3) * f), y + (wi/3), (wi/3) - (defSpace * 2), (defCellSize/2)-10,r);  // MAX
            ofSetColor(0, 0, 0);
            fixText.drawString("MAX", x + defSpace + ((wi/3)/2)-20 + ((wi/3) * f), y + (wi/3)+ 20);
            if (mouseX > x + defSpace + ((wi/3) * f) && mouseX < x + defSpace +  (wi/3) - (defSpace * 2) + ((wi/3) * f) && mouseY > y + (wi/3) && mouseY < y + (wi/3) + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0){
                mouseColors[f] = 255;
            }
            ofSetColor(255, 255, 255);
            ofDrawRectRounded(x + defSpace + ((wi/3) * f), y + hi - (defCellSize/2) + 4, (wi/3) - (defSpace * 2), (defCellSize/2)-10,r);  // MIN
            ofSetColor(0, 0, 0);
            fixText.drawString("MIN", x + defSpace + ((wi/3)/2)-20 + ((wi/3) * f),  y + hi - (defCellSize/2) + 20);
            if (mouseX > x + defSpace + ((wi/3) * f) && mouseX < x + defSpace +  (wi/3) - (defSpace * 2) + ((wi/3) * f) && mouseY > y + hi - (defCellSize/2) + 4 && mouseY <  y + hi - (defCellSize/2) + 4 + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0){
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
                    if (mouseX > x + defSpace + ((wi/3) * f) && mouseX < x + defSpace + (wi/3) - (defSpace * 2) + ((wi/3) * f) && mouseY >  y + (wi/3) + ((wi/3)/2) && mouseY < y + (wi/3) + ((wi/3)/2) + hi-(wi/3)*2 && mouseExe == 1 && level < 2 && overlay == 0){   // check if mouse is in position and map values.
                        mouseColors[3+f] = ofMap(mouseY, y + (wi/3) + ((wi/3)/2) + hi-(wi/3)*2-1, y + (wi/3) + ((wi/3)/2)+1, 0, 255);
                    }
                    ofDrawRectRounded(x + defSpace + 3 + ((wi/3) * f), y + (wi/3) + ((wi/3)/2) + hi-(wi/3)*2 -3, (wi/3) - (defSpace * 2)-6,ofMap(mouseColors[3+f], 0, 255, 0, ((hi-(wi/3)*2)-6)*-1),r);  //  draw mapped value
                    ofSetColor(255, 255, 255);
                    ofDrawRectRounded(x + defSpace + ((wi/3) * f), y + (wi/3), (wi/3) - (defSpace * 2), (defCellSize/2)-10,r);  // MAX
                    ofSetColor(0, 0, 0);
                    fixText.drawString("MAX", x + defSpace + ((wi/3)/2)-20 + ((wi/3) * f), y + (wi/3)+ 20);
                    if (mouseX > x + defSpace + ((wi/3) * f) && mouseX < x + defSpace +  (wi/3) - (defSpace * 2) + ((wi/3) * f) && mouseY > y + (wi/3) && mouseY < y + (wi/3) + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0){
                        mouseColors[3+f] = 255;
                    }
                    ofSetColor(255, 255, 255);
                    ofDrawRectRounded(x + defSpace + ((wi/3) * f), y + hi - (defCellSize/2) + 4, (wi/3) - (defSpace * 2), (defCellSize/2)-10,r);  // MIN
                    ofSetColor(0, 0, 0);
                    fixText.drawString("MIN", x + defSpace + ((wi/3)/2)-20 + ((wi/3) * f),  y + hi - (defCellSize/2) + 20);
                    if (mouseX > x + defSpace + ((wi/3) * f) && mouseX < x + defSpace +  (wi/3) - (defSpace * 2) + ((wi/3) * f) && mouseY > y + hi - (defCellSize/2) + 4 && mouseY <  y + hi - (defCellSize/2) + 4 + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0){
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
                    if (mouseX > x + defSpace + ((wi/2) * f) && mouseX < x + defSpace + (wi/2) - (defSpace * 2) + ((wi/2) * f) && mouseY >  y + (wi/2) + ((wi/2)/2) && mouseY < y + (wi/2) + ((wi/2)/2) + hi-(wi/2)*2 && mouseExe == 1 && level < 2 && overlay == 0){   // check if mouse is in position and map values.
                        mousePosition[f] = ofMap(mouseY, y + (wi/2) + ((wi/2)/2) + hi-(wi/2)*2-1, y + (wi/2) + ((wi/2)/2)+1, 0, 255);
                    }
                    ofDrawRectRounded(x + defSpace + 3 + ((wi/2) * f), y + (wi/2) + ((wi/2)/2) + hi-(wi/2)*2 -3, (wi/2) - (defSpace * 2)-6,ofMap(mousePosition[f], 0, 255, 0, ((hi-(wi/2)*2)-6)*-1),r);  //  draw mapped value
                    ofSetColor(255, 255, 255);
                    ofDrawRectRounded(x + defSpace + ((wi/2) * f), y + (wi/2), (wi/2) - (defSpace * 2), (defCellSize/2)-10,r);  // MAX
                    ofSetColor(0, 0, 0);
                    fixText.drawString("MAX", x + defSpace + ((wi/2)/2)-20 + ((wi/2) * f), y + (wi/2)+ 20);
                    if (mouseX > x + defSpace + ((wi/2) * f) && mouseX < x + defSpace +  (wi/2) - (defSpace * 2) + ((wi/2) * f) && mouseY > y + (wi/2) && mouseY < y + (wi/2) + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0){
                        mousePosition[f] = 255;
                    }
                    ofSetColor(255, 255, 255);
                    ofDrawRectRounded(x + defSpace + ((wi/2) * f), y + hi - (defCellSize/2) + 4, (wi/2) - (defSpace * 2), (defCellSize/2)-10,r);  // MIN
                    ofSetColor(0, 0, 0);
                    fixText.drawString("MIN", x + defSpace + ((wi/2)/2)-20 + ((wi/2) * f),  y + hi - (defCellSize/2) + 20);
                    if (mouseX > x + defSpace + ((wi/2) * f) && mouseX < x + defSpace +  (wi/2) - (defSpace * 2) + ((wi/2) * f) && mouseY > y + hi - (defCellSize/2) + 4 && mouseY <  y + hi - (defCellSize/2) + 4 + (defCellSize/2)-10 && mouseExe == 1 && level < 2 && overlay == 0){
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
                        ofNoFill();
                        
                        
                        
                    } else {
                        ofSetColor(217, 128, 255);   //set colour of cell
                        ofNoFill();
                        ofDrawRectRounded(x + (c * defCellSize), y + (r*defCellSize), defCellSize, defCellSize,10);
                        ofSetLineWidth(1);
                        fixText.drawString(to_string(panels[i].savedColors[getCellByIden].iden),x+ 10 + (c * defCellSize),y + 17 + (r*defCellSize));
                        ofNoFill();
                    }
                    
                    if (clickLeft(x + (c * defCellSize), y + (r*defCellSize), defCellSize, defCellSize) && level < 2 && overlay == 0){   //if user clicks on a cell. store a color from color data mouse has.
                        if (mode == "store"){
                            mode = "";
                            panels[i].savedColors[getCellByIden].r = mouseColors[0];
                            panels[i].savedColors[getCellByIden].g = mouseColors[1];
                            panels[i].savedColors[getCellByIden].b = mouseColors[2];
                            panels[i].savedColors[getCellByIden].name = "Color #" + to_string(getCellByIden);
                            panels[i].savedColors[getCellByIden].set = true;
                        } else if (mode == "delete") {
                            mode = "";
                            panels[i].savedColors[getCellByIden].r = 255;
                            panels[i].savedColors[getCellByIden].g = 255;
                            panels[i].savedColors[getCellByIden].b = 255;
                            panels[i].savedColors[getCellByIden].name = "";
                            panels[i].savedColors[getCellByIden].set = false;
                        } else if (mode == "rename" && fieldInputState == 0){   // incomplete renaming function
                            fieldInputState = 1;
                            strInput("#", 6);
                            
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
                    
                    if (clickLeft(x + (c * defCellSize), y + (r*defCellSize), defCellSize, defCellSize) && level < 2 && overlay == 0){   //if user clicks on a cell. store a color from color data mouse has.
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
                        panelType.drawString(panels[i].savedPositions[getCellByIden].name,x+ 15 + (c * defCellSize),y + 44 + (r*defCellSize));
                        ofNoFill();
                    } else {
                        ofSetColor(59, 194, 41);   //set colour of cell
                        ofNoFill();
                        ofDrawRectRounded(x + (c * defCellSize), y + (r*defCellSize), defCellSize, defCellSize,10);
                        ofSetLineWidth(1);
                        fixText.drawString(to_string(panels[i].savedPositions[getCellByIden].iden),x+ 10 + (c * defCellSize),y + 17 + (r*defCellSize));
                        ofNoFill();
                        
                    }
                    
                    if (clickLeft(x + (c * defCellSize), y + (r*defCellSize), defCellSize, defCellSize) && level < 2 && overlay == 0){   //if user clicks on a cell. store a color from color data mouse has.
                        if (mode == "store"){
                            mode = "";
                            panels[i].savedPositions[getCellByIden].position = mousePosition;
                            panels[i].savedPositions[getCellByIden].set = true;
                        } else if (mode == "delete") {
                            mode = "";
                            panels[i].savedPositions[getCellByIden].position = {};
                            panels[i].savedPositions[getCellByIden].set = false;
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
    //$%$%% fixtures[fixtures.size()-1].channelData = {0,0,50,rand() % 255,rand() % 255,rand() % 255,8,0,0};
    
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
    //$%$%% ofSetColor(fixtures[fixAddress].channelData[3], fixtures[fixAddress].channelData[4], fixtures[fixAddress].channelData[5]);
    ofDrawRectangle(x, y, w, h);
    ofSetColor(255, 255, 255);
    //$%$%% fixMain.drawString(to_string(fixtures[fixAddress].channelData[2]),x + 20,y+38);
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
    fixMain.load("Lato-Bold.ttf", 20);
    pageMain.load("Lato-Bold.ttf", 45);
    usrInput.load("Lato-Regular.ttf", 30);
    fixText.load("Lato-Regular.ttf", 10);
    
    // icons / images
    colorsIcon.load("panel_icons/colors.png");
}

//--------------------------------------------------------------
void ofApp::setup(){
    
    loadIconsFonts();
    
    
    
    panels.push_back(panelObj);
    panels[0].type = "fixsimulation";
    panels[0].name = "Fixture Simulation";
    panels[0].x = 5;
    panels[0].y = 1;
    panels[0].wi = 13;
    panels[0].hi = 2;
    panels[0].cellSize = 40;
    
    
    panels.push_back(panelObj);
    panels[1].type = "controlpanel";
    panels[1].name = "Control Panel";
    panels[1].x = 24;
    panels[1].y = 8;
    panels[1].wi = 3;
    panels[1].hi = 5;
    panels[1].cellSize = 40;
    
    panels.push_back(panelObj);
    panels[2].type = "colorstor";
    panels[2].name = "Defined Colors";
    panels[2].x = 14;
    panels[2].y = 4;
    panels[2].wi = 5;
    panels[2].hi = 7;
    panels[2].cellSize = 40;


    
     
    panels.push_back(panelObj);
    panels[3].type = "colorstor";
    panels[3].name = "Defined Colors";
    panels[3].x = 6;
    panels[3].y = 6;
    panels[3].wi = 4;
    panels[3].hi = 5;
    panels[3].cellSize = 40;

    
    panels.push_back(panelObj);
    panels[4].type = "rgbmixer";
    panels[4].name = "Color mixer";
    panels[4].x = 1;
    panels[4].y = 1;
    panels[4].wi = 3;
    panels[4].hi = 4;
    panels[4].cellSize = defCellSize;
    panels[4].defSpace = 5;
    
    panels.push_back(panelObj);
    panels[5].type = "brimixer";
    panels[5].name = "Bri Mix";
    panels[5].x = 11;
    panels[5].y = 5;
    panels[5].wi = 2;
    panels[5].hi = 6;
    panels[5].cellSize = defCellSize;
    panels[5].defSpace = 5;
    
    panels.push_back(panelObj);
    panels[6].type = "bristor";
    panels[6].name = "Stored Brightness";
    panels[6].x = 19;
    panels[6].y = 2;
    panels[6].wi = 6;
    panels[6].hi = 2;
    panels[6].cellSize = 40;
   
    panels.push_back(panelObj);
    panels[7].type = "wauv";
    panels[7].name = "White Amber UV mixer";
    panels[7].x = 20;
    panels[7].y = 6 ;
    panels[7].wi = 3;
    panels[7].hi = 4;
    panels[7].cellSize = defCellSize;
    panels[7].defSpace = 5;
    
    panels.push_back(panelObj);
    panels[8].type = "posMixer";
    panels[8].name = "Pan Tilt Mixer";
    panels[8].x = 1;
    panels[8].y = 6 ;
    panels[8].wi = 2;
    panels[8].hi = 4;
    panels[8].cellSize = defCellSize;
    panels[8].defSpace = 5;
    
    panels.push_back(panelObj);
    panels[9].type = "posStor";
    panels[9].name = "Positn";
    panels[9].x = 10;
    panels[9].y = 11;
    panels[9].wi = 9;
    panels[9].hi = 3;
    panels[9].cellSize = defCellSize;
    panels[9].defSpace = 5;
    
    fixtures.push_back(fixObj);
    fixtures[0].name = "cobra";
    fixtures[0].simShow = true;
    fixtures[0].iden = 0;
    fixtures[0].x = 6;
    fixtures[0].y = 4;
    fixtures[0].w = defCellSize;
    fixtures[0].h = defCellSize;
    fixtures[0].universe = 1;
    fixtures[0].address = 1;
    //$%$%% fixtures[0].channelData = {0,0,50,124,8,207,8,0,0};
    //$%$%% fixtures[0].channels = {"pan", "tilt", "dimmer", "red", "green", "blue", "white", "amber", "uv"};    // purpose of each channel
    fixtures[0].channelCount = fixtures[0].channels.size();
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    level = 0;
    drawGrid();
    
//    for (int i = 0; i < panels.size(); i++){ //draw panels
//        uipanel(i);
//        simulatedFixture(0);
//        simulatedFixture(1);
//    }
    
    uipanel(0);   //simulated fixture
    uipanel(1);   //control panel
    uipanel(2);   // color panel
    uipanel(3);   // color panel
    uipanel(4);   // color mixer
    uipanel(5);   // color mixer
    uipanel(6);
    uipanel(7);
    uipanel(8);
    uipanel(9);
//    simulatedFixture(0);
//    simulatedFixture(1);
    
    
    for (int i = 0; i < fixtures.size(); i++){
        simulatedFixture(i);
        
    }
    
    addFixButton();
    
    ofSetColor(255, 255, 255);
    
//    strInput("#", 6);
    
    debugText.drawString("Lucent Developer | " + to_string(ofGetMouseX()) + " " + to_string(ofGetMouseY()) + " | F" + to_string(fixtures.size()) + " | W" + to_string(ofGetWindowWidth()) + " H" + to_string(ofGetWindowHeight()) + " | GR" + to_string(int(floor(((ofGetMouseX()-defCellGap)/defCellSize)))) + " " + to_string(int(floor(((ofGetMouseY()-defCellGap)/defCellSize)))) + " | m" + to_string(level) + " " + mode + " | o" + to_string(overlay), 20, ofGetWindowHeight() - 20); //Debug text
    mouseExe = 0;   //allows user to click on something, drag mouse away and stop execution of that function from occuring. (Like accidently clicking something)
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key > 32 && key < 126 && textValue.length() < maxCharacterCount){
        char myChar = key;
        textValue = textValue + myChar;
        cout << "TEXT: " << textValue << endl;
        cout << "KEY: " << myChar << endl;
        cout << "NUM: " << to_string(key) << endl;
        
        vector<string> myVect = {};
        
        cout << textValue[0] << endl;
    }
    
    
    if (key == OF_KEY_BACKSPACE && textValue.size() > 0){
        string original = textValue;
        textValue = "";
        for (int i = 0; i < original.size()-1; i++){
            textValue = textValue + original[i];
        }
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
