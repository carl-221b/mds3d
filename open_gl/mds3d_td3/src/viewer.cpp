#include "viewer.h"
#include "camera.h"

using namespace Eigen;

Viewer::Viewer()
  : _winWidth(0), _winHeight(0),_zoom(1),_offset(0,0),_fil(0)
{
}

Viewer::~Viewer()
{

}

////////////////////////////////////////////////////////////////////////////////
// GL stuff

// initialize OpenGL context
void Viewer::init(int w, int h){


    loadShaders();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    //glDepthFunc(GL_EQUAL);
    if(!_mesh.load(DATA_DIR"/models/pdp.obj")) exit(1);
    _mesh.initVBA();
    reshape(w,h);
    _trackball.setCamera(&_cam);
}

void Viewer::reshape(int w, int h){
    _winWidth = w;
    _winHeight = h;
    _cam.setViewport(w,h);


}


/*!
   callback to draw graphic primitives
 */
void Viewer::drawScene()
{
  _shader.activate();
  glViewport(0, 0, _winWidth/2, _winHeight);
  glClearColor(0.3, 0.3, 0.3, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);

  glUniform1f(_shader.getUniformLocation("zoom"), _zoom);
  glUniform2fv(_shader.getUniformLocation("offset"),1, _offset.data());
  if(_fil==0){
    glUniform1i(_shader.getUniformLocation("fil"), 0);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

    _mesh.draw(_shader);
  }
  if(_fil==1){

      glUniform1i(_shader.getUniformLocation("fil"), 1);
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

      _mesh.draw(_shader);

      glUniform1i(_shader.getUniformLocation("fil"), 0);
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

      _mesh.draw(_shader);
  }
    _shader.deactivate();

  _shader2.activate();
  glViewport(_winWidth/2, 0, _winWidth/2, _winHeight);


  glUniform1f(_shader2.getUniformLocation("zoom"), _zoom);
  glUniform2fv(_shader2.getUniformLocation("offset"),1, _offset.data());
  if(_fil==0){
      glUniform1i(_shader2.getUniformLocation("fil"), 0);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

    _mesh.draw(_shader2);
  }
  if(_fil==1){

      glUniform1i(_shader2.getUniformLocation("fil"), 1);
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

      _mesh.draw(_shader2);

      glUniform1i(_shader2.getUniformLocation("fil"), 0);
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

      _mesh.draw(_shader2);
  }

  _shader2.deactivate();

}


void Viewer::updateAndDrawScene()
{

    drawScene();
}

void Viewer::loadShaders()
{
    // Here we can load as many shaders as we want, currently we have only one:
    _shader.loadFromFiles(DATA_DIR"/shaders/simple.vert", DATA_DIR"/shaders/simple.frag");

    _shader2.loadFromFiles(DATA_DIR"/shaders/second.vert", DATA_DIR"/shaders/simple.frag");

    checkError();
}

////////////////////////////////////////////////////////////////////////////////
// Events

/*!
   callback to manage keyboard interactions
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::keyPressed(int key, int action, int /*mods*/)
{
  if(key == GLFW_KEY_R && action == GLFW_PRESS)
  {
    loadShaders();
  }

  if(action == GLFW_PRESS || action == GLFW_REPEAT )
  {
    if (key==GLFW_KEY_UP)
    {
      _offset[1]+=0.1;
    }
    else if (key==GLFW_KEY_DOWN)
    {

        _offset[1]-=0.1;
    }
    else if (key==GLFW_KEY_LEFT)
    {
      _offset[0]-=0.1;
    }
    else if (key==GLFW_KEY_RIGHT)
    {

        _offset[0]+=0.1;
    }
    else if (key==GLFW_KEY_PAGE_UP)
    {
        _zoom*=1.2;
    }
    else if (key==GLFW_KEY_PAGE_DOWN)
    {
        _zoom/=1.2;
    }
    else if(key==GLFW_KEY_L){
      if(_fil==1)
        _fil=0;
      else
        _fil=1;
    }
  }
}

void Viewer::keyPressed(int key, int action, int /*mods*/)
{
  if(key == GLFW_KEY_R && action == GLFW_PRESS)
  {
    loadShaders();
  }

  if(action == GLFW_PRESS || action == GLFW_REPEAT )
  {
    if (key==GLFW_KEY_UP)
    {
      _offset[1]+=0.1;
    }
    else if (key==GLFW_KEY_DOWN)
    {

        _offset[1]-=0.1;
    }
    else if (key==GLFW_KEY_LEFT)
    {
      _offset[0]-=0.1;
    }
    else if (key==GLFW_KEY_RIGHT)
    {

        _offset[0]+=0.1;
    }
    else if (key==GLFW_KEY_PAGE_UP)
    {
        _zoom*=1.2;
    }
    else if (key==GLFW_KEY_PAGE_DOWN)
    {
        _zoom/=1.2;
    }
    else if(key==GLFW_KEY_L){
      if(_fil==1)
        _fil=0;
      else
        _fil=1;
    }
  }
}

/*!
   callback to manage mouse : called when user press or release mouse button
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::mousePressed(GLFWwindow * /*window*/, int /*button*/, int action)
{
  if(action == GLFW_PRESS)
  {
      _trackingMode = TM_ROTATE_AROUND;
      _trackball.start();
      _trackball.track(_lastMousePos);
  }
  else if(action == GLFW_RELEASE)
  {
      _trackingMode = TM_NO_TRACK;
  }
}


/*!
   callback to manage mouse : called when user move mouse with button pressed
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::mouseMoved(int x, int y)
{
    if(_trackingMode == TM_ROTATE_AROUND)
    {
        _trackball.track(Vector2i(x,y));
    }

    _lastMousePos = Vector2i(x,y);
}

void Viewer::mouseScroll(double /*x*/, double y)
{
  _cam.zoom(-0.1*y);
}

void Viewer::charPressed(int /*key*/)
{
}
