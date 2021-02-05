#include "ofApp.h"
#include <math.h>


vector<panelClass> panels;  //stores panel object
panelClass panelObj;

vector<dmxFixtureclass> fixtures;   //stores all dmxFixture objects
dmxFixtureclass fixObj;

fixColor colorObj;



int ofApp::gX(int x){ //function to generate X coords for element on grid
    return (x * defCellSize)+defCellGap;
}

int ofApp::gY(int Y){ //function to generate Y coords for element on grid
    return (Y * defCellSize)+defCellGap;
}



void ofApp::colorsPanel(int i){
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
    int dataIden = panels[i].dataIden;
    int wid = panels[i].wi;
    int hei = panels[i].hi;
    int multi = wid * hei;
    
    
//    if (colors[dataIden].size() == 0){
//        for (int i = 0; i < multi; i++){
//            colors.push_back({});
//            colors[dataIden].push_back(colorObj);
//            colors[dataIden][i].iden = i;
//            colors[dataIden][i].r = rand() % 255;
//            colors[dataIden][i].g = rand() % 255;
//            colors[dataIden][i].b = rand() % 255;
//        }
//    }
    
    if (panels[i].savedColors.size() == 0){
                for (int u = 0; u < multi; u++){
                    panels[i].savedColors.push_back(colorObj);
                    panels[i].savedColors[u].iden = u;
                    panels[i].savedColors[u].r = rand() % 255;
                    panels[i].savedColors[u].g = rand() % 255;
                    panels[i].savedColors[u].b = rand() % 255;
                    
                }
    }
    
    
    
    ofFill();
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
                        //ofDrawTriangle(x + (c * defCellSize), y+25 + (r*defCellSize), x+defCellSize, y+25 + (r*defCellSize), x + (c * defCellSize), y+35 + (r*defCellSize));
                        ofSetColor(255, 255, 255);
                        fixText.drawString(to_string(panels[i].savedColors[getCellByIden].iden),x+ 10 + (c * defCellSize),y + 17 + (r*defCellSize));
                        ofNoFill();
                        //ofSetLineWidth(2);
                        //ofDrawRectangle(x, y, defCellSize, defCellSize);
                    } else {
                        ofSetColor(217, 128, 255);   //set colour of cell
                        ofNoFill();
                        ofDrawRectRounded(x + (c * defCellSize), y + (r*defCellSize), defCellSize, defCellSize,10);
                        ofSetLineWidth(1);
                        
                        //ofDrawTriangle(x + (c * defCellSize), y+25 + (r*defCellSize), x+defCellSize, y+25 + (r*defCellSize), x + (c * defCellSize), y+35 + (r*defCellSize));
                        //ofSetColor(255, 255, 255);
                        fixText.drawString(to_string(panels[i].savedColors[getCellByIden].iden),x+ 10 + (c * defCellSize),y + 17 + (r*defCellSize));
                        ofNoFill();
                        //ofSetLineWidth(2);
                        //ofDrawRectangle(x, y, defCellSize, defCellSize);
                    }
                    
                    
                    if (clickLeft(x + (c * defCellSize), y + (r*defCellSize), defCellSize, defCellSize)){   //if user clicks on a cell. store a random color.
                        panels[i].savedColors[getCellByIden].r = rand() % 255;
                        panels[i].savedColors[getCellByIden].g = rand() % 255;
                        panels[i].savedColors[getCellByIden].b = rand() % 255;
                        panels[i].savedColors[getCellByIden].set = true;
                
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
    fixtures[fixtures.size()-1].channelData = {0,0,50,rand() % 255,rand() % 255,rand() % 255,8,0,0};
    
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
    
    if (type == "fixsimulation"){       //simulated fixtures panel
        ofFill();
        ofSetColor(22, 22, 22);
        ofDrawRectRounded(x, y, wi, hi,r);
        ofSetColor(255, 255, 255);
        panelName.drawString(name, x + 30, y+10);
    } else if (type == "controlpanel"){     //control panel
            ofFill();
            ofSetColor(77, 77, 77);
            ofDrawRectRounded(x, y, wi, hi,r);
            ofSetColor(255, 255, 255);
            panelName.drawString(name, x + 30, y+10);
    } else if (type=="rgbmixer"){
        ofFill();
        ofSetColor(77, 77, 77);
        ofDrawRectRounded(x, y, wi, hi,r);
        ofSetColor(255, 255, 255);
        panelName.drawString(name, x + 30, y+10);
        
    } else if (type =="colorstor"){ //draw panels where colour can be stored.

        colorsPanel(i);

    }
}


void ofApp::simulatedFixture(int fixAddress){
    ofFill();
    int x = gX(fixtures[fixAddress].x);
    int y = gY(fixtures[fixAddress].y);
    int w = fixtures[fixAddress].w;
    int h = fixtures[fixAddress].h;
    ofSetColor(fixtures[fixAddress].channelData[3], fixtures[fixAddress].channelData[4], fixtures[fixAddress].channelData[5]);
    ofDrawRectangle(x, y, w, h);
    ofSetColor(255, 255, 255);
    fixMain.drawString(to_string(fixtures[fixAddress].channelData[2]),x + 20,y+38);
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



//--------------------------------------------------------------
void ofApp::setup(){
    
    
    
//    ofxXmlSettings settings;
    
    debugText.load("Lato-Regular.ttf", 15);
    panelName.load("Lato-Regular.ttf", 10);
    panelType.load("Lato-Bold.ttf", 15);
    fixMain.load("Lato-Bold.ttf", 20);
    fixText.load("Lato-Regular.ttf", 10);
    
    colorsIcon.load("panel_icons/colors.png");
    
    
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
    panels[1].x = 20;
    panels[1].y = 4;
    panels[1].wi = 5;
    panels[1].hi = 7;
    panels[1].cellSize = 40;
    
    panels.push_back(panelObj);
    panels[2].type = "colorstor";
    panels[2].name = "Defined Colors";
    panels[2].x = 14;
    panels[2].y = 4;
    panels[2].wi = 5;
    panels[2].hi = 7;
    panels[2].cellSize = 40;
    panels[2].dataIden = 0;

    
     
    panels.push_back(panelObj);
    panels[3].type = "colorstor";
    panels[3].name = "Defined Colors";
    panels[3].x = 4;
    panels[3].y = 6;
    panels[3].wi = 4;
    panels[3].hi = 5;
    panels[3].cellSize = 40;
    panels[3].dataIden = 1;
    
    
    
    fixtures.push_back(fixObj);
    fixtures[0].name = "cobra";
    fixtures[0].simShow = true;
    fixtures[0].x = 6;
    fixtures[0].y = 4;
    fixtures[0].w = defCellSize;
    fixtures[0].h = defCellSize;
    fixtures[0].universe = 1;
    fixtures[0].address = 1;
    fixtures[0].channelCount = 9;
    fixtures[0].channelData = {0,0,50,124,8,207,8,0,0};
    
    fixtures.push_back(fixObj);
    fixtures[1].name = "cobra";
    fixtures[1].simShow = true;
    fixtures[1].x = 8;
    fixtures[1].y = 4;
    fixtures[1].w = defCellSize;
    fixtures[1].h = defCellSize;
    fixtures[1].universe = 1;
    fixtures[1].address = 7;
    fixtures[1].channelCount = 9;
    fixtures[1].channelData = {0,0,50,rand() % 255,rand() % 255,rand() % 255,8,0,0};
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
   
    
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
    
//    simulatedFixture(0);
//    simulatedFixture(1);
    
    
    for (int i = 0; i < fixtures.size(); i++){
        simulatedFixture(i);
        
    }
    
    addFixButton();
    
    ofSetColor(255, 255, 255);
    
    
    debugText.drawString("Lucent Developer | " + to_string(ofGetMouseX()) + " " + to_string(ofGetMouseY()) + " | F" + to_string(fixtures.size()) + " | W" + to_string(ofGetWindowWidth()) + " H" + to_string(ofGetWindowHeight()) + " | GR" + to_string(int(floor(((ofGetMouseX()-defCellGap)/defCellSize)))) + " " + to_string(int(floor(((ofGetMouseY()-defCellGap)/defCellSize)))), 20, ofGetWindowHeight() - 20); //Debug text
    
    
    
    
   
    mouseExe = 0;   //allows user to click on something, drag mouse away and stop execution of that function from occuring. (Like accidently clicking something)
}






//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

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
