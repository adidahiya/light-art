#pragma once

#include "ofMain.h"
#include "ofMath.h"
#include "ofxGui.h"
#include "ofxMLTK.h"
#include "ofxSyphon.h"
#include "ofxEasing.h"

#define DMX_DATA_LENGTH 196

class ofApp : public ofBaseApp {
public:
  bool showGui = true;
  bool showBands = true;
  bool showCoefs = true;
  bool showHpcp = false;
  // -1 = mono aggregate
  int activeChannel = -1;
  
  int numberOfOutputChannels = 2;
  // should match the same variable in ofMLTK.h
  int numberOfInputChannels = 4;
  int sampleRate = 44100;
  int frameSize = 1024;
  int numberOfBuffers = 4;
  int numPixelsPerChannel = 60;
  // changing this controls the "speed" of the pixel animation
  int numFramesPerPixel = 4;

  ofColor backgroundColor;
  ofMutex mutex;
  MLTK mltk;

  Pool aggrPool;

  ofxPanel gui;
  ofParameter<bool> showFps;
  ofParameter<float> bandsNoveltyFactor;
  ofParameter<float> brightnessFactor;

  ofSoundStream soundStream;

  // pixel strip colors for each channel
  vector<deque<ofColor>> pixelColors;
  
  // for each pixel, we accumulate some # of frames which are averaged together to get the final pixel value
  vector<vector<ofColor>> currentPixelFrames;

  void audioIn(ofSoundBuffer &inBuffer);
  void audioOut(ofSoundBuffer &outBuffer);

  ofTexture tex;
  ofxSyphonServer mainOutputSyphonServer;
  ofxSyphonServer individualTextureSyphonServer;
  ofxSyphonClient mClient;
  
  // These are the built-ins
  void setup();
  void update();
  void draw();
  void exit();
  
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
  
private:
  bool hasTimelineReachedEnd = false;
  // flush these to audioOut() once hasTimelineReachedEnd == true
  queue<ofSoundBuffer> delayedOutputBuffers;
  void fillPixelColorsFromIncomingAudio();
  void drawPixelColors();
  void drawLiveMFCCBands();
};
