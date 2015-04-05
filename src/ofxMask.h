#pragma once

#include "ofFbo.h"
#include "ofShader.h"

class ofxMask
{
public:
	enum Type
	{
		ALPHA,
		LUMINANCE
	};
    
    void setup(ofFbo::Settings s, Type type, bool useABMaskees = false);
	void setup(int width, int height, GLint internalFormat, Type type, bool useABMaskees = false);

	void beginMask(bool clear=true);
	void endMask();
	void clearMask();

	void begin(bool clear=true);
	void end();
    
    void beginA(bool clear=true);
    void endA();
    
    void beginB(bool clear=true);
    void endB();

	void draw();
	
	void drawMasker();
	void drawMaskee();
    void drawMaskeeA();
    void drawMaskeeB();
    
    ofFbo* getMasker();
    ofFbo* getMaskee();
    ofFbo* getMaskeeA();
    ofFbo* getMaskeeB();
    
    int getWidth();
    int getHeight();
    void invertMask(bool invert);

private:
	ofFbo masker_, maskeeA_, maskeeB_;
	ofShader shader_;
	float vertices_[8];
	float tex_coords_[8];
    bool invert;
    bool useABMaskees;
//    GLint internalFormat;
    Type type;
    void beginMaskee(ofFbo* maskee, bool clear = true);
    void endMaskee(ofFbo* maskee);
    
    ofFbo::Settings fboS;
};

/* EOF */