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
  
  // set up aggregation
  const char *stats[4] = { "mean", "var", "min", "max" };
  standard::Algorithm* aggr = standard::AlgorithmFactory::create("PoolAggregator",
                                                                 "defaultStats", arrayToVector<string>(stats));

  // Let's make ourselves some GUI
  gui.setup("Press 'd' to toggle.");
  
  gui.add(showFps.set("Show Framerate In Title", true));
  gui.add(graphColor.set("Graph Color", ofColor(240,240,255)));
//  gui.add(coefsNoveltyFactor.set("MFCC Coefs Novelty Factor", 4.0, 0.0001, 8.0));
  gui.add(bandsNoveltyFactor.set("MFCC Bands Novelty Factor", 1.0, 0.0001, 1.0));
  gui.add(hpcpNoveltyFactor.set("HPCP Novelty Factor", 1.0, 0.0001, 1.0));
  
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
  
  Real rms = mltk.getValue("RMS", activeChannel);
  vector<Real> mfcc_bands = mltk.getData("MFCC.bands", activeChannel);
  vector<Real> spectrum = mltk.getData("Spectrum", activeChannel);
  
  // We figure out the width of the buckets by just dividing the screen width
  // by the number of values in our frame
  float spectrumBucketWidth = (ofGetWidth() / float(spectrum.size()));
  float mfccBandWidth = (ofGetWidth() / float(mfcc_bands.size()));

  ofSetColor(graphColor.get());

  if (showBands) {
    for (int i = 0; i < mfcc_bands.size(); i++) {
      ofDrawRectangle(i * mfccBandWidth,
                      ofGetHeight(),
                      mfccBandWidth,
                      -ofMap(mfcc_bands[i]/rms, 0, 1.0, 0, ofGetHeight(), true) * bandsNoveltyFactor.get());
    }
  }


  // Syphon Stuff
  
  mClient.draw(50, 50);
  mainOutputSyphonServer.publishScreen();

  
  if (showGui) {
    gui.draw();
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
