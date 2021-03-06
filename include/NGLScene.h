#ifndef NGLSCENE_H__
#define NGLSCENE_H__

#ifdef DARWIN
    #include <OpenGL/gl3.h>
#else
    #include <GL/glew.h>
    #ifndef WIN32
        #include <GL/gl.h>
    #endif
#endif


#include <QGLWidget>
#include <QString>
#include <QTime>
#include <glm/vec3.hpp>
#include "Text.h"
#include "Camera.h"
#include "ShaderProgram.h"
#include "Texture.h"


//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene.h
/// @brief this class inherits from the Qt OpenGLWindow and allows us to use NGL to draw OpenGL
/// @author Jonathan Macey
/// @version 1.0
/// @date 10/9/13
/// Revision History :
/// This is an initial version used for the new NGL6 / Qt 5 demos
/// @class NGLScene
/// @brief our main glwindow widget for NGL applications all drawing elements are
/// put in this file
//----------------------------------------------------------------------------------------------------------------------

class NGLScene : public QGLWidget
{
  Q_OBJECT
  public:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief ctor for our NGL drawing class
    /// @param [in] parent the parent window to the class
    //----------------------------------------------------------------------------------------------------------------------
    explicit NGLScene(const QGLFormat _format, QWidget *_parent=0);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief dtor must close down ngl and release OpenGL resources
    //----------------------------------------------------------------------------------------------------------------------
    ~NGLScene();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the initialize class is called once when the window is created and we have a valid GL context
    /// use this to setup any default GL stuff
    //----------------------------------------------------------------------------------------------------------------------
    void initializeGL();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief timer to update our fluid simulation
    //----------------------------------------------------------------------------------------------------------------------
    void timerEvent(QTimerEvent *);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this is called everytime we want to draw the scene
    //----------------------------------------------------------------------------------------------------------------------
    void paintGL();

    //----------------------------------------------------------------------------------------------------------------------
    // Qt 5.5.1 must have this implemented and uses it
    //----------------------------------------------------------------------------------------------------------------------
    void resizeGL(QResizeEvent *_event);
    //----------------------------------------------------------------------------------------------------------------------
    // Qt 5.x uses this instead! http://doc.qt.io/qt-5/qopenglwindow.html#resizeGL
    //----------------------------------------------------------------------------------------------------------------------
    void resizeGL(int _w, int _h);
    //----------------------------------------------------------------------------------------------------------------------


    public slots:
    private:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief if we are playing
    //----------------------------------------------------------------------------------------------------------------------
    bool m_play;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief bool to tell us is we are working in color or not
    //----------------------------------------------------------------------------------------------------------------------
    bool m_color;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief time
    //----------------------------------------------------------------------------------------------------------------------
    float m_time;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief number of points in our spiral
    //----------------------------------------------------------------------------------------------------------------------
    int m_numPoints;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our line VAO handle
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_VAO;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our position VBO handle
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_posVBO;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our normals VBO handle
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_normVBO;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief location of our colour uniform
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_colorHndl;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief location of our inner uniform
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_sizeImageHndl;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief location of our MVP uniform
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_MVPHndl;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief location of our thickness uniform
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_thicknessHndl;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief location of our time uniform
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_timeHndl;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief mode handle
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_modeHndl;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our line drawer shader
    //----------------------------------------------------------------------------------------------------------------------
    ShaderProgram *m_lineDrawerShader;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our gl texture for later use
    //----------------------------------------------------------------------------------------------------------------------
    Texture *m_texture;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief start time of sim
    //----------------------------------------------------------------------------------------------------------------------
    QTime m_startTime;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief used to store the x rotation mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_spinXFace;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief used to store the y rotation mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_spinYFace;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag to indicate if the mouse button is pressed when dragging
    //----------------------------------------------------------------------------------------------------------------------
    bool m_rotate;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag to indicate if the Right mouse button is pressed when dragging
    //----------------------------------------------------------------------------------------------------------------------
    bool m_translate;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous x mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_origX;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous y mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_origY;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous x mouse value for Position changes
    //----------------------------------------------------------------------------------------------------------------------
    int m_origXPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous y mouse value for Position changes
    //----------------------------------------------------------------------------------------------------------------------
    int m_origYPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief window width
    //----------------------------------------------------------------------------------------------------------------------
    int m_width;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief window height
    //----------------------------------------------------------------------------------------------------------------------
    int m_height;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our text drawer
    //----------------------------------------------------------------------------------------------------------------------
    Text *m_text;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief used to store the global mouse transforms
    //----------------------------------------------------------------------------------------------------------------------
    glm::mat4 m_mouseGlobalTX;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Our Camera
    //----------------------------------------------------------------------------------------------------------------------
    Camera m_cam;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the model position for mouse movement
    //----------------------------------------------------------------------------------------------------------------------
    glm::vec3 m_modelPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief M Matrix handle in our shader
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_MLoc;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief MV Matrix handle in our shader
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_MVLoc;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief MVP Matrix handle in our shader
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_MVPLoc;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Normal Matrix handle in our shader
    //----------------------------------------------------------------------------------------------------------------------
    GLuint m_normalMatLoc;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief method to load transform matrices to the shader
    //----------------------------------------------------------------------------------------------------------------------
    void loadMatricesToShader();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Qt Event called when a key is pressed
    /// @param [in] _event the Qt event to query for size etc
    //----------------------------------------------------------------------------------------------------------------------
    void keyPressEvent(QKeyEvent *_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called every time a mouse is moved
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mouseMoveEvent (QMouseEvent * _event );
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse button is pressed
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mousePressEvent ( QMouseEvent *_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse button is released
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mouseReleaseEvent ( QMouseEvent *_event );

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse wheel is moved
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void wheelEvent( QWheelEvent *_event);


};



#endif
