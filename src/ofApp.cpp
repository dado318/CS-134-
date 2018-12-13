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
	landerScale = 0.5f;
	if (lander.loadModel("geo/lander.obj")) {
		lander.setScaleNormalization(false);
		lander.setScale(landerScale, landerScale, landerScale);
		lander.setRotation(0, -180, 1, 0, 0);
		lander.setPosition(0,0,0);

		bLanderLoaded = true;
	}
	else {
		cout << "Error: Can't load model" << "geo/lander.obj" << endl;
		ofExit(0);
	}
	landerBox = meshBounds(lander.getMesh(0));
	if (terrain.loadModel("geo/terrain_1.obj")) {
		terrain.setScaleNormalization(false);
		terrain.setScale(1, 1, 1);
		terrain.setRotation(0, 0, 1, 0, 0);
		terrain.setPosition(0, -2, 0);
		terrainLoaded = true;
	}
	else {
		cout << "Can't load model: " << "geo/terrain_1.obj" << endl;
		ofExit(0);
	}
	
	meshTerrain = terrain.getMesh(0);
	numLevel = 2;
	octree.create(meshTerrain, numLevel);

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

	minDist = FLT_MAX;
}

Box ofApp::meshBounds(const ofMesh &mesh)
{
	int n = mesh.getNumVertices();
	ofVec3f v = mesh.getVertex(0);
	ofVec3f max = v;
	ofVec3f min = v;
	for (int i = 1; i < n; i++) {
		ofVec3f v = mesh.getVertex(i);

		if (v.x > max.x) max.x = v.x;
		else if (v.x < min.x) min.x = v.x;

		if (v.y > max.y) max.y = v.y;
		else if (v.y < min.y) min.y = v.y;

		if (v.z > max.z) max.z = v.z;
		else if (v.z < min.z) min.z = v.z;
	}
	return Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
}

bool ofApp::collisionDetect()
{
	Vector3 c = landerBox.center();
	ofPoint posLander = lander.getPosition();
	ofPoint posTerrain = terrain.getPosition();
	Vector3 o = Vector3(c.x() + posLander.x, c.y() - landerBox.height() / 2 * landerScale + posLander.y, c.z() + posLander.z);
	Ray ray(o, Vector3(0.f, 1.f, 0.f));

	TreeNode hitNode;
	
	if (octree.intersect(ray, octree.root, hitNode)) {
		int count = hitNode.points.size();
		for (int i = 0; i < count; i++) {
			ofVec3f vert = meshTerrain.getVertex(hitNode.points[i]);
			vert += posTerrain;
			float distance = sqrtf((vert.x - o.x()) * (vert.x - o.x()) + (vert.y - o.y()) * (vert.y - o.y()) + (vert.z - o.z()) * (vert.z - o.z()));
//			if (minDist > distance) {
//				minDist = distance;
//			}
			if (distance < COLLISION_DISTANCE) {
				return true;
			}
		}
	}
//	cout << "Min Distance:" << minDist << endl;
	return false;
}

void ofApp::update() {
	// Added: Updates lem, lander, and exhaust position based on lem's first launched particle
	if (collisionDetect()) {
		return;
	}
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

	//ofNoFill();
	//ofSetColor(ofColor::white);
	//octree.draw(numLevel, 0);
	
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
	{0.0, -20.0, 0.0, 0.0 };

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
