#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
  // clear any leftovers from the memory
  ofClear(ofColor::black);

  // VerticalSync helps to prevent tearing but locks the framerate at the screen
  // refresh rate.
  ofSetVerticalSync(false);
  ofSetFrameRate(60);
  ofSetBackgroundAuto(false);
  ofSetWindowTitle("Spatial Synesthesia - ");

  // setup the audio stream
  soundStream.setup(numberOfOutputChannels, numberOfInputChannels, sampleRate, frameSize, numberOfBuffers);

  mltk.setup(frameSize, sampleRate, frameSize/2);
  mltk.run();

  pixelColors.resize(numberOfInputChannels);
  
  // set up aggregation
  const char *stats[4] = { "mean", "var", "min", "max" };
  standard::Algorithm* aggr = standard::AlgorithmFactory::create("PoolAggregator",
                                                                 "defaultStats", arrayToVector<string>(stats));

  // Let's make ourselves some GUI
  gui.setup("Press 'd' to toggle.");
  
  gui.add(showFps.set("Show Framerate In Title", true));
  gui.add(bandsNoveltyFactor.set("MFCC Bands Novelty Factor", 1.0, 0.0001, 1.0));
  gui.add(brightnessFactor.set("brightness factor", 1.0, 0.0001, 1.0));

  mainOutputSyphonServer.setName("Screen Output");
  individualTextureSyphonServer.setName("Texture Output");
  mClient.setup();
  // using Syphon app Simple Server, found at http://syphon.v002.info/
  mClient.set("","Simple Server");
//  tex.allocate(200, 100, GL_RGBA);
  ofSetFrameRate(60);
}

//--------------------------------------------------------------
void ofApp::update(){
  // show framerate in window title bar
  if (showFps.get()) {
    ofSetWindowTitle("Spatial Synesthesia - " + to_string(ofGetFrameRate()));
  }

  mltk.run();
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofBackground(0);

  fillPixelColorsFromIncomingAudio();
  
  drawPixelColors();

  // Syphon Stuff
  mClient.draw(50, 50);
  mainOutputSyphonServer.publishScreen();

  if (showGui) {
    gui.draw();
  }
}

void ofApp::fillPixelColorsFromIncomingAudio() {
  if (ofGetFrameNum() % 4 > 0) {
    return;
  }
  
  for (int i = 0; i < numberOfInputChannels; i++) {
    Real rms = mltk.getValue("RMS", i);
    vector<Real> mfcc_bands = mltk.getData("MFCC.bands", i);

    ofColor newPixelColor = ofColor();
    newPixelColor.setHsb(0, 0, 0);
    for (int j = 0; j < mfcc_bands.size(); j++) {
      ofColor contributingBandColor = ofColor();
      float hue = ofMap(j, 0, mfcc_bands.size(), 0, 255, true);
      float brightness = ofMap(mfcc_bands[j]/rms, 0, 1.0, 0, 40 * brightnessFactor.get(), true);
      contributingBandColor.setHsb(hue, 240, brightness);
      newPixelColor += contributingBandColor;
    }
//    ofColor red = ofColor();
//    red.setHsb(10, 100, 10);
//    ofColor blue = ofColor();
//    blue.setHsb(100, 100, 10);
//    newPixelColor += red;
//    newPixelColor += blue;

    pixelColors[i].push_front(newPixelColor);
    
    if (pixelColors[i].size() > numPixelsPerChannel) {
      pixelColors[i].resize(numPixelsPerChannel);
    }
  }
}

void ofApp::drawPixelColors() {
  int windowWidth = ofGetWidth();
  int windowHeight = ofGetHeight();
  float bucketWidth = windowWidth / numberOfInputChannels;

  // draw the pixel colors to the screen
  for (int i = 0; i < pixelColors.size(); i++) {
    float bucketHeight = windowHeight / pixelColors[i].size();
    for (int j = 0; j < pixelColors[i].size(); j++) {
      ofSetColor(pixelColors[i][j]);
      ofDrawRectangle(i * bucketWidth, j * bucketHeight, bucketWidth, bucketHeight);
    }
  }
}

void ofApp::drawLiveMFCCBands() {
  int windowWidth = ofGetWidth();
  int windowHeight = ofGetHeight();
  float bucketWidth = windowWidth / numberOfInputChannels;

  for (int i = 0; i < numberOfInputChannels; i++) {
    ofNoFill();
    ofSetColor(255, 255, 255);
    ofDrawRectangle(i * bucketWidth, 0, bucketWidth, windowHeight);
    ofFill();

    Real rms = mltk.getValue("RMS", i);
    vector<Real> mfcc_bands = mltk.getData("MFCC.bands", i);
    vector<Real> spectrum = mltk.getData("Spectrum", i);
    float bucketX = i * bucketWidth;
    float bucketHeight = windowHeight / mfcc_bands.size();

    for (int j = 0; j < mfcc_bands.size(); j++) {
      float bucketY = j * bucketHeight;
      ofColor channelBucketColor = ofColor();
      float hue = ofMap(j, 0, mfcc_bands.size(), 0, 255, true);
      float brightness = ofMap(mfcc_bands[j]/rms * 10, 0, 1.0, 0, 255, true);
      channelBucketColor.setHsb(hue, 200, brightness);
      ofSetColor(channelBucketColor);
      ofDrawRectangle(bucketX, bucketY, bucketWidth - 1, bucketHeight);
    }
  }
}


//-----
void ofApp::audioIn(ofSoundBuffer &inBuffer){
  for (int i = 0; i < numberOfInputChannels; i++) {
    inBuffer.getChannel(mltk.channelSoundBuffers[i], i);
  }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  switch(key){
    case 'd':
      showGui = !showGui;
      break;
    case 'q':
      activeChannel = -1;
      break;
    case 'w':
      activeChannel = 0;
      break;
    case 'e':
      activeChannel = 1;
      break;
    case 'r':
      activeChannel = 2;
      break;
    case 't':
      activeChannel = 3;
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

//--------------------------------------------------------------
void ofApp::exit() {

}
