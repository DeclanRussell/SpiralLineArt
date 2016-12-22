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
  m_time = 0.f;
  m_color = false;
  m_play = false;
}

//----------------------------------------------------------------------------------------------------------------------
NGLScene::~NGLScene()
{
  // Delete our buffers
  glDeleteBuffers(1,&m_posVBO);
  glDeleteBuffers(1,&m_normVBO);
  glDeleteVertexArrays(1,&m_VAO);
  delete m_text;
  delete m_texture;
  delete m_lineDrawerShader;
}
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::resizeGL(QResizeEvent *_event)
{
  m_width=_event->size().width()*devicePixelRatio();
  m_height=_event->size().height()*devicePixelRatio();
  m_text->setScreenSize(m_width,m_height);
  // now set the camera size values as the screen size has changed
  m_cam.setShape(45.0f,(float)width(),(float)height(),0.05f,350.0f);
}
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::resizeGL(int _w , int _h)
{
  m_cam.setShape(45.0f,(float)_w,(float)_h,0.05f,350.0f);
  m_width=_w*devicePixelRatio();
  m_height=_h*devicePixelRatio();
  m_text->setScreenSize(m_width,m_height);
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

  glClearColor(1.f, 1.f, 1.0f, 0.0f);			   // White Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);

  // Make our line drawer shader
  m_lineDrawerShader = new ShaderProgram();
  //load the source
  Shader vert("shaders/lineVert.glsl",GL_VERTEX_SHADER);
  Shader geom("shaders/lineGeom.glsl",GL_GEOMETRY_SHADER);
  Shader frag("shaders/lineFrag.glsl",GL_FRAGMENT_SHADER);
  m_lineDrawerShader->attachShader(&vert);
  m_lineDrawerShader->attachShader(&geom);
  m_lineDrawerShader->attachShader(&frag);
  m_lineDrawerShader->bindFragDataLocation(0,"fragout");
  m_lineDrawerShader->link();
  m_lineDrawerShader->use();

  // Get our uniform locations for later use
  m_colorHndl = m_lineDrawerShader->getUniformLoc("color");
  m_sizeImageHndl = m_lineDrawerShader->getUniformLoc("sizeImage");
  m_MVPHndl = m_lineDrawerShader->getUniformLoc("MVP");
  m_thicknessHndl = m_lineDrawerShader->getUniformLoc("thickness");
  m_timeHndl = m_lineDrawerShader->getUniformLoc("time");
  m_modeHndl = m_lineDrawerShader->getUniformLoc("mode");

  //Set our uniforms
  glUniform3f(m_colorHndl,0.f,0.f,0.f);
  glUniform1f(m_thicknessHndl,0.0125664f);
  glUniform1f(m_sizeImageHndl,0);
  glUniform1f(m_timeHndl,0.f);
  glUniform1i(m_modeHndl,0);
  glm::mat4 MVP = m_cam.getProjectionMatrix()*m_cam.getViewMatrix();
  glUniformMatrix4fv(m_MVPHndl, 1, GL_FALSE, glm::value_ptr(MVP));

  // Create our texture
  m_texture = new Texture("images/testImage.png");
//  m_texture = new Texture("images/dad.jpg");


  // Now we will create a basic Camera from the graphics library
  // This is a static camera so it only needs to be set once

  m_cam = Camera(glm::vec3(0.0, 0.0, 1.8));
  // set the shape using FOV 45 Aspect Ratio based on Width and Height
  // The final two are near and far clipping planes of 0.5 and 10
  m_cam.setShape(45.0f,720.0f,576.0f,0.05f,350.0f);

  //Create our text drawer
  m_text = new Text(QFont("Ariel"));
  m_text->setScreenSize(width(),height());
  m_text->setColour(1.f,0.f,0.f);



  float spacing = 0.01f;
  float length = 0.01f;
  float inc = 0.0002f;
  float circumference = 2.f*M_PI*0.1;
  float normaliser = circumference/spacing;
  float gap = normaliser*inc;
  std::cout<<"gap = "<<gap<<std::endl;
  m_lineDrawerShader->use();
  glUniform1f(m_thicknessHndl,gap);

  glm::vec3 v(1,0,0);
  glm::vec3 t(0,-1,0);
  glm::vec3 temp;
  glm::mat4 rotM(1.0);

  std::vector<glm::vec3> testPos;
  std::vector<glm::vec3> testNorms;
  while(length<1.f)
  {
      v/=glm::length(v);
      testNorms.push_back(v);
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
  for(unsigned int i=0;i<testNorms.size();i++)
  {
      testNorms[i].z = (float)i/(float)testNorms.size();
  }
  m_numPoints = testPos.size();

  // Create our VAO and position buffer
  glGenVertexArrays(1, &m_VAO);
  glBindVertexArray(m_VAO);

  // Put our vertices into an OpenGL buffer
  glGenBuffers(1, &m_posVBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_posVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*testPos.size(), &testPos[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // Create our normal openGL buffer
  glGenBuffers(1, &m_normVBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_normVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*testNorms.size(), &testNorms[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

  //unbind everthing
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindVertexArray(0);


  // Start our timer event. This will begin calling the TimerEvent function that updates our simulation.
  startTimer(0);
}


void NGLScene::loadMatricesToShader()
{
    m_lineDrawerShader->use();
    glm::mat4 MVP = m_cam.getProjectionMatrix()*m_cam.getViewMatrix()*m_mouseGlobalTX;
    glUniformMatrix4fv(m_MVPHndl, 1, GL_FALSE, glm::value_ptr(MVP));
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::timerEvent(QTimerEvent *)
{
    if(m_play)
    {
        m_time += 0.00025f;
    }
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

  loadMatricesToShader();


  //disable our depth test
  glDisable(GL_DEPTH_TEST);
  //Enable alpha blending
  glEnable (GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  //bind our VAO
  glBindVertexArray(m_VAO);
  //bind our texture
  glActiveTexture (GL_TEXTURE0);
  m_texture->bind(0);
  m_lineDrawerShader->use();

  // update our time
  glUniform1f(m_timeHndl,m_time);

  if(!m_color)
  {

      //Disable alpha blending
      glDisable (GL_BLEND);
      glUniform1i(m_modeHndl,0);
      glDrawArrays(GL_LINE_STRIP, 0, m_numPoints);
  }
  else
  {
      //disable our depth test
      glDisable(GL_DEPTH_TEST);
      //Enable alpha blending
      glEnable (GL_BLEND);
      glBlendFunc(GL_ONE, GL_ONE);

    //  glUniform1i(m_modeHndl,1);
    //  glDrawArrays(GL_LINE_STRIP, 0, m_numPoints);
    //  glUniform1i(m_modeHndl,2);
    //  glDrawArrays(GL_LINE_STRIP, 0, m_numPoints);
    //  glUniform1i(m_modeHndl,3);
    //  glDrawArrays(GL_LINE_STRIP, 0, m_numPoints);
    //  glUniform1i(m_modeHndl,4);
    //  glDrawArrays(GL_LINE_STRIP, 0, m_numPoints);

      glUniform1i(m_modeHndl,5);
      glDrawArrays(GL_LINE_STRIP, 0, m_numPoints);
      // update our time
      glUniform1f(m_timeHndl,m_time-0.33);
      glUniform1i(m_modeHndl,6);
      glDrawArrays(GL_LINE_STRIP, 0, m_numPoints);
      // update our time
      glUniform1f(m_timeHndl,m_time-0.66);
      glUniform1i(m_modeHndl,7);
      glDrawArrays(GL_LINE_STRIP, 0, m_numPoints);
  }


  QTime currentTime;
  currentTime = currentTime.currentTime();


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
  case Qt::Key_R: m_time = 0.f; break;
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
      // toggle play
  case Qt::Key_Space : m_play = !m_play; break;
      //Toggle color
  case Qt::Key_C :{
      if(m_color)
      {
          m_color = false;
          glClearColor(1.f, 1.f, 1.0f, 0.0f);
      }
      else
      {
          m_color = true;
          glClearColor(0.f, 0.f, 0.0f, 0.0f);

      }
  }
      break;

  default : break;
  }
  // finally update the GLWindow and re-draw
  //if (isExposed())
    update();
}
