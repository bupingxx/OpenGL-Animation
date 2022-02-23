#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#ifdef MAC_OS
#include <QtOpenGL/QtOpenGL>
#else
#include <GL/glew.h>
#endif
#include <glm/glm.hpp>
#include <glm/ext.hpp> 
#include <QtGui>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;
using namespace glm;

class MyGLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions {
    Q_OBJECT

public:
    MyGLWidget(QWidget* parent = nullptr);
    ~MyGLWidget();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void Move();
    void keyPressEvent(QKeyEvent* e);
    void wheelEvent(QWheelEvent* event);
    void loadOBJ3(const char* path, vector<vec3>& out_vertices,
        vector<vec2>& out_uvs, vector<vec3>& out_normals, int& obj_size);
    void loadOBJ4(const char* path, vector<vec3>& out_vertices,
        vector<vec2>& out_uvs, vector<vec3>& out_normals, int& obj_size);
    GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
    GLuint loadBMP(const char* path);

private:
    QTimer* timer;
    // 运动参数
    GLint paintTimes;
    GLboolean silent;
    vector<vec3> cobble_Trans;
    vector<vec3> tree_Trans;
    vec3 bird_o_Tran;
    vec3 bird_t_Tran;
    vector<vec3> bird_Trans;
    vec3 birdRotationAxis;
    GLfloat bird_agl;
    vec3 frog_Trans;
    GLboolean frog_hide;
    vec3 deerRotationAxis;
    GLfloat deer_agl;
    GLfloat deer_part;
    vec3 deer_Trans;
    vec3 deer_front_Trans;
    vec3 deer_behind_Trans;
    
    //着色器id
    GLuint program_DepthShader;
    GLuint program_debugDepthQ;
    GLuint program_shader;
    // 着色器参数ID
    //program_DepthShader
    GLuint lightSpaceMatrixID;
    GLuint modelID;

    //program_debugDepthQ
    GLuint depthMapID;
    GLuint near_planeID;
    GLuint far_planeID;

    //program_shader
    GLuint  projectionID;
    GLuint	viewID;
    GLuint	modelID_shader;
    GLuint	lightSpaceMatrixID_shader;
    GLuint  lightPosID;
    GLuint  diffuseTextureID;
    GLuint  shadowMapID;
    GLuint  viewPosID;

    //阴影参数
    const unsigned int SCR_WIDTH = 1024;
    const unsigned int SCR_HEIGHT = 768;
    GLuint depthMapFBO;
    GLuint depthMap;
    GLuint depthMapid;
    GLuint near_planeid;
    GLuint far_planeid;
    float move_light;


    // 投影、观察、模型矩阵
    mat4 projection;
    mat4 view;
    mat4 model;
    vec3 light_pos;
    // 视角参数
    GLfloat field;
    GLfloat posX;
    GLfloat posY;
    GLfloat posZ;
    GLfloat centerX;
    GLfloat centerY;
    GLfloat centerZ;
    // 缓冲区
    vector<vec3> vertices;
    vector<vec2> uvs;
    vector<vec3> normals;
    GLuint vertexbuffer;
    GLuint uvbuffer;
    GLuint normalbuffer;
    // 模型大小
    GLint skybox_size;
    GLint ground_size;
    GLint flower_size;
    GLint mushroom_size;
    GLint rock_size;
    GLint branch_size;
    GLint leaf_size;
    GLint grass_size;
    GLint cobble_size;
    GLint frog_size;
    GLint tree_size;
    GLint trunk_size;
    GLint bird_size;
    GLint bird_wing_left_size;
    GLint bird_wing_right_size;
    GLint deer_head_size;
    GLint deer_body_size;
    GLint deer_front_left_size;
    GLint deer_front_right_size;
    GLint deer_behind_left_size;
    GLint deer_behind_right_size; 
    GLint bird2_size;
    GLint bird3_size;
    // 纹理
    GLuint skybox_texture;
    GLuint ground_texture;
    GLuint flower_texture;
    GLuint mushroom_texture;
    GLuint rock_texture;
    GLuint branch_texture;
    GLuint leaf_texture;
    GLuint grass_texture;
    GLuint cobble_texture;
    GLuint frog_texture;
    GLuint tree_texture;
    GLuint trunk_texture;
    GLuint bird_blue_texture;
    GLuint bird_yellow_texture;
    GLuint deer_texture;
};
#endif // MYGLWIDGET_H
