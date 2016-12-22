#ifndef __TEXTUREUTILS_H_
#define __TEXTUREUTILS_H_

#ifdef DARWIN
    #include <OpenGL/gl3.h>
#else
    #include <GL/glew.h>
    #ifndef WIN32
        #include <GL/gl.h>
    #endif
#endif
#include <QImage>
#include <iostream>

class TextureUtils{
public:
   GLuint createTexture(QString path);
   GLuint createTexture(QImage _image);
   inline QImage* getImage(){return m_image;}
   ~TextureUtils();
private:
    QImage *m_image;
};

#endif
