#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( int argc, char** argv ){
	ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context

	if(argc < 3){
		printf("exe [CamId_0] [CamId_1]\n");
		
		int CamId_0 = -1;
		int CamId_1 = -1;
		ofRunApp(new ofApp(CamId_0, CamId_1));
		
	}else{
		int CamId_0 = atoi(argv[1]);
		if(CamId_0 < 0) CamId_0 = 0;
		
		int CamId_1 = atoi(argv[2]);
		if(CamId_1 < 0) CamId_1 = 0;
		
		if(CamId_0 == CamId_1){
			printf("Cam 0 and 1 must be different\n");
		}else{
			ofRunApp(new ofApp(CamId_0, CamId_1));
		}
	}
}
