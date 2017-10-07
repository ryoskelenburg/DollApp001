#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(30);
    ofBackground(0, 0);

    ofDisableArbTex(); // we need GL_TEXTURE_2D for our models coords.

    bAnimate = false;
    bAnimateMouse = false;
    animationPosition = 0;
    ofPoint modelPosition(ofGetWidth() * 0.5, (float)ofGetHeight() * 0.75);
    
    model.loadModel("bear/body.stl");
    model.setPosition(modelPosition.x, modelPosition.y, modelPosition.z);
    model.setRotation(0, -180, 1, 0, 0);
    arm[0].loadModel("bear/left_arm.stl");
    arm[0].setPosition(modelPosition.x, modelPosition.y, modelPosition.z);
    arm[0].setRotation(0, -180, 1, 0, 0);
    arm[1].loadModel("bear/right_arm.stl");
    arm[1].setPosition(modelPosition.x, modelPosition.y, modelPosition.z);
    arm[1].setRotation(0, -180, 1, 0, 0);
    
    ofDisableSeparateSpecularLight();
    if(!bAnimate) {
        model.setPausedForAllAnimations(true);
    }
    
    //////////
    
    buttonState = "digital pin:";
    potValue = "analog pin:";
    
    bgImage.load("background.png");
    font.load("franklinGothic.otf", 20);
    smallFont.load("franklinGothic.otf", 14);
    
    // replace the string below with the serial port for your Arduino board
    // you can get this from the Arduino application or via command line
    // for OSX, in your terminal type "ls /dev/tty.*" to get a list of serial devices
    ard.connect("/dev/tty.usbmodem14411", 57600);
    
    // listen for EInitialized notification. this indicates that
    // the arduino is ready to receive commands and it is safe to
    // call setupArduino()
    ofAddListener(ard.EInitialized, this, &ofApp::setupArduino);
    bSetupArduino	= false;	// flag so we setup arduino when its ready, you don't need to touch this :)
    
}

//--------------------------------------------------------------
void ofApp::update(){
    cnt++;
    ang[1] = ofMap(ard.getAnalog(0),340,470,0,180,true);
    ang[0] = ofMap(ard.getAnalog(1),750,810,0,180,true);
    cout << ard.getAnalog(0)<< ";"<< ang[0] << ";" << ard.getAnalog(1)<< ";"<<ang[1] <<endl;
    
    model.update();
    
    if(bAnimateMouse) {
        model.setPositionForAllAnimations(animationPosition);
    }

    mesh = model.getCurrentAnimatedMesh(0);
    
    /////////
    
    updateArduino();
}

//--------------------------------------------------------------
void ofApp::setupArduino(const int & version) {
    
    // remove listener because we don't need it anymore
    ofRemoveListener(ard.EInitialized, this, &ofApp::setupArduino);
    
    // it is now safe to send commands to the Arduino
    bSetupArduino = true;
    
    // print firmware name and version to the console
    ofLogNotice() << ard.getFirmwareName();
    ofLogNotice() << "firmata v" << ard.getMajorFirmwareVersion() << "." << ard.getMinorFirmwareVersion();
    
    // set pin A0 to analog input
    ard.sendAnalogPinReporting(0, ARD_ANALOG);
    ard.sendAnalogPinReporting(1, ARD_ANALOG);
    
    // Listen for changes on the digital and analog pins
    ofAddListener(ard.EDigitalPinChanged, this, &ofApp::digitalPinChanged);
    ofAddListener(ard.EAnalogPinChanged, this, &ofApp::analogPinChanged);
}

//--------------------------------------------------------------
void ofApp::updateArduino(){
    
    // update the arduino, get any data or messages.
    // the call to ard.update() is required
    ard.update();
    
    
}

// digital pin event handler, called whenever a digital pin value has changed
// note: if an analog pin has been set as a digital pin, it will be handled
// by the digitalPinChanged function rather than the analogPinChanged function.

//--------------------------------------------------------------
void ofApp::digitalPinChanged(const int & pinNum) {
    // do something with the digital input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    buttonState = "digital pin: " + ofToString(pinNum) + " = " + ofToString(ard.getDigital(pinNum));
}

// analog pin event handler, called whenever an analog pin value has changed

//--------------------------------------------------------------
void ofApp::analogPinChanged(const int & pinNum) {
    // do something with the analog input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    potValue = "analog pin: " + ofToString(pinNum) + " = " + ofToString(ard.getAnalog(pinNum));
}


//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255);
    
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    
	ofEnableDepthTest();
#ifndef TARGET_PROGRAMMABLE_GL    
    glShadeModel(GL_SMOOTH); //some model / light stuff
#endif
    light.enable();
    ofEnableSeparateSpecularLight();
    
    light.setDiffuseColor(ofColor(255,100,0));
    //cout << -mouseX+90 << endl;
    ofPushMatrix();
    ofTranslate(0,0,-1000);
    ofTranslate(model.getPosition().x, model.getPosition().y, 0);
    //ofRotate(-mouseX, 0, 1, 0);
    //ofRotateX(180);
    ofTranslate(-model.getPosition().x, -model.getPosition().y, 0);
    ofSetColor(255,0,0);
    
    model.drawFaces();
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(300,50,-1000);
    ofTranslate(arm[0].getPosition().x, arm[0].getPosition().y-100, 0);
    ofRotate(-720, 0, 1, 0);
    ofTranslate(10,80,0);
    ofRotate(ang[0], 1, 0, 0);
    //ofRotateX(180);
    ofTranslate(500-arm[0].getPosition().x, -arm[0].getPosition().y, 0);
    ofSetColor(255,0,0);
    arm[0].setScale(0.5,0.5,0.5);
    arm[0].drawFaces();
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(-300,50,-1000);
    ofTranslate(arm[1].getPosition().x, arm[1].getPosition().y-170, 50);
    ofRotate(-720+180, 0, 1, 0);
    ofTranslate(160,180,400);
    ofRotate(-ang[1], 1, 0, 0);
    ofRotateY(180);
    ofTranslate(-500-arm[1].getPosition().x, -arm[1].getPosition().y, 0);
    ofSetColor(255,0,0);
    arm[1].setScale(0.5,0.5,0.5);
    arm[1].drawFaces();
    ofPopMatrix();

    
#ifndef TARGET_PROGRAMMABLE_GL    
    glEnable(GL_NORMALIZE);
#endif
    /*
    ofPushMatrix();
    ofTranslate(model.getPosition().x-300, model.getPosition().y, 0);
    ofRotate(-mouseX, 0, 1, 0);
    ofRotateX(180);
    ofTranslate(-model.getPosition().x, -model.getPosition().y, 0);
    
    ofxAssimpMeshHelper & meshHelper = model.getMeshHelper(0);
    
    ofMultMatrix(model.getModelMatrix());
    ofMultMatrix(meshHelper.matrix);
    
    ofMaterial & material = meshHelper.material;
    if(meshHelper.hasTexture()){
        meshHelper.getTextureRef().bind();
    }
    material.begin();
    mesh.drawWireframe();
    material.end();
    if(meshHelper.hasTexture()){
        meshHelper.getTextureRef().unbind();
    }
    ofPopMatrix();*/
    
    ofDisableDepthTest();
    light.disable();
    ofDisableLighting();
    ofDisableSeparateSpecularLight();
    
    /*
    ofSetColor(255, 255, 255 );
    ofDrawBitmapString("fps: "+ofToString(ofGetFrameRate(), 2), 10, 15);
    ofDrawBitmapString("keys 1-5 load models, spacebar to trigger animation", 10, 30);
    ofDrawBitmapString("drag to control animation with mouseY", 10, 45);
    ofDrawBitmapString("num animations for this model: " + ofToString(model.getAnimationCount()), 10, 60);*/
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    ofPoint modelPosition(ofGetWidth() * 0.5, (float)ofGetHeight() * 0.75);
    switch (key) {
        case '1':
            model.loadModel("astroBoy_walk.dae");
            model.setPosition(modelPosition.x, modelPosition.y, modelPosition.z);
            ofEnableSeparateSpecularLight();
            break;
        case '2':
            model.loadModel("TurbochiFromXSI.dae");
            model.setPosition(modelPosition.x, modelPosition.y, modelPosition.z);
            model.setRotation(0, -180, 1, 0, 0);
            ofEnableSeparateSpecularLight();
            break;
        case '3':
            model.loadModel("dwarf.x");
            model.setPosition(modelPosition.x, modelPosition.y, modelPosition.z);
            ofDisableSeparateSpecularLight();
            break;
        case '4':
            model.loadModel("monster-animated-character-X.X");
            model.setPosition(modelPosition.x, modelPosition.y, modelPosition.z);
            model.setRotation(0, -90, 0, 0, 1);
            ofDisableSeparateSpecularLight();
            break;
		case '5':
			model.loadModel("bear/body.stl");
		    model.setPosition(modelPosition.x, modelPosition.y, modelPosition.z);
            model.setRotation(0, -180, 1, 0, 0);
            arm[0].loadModel("bear/left_arm.stl");
            arm[0].setPosition(modelPosition.x, modelPosition.y, modelPosition.z);
            arm[0].setRotation(0, -180, 1, 0, 0);
            arm[1].loadModel("bear/right_arm.stl");
            arm[1].setPosition(modelPosition.x, modelPosition.y, modelPosition.z);
            arm[1].setRotation(0, -180, 1, 0, 0);
            
            ofDisableSeparateSpecularLight();
			break;
		case ' ':
			bAnimate = !bAnimate;
			break;
        case 'f':
            ofSetFullscreen(true);
            break;
            
        default:
            break;
    }

	mesh = model.getMesh(0);

    model.setLoopStateForAllAnimations(OF_LOOP_NORMAL);
    model.playAllAnimations();
    if(!bAnimate) {
        model.setPausedForAllAnimations(true);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    //
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    // scrub through aninations manually.
	animationPosition = y / (float)ofGetHeight();
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    // pause all animations, so we can scrub through them manually.
    model.setPausedForAllAnimations(true);
	animationPosition = y / (float)ofGetHeight();
    bAnimateMouse = true;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    // unpause animations when finished scrubbing.
    if(bAnimate) {
        model.setPausedForAllAnimations(false);
    }
    bAnimateMouse = false;
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

