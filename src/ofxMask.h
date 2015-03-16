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
	void setup(int width, int height, Type type);

	void beginMask(bool clear=true);
	void endMask();
	void clearMask();

	void begin(bool clear=true);
	void end();

	void draw();
	
	void drawMasker();
	void drawMaskee();
    
    ofFbo* getMasker();
    ofFbo* getMaskee();
    
    int getWidth();
    int getHeight();
    
    void setInvertMask(bool invert);

private:
	ofFbo masker_, maskee_;
	ofShader shader_;
	float vertices_[8];
	float tex_coords_[8];
    int width;
    int height;
    bool invert;
};

/* EOF */