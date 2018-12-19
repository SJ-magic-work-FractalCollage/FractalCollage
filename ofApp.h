/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"
#include "ofxGui.h"
#include "ofxSyphon.h"
#include "ofxFastFboReader.h"

#include "sj_common.h"


/************************************************************
************************************************************/

/**************************************************
**************************************************/
class ofApp : public ofBaseApp{
private:
	/****************************************
	****************************************/
	enum{
		IMG_WIDTH	= 1280,
		IMG_HEIGHT	= 720,
		
		IMG_SMALL_WIDTH		= 320,
		IMG_SMALL_HEIGHT	= 180,
	};
	
	enum{
		BUF_SIZE_S = 500,
		BUF_SIZE_M = 1000,
		BUF_SIZE_L = 6000,
	};
	
	enum{
		NUM_CAMS = 2,
	};
	
	enum DRAW_CONTENTS{
		DRAW_CONTENTS__CAM0,
		DRAW_CONTENTS__CAM1,
		DRAW_CONTENTS__COLLAGE,
	};
	
	/****************************************
	****************************************/
	/********************
	********************/
	float now;
	float LastINT;
	
	/********************
	********************/
	ofSoundPlayer sound;
	bool b_SoundPaused;
	
	/********************
	********************/
	ofxPanel gui;
	ofxFloatSlider gui__y_ofs;
	float x_ofs;
	ofxFloatSlider gui__AnimSpeed_PixPerSec;
	ofxToggle gui__b_Anim;
	ofxFloatSlider gui__MusicPos;
	
	bool b_DispGUI;

	/********************
	********************/
	ofTrueTypeFont font;
	
	/********************
	********************/
	int CamId[NUM_CAMS];
	ofVideoGrabber *VideoCam[NUM_CAMS];
	ofxSyphonServer SyphonServer_Video[NUM_CAMS];

	int id_Parts;
	int id_Mosaic;
	
	DRAW_CONTENTS Draw_Contents;
	
	/********************
	********************/
	ofFbo fbo_Parts;
	
	ofFbo fbo_Parts_SmallGray;
	ofPixels pix_Parts;
	ofShader shader_Gray;
	float Ave_Parts;
	ofxFastFboReader FastFboReader;
	
	/********************
	********************/
	ofFbo fbo_Mosaic;
	ofShader shader_Mosaic;
	
	/********************
	********************/
	ofFbo fbo_FractalCollage;
	ofShader shader_FractalCollage;
	ofxSyphonServer SyphonServer_FractalCollage;
	
	/****************************************
	****************************************/
	void setup_VideoCam();
	void setup_Gui();
	void update__fbo_Parts();
	void Clear_fbo(ofFbo& fbo);
	void update__fbo_Mosaic();
	void draw_FractalCollage_to_fbo();
	void cal_AveParts();
	int CamId_Next(int _id);
	void load_and_start_backMusic();
	void On_Drug_MusicSlider();
	void publish_syphon();

public:
	/****************************************
	****************************************/
	ofApp(int _CamId_0, int _CamId_1);
	~ofApp();

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
	
};
