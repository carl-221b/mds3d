#include "viewer.h"
#include "camera.h"
#include "SOIL2.h"

using namespace Eigen;

Viewer::Viewer()
    : _winWidth(0), _winHeight(0), _theta(0), _rotate(true)
{
}

Viewer::~Viewer()
{
}

////////////////////////////////////////////////////////////////////////////////
// GL stuff

// initialize OpenGL context
void Viewer::init(int w, int h){

    // Background color
    glClearColor(0.0, 0.0, 0.0, 0.0);

    loadShaders();
    if(!_mesh.load(DATA_DIR"/models/planet.obj")) exit(1);
    _mesh.initVBA();

    reshape(w,h);
    _cam.setPerspective(M_PI/3,0.1f,20000.0f);
    _cam.lookAt(Vector3f(0,0,4), Vector3f(0,0,0), Vector3f(0,1,0));
    _trackball.setCamera(&_cam);
    // Load earth
    _texture_1 = SOIL_load_OGL_texture(
                DATA_DIR"/textures/earth.jpg",
                SOIL_LOAD_AUTO,
                SOIL_CREATE_NEW_ID,
                SOIL_FLAG_INVERT_Y | SOIL_FLAG_MIPMAPS
                );
    if( _texture_1 == 0 ) std::cerr << "Soil loading error : " << SOIL_last_result() << std::endl;


    // Load earth clouds

    _texture_2 = SOIL_load_OGL_texture(
                DATA_DIR"/textures/earth_clouds.jpg",
                SOIL_LOAD_AUTO,
                SOIL_CREATE_NEW_ID,
                SOIL_FLAG_INVERT_Y | SOIL_FLAG_MIPMAPS
                );
    if( _texture_2 == 0 ) std::cerr << "Soil loading error : " << SOIL_last_result() << std::endl;

    // // Load earth night

    _texture_3 = SOIL_load_OGL_texture(
                DATA_DIR"/textures/earth_night.jpg",
                SOIL_LOAD_AUTO,
                SOIL_CREATE_NEW_ID,
                SOIL_FLAG_INVERT_Y | SOIL_FLAG_MIPMAPS
                );
    if( _texture_3 == 0 ) std::cerr << "Soil loading error : " << SOIL_last_result() << std::endl;


    _texture_4 = SOIL_load_OGL_texture(
                DATA_DIR"/textures/checkerboard.png",
                SOIL_LOAD_AUTO,
                SOIL_CREATE_NEW_ID,
                SOIL_FLAG_INVERT_Y
                );
    if( _texture_4 == 0 ) std::cerr << "Soil loading error : " << SOIL_last_result() << std::endl;




    _texture_5 = SOIL_load_OGL_texture(
                DATA_DIR"/textures/cow.jpg",
                SOIL_LOAD_AUTO,
                SOIL_CREATE_NEW_ID,
                SOIL_FLAG_INVERT_Y
                );
    if( _texture_5 == 0 ) std::cerr << "Soil loading error : " << SOIL_last_result() << std::endl;


    glGenSamplers(1,&_samplerId);


    int texW = 256;
    int texH = 256;
    unsigned char colors[6][3] = { {255,  0 , 0}, {0, 255,  0 }, { 0,  0, 255},
                                   {255, 255, 0}, {0, 255, 255}, {255, 0, 255} };
    unsigned char data[texW*texH*3];
    glGenTextures(1,&_texture_5);
    glBindTexture(GL_TEXTURE_2D, _texture_5);
    for(int l=0; ; ++l) {
        for(int i=0; i<texW*texH; ++i){
            data[3*i  ] = colors[l][0];
            data[3*i+1] = colors[l][1];
            data[3*i+2] = colors[l][2];
        }
        glTexImage2D(GL_TEXTURE_2D, l, (GLint)GL_RGB, texW, texH, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        if(texW==1 && texH==1)
          break;
        texW = std::max(1, texW/2);
        texH = std::max(1, texH/2);
    }

    glEnable(GL_DEPTH_TEST);
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
    // configure the rendering target size (viewport)
    glViewport(0, 0, _winWidth, _winHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _shader.activate();

    glUniformMatrix4fv(_shader.getUniformLocation("view_mat"),1,GL_FALSE,_cam.viewMatrix().data());
    glUniformMatrix4fv(_shader.getUniformLocation("proj_mat"),1,GL_FALSE,_cam.projectionMatrix().data());

    Affine3f M(AngleAxisf(_theta,Vector3f(0,1,0)));

    glUniformMatrix4fv(_shader.getUniformLocation("obj_mat"),1,GL_FALSE,M.matrix().data());

    Matrix4f matLocal2Cam = _cam.viewMatrix() * M.matrix();
    Matrix3f matN = matLocal2Cam.topLeftCorner<3,3>().inverse().transpose();
    glUniformMatrix3fv(_shader.getUniformLocation("normal_mat"),1,GL_FALSE,matN.data());

    Vector3f lightDir = Vector3f(1,0,1).normalized();

    lightDir = (matLocal2Cam.topLeftCorner<3,3>() * lightDir).normalized();
//    lightDir = (_cam.viewMatrix().topLeftCorner<3,3>() * lightDir).normalized();
    glUniform3fv(_shader.getUniformLocation("lightDir"),1,lightDir.data());




    // Earth texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,_texture_1);
    glUniform1i(_shader.getUniformLocation("tex_1"), 0);

    // Cloudy earth


    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,_texture_2);
    glUniform1i(_shader.getUniformLocation("tex_2"), 1);

    // Nighty earth


    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,_texture_3);
    glUniform1i(_shader.getUniformLocation("tex_3"), 2);




    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D,_texture_4);
    glUniform1i(_shader.getUniformLocation("tex_4"), 3);



    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D,_texture_5);
    glUniform1i(_shader.getUniformLocation("tex_5"), 4);

    glSamplerParameteri(_samplerId,GL_TEXTURE_MIN_FILTER,(GLint)_minFilter);


    glSamplerParameteri(_samplerId,GL_TEXTURE_MAG_FILTER,(GLint)_magFilter);

//    glBindSampler((GLint)0, _samplerId);

//    glBindSampler((GLint)1, _samplerId);

//    glBindSampler((GLint)2, _samplerId);
//    glBindSampler((GLint)3, _samplerId);
    glBindSampler((GLint)4, _samplerId);






    _mesh.draw(_shader);

    _shader.deactivate();
}


void Viewer::updateAndDrawScene()
{
    if(_rotate)
        _theta += 0.01*M_PI;
    drawScene();
}

void Viewer::loadShaders()
{
    // Here we can load as many shaders as we want, currently we have only one:
    _shader.loadFromFiles(DATA_DIR"/shaders/simple.vert", DATA_DIR"/shaders/simple.frag");
    checkError();
}

////////////////////////////////////////////////////////////////////////////////
// Events

/*!
   callback to manage mouse : called when user press or release mouse button
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::mousePressed(GLFWwindow */*window*/, int /*button*/, int action)
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
    else if(key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        _rotate = !_rotate;
    }

    if(action == GLFW_PRESS || action == GLFW_REPEAT )
    {
        if (key==GLFW_KEY_UP)
        {
            _minFilter = GL_LINEAR_MIPMAP_NEAREST;
        }
        else if (key==GLFW_KEY_DOWN)
        {
            _minFilter = GL_NEAREST_MIPMAP_LINEAR;
        }
        else if (key==GLFW_KEY_LEFT)
        {
            _minFilter = GL_LINEAR_MIPMAP_LINEAR;
        }
        else if (key==GLFW_KEY_RIGHT)
        {

            _minFilter = GL_NEAREST_MIPMAP_NEAREST;
        }
        else if (key==GLFW_KEY_PAGE_UP)
        {
            _magFilter = GL_LINEAR;
        }
        else if (key==GLFW_KEY_PAGE_DOWN)
        {
            _magFilter = GL_NEAREST;
        }
    }
}

void Viewer::charPressed(int /*key*/)
{
}
