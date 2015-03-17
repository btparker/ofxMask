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
	void setup(int width, int height, GLint internalFormat, Type type, bool useSecondMaskee = false);

	void beginMask(bool clear=true);
	void endMask();
	void clearMask();

	void begin(bool clear=true);
	void end();
    
    void beginSecond(bool clear=true);
    void endSecond();

	void draw();
	
	void drawMasker();
	void drawMaskee();
    void drawSecondMaskee();
    
    ofFbo* getMasker();
    ofFbo* getMaskee();
    ofFbo* getSecondMaskee();
    
    int getWidth();
    int getHeight();
    void setUseSecondMaskee(bool useSecondMaskee);
    void setInvertMask(bool invert);

private:
	ofFbo masker_, maskee_, secondMaskee_;
	ofShader shader_;
	float vertices_[8];
	float tex_coords_[8];
    int width;
    int height;
    bool invert;
    bool useSecondMaskee;
    GLint internalFormat;
};

/* EOF */