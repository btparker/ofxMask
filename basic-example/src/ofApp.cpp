#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    
    imageToMask.load("images/bikers.jpg");
    
    mask.setup(
               imageToMask.getWidth(), // width
               imageToMask.getHeight(), // height
               GL_RGBA, // internal format of the maskee(s). This one draws with transparency
               ofxMask::LUMINANCE // Type of masker. LUMINANCE takes the grayscale and uses that to drive the mask
               // ofxMask::ALPHA  -- The alternative masker type, uses the alpha of the masker to drive content
            );
    
    ofSetWindowShape(imageToMask.getWidth(), imageToMask.getHeight());
    
    circleRadius = 50;
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(80);
    
    mask.beginMask();
    // Set the background to black. Black is drawn as transparent
    ofBackground(0, 0, 0, 255);
    
    // Drawing a white circle at the mouse, white is drawn as opaque
    ofSetColor(255, 255, 255, 255);
    ofDrawCircle(mouseX, mouseY, circleRadius);
    mask.endMask();
    
    // Draw the image into the mask (the maskee)
    mask.begin();
    imageToMask.draw(0,0);
    mask.end();
    
    // Now drawing the mask, with transparency (since we enabled alpha blending in the setup)
    mask.draw();
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
