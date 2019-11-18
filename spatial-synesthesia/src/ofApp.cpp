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
  for (int i = 0; i < numberOfInputChannels; i++) {
    pixelColors[i].assign(numPixelsPerChannel, ofColor::black);
  }
  currentPixelFrames.resize(numberOfInputChannels);
  for (int i = 0; i < NUM_EMPTY_OUTPUT_BUFFERS; i++) {
    delayedOutputBuffers.push(ofSoundBuffer());
  }
  
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
  fillPixelColorsFromIncomingAudio();
  // hack, confusing logic
  if (ofGetFrameNum() > numFramesPerPixel * numPixelsPerChannel) {
    hasTimelineReachedEnd = true;
  }
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofBackground(0);
  
  drawPixelColors();

  // Syphon Stuff
  mClient.draw(50, 50);
  mainOutputSyphonServer.publishScreen();

  if (showGui) {
    gui.draw();
  }
}

void ofApp::fillPixelColorsFromIncomingAudio() {
  for (int i = 0; i < numberOfInputChannels; i++) {
    const Real rms = mltk.getValue("RMS", i);

    // MFCC
    vector<Real> mfcc_bands = mltk.getData("MFCC.bands", i);
    tuple<float, float> mfcc_normalized_range = make_tuple(0, 1.0);

//    // GFCC
//    vector<Real> gfcc_bands = mltk.getMeanData("GFCC.bands", i);
//    tuple<float, float> gfcc_normalized_range = make_tuple(0, 1.0);
//
//    // BFCC
//    vector<Real> bfcc_bands = mltk.getMeanData("BFCC.bands", i);
//    tuple<float, float> bfcc_normalized_range = make_tuple(0, 1.0);

    // ConstantQ
    vector<Real> constantq = mltk.getData("ConstantQ.magnitude", i);
    tuple<float, float> constantq_range = make_tuple(minCQ, maxCQ);

    vector<Real> &results = mfcc_bands;
    tuple<float, float> &results_range = mfcc_normalized_range;
    if (whichAlgorithm == "mfcc") {
      results = mfcc_bands;
      results_range = mfcc_normalized_range;
//    } else if (whichAlgorithm == "gfcc") {
//      results = gfcc_bands;
//      results_range = gfcc_normalized_range;
//    } else if (whichAlgorithm == "bfcc") {
//      results = bfcc_bands;
//      results_range = bfcc_normalized_range;
    } else if (whichAlgorithm == "constantq") {
      results = constantq;
      results_range = constantq_range;
    }

    ofColor framePixelColor = ofColor();
    framePixelColor.setHsb(0, 0, 0);
    for (int j = 0; j < results.size(); j++) {
      ofColor contributingBandColor = ofColor();
      
      const float hue = ofxeasing::map(j, 0, results.size(), 0, 255, ofxeasing::linear::easeOut);

      Real result = results[j];
      if (whichAlgorithm == "mfcc" || whichAlgorithm == "gfcc" || whichAlgorithm == "bfcc") {
        result /= rms;
      }
      if (whichAlgorithm == "constantq") {
        if (result < minCQ) {
          minCQ = result;
          get<0>(results_range) = minCQ;
        } else if (result > maxCQ) {
          maxCQ = result;
          get<1>(results_range) = maxCQ;
        }
      }

      const float brightness = ofMap(result, get<0>(results_range), get<1>(results_range), 0, 40 * brightnessFactor.get(), true);
      
      contributingBandColor.setHsb(hue, 240, brightness);
      framePixelColor += contributingBandColor;
    }


    if (currentPixelFrames[i].size() < numFramesPerPixel) {
      currentPixelFrames[i].push_back(framePixelColor);
    } else {
      ofColor newPixelColor = ofColor();
      for (int k = 0; k < numFramesPerPixel; k++) {
        newPixelColor += currentPixelFrames[i][k];
      }
      // commit new pixel to the timeline
      pixelColors[i].push_front(framePixelColor);
      if (pixelColors[i].size() > numPixelsPerChannel) {
        pixelColors[i].resize(numPixelsPerChannel);
      }
      
      // clear out our "buffer" of frames for the current pixel
      currentPixelFrames[i].resize(0);
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
void ofApp::audioIn(ofSoundBuffer &inBuffer) {
  for (int i = 0; i < numberOfInputChannels; i++) {
    // send to MLTK for analysis
    inBuffer.getChannel(mltk.channelSoundBuffers[i], i);
  }

  // save for audio output later
  ofSoundBuffer inBufferCopy = ofSoundBuffer();
  inBufferCopy.allocate(sampleRate, numberOfInputChannels);
  inBuffer.copyTo(inBufferCopy);
  delayedOutputBuffers.push(inBufferCopy);
}

void ofApp::audioOut(ofSoundBuffer &outBuffer) {
  if (hasTimelineReachedEnd) {
    ofSoundBuffer& bufferToFlush = delayedOutputBuffers.front();
    bufferToFlush.copyTo(outBuffer);
    delayedOutputBuffers.pop();
  }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  switch(key){
    case 'd':
      showGui = !showGui;
      break;
    case 'm':
      whichAlgorithm = "mfcc";
      break;
    case 'c':
      whichAlgorithm = "constantq";
      break;
    // the rest are unsupported for now
//    case 'b':
//      whichAlgorithm = "bfcc";
//      break;
//    case 'g':
//      whichAlgorithm = "gfcc";
//      break;
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
