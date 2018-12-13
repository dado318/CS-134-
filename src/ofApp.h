//
//   CS 134 - Midterm Starter File - Fall 2018
//
//
//   This file contains all the necessary startup code for the Midterm problem Part II
//   Please make sure to you the required data files installed in your $OF/data directory.
//
//                                             (c) Kevin M. Smith  - 2018
#pragma once

#include "ofMain.h"
#include  "ofxAssimpModelLoader.h"
#include "ofxGui.h"
#include "ParticleSystem.h"
#include "ParticleEmitter.h"
#include "box.h"
#include "ray.h"
#include "Octree.h"

// Davor Koret and Galen Rivoire
// CS 134-01
// Final Game Project


class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void drawAxis(ofVec3f);
		void initLightingAndMaterials();
		void savePicture();
		void toggleWireframeMode();
		void togglePointsDisplay();

		ofxAssimpModelLoader lander;
		ofLight light;
		ofImage backgroundImage;

		bool bAltKeyDown;
		bool bCtrlKeyDown;
		bool bWireframe;
		bool bDisplayPoints;
	
		bool bBackgroundLoaded = false;
		bool bLanderLoaded = false;
        bool bBackgroundSound = false;
        ofSoundPlayer bkgroundSound;
        ofSoundPlayer exhaustAudio;
	
		// My added elements
		ParticleEmitter lem;
		ParticleEmitter exhaust;
		bool activateExhaust;
    
        // gui
        ofxPanel gui;
        ofxLabel labelAltitude;
        float altitude;

		ofxAssimpModelLoader terrain;
		bool terrainLoaded = false;

		Octree octree;
		int numLevel;
    
		Box landerBox; // lander's box
		float landerScale;

		ofMesh meshTerrain;
		Box meshBounds(const ofMesh &mesh);
		bool collisionDetect();
    
        // Camera Additions
        ofCamera trackingCam;
        ofCamera landerFront;
        ofCamera landerDown;
        ofEasyCam cam;
        ofCamera *theCam = NULL;
        ofCamera topCam;
};
