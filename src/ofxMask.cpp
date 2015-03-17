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
void ofxMask::setup(int width, int height, GLint internalFormat, Type type, bool useABMaskees)
{
    this->internalFormat = internalFormat;
    this->useABMaskees = useABMaskees;
    this->invert = false;
    this->width = width;
    this->height = height;
    this->type = type;
#define _S(a) #a
    switch(type) {
        case ALPHA: {
            masker_.allocate(width, height, GL_RGBA);
            string shader_src = _S(
                                   uniform float useABMaskees;
                                   uniform float invert;
                                   uniform sampler2DRect masker;
                                   uniform sampler2DRect maskeeA;
                                   uniform sampler2DRect maskeeB;
                                   void main()
                                   {
                                       gl_FragColor = texture2DRect(maskeeA, gl_TexCoord[0].st);
                                       gl_FragColor.a *= texture2DRect(masker, gl_TexCoord[0].st).a;
                                       gl_FragColor.a = sqrt(gl_FragColor.a);
                                       if(invert > 0.5){
                                           gl_FragColor.a = abs(gl_FragColor.a-1.0);
                                       }
                                       if(useABMaskees > 0.5){
                                           vec4 rgb2 = texture2DRect(maskeeB, gl_TexCoord[0].st);
                                           gl_FragColor.r = gl_FragColor.a*gl_FragColor.r + (1.0-gl_FragColor.a)*rgb2.r;
                                           gl_FragColor.g = gl_FragColor.a*gl_FragColor.g + (1.0-gl_FragColor.a)*rgb2.g;
                                           gl_FragColor.b = gl_FragColor.a*gl_FragColor.b + (1.0-gl_FragColor.a)*rgb2.b;
                                           gl_FragColor.a = gl_FragColor.a*gl_FragColor.a + (1.0-gl_FragColor.a)*rgb2.a;
                                       }
                                   }
                                   );
            shader_.setupShaderFromSource(GL_FRAGMENT_SHADER, shader_src);
            shader_.linkProgram();
        }	break;
        case LUMINANCE: {
            masker_.allocate(width, height, GL_RGB);
            string shader_src = _S(
                                   uniform float useABMaskees;
                                   uniform float invert;
                                   uniform sampler2DRect masker;
                                   uniform sampler2DRect maskeeA;
                                   uniform sampler2DRect maskeeB;
                                   void main()
                                   {
                                       gl_FragColor = texture2DRect(maskeeA, gl_TexCoord[0].st);
                                       vec4 rgb = texture2DRect(masker, gl_TexCoord[0].st);
                                       gl_FragColor.a *= 0.298912*rgb.r + 0.586611*rgb.g + 0.114478*rgb.b;
                                       gl_FragColor.a = sqrt(gl_FragColor.a);
                                       if(invert > 0.5){
                                           gl_FragColor.a = abs(gl_FragColor.a-1.0);
                                       }
                                       if(useABMaskees > 0.5){
                                           vec4 rgb2 = texture2DRect(maskeeB, gl_TexCoord[0].st);
                                           gl_FragColor.r = gl_FragColor.a*gl_FragColor.r + (1.0-gl_FragColor.a)*rgb2.r;
                                           gl_FragColor.g = gl_FragColor.a*gl_FragColor.g + (1.0-gl_FragColor.a)*rgb2.g;
                                           gl_FragColor.b = gl_FragColor.a*gl_FragColor.b + (1.0-gl_FragColor.a)*rgb2.b;
                                           gl_FragColor.a = gl_FragColor.a*gl_FragColor.a + (1.0-gl_FragColor.a)*rgb2.a;
                                       }
                                   }
                                   );
            shader_.setupShaderFromSource(GL_FRAGMENT_SHADER, shader_src);
            shader_.linkProgram();
        }	break;
    }
#undef _S
    maskeeA_.allocate(width, height, internalFormat);
    if(useABMaskees){
        maskeeB_.allocate(width, height, internalFormat);
    }
    makeTexCoords(tex_coords_, masker_.getTextureReference().getTextureData());
    makeVertices(vertices_, masker_.getTextureReference().getTextureData());
}

void ofxMask::beginMask(bool clear)
{
    glPushAttrib(GL_BLEND);
    switch(type){
        case LUMINANCE:
            glDisable(GL_BLEND);
            break;
        case ALPHA:
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            break;
    }
    
    masker_.begin();
    if(clear) {
        ofClear(0.0f, 0.0f, 0.0f, 0.0f);
    }
}

void ofxMask::endMask()
{
    masker_.end();
    glPopAttrib();
}

void ofxMask::clearMask()
{
    masker_.begin();
    ofClear(0.0f, 0.0f, 0.0f, 0.0f);
    masker_.end();
}

void ofxMask::begin(bool clear){
    beginA(clear);
}

void ofxMask::end(){
    endA();
}

void ofxMask::beginA(bool clear)
{
    beginMaskee(&maskeeA_,clear);
}

void ofxMask::endA()
{
    endMaskee(&maskeeA_);
}


void ofxMask::beginB(bool clear)
{
    beginMaskee(&maskeeB_, clear);
}

void ofxMask::endB()
{
    endMaskee(&maskeeB_);
}

void ofxMask::beginMaskee(ofFbo *maskee, bool clear)
{
    glPushAttrib(GL_BLEND);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    maskee->begin();
    
    if(clear) {
        ofClear(0.0f, 0.0f, 0.0f, 0.0f);
    }
}

void ofxMask::endMaskee(ofFbo *maskee)
{
    maskee->end();
    glPopAttrib();
}

void ofxMask::draw()
{
    glPushAttrib(GL_BLEND);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    shader_.begin();
    shader_.setUniform1f("invert", invert ? 1.0 : 0.0);
    shader_.setUniform1f("useABMaskees", useABMaskees ? 1.0 : 0.0);
    shader_.setUniformTexture("masker", masker_, 0);
    shader_.setUniformTexture("maskeeA", maskeeA_, 1);
    if(useABMaskees){
        shader_.setUniformTexture("maskeeB", maskeeB_, 2);
    }
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glTexCoordPointer(2, GL_FLOAT, 0, tex_coords_ );
    glEnableClientState(GL_VERTEX_ARRAY);		
    glVertexPointer(2, GL_FLOAT, 0, vertices_ );
    glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    shader_.end();
    glPopAttrib();
}

void ofxMask::drawMasker()
{
    masker_.draw(0,0,masker_.getWidth(),masker_.getHeight());
}
void ofxMask::drawMaskee()
{
    drawMaskeeA();
}

void ofxMask::drawMaskeeA(){
    maskeeA_.draw(0,0,maskeeA_.getWidth(),maskeeA_.getHeight());
}

void ofxMask::drawMaskeeB()
{
    maskeeB_.draw(0,0,maskeeB_.getWidth(),maskeeB_.getHeight());
}

ofFbo* ofxMask::getMasker(){
    return &masker_;
}

ofFbo* ofxMask::getMaskee(){
    return getMaskeeA();
}

ofFbo* ofxMask::getMaskeeA(){
    return &maskeeA_;
}

ofFbo* ofxMask::getMaskeeB(){
    return &maskeeB_;
}

int ofxMask::getWidth(){
    return width;
}

int ofxMask::getHeight(){
    return height;
}

void ofxMask::invertMask(bool invert){
    this->invert = invert;
}
/* EOF */