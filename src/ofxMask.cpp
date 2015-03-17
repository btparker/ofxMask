#include "ofxMask.h"
#include "ofGraphics.h"

namespace {
    void makeVertices(float *dst, ofTextureData& texture_data)
    {
        dst[0] =
        dst[1] =
        dst[3] =
        dst[6] = 0;
        dst[2] =
        dst[4] = texture_data.width;
        dst[5] =
        dst[7] = texture_data.height;
    }
    void makeTexCoords(float *dst, ofTextureData& texture_data)
    {
        dst[0] =
        dst[1] =
        dst[3] =
        dst[6] = 0;
#ifndef TARGET_OPENGLES
        if( texture_data.textureTarget==GL_TEXTURE_RECTANGLE_ARB && ofGLSupportsNPOTTextures() ){
            dst[2] =
            dst[4] = texture_data.width;
            dst[5] =
            dst[7] = texture_data.height;
        }else
#endif
        {
            dst[2] =
            dst[4] = texture_data.tex_t;
            dst[5] =
            dst[7] = texture_data.tex_u;
        }
    }
}
void ofxMask::setup(int width, int height, GLint internalFormat, Type type, bool useSecondMaskee)
{
    this->internalFormat = internalFormat;
    this->useSecondMaskee = useSecondMaskee;
    this->invert = false;
    this->width = width;
    this->height = height;
#define _S(a) #a
    switch(type) {
        case ALPHA: {
            masker_.allocate(width, height, GL_RGBA);
            string shader_src = _S(
                                   uniform float useSecondMaskee;
                                   uniform float invert;
                                   uniform sampler2DRect masker;
                                   uniform sampler2DRect maskee;
                                   uniform sampler2DRect secondMaskee;
                                   void main()
                                   {
                                       gl_FragColor = texture2DRect(maskee, gl_TexCoord[0].st);
                                       gl_FragColor.a *= texture2DRect(masker, gl_TexCoord[0].st).a;
                                       gl_FragColor.a = sqrt(gl_FragColor.a);
                                       if(invert > 0.5){
                                           gl_FragColor.a = abs(gl_FragColor.a-1.0);
                                       }
                                       if(useSecondMaskee > 0.5){
                                           vec4 rgb2 = texture2DRect(secondMaskee, gl_TexCoord[0].st);
                                           gl_FragColor.r = gl_FragColor.a*gl_FragColor.r + (1.0-gl_FragColor.a)*rgb2.r;
                                           gl_FragColor.g = gl_FragColor.a*gl_FragColor.g + (1.0-gl_FragColor.a)*rgb2.g;
                                           gl_FragColor.b = gl_FragColor.a*gl_FragColor.b + (1.0-gl_FragColor.a)*rgb2.b;
                                           gl_FragColor.a = 1.0;
                                       }
                                   }
                                   );
            shader_.setupShaderFromSource(GL_FRAGMENT_SHADER, shader_src);
            shader_.linkProgram();
        }	break;
        case LUMINANCE: {
            masker_.allocate(width, height, GL_RGB);
            string shader_src = _S(
                                   uniform float useSecondMaskee;
                                   uniform float invert;
                                   uniform sampler2DRect masker;
                                   uniform sampler2DRect maskee;
                                   uniform sampler2DRect secondMaskee;
                                   void main()
                                   {
                                       gl_FragColor = texture2DRect(maskee, gl_TexCoord[0].st);
                                       vec4 rgb = texture2DRect(masker, gl_TexCoord[0].st);
                                       gl_FragColor.a *= 0.298912*rgb.r + 0.586611*rgb.g + 0.114478*rgb.b;
                                       gl_FragColor.a = sqrt(gl_FragColor.a);
                                       if(invert > 0.5){
                                           gl_FragColor.a = abs(gl_FragColor.a-1.0);
                                       }
                                       if(useSecondMaskee > 0.5){
                                           vec4 rgb2 = texture2DRect(secondMaskee, gl_TexCoord[0].st);
                                           gl_FragColor.r = gl_FragColor.a*gl_FragColor.r + (1.0-gl_FragColor.a)*rgb2.r;
                                           gl_FragColor.g = gl_FragColor.a*gl_FragColor.g + (1.0-gl_FragColor.a)*rgb2.g;
                                           gl_FragColor.b = gl_FragColor.a*gl_FragColor.b + (1.0-gl_FragColor.a)*rgb2.b;
                                           gl_FragColor.a = 1.0;
                                       }
                                   }
                                   );
            shader_.setupShaderFromSource(GL_FRAGMENT_SHADER, shader_src);
            shader_.linkProgram();
        }	break;
    }
#undef _S
    maskee_.allocate(width, height, internalFormat);
    if(useSecondMaskee){
        secondMaskee_.allocate(width, height, internalFormat);
    }
    makeTexCoords(tex_coords_, masker_.getTextureReference().getTextureData());
    makeVertices(vertices_, masker_.getTextureReference().getTextureData());
}

void ofxMask::beginMask(bool clear)
{
    glDisable(GL_BLEND);
    masker_.begin();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
    if(clear) {
        ofClear(0.0f, 0.0f, 0.0f, 0.0f);
    }
}

void ofxMask::endMask()
{
    glDisable(GL_BLEND);
    glPopAttrib();
    masker_.end();
}

void ofxMask::clearMask()
{
    masker_.begin();
    ofClear(0.0f, 0.0f, 0.0f, 0.0f);
    masker_.end();
}

void ofxMask::begin(bool clear)
{
    glDisable(GL_BLEND);
    maskee_.begin();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
    if(clear) {
        ofClear(0.0f, 0.0f, 0.0f, 0.0f);
    }
}

void ofxMask::end()
{
    glDisable(GL_BLEND);
    glPopAttrib();
    maskee_.end();
}


void ofxMask::beginSecond(bool clear)
{
    glDisable(GL_BLEND);
    secondMaskee_.begin();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA);

    if(clear) {
        ofClear(0.0f, 0.0f, 0.0f, 0.0f);
    }
}

void ofxMask::endSecond()
{
    glDisable(GL_BLEND);
    glPopAttrib();
    secondMaskee_.end();
}

void ofxMask::draw()
{
    ofSetColor(255);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    shader_.begin();
    shader_.setUniform1f("invert", invert ? 1.0 : 0.0);
    shader_.setUniform1f("useSecondMaskee", useSecondMaskee ? 1.0 : 0.0);
    shader_.setUniformTexture("masker", masker_, 0);
    shader_.setUniformTexture("maskee", maskee_, 1);
    if(useSecondMaskee){
        shader_.setUniformTexture("secondMaskee", secondMaskee_, 2);
    }
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glTexCoordPointer(2, GL_FLOAT, 0, tex_coords_ );
    glEnableClientState(GL_VERTEX_ARRAY);		
    glVertexPointer(2, GL_FLOAT, 0, vertices_ );
    glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    shader_.end();
}

void ofxMask::drawMasker()
{
    masker_.draw(0,0,masker_.getWidth(),masker_.getHeight());
}
void ofxMask::drawMaskee()
{
    maskee_.draw(0,0,maskee_.getWidth(),maskee_.getHeight());
}

void ofxMask::drawSecondMaskee()
{
    secondMaskee_.draw(0,0,secondMaskee_.getWidth(),secondMaskee_.getHeight());
}

ofFbo* ofxMask::getMasker(){
    return &masker_;
}

ofFbo* ofxMask::getMaskee(){
    return &maskee_;
}

ofFbo* ofxMask::getSecondMaskee(){
    return &secondMaskee_;
}

int ofxMask::getWidth(){
    return width;
}

int ofxMask::getHeight(){
    return height;
}

void ofxMask::setInvertMask(bool invert){
    this->invert = invert;
}

void ofxMask::setUseSecondMaskee(bool useSecondMaskee){
    this->useSecondMaskee = useSecondMaskee;
}
/* EOF */