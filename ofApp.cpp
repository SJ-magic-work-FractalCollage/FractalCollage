/************************************************************
************************************************************/
#include "ofApp.h"

/************************************************************
************************************************************/

/******************************
******************************/
ofApp::ofApp(int _CamId_0, int _CamId_1)
: b_DispGUI(true)
, Ave_Parts(0)
, now(0)
, LastINT(0)
, id_Parts(0)
, id_Mosaic(1)
, Draw_Contents(DRAW_CONTENTS__COLLAGE)
, png_id(0)
{
	CamId[0] = _CamId_0;
	CamId[1] = _CamId_1;
	
	fp_Log = fopen("../../../data/Log.csv", "w");
}

/******************************
******************************/
ofApp::~ofApp()
{
	if(fp_Log) fclose(fp_Log);
}

/******************************
******************************/
void ofApp::setup(){
	/********************
	********************/
	font.load("font/RictyDiminishedDiscord-Bold.ttf", 15, true, true, true);
	
	/********************
	********************/
	ofSetBackgroundAuto(true);
	
	ofSetWindowTitle("FractalCollage");
	ofSetVerticalSync(true);
	ofSetFrameRate(30);
	ofSetWindowShape(IMG_WIDTH, IMG_HEIGHT);
	ofSetEscapeQuitsApp(false);
	
	ofEnableAlphaBlending();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	// ofEnableBlendMode(OF_BLENDMODE_ADD);
	// ofEnableSmoothing();
	
	/********************
	********************/
	setup_Gui();
	
	/********************
	********************/
	setup_VideoCam();
	
	/********************
	********************/
	fbo_Parts.allocate(IMG_WIDTH, IMG_HEIGHT, GL_RGBA, 0);
	Clear_fbo(fbo_Parts);
	
	fbo_Parts_SmallGray.allocate(IMG_SMALL_WIDTH, IMG_SMALL_HEIGHT, GL_RGBA, 0);
	Clear_fbo(fbo_Parts_SmallGray);
	shader_Gray.load( "shader/Gray.vert", "shader/Gray.frag" );
	
	fbo_Mosaic.allocate(IMG_WIDTH, IMG_HEIGHT, GL_RGBA, 0);
	Clear_fbo(fbo_Mosaic);
	shader_Mosaic.load( "shader/Mosaic.vert", "shader/Mosaic.frag" );
	
	fbo_FractalCollage.allocate(IMG_WIDTH, IMG_HEIGHT, GL_RGBA, 0);
	Clear_fbo(fbo_FractalCollage);
	shader_FractalCollage.load( "shader/FractalCollage.vert", "shader/FractalCollage.frag" );
	
	/********************
	********************/
	SyphonServer_Video[0].setName("FractalCollage_Video0");
	SyphonServer_Video[1].setName("FractalCollage_Video1");
	SyphonServer_FractalCollage.setName("FractalCollage");
	
	/********************
	********************/
	load_and_start_backMusic();
}

/******************************
******************************/
void ofApp::load_and_start_backMusic()
{
	/********************
	load時間短縮のため、mp3->wav としておくこと : ffmpeg
		https://qiita.com/suzutsuki0220/items/43c87488b4684d3d15f6
		> ffmpeg -i "input.mp3" -vn -ac 2 -ar 44100 -acodec pcm_s16le -f wav "output.wav"
	********************/
	// sound.loadSound("sound/FractalZoom.wav");
	sound.loadSound("sound/SmokeWeed.wav");
	if(!sound.isLoaded()) { ERROR_MSG(); std::exit(1); }
	
	/********************
	********************/
	sound.setLoop(true);
	sound.setMultiPlay(true);
	sound.setVolume(1.0);
	
	sound.play();
	b_SoundPaused = true;
	sound.setPaused(b_SoundPaused);
}

/******************************
******************************/
void ofApp::Clear_fbo(ofFbo& fbo)
{
	fbo.begin();
	
	// Clear with alpha, so we can capture via syphon and composite elsewhere should we want.
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	ofClear(0, 0, 0, 0);
	
	fbo.end();
}

/******************************
******************************/
void ofApp::setup_Gui()
{
	/********************
	********************/
	gui.setup("Param", "gui.xml", 10, 10);
	
	/********************
	********************/
	gui.add(gui__y_ofs.setup("y_ofs", 0, 0, IMG_HEIGHT/2 - 1));
	gui.add(gui__AnimSpeed_PixPerSec.setup("AnimSpeed", 40, 0, 50));
	gui.add(gui__b_Anim.setup("b_Anim", false));
	gui.add(gui__MusicPos.setup("MusicPos", 0, 0, 1.0));
	
	/********************
	********************/
	gui.minimizeAll();
}

/******************************
******************************/
void ofApp::On_Drug_MusicSlider(){
	if(!b_SoundPaused){
		b_SoundPaused = true;
		sound.setPaused(b_SoundPaused);
	}
}

/******************************
******************************/
void ofApp::setup_VideoCam(){
	/********************
	********************/
	for(int i = 0; i < NUM_CAMS; i++){
		VideoCam[i] = new ofVideoGrabber;
		
		ofSetLogLevel(OF_LOG_VERBOSE);
		VideoCam[i]->setVerbose(true);
		
		vector< ofVideoDevice > Devices = VideoCam[i]->listDevices();// 上の行がないと、List表示されない.
		
		if(CamId[i] == -1){
			std::exit(1);
		}else{
			if(Devices.size() <= CamId[i]) { ERROR_MSG(); std::exit(1); }
			
			VideoCam[i]->setDeviceID(CamId[i]);
			VideoCam[i]->initGrabber(IMG_WIDTH, IMG_HEIGHT);
		}
	}
}

/******************************
******************************/
void ofApp::update(){
	/********************
	********************/
	now = ofGetElapsedTimef();
	
	/********************
	********************/
	ofSoundUpdate();
	
	if(!b_SoundPaused)	gui__MusicPos = sound.getPosition();
	else				sound.setPosition(gui__MusicPos);
	
	/********************
	********************/
	if(gui__b_Anim){
		gui__y_ofs = gui__y_ofs + gui__AnimSpeed_PixPerSec * (now - LastINT);
		if(IMG_HEIGHT/2 - 2 < gui__y_ofs){
			gui__y_ofs = 0;
			
			id_Parts	= CamId_Next(id_Parts); // 一つ送る.
			id_Mosaic	= CamId_Next(id_Parts); // id_Partsの次.
		}
	}
	
	/* */
	x_ofs = gui__y_ofs * IMG_WIDTH / IMG_HEIGHT;
	
	/********************
	********************/
	for(int i = 0; i < NUM_CAMS; i++){
		VideoCam[i]->update();
	}
	
	if(VideoCam[id_Parts]->isFrameNew())	update__fbo_Parts();
	if(VideoCam[id_Mosaic]->isFrameNew())	update__fbo_Mosaic();
	
}

/******************************
******************************/
int ofApp::CamId_Next(int _id)
{
	_id++;
	if(NUM_CAMS <= _id) _id = 0;
	
	return _id;
}

/******************************
******************************/
void ofApp::update__fbo_Parts()
{
	fbo_Parts.begin();
		/********************
		********************/
		// Clear with alpha, so we can capture via syphon and composite elsewhere should we want.
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		ofClear(0, 0, 0, 0);
		
		/********************
		********************/
		ofSetColor(255, 255, 255, 255);
		VideoCam[id_Parts]->draw(0, 0, fbo_Parts.getWidth(), fbo_Parts.getHeight());
	fbo_Parts.end();
}

/******************************
******************************/
void ofApp::update__fbo_Mosaic()
{
	/********************
	********************/
	fbo_Mosaic.begin();
		ofClear(0, 0, 0, 0);
		
		shader_Mosaic.begin();
			
			shader_Mosaic.setUniform1i( "w0", fbo_Mosaic.getWidth() );
			shader_Mosaic.setUniform1i( "h0", fbo_Mosaic.getHeight() );
			shader_Mosaic.setUniform1i( "x_ofs", int(x_ofs));
			shader_Mosaic.setUniform1i( "y_ofs", int(gui__y_ofs));
			
			ofSetColor( 255, 255, 255 );
			VideoCam[id_Mosaic]->draw(0, 0, fbo_Mosaic.getWidth(), fbo_Mosaic.getHeight());
		
		shader_Mosaic.end();		
	fbo_Mosaic.end();
}

/******************************
******************************/
void ofApp::draw_FractalCollage_to_fbo()
{
	fbo_FractalCollage.begin();
		ofClear(0, 0, 0, 0);
		
		shader_FractalCollage.begin();
			
			shader_FractalCollage.setUniform1i( "w0", fbo_FractalCollage.getWidth() );
			shader_FractalCollage.setUniform1i( "h0", fbo_FractalCollage.getHeight() );
			shader_FractalCollage.setUniform1i( "x_ofs", int(x_ofs));
			shader_FractalCollage.setUniform1i( "y_ofs", int(gui__y_ofs));
			
			shader_FractalCollage.setUniform1f( "AveParts", Ave_Parts);
			
			if(Draw_Contents == DRAW_CONTENTS__SPLIT)	shader_FractalCollage.setUniform1i( "b_Enable_MosaicMix", false);
			else										shader_FractalCollage.setUniform1i( "b_Enable_MosaicMix", true);
			
			shader_FractalCollage.setUniformTexture( "texture_1", fbo_Mosaic.getTexture(), 1);
			
			ofSetColor( 255, 255, 255 );
			fbo_Parts.draw( 0, 0 );
		
		shader_FractalCollage.end();		
	fbo_FractalCollage.end();
}

/******************************
******************************/
void ofApp::cal_AveParts()
{
	/********************
	処理時間 短縮のため、一旦small image(Gray)に描画し、
	このimageの平均値を算出.
	********************/
	fbo_Parts_SmallGray.begin();
	shader_Gray.begin();
		ofClear(0, 0, 0, 0);
		fbo_Parts.draw(0, 0, fbo_Parts_SmallGray.getWidth(), fbo_Parts_SmallGray.getHeight());
		
	shader_Gray.end();
	fbo_Parts_SmallGray.end();
	
	/********************
	********************/
	fbo_Parts_SmallGray.readToPixels(pix_Parts);
	// FastFboReader.readToPixels(fbo_Parts_SmallGray, pix_Parts);
	
	Ave_Parts = 0;
	int num = 0;
	for(int y = 0; y < pix_Parts.getHeight(); y++){
		for(int x = 0; x < pix_Parts.getWidth(); x++){
			ofColor color = pix_Parts.getColor(x, y);
			// float val = 0.299 * float(color.r)/255 + 0.587 * float(color.g)/255 + 0.114 * float(color.b)/255;
			float val = float(color.r)/255;
			Ave_Parts += val;
			num++;
		}
	}
	
	Ave_Parts /= num;
}

/******************************
******************************/
void ofApp::draw(){
	/********************
	********************/
	if(now < 10.0) fprintf(fp_Log, "%f,", now);
	
	/********************
	********************/
	cal_AveParts();
	draw_FractalCollage_to_fbo();
	
	/********************
	********************/
	ofClear(0, 0, 0, 0);
	
	ofSetColor(255, 255, 255, 255);
	switch(Draw_Contents){
		case DRAW_CONTENTS__CAM0:
			VideoCam[0]->draw(0, 0, ofGetWidth(), ofGetHeight());
			break;
			
		case DRAW_CONTENTS__CAM1:
			VideoCam[1]->draw(0, 0, ofGetWidth(), ofGetHeight());
			break;
			
		case DRAW_CONTENTS__MOSAIC:
			fbo_Mosaic.draw(0, 0, ofGetWidth(), ofGetHeight());
			break;
			
		case DRAW_CONTENTS__SPLIT:
			fbo_FractalCollage.draw(0, 0, ofGetWidth(), ofGetHeight());
			break;
			
		case DRAW_CONTENTS__COLLAGE:
			fbo_FractalCollage.draw(0, 0, ofGetWidth(), ofGetHeight());
			break;
	}
	
	// fbo_Parts.draw(0, 0, ofGetWidth(), ofGetHeight());
	// fbo_Mosaic.draw(0, 0, ofGetWidth(), ofGetHeight());
	// fbo_Parts_SmallGray.draw(0, 0);
	
	/********************
	********************/
	publish_syphon();
	
	/********************
	********************/
	if(b_DispGUI){
		gui.draw();
	
		ofSetColor(255, 255, 255, 255);
		char buf[BUF_SIZE_S];
		if(!b_SoundPaused){
			sprintf(buf, "%5.1f\n%5.3f\nParts, Mosaic = (%2d, %2d)\nmusic = %5.3f", ofGetFrameRate(), Ave_Parts, id_Parts, id_Mosaic, sound.getPosition());
		}else{
			sprintf(buf, "%5.1f\n%5.3f\nParts, Mosaic = (%2d, %2d)", ofGetFrameRate(), Ave_Parts, id_Parts, id_Mosaic);
		}
		
		font.drawString(buf, ofGetWidth() - font.stringWidth(buf) * 1.2, 50);
	}
	
	/********************
	********************/
	if(now < 10.0) fprintf(fp_Log, "%f\n", ofGetElapsedTimef());
	
	LastINT = now;
}

/******************************
******************************/
void ofApp::publish_syphon()
{
	for(int i = 0; i < NUM_CAMS; i++){
		ofTexture tex = VideoCam[i]->getTextureReference();
		SyphonServer_Video[i].publishTexture(&tex);
	}
	
	{
		ofTexture tex = fbo_FractalCollage.getTextureReference();
		SyphonServer_FractalCollage.publishTexture(&tex);
	}
}

/******************************
******************************/
void ofApp::keyPressed(int key){
	switch(key){
		case '0':
			Draw_Contents = DRAW_CONTENTS__CAM0;
			break;
			
		case '1':
			Draw_Contents = DRAW_CONTENTS__CAM1;
			break;
			
		case '2':
			Draw_Contents = DRAW_CONTENTS__MOSAIC;
			break;
			
		case '3':
			Draw_Contents = DRAW_CONTENTS__SPLIT;
			break;
			
		case '4':
			Draw_Contents = DRAW_CONTENTS__COLLAGE;
			break;
			
		case 'd':
			b_DispGUI = !b_DispGUI;
			break;
			
		case 'p':
			b_SoundPaused = !b_SoundPaused;
			sound.setPaused(b_SoundPaused);
			break;
			
		case ' ':
			{
				char buf[BUF_SIZE_S];
				
				sprintf(buf, "image_%d.png", png_id);
				ofSaveScreen(buf);
				// ofSaveFrame();
				printf("> %s saved\n", buf);
				
				png_id++;
			}
			break;
			
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

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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
