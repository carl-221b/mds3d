#include "viewer.h"
#include "camera.h"

using namespace Eigen;
using namespace std;
Viewer::Viewer()
    : _winWidth(0), _winHeight(0),_zoom(1),_offset(0,0),_angle(0),fil(0)
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
    if(!_mesh.load(DATA_DIR"/models/monkey2.obj")) exit(1);
    _mesh.computeNormals();
    _mesh.initVBA();
    reshape(w,h);
    _trackball.setCamera(&_cam);
    _cam.setPerspective(M_PI/2,0.1,100000);
    _cam.lookAt(Vector3f(0,3,2.3), Vector3f(0,0,0), Vector3f(0,2,0));

    //-----------------------------------------------------------------------------------------------------------------------

    // Earth matrix init
    _earth.setIdentity();
    Affine3f t_earth = Translation3f(2,0,0) * AngleAxisf(23.44 * M_PI /180 , Vector3f::UnitZ()) * Scaling(Vector3f(0.4,0.4,0.4));
    _earth = t_earth.matrix();


    //-----------------------------------------------------------------------------------------------------------------------

    // Moon matrix init

    //translation moon/sun
    _moon.setIdentity();
    Affine3f t_moon = Translation3f(2.7,0,0) * AngleAxisf(23.44 * M_PI /180 , Vector3f::UnitZ()) * Scaling(Vector3f(0.15,0.15,0.15));
    _moon = t_moon.matrix();

    //translation earth/moon
    Vector3f t_moon_earth = Affine3f(_earth) * Vector3f(0,0,0);
    t_moon =  Translation3f(t_moon_earth) * AngleAxisf(5.14 * M_PI /180,  Vector3f::UnitX()) * Translation3f(-t_moon_earth) ;
    _moon = t_moon.matrix() * _moon;


    //-----------------------------------------------------------------------------------------------------------------------

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
    /* init */
    _shader.activate();
    glViewport(0, 0, _winWidth, _winHeight);
    glClearColor(0.3, 0.3, 0.3, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);


    //    -------------------------------------------------------------------------------------------------------------------------------------
    // Sun

    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

    _sun.setIdentity();
    Affine3f t_sun = Translation3f(0,0,0) * Scaling(Vector3f(1,1,1));
    _sun = _sun * t_sun.matrix();

    glUniformMatrix4fv(_shader.getUniformLocation("obj_mat"), 1, GL_FALSE, _sun.data());
    glUniformMatrix4fv(_shader.getUniformLocation("view_mat"), 1, GL_FALSE, _cam.viewMatrix().data());
    glUniformMatrix4fv(_shader.getUniformLocation("proj_mat"), 1, GL_FALSE, _cam.projectionMatrix().data());
    glUniform3f(_shader.getUniformLocation("light_pos"), 13,17,5);
    Eigen::Matrix3f normal;

    normal= ((_cam.viewMatrix().topLeftCorner(3,3) * _sun.topLeftCorner(3,3)).inverse()).transpose();
    glUniformMatrix3fv(_shader.getUniformLocation("normal_mat"), 1, GL_FALSE, normal.data());
    _mesh.draw(_shader);
    //    -------------------------------------------------------------------------------------------------------------------------------------
    //  Earth

    // Rotation
    Vector3f t_earth = Affine3f(_earth) * Vector3f(0,0,0); //recuperer vecteur translation
    Affine3f affine_earth = Translation3f(t_earth) * AngleAxisf(0.2, Vector3f::UnitY()) * Translation3f(-t_earth);  // effectuer la translation + rotation en augmentant l'angle de 0.01 sur l'axe y , puis translation inverse
    _earth = affine_earth.matrix() * _earth; // application de la rotation



    //Revolution
    affine_earth = AngleAxisf(0.01, Vector3f::UnitY()); // rotation en augmentant l'angle de 0.01 sur l'axe y (pas de translation inverse car autour du soleil
    _earth = affine_earth.matrix() * _earth; //application de la rotation

    glUniformMatrix4fv(_shader.getUniformLocation("obj_mat"), 1, GL_FALSE, _earth.data());
    glUniformMatrix4fv(_shader.getUniformLocation("view_mat"), 1, GL_FALSE, _cam.viewMatrix().data());
    glUniformMatrix4fv(_shader.getUniformLocation("proj_mat"), 1, GL_FALSE, _cam.projectionMatrix().data());
    glUniform3f(_shader.getUniformLocation("light_pos"), 13,17,5);

    normal= ((_cam.viewMatrix().topLeftCorner(3,3) * _earth.topLeftCorner(3,3)).inverse()).transpose();
    glUniformMatrix3fv(_shader.getUniformLocation("normal_mat"), 1, GL_FALSE, normal.data());

    _mesh.draw(_shader);


    //    -------------------------------------------------------------------------------------------------------------------------------------

    // Moon
    // Rotation around self
    Vector3f t_moon = Affine3f(_moon) * Vector3f(0,0,0);
    Affine3f affine_moon = Translation3f(t_moon) * AngleAxisf(0.2,Vector3f::UnitY()) * Translation3f(-t_moon);
    _moon=affine_moon.matrix()*_moon;


    // Revolution

    affine_moon = AngleAxisf(0.01, Vector3f::UnitY());
    _moon = affine_moon.matrix() * _moon;

    //Rotation around earth
    t_moon = Affine3f (_earth) * Vector3f(0,0,0);
    affine_moon= Translation3f(t_moon) * AngleAxisf(0.1,Vector3f::UnitY()) * Translation3f(-t_moon);
    _moon=affine_moon.matrix()*_moon;


    glUniformMatrix4fv(_shader.getUniformLocation("obj_mat"), 1, GL_FALSE, _moon.data());
    glUniformMatrix4fv(_shader.getUniformLocation("view_mat"), 1, GL_FALSE, _cam.viewMatrix().data());
    glUniformMatrix4fv(_shader.getUniformLocation("proj_mat"), 1, GL_FALSE, _cam.projectionMatrix().data());
    glUniform3f(_shader.getUniformLocation("light_pos"), 13,17,5);

    normal= ((_cam.viewMatrix().topLeftCorner(3,3) * _moon.topLeftCorner(3,3)).inverse()).transpose();
    glUniformMatrix3fv(_shader.getUniformLocation("normal_mat"), 1, GL_FALSE, normal.data());

    _mesh.draw(_shader);


    //    -------------------------------------------------------------------------------------------------------------------------------------
    /* line mode */
    if(fil==1){
        _shader2.activate();

        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        glUniformMatrix4fv(_shader2.getUniformLocation("obj_mat"), 1, GL_FALSE, _sun.data());
        glUniformMatrix4fv(_shader2.getUniformLocation("view_mat"), 1, GL_FALSE, _cam.viewMatrix().data());
        glUniformMatrix4fv(_shader2.getUniformLocation("proj_mat"), 1, GL_FALSE, _cam.projectionMatrix().data());
        _mesh.draw(_shader2);


        glUniformMatrix4fv(_shader2.getUniformLocation("obj_mat"), 1, GL_FALSE, _earth.data());
        glUniformMatrix4fv(_shader2.getUniformLocation("view_mat"), 1, GL_FALSE, _cam.viewMatrix().data());
        glUniformMatrix4fv(_shader2.getUniformLocation("proj_mat"), 1, GL_FALSE, _cam.projectionMatrix().data());
        _mesh.draw(_shader2);



        glUniformMatrix4fv(_shader2.getUniformLocation("obj_mat"), 1, GL_FALSE, _moon.data());
        glUniformMatrix4fv(_shader2.getUniformLocation("view_mat"), 1, GL_FALSE, _cam.viewMatrix().data());
        glUniformMatrix4fv(_shader2.getUniformLocation("proj_mat"), 1, GL_FALSE, _cam.projectionMatrix().data());
        _mesh.draw(_shader2);




        _shader2.deactivate();
    }
}

void Viewer::drawScene2D()
{
    _shader.activate();
    glViewport(0, 0, _winWidth, _winHeight);
    glClearColor(0.3, 0.3, 0.3, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    M.setIdentity();
    M <<  0.5, 0, 0, -0.6,
            0, 0.5, 0, -1,
            0, 0, 0.5, 0,
            0, 0, 0, 1;
    glUniformMatrix4fv(_shader.getUniformLocation("obj_mat"), 1, GL_FALSE, M.data());

    _mesh.draw(_shader);


    M.setIdentity();
    M <<  -0.5, 0, 0, 0.5,
            0, 0.5, 0, -1,
            0, 0, 0.5, 0,
            0, 0, 0, 1;
    glUniformMatrix4fv(_shader.getUniformLocation("obj_mat"), 1, GL_FALSE, M.data());
    _mesh.draw(_shader);


    /*
    Rotation axe z
    M <<  cos(_angle)*_zoom, -sin(_angle), 0, _offset.x(),
          sin(_angle), cos(_angle)*_zoom, 0, _offset.y(),
          0, 0, _zoom, 0,
          0, 0, 0, 1;*/


    M <<  cos(_angle)*_zoom,    -sin(_angle), 0,_offset.x(),
            sin(_angle),     cos(_angle)*_zoom, 0, _offset.y(),
            0,                   0,        1,      0,
            0,                   0,        0,      1;

    Eigen::Matrix4f T;
    Eigen::Matrix4f Rz;
    Rz << cos(_angle), -sin(_angle),0 ,0,
            sin(_angle), cos(_angle),0  ,0,
            0,            0         ,0  ,0,
            0,            0        ,0  ,1;
    T<< 1, 0, 0,  0,
            0, 1, 0, 0.5,
            0, 0, 1,  0,
            0, 0, 0,  1;

    M=T*Rz*T.inverse();
    /*Vector3f t(0,0,0);
    Affine3f A= Translation3f(t)*AngleAxisf(_angle,Vector3f::UnitZ()) * Translation3f(-t);
    glUniformMatrix4fv(_shader.getUniformLocation("obj_mat"), 1, GL_FALSE, A.matrix().data());

    glUniformMatrix4fv(_shader.getUniformLocation("view_mat"), 1, GL_FALSE, _cam.viewMatrix().data());

    glUniformMatrix4fv(_shader.getUniformLocation("proj_mat"), 1, GL_FALSE, _cam.projectionMatrix().data());*/
    glUniformMatrix4fv(_shader.getUniformLocation("obj_mat"), 1, GL_FALSE, M.data());
    _mesh.draw(_shader);


    _shader.deactivate();

}

void Viewer::updateAndDrawScene()
{

    drawScene();
    //drawScene2D();
}

void Viewer::loadShaders()
{
    // Here we can load as many shaders as we want, currently we have only one:
    _shader.loadFromFiles(DATA_DIR"/shaders/blinn.vert", DATA_DIR"/shaders/blinn.frag");
    _shader2.loadFromFiles(DATA_DIR"/shaders/wire.vert", DATA_DIR"/shaders/wire.frag");
    //_shader2.loadFromFiles(DATA_DIR"/shaders/second.vert", DATA_DIR"/shaders/simple.frag");

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

        }
        else if (key==GLFW_KEY_DOWN)
        {

        }
        else if (key==GLFW_KEY_LEFT)
        {

        }
        else if (key==GLFW_KEY_RIGHT)
        {

        }
        else if (key==GLFW_KEY_PAGE_UP)
        {

        }
        else if (key==GLFW_KEY_PAGE_DOWN)
        {

        }
        else if(key==GLFW_KEY_A){

        }

        else if(key==GLFW_KEY_D){

        }
        else if(key==GLFW_KEY_L){
            if(fil==0)
                fil=1;
            else
                fil=0;
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
