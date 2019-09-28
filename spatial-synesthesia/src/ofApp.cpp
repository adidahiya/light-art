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
  
  mainOutputSyphonServer.setName("Screen Output");
  individualTextureSyphonServer.setName("Texture Output");
  mClient.setup();
  //using Syphon app Simple Server, found at http://syphon.v002.info/
  mClient.set("","Simple Server");
//  tex.allocate(200, 100, GL_RGBA);
  ofSetFrameRate(60);

//  memset(dmxData, 0, DMX_DATA_LENGTH);
//  dmxInterface = ofxGenericDmx::createDevice(DmxDevice::DMX_DEVICE_RAW);
//  bool opened = dmxInterface->open();
//  if (dmxInterface == 0 || !opened) {
//    ofLog(OF_LOG_WARNING, "No FTDI Device Found\n" );
//  } else {
//    ofLog(OF_LOG_NOTICE, "isOpen: %i\n", dmxInterface->isOpen());
//  }
}

//--------------------------------------------------------------
void ofApp::update(){
  // This will show us the framerate in the window's title bar
  if(showFps.get()){
    ofSetWindowTitle("Spatial Synesthesia - " + to_string(ofGetFrameRate()));
  }
  
  // this runs the analysis chain that's been declared inside ofxMLTK
  mltk.run();
//
//  setColorsToSend();
//
//  dmxData[1] = int(red);
//  dmxData[2] = int(green);
//  dmxData[3] = int(blue);
//  // force first byte to zero (it is not a channel but DMX type info - start code)
//  dmxData[0] = 0;
//
//  if (!dmxInterface || !dmxInterface->isOpen()) {
//    // noop
//  } else {
//    //send the data to the dmx interface
//    dmxInterface->writeDmx(dmxData, DMX_DATA_LENGTH);
//  }
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
    for (int i = 0; i < hpcp.size(); i++) {
      ofDrawRectangle(i * hpcpBandWidth,
                      ofGetHeight(),
                      hpcpBandWidth,
                      -ofMap(hpcp[i], 0, 1.0, 0, ofGetHeight(), true) * hpcpNoveltyFactor.get());
    }
  }


  // Syphon Stuff
  
  mClient.draw(50, 50);
  mainOutputSyphonServer.publishScreen();

  
  if (showGui) {
    gui.draw();
  }
}


//void ofApp::setColorsToSend() {
//  red = 0;
//  green = 100;
//  blue = 50;
//}


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

//--------------------------------------------------------------
void ofApp::exit() {
//  if (dmxInterface && dmxInterface->isOpen()) {
//    // send all zeros (black) to every dmx channel and close!
//    for (int i = 0; i <= DMX_DATA_LENGTH; i++) {
//      dmxData[i] = 0;
//    }
//
//    dmxInterface->writeDmx(dmxData, DMX_DATA_LENGTH);
//    dmxInterface->close();
//  }
}
