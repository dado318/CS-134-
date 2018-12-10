//
//   CS 134 - Midterm Starter File - Fall 2018
//
//
//   This file contains all the necessary startup code for the Midterm problem Part II
//   Please make sure to you the required data files installed in your $OF/data directory.
//
//                                             (c) Kevin M. Smith  - 2018
// Davor Koret and Galen Rivoire
// CS 134-01
// Final Game Project


#include "ofApp.h"

//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup(){


	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bLanderLoaded = false;

	cam.setDistance(10);
	cam.setNearClip(.1);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	cam.disableMouseInput();

	topCam.setNearClip(.1);
	topCam.setFov(65.5);   
	topCam.setPosition(0, 10, 0);
	topCam.lookAt(glm::vec3(0, 0, 0));

	// set current camera;
	//
	theCam = &cam;
	
	ofSetVerticalSync(true);
	ofEnableSmoothing();
	ofEnableDepthTest();

	// load BG image
	//
	bBackgroundLoaded = backgroundImage.load("images/starfield-plain.jpg");


	// setup rudimentary lighting 
	//
	initLightingAndMaterials();


	// load lander model
	//
	if (lander.loadModel("geo/lander.obj")) {
		lander.setScaleNormalization(false);
		lander.setScale(.5, .5, .5);
		lander.setRotation(0, -180, 1, 0, 0);
		lander.setPosition(0,0,0);

		bLanderLoaded = true;
	}
	else {
		cout << "Error: Can't load model" << "geo/lander.obj" << endl;
		ofExit(0);
	}
	if (terrain.loadModel("geo/terrain_1.obj")) {
		terrain.setScaleNormalization(false);
		terrain.setScale(1, 1, 1);
		terrain.setRotation(0, 0, 1, 0, 0);
		terrain.setPosition(0, 0, 0);
		terrainLoaded = true;
	}
	else {
		cout << "Can't load model: " << "geo/terrain_1.obj" << endl;
		ofExit(0);
	}

	// Added: set up basic drifting lem movement
	//
	lem = ParticleEmitter(new ParticleSystem);
	lem.setPosition(lander.getPosition());//
	lem.setLifespan(1000000);
	lem.setVelocity(ofVec3f(0, 0, 0));
	lem.sys->addForce(new TurbulenceForce(ofVec3f(-.1, -.3, -.1), ofVec3f(.2, .2, .1)));
	//lem.start();

	// Added: set up exhaust emitter
	//
	exhaust = ParticleEmitter(new ParticleSystem);
	exhaust.setPosition(lander.getPosition());
	exhaust.sys->addForce(new ImpulseRadialForce(200));
	exhaust.setEmitterType(DiscEmitter);
	exhaust.setGroupSize(100);
	exhaust.start();
}


void ofApp::update() {
	// Added: Updates lem, lander, and exhaust position based on lem's first launched particle
	if (lem.sys->particles.size() > 0) {
		lem.stop();
	}
	if (lem.sys->particles.size() < 1) {
		lem.start();
	}
	for (int i = 0; i < lem.sys->particles.size(); i++) {
		lander.setPosition(lem.sys->particles[i].position.x, lem.sys->particles[i].position.y, lem.sys->particles[i].position.z);
		lem.setPosition(lander.getPosition());
		exhaust.setPosition(lander.getPosition());
	}
	lem.update();
	lander.update();

	// Added: Updates exhaust emitter position
	if (activateExhaust) {
		exhaust.setLifespan(5000);
		exhaust.setRate(10);
		exhaust.setVelocity(ofVec3f(0, -20, 0));
	}
	else {
		exhaust.setRate(0);
	}
	exhaust.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
	//	ofBackgroundGradient(ofColor(20), ofColor(0));   // pick your own backgroujnd
	//	ofBackground(ofColor::black);
	if (bBackgroundLoaded) {
		ofPushMatrix();
		ofDisableDepthTest();
		ofSetColor(50, 50, 50);
		ofScale(2, 2);
		backgroundImage.draw(-200, -100);
		ofEnableDepthTest();
		ofPopMatrix();
	}

	theCam->begin();
	ofPushMatrix();
	if (bWireframe) {                    // wireframe mode  (include axis)
		ofDisableLighting();
		ofSetColor(ofColor::slateGray);
		if (bLanderLoaded) {
			lander.drawWireframe();
		}
		if (terrainLoaded) {
			terrain.drawWireframe();
		}
	}
	else {
		ofEnableLighting();              // shaded mode
		if (bLanderLoaded) {
			lander.drawFaces();
		}
		if (terrainLoaded) {
			terrain.drawFaces();
		}
	}

	
	ofPopMatrix();
	theCam->end();

	// draw screen data
	//
	string str;
	str += "Frame Rate: " + std::to_string(ofGetFrameRate());
	ofSetColor(ofColor::white);
	ofDrawBitmapString(str, ofGetWindowWidth() - 170, 15);

	exhaust.draw();
}


// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));
	

	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}


void ofApp::keyPressed(int key) {

	switch (key) {
	case 'C':
	case 'c':
		if (cam.getMouseInputEnabled()) cam.disableMouseInput();
		else cam.enableMouseInput();
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		break;
	case 'P':
	case 'p':
		break;
	case 'r':
		cam.reset();
		break;
	case 's':
		savePicture();
		break;
	case 't':
		break;
	case 'u':
		break;
	case 'v':
		togglePointsDisplay();
		break;
	case 'V':
		break;
	case 'w':
		toggleWireframeMode();
		break;
	case OF_KEY_F1:
		theCam = &cam;
		break;
	case OF_KEY_F3:
		theCam = &topCam;
		break;
	case OF_KEY_ALT:
		cam.enableMouseInput();
		bAltKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
	case OF_KEY_UP:
		activateExhaust = true;
		if (!bCtrlKeyDown) {
			lem.sys->addForce(new ThrusterForce(ofVec3f(0, 1, 0)));
		}
		else {
			lem.sys->addForce(new ThrusterForce(ofVec3f(0, 0, 1)));
		}
		break;
	case OF_KEY_DOWN:
		activateExhaust = true;
		if (!bCtrlKeyDown) {
			lem.sys->addForce(new ThrusterForce(ofVec3f(0, -1, 0)));
		}
		else {
			lem.sys->addForce(new ThrusterForce(ofVec3f(0, 0, -1)));
		}
		break;
	case OF_KEY_LEFT:
		activateExhaust = true;
		if (!bCtrlKeyDown) {
			lem.sys->addForce(new ThrusterForce(ofVec3f(-1, 0, 0)));
		}
		else {
			lem.sys->addForce(new ThrusterForce(ofVec3f(0, 0, -1)));
		}
		break;
	case OF_KEY_RIGHT:
		activateExhaust = true;
		if (!bCtrlKeyDown) {
			lem.sys->addForce(new ThrusterForce(ofVec3f(1, 0, 0)));
		}
		else {
			lem.sys->addForce(new ThrusterForce(ofVec3f(0, 0, 1)));
		}
		break;
	default:
		break;
	}
}

void ofApp::toggleWireframeMode() {
	bWireframe = !bWireframe;
}


void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {

	switch (key) {
	
	case OF_KEY_ALT:
		cam.disableMouseInput();
		bAltKeyDown = false;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_UP:
		activateExhaust = false;
		lem.sys->addForce(new ThrusterForce(ofVec3f(0, -.1, 0)));
		break;
	case OF_KEY_DOWN:
		activateExhaust = false;
		lem.sys->addForce(new ThrusterForce(ofVec3f(0, .1, 0)));
		break;
	case OF_KEY_RIGHT:
		activateExhaust = false;
		lem.sys->addForce(new ThrusterForce(ofVec3f(-.1, 0, 0)));
		break;
	case OF_KEY_LEFT:
		activateExhaust = false;
		lem.sys->addForce(new ThrusterForce(ofVec3f(.1, 0, 0)));
		break;
	default:
		break;

	}
}


//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {


}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {


}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

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
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ .7f, .7f, .7f, 1.0f };

	static float position[] =
	{20.0, 20.0, 20.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
//	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
//	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
//	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
} 

void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
