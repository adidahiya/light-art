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
  gui.add(coefsNoveltyFactor.set("MFCC Coefs Novelty Factor", 4.0, 0.0001, 8.0));
  gui.add(bandsNoveltyFactor.set("MFCC Bands Novelty Factor", 1.0, 0.0001, 1.0));
  gui.add(hpcpNoveltyFactor.set("HPCP Novelty Factor", 1.0, 0.0001, 1.0));
}

//--------------------------------------------------------------
void ofApp::update(){
  // This will show us the framerate in the window's title bar
  if(showFps.get()){
    ofSetWindowTitle("Spatial Synesthesia - " + to_string(ofGetFrameRate()));
  }
  
  // this runs the analysis chain that's been declared inside ofxMLTK
  mltk.run();
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofBackground(0);
  
  Real rms = mltk.getValue("RMS");
  vector<Real> mfcc_bands = mltk.getData("MFCC.bands");
  vector<Real> mfcc_coefs = mltk.getData("MFCC.coefs");
  vector<Real> spectrum = mltk.getData("Spectrum");
  vector<Real> hpcp = mltk.getData("HPCP");
  
  // We figure out the width of the buckets by just dividing the screen width
  // by the number of values in our frame
  float spectrumBucketWidth = (ofGetWidth() / float(spectrum.size()));
  float mfccBandWidth = (ofGetWidth() / float(mfcc_bands.size()));
  float mfccCoefsWidth = (ofGetWidth() / float(mfcc_coefs.size()));
  float hpcpBandWidth = (ofGetWidth() / float(hpcp.size()));
  
  ofSetColor(graphColor.get());
  
  if (showBands) {
    for (int i = 0; i < mfcc_bands.size(); i++) {
      ofDrawRectangle(i * mfccBandWidth,
                      ofGetHeight(),
                      mfccBandWidth,
                      -ofMap(mfcc_bands[i]/rms, 0, 1.0, 0, ofGetHeight(), true) * bandsNoveltyFactor.get());
    }
  }
  
  ofPolyline line;
  
  ofSetLineWidth(6);
  for (int i = 0; i < mfcc_coefs.size(); i++) {
    line.lineTo(i * mfccCoefsWidth,
                ofGetHeight()/2 + ((mfcc_coefs[i]) * coefsNoveltyFactor.get()));
  }
  line.lineTo(ofGetWidth(), ofGetHeight()/2);
  if (showCoefs) {
    line.draw();
  }
  
  if (showHpcp) {
    // TODO: make bars non-overlapping w/ same colors
    for (int i = 0; i < hpcp.size(); i++) {
      ofDrawRectangle(i * hpcpBandWidth,
                      ofGetHeight(),
                      hpcpBandWidth,
                      -ofMap(hpcp[i], 0, 1.0, 0, ofGetHeight(), true) * hpcpNoveltyFactor.get());
    }
  }
  
  if (showGui) {
    gui.draw();
  }
}


//-----
void ofApp::audioIn(ofSoundBuffer &inBuffer){
  inBuffer.getChannel(mltk.ch1Buffer, 0);
  inBuffer.getChannel(mltk.ch2Buffer, 1);
  inBuffer.getChannel(mltk.ch3Buffer, 2);
  inBuffer.getChannel(mltk.ch4Buffer, 3);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  switch(key){
    case 'd':
      showGui = !showGui;
      break;
    case 'b':
      showBands = !showBands;
      break;
    case 'c':
      showCoefs = !showCoefs;
      break;
    case 'h':
      showHpcp = !showHpcp;
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
