#include "NGLScene.h"

#include <QMouseEvent>
#include <QGuiApplication>
#include <fstream>
#include <vector>
#include <string>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define DtoR M_PI/180.0f

//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for x/y translation with mouse movement
//----------------------------------------------------------------------------------------------------------------------
const static float INCREMENT=0.03f;
//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for the wheel zoom
//----------------------------------------------------------------------------------------------------------------------
const static float ZOOM=0.5f;

NGLScene::NGLScene(const QGLFormat _format, QWidget *_parent) : QGLWidget(_format,_parent)
{
  // set this widget to have the initial keyboard focus
  setFocus();
  setFocusPolicy( Qt::StrongFocus );
  // re-size the widget to that of the parent (in that case the GLFrame passed in on construction)
  m_rotate=false;
  // mouse rotation values set to 0
  m_spinXFace=0.0f;
  m_spinYFace=0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NGLScene::~NGLScene()
{
  delete m_text;
  delete m_particleDrawer;
}
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::resizeGL(QResizeEvent *_event)
{
  m_width=_event->size().width()*devicePixelRatio();
  m_height=_event->size().height()*devicePixelRatio();
  m_text->setScreenSize(m_width,m_height);
  // now set the camera size values as the screen size has changed
  m_cam.setShape(45.0f,(float)width(),(float)height(),0.05f,350.0f);
  m_particleDrawer->setScreenWidth(_event->size().width());
}
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::resizeGL(int _w , int _h)
{
  m_cam.setShape(45.0f,(float)_w,(float)_h,0.05f,350.0f);
  m_width=_w*devicePixelRatio();
  m_height=_h*devicePixelRatio();
  m_text->setScreenSize(m_width,m_height);
  m_particleDrawer->setScreenWidth(_w);
}
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::initializeGL()
{
#ifndef DARWIN
    glewExperimental = GL_TRUE;
    GLenum error = glewInit();
    if(error != GLEW_OK){
        std::cerr<<"GLEW IS NOT OK!!! "<<std::endl;
    }
#endif

  glClearColor(1.f, 1.f, 1.0f, 1.0f);			   // White Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);

  // Now we will create a basic Camera from the graphics library
  // This is a static camera so it only needs to be set once

  m_cam = Camera(glm::vec3(0.0, 0.0, 1.0));
  // set the shape using FOV 45 Aspect Ratio based on Width and Height
  // The final two are near and far clipping planes of 0.5 and 10
  m_cam.setShape(45.0f,720.0f,576.0f,0.05f,350.0f);

  //Create our text drawer
  m_text = new Text(QFont("Ariel"));
  m_text->setScreenSize(width(),height());
  m_text->setColour(1.f,0.f,0.f);

  //Create our nice efficient particle drawer
  m_particleDrawer = new ParticleDrawer;
  m_particleDrawer->setParticleSize(0.025f);
  m_particleDrawer->setScreenWidth(width());
  m_particleDrawer->setColour(1.f,0.f,0.f);

  float spacing = 0.01f;
  float length = 0.01f;
  float inc = 0.0002f;
  float circumference = 2.f*M_PI*0.1;
  float normaliser = circumference/spacing;
  float gap = normaliser*inc;
  std::cout<<"gap = "<<gap<<std::endl;

  glm::vec3 v(1,0,0);
  glm::vec3 t(0,-1,0);
  glm::vec3 temp;
  glm::mat4 rotM(1.0);

  std::vector<glm::vec3> testPos;
  while(length<1.f)
  {
      v/=glm::length(v);
      v*=length;
      testPos.push_back(v);

      temp = v+ (t*spacing);
      float angle = acos((glm::dot(v,temp))/(glm::length(v)*glm::length(temp)));
      rotM = glm::mat4(1.0);
      rotM = glm::rotate(rotM,angle,glm::vec3(0,0,1));
      v = glm::vec3(rotM*glm::vec4(v,1.0));
      t = glm::vec3(rotM*glm::vec4(t,1.0));

      circumference = 2.f*M_PI*length;
      float density = circumference/spacing;
      length+=inc*(normaliser/density);
  }
  m_particleDrawer->setPositions(testPos);

  // Start our timer event. This will begin calling the TimerEvent function that updates our simulation.
  startTimer(0);
}


void NGLScene::loadMatricesToShader()
{
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::timerEvent(QTimerEvent *)
{
    updateGL();
}
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::paintGL()
{
  glViewport(0,0,m_width,m_height);
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Rotation based on the mouse position for our global transform
  glm::mat4 rotX;
  glm::mat4 rotY;
  // create the rotation matrices
  rotX = glm::rotate(rotX,(float)m_spinXFace*(float)DtoR,glm::vec3(1,0,0));
  rotY = glm::rotate(rotY,(float)m_spinYFace*(float)DtoR,glm::vec3(0,1,0));
  // multiply the rotations
  m_mouseGlobalTX=rotY*rotX;
  // add the translations
  m_mouseGlobalTX[3][0] = m_modelPos.x;
  m_mouseGlobalTX[3][1] = m_modelPos.y;
  m_mouseGlobalTX[3][2] = m_modelPos.z;

  QTime currentTime;
  currentTime = currentTime.currentTime();

  m_particleDrawer->draw(m_mouseGlobalTX,m_cam.getViewMatrix(),m_cam.getProjectionMatrix());

  QString text("Rendering");
  m_text->setColour(1.f,0.f,0.f);
  m_text->renderText(0,0,text);

}
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent (QMouseEvent * _event)
{

  // right mouse translate code
  if(m_translate && _event->buttons() == Qt::RightButton)
  {
    int diffX = (int)(_event->x() - m_origXPos);
    int diffY = (int)(_event->y() - m_origYPos);
    m_origXPos=_event->x();
    m_origYPos=_event->y();
    m_modelPos.x += INCREMENT * diffX;
    m_modelPos.y -= INCREMENT * diffY;
    update();

   }
}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent ( QMouseEvent * _event)
{
  // that method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if(_event->button() == Qt::LeftButton)
  {
    m_origX = _event->x();
    m_origY = _event->y();
    m_rotate =true;
  }
  // right mouse translate mode
  else if(_event->button() == Qt::RightButton)
  {
    m_origXPos = _event->x();
    m_origYPos = _event->y();
    m_translate=true;
  }

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent ( QMouseEvent * _event )
{
  // that event is called when the mouse button is released
  // we then set Rotate to false
  if (_event->button() == Qt::LeftButton)
  {
    m_rotate=false;
  }
        // right mouse translate mode
  if (_event->button() == Qt::RightButton)
  {
    m_translate=false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent(QWheelEvent *_event)
{

	// check the diff of the wheel position (0 means no change)
	if(_event->delta() > 0)
	{
        m_modelPos.z+=ZOOM;
	}
	else if(_event->delta() <0 )
	{
        m_modelPos.z-=ZOOM;
	}
	update();
}
//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // that method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quit
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  // turn on wirframe rendering
  case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;
  // turn off wire frame
  case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;
  // show full screen
  case Qt::Key_F : showFullScreen(); break;
  // show windowed
  case Qt::Key_N : showNormal(); break;
  case Qt::Key_Minus : m_particleDrawer->setParticleSize(m_particleDrawer->getParticleSize()-0.01f); break;
  case Qt::Key_Plus : m_particleDrawer->setParticleSize(m_particleDrawer->getParticleSize()+0.01f); break;

  default : break;
  }
  // finally update the GLWindow and re-draw
  //if (isExposed())
    update();
}
