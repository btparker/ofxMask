#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    
    imageA.load("images/tdf_1972_poster.jpg");
    imageB.load("images/indispensable.jpg");
    
    mask.setup(
               imageA.getWidth(), // width
               imageA.getHeight(), // height
               GL_RGB, // internal format of the maskee(s). This time, we won't be using transparency
               ofxMask::LUMINANCE, // Type of masker. LUMINANCE takes the grayscale and uses that to drive the mask
               true // useABMaskees : instead of one maskee, we mask between A and B
            );
    
    ofSetWindowShape(imageA.getWidth(), imageA.getHeight());
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(80);
    
    mask.beginMask();
    // Set the background to black. Black is drawn as maskee B
    ofBackground(0, 0, 0, 255);

    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    ofRotate(ofMap(mouseX, 0, ofGetWidth(), 0, 359));
    ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2);
    
    // Drawing a white rectangle, rotated by mouseX. White is drawn as maskee A
    ofSetColor(255, 255, 255, 255);
    
    // Scaling the rectangle so that when it rotates it still perfectly bisects
    ofDrawRectangle(ofGetWidth()/2, -ofGetHeight()/2, ofGetWidth(), ofGetHeight()*2);
    ofPopMatrix();
    mask.endMask();
    
    // Draw into maskee A
    mask.beginA();
    imageA.draw(0,0);
    mask.endA();
    
    // Draw into maskee B
    mask.beginB();
    imageB.draw(0,0);
    mask.endB();
    
    mask.draw();
    
    // If you want to draw the mask
    //mask.drawMasker();
    
    // If you want to draw maskee A
    //mask.drawMaskeeA();
    
    // If you want to draw maskee B
    //mask.drawMaskeeB();
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
