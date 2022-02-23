#include "myglwidget.h"





/*###################################################
##  函数: MyGLWidget
##  函数描述： MyGLWidget类的构造函数，实例化定时器timer
##  参数描述：
##  parent: MyGLWidget的父对象
#####################################################*/
MyGLWidget::MyGLWidget(QWidget* parent)
	:QOpenGLWidget(parent)
{
	timer = new QTimer(this); // 实例化一个定时器
	timer->start(32); // 时间间隔设置为32ms，可以根据需要调整
	// 连接update()函数，每16ms触发一次update()函数进行重新绘图
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));

}

/*###################################################
##  函数: ~MyGLWidget
##  函数描述： ~MyGLWidget类的析构函数，删除timer
##  参数描述： 无
#####################################################*/
MyGLWidget::~MyGLWidget()
{
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	delete this->timer;
}

/*###################################################
##  函数: initializeGL
##  函数描述： 初始化绘图参数，如视窗大小、背景色、OBJ模型
##  参数描述： 无
#####################################################*/
void MyGLWidget::initializeGL()
{
	paintTimes = 0;
	silent = false;
	// 视野
	field = 75.0f;
	// 摄像机位置
	posX = 0.0f;
	posY = 50.0f;
	posZ = 130.0f;
	// 观察点位置
	centerX = 0.0f;
	centerY = 10.0f;
	centerZ = 0.0f;
	move_light = 350.0f;
	// 卵石路参数
	for (int i = 0; i < 32; i++) {
		GLfloat cobble_x = 2.625 * i;
		GLfloat cobble_y = 1.735;
		GLfloat cobble_z = 6.875 * i;
		if (i < 4) {
			cobble_x = 0.0;
			cobble_y = i * 0.25;
		}
		else if (i >= 4 && i < 13) {
			cobble_x = 2.625 * (i - 4);
		}
		else if (i >= 13 && i < 26) {
			cobble_x = 42 - 2.625 * (i - 4);
		}
		else if (i >= 26) {
			cobble_x = -15.75 + 2.625 * (i - 26);
		}
		cobble_Trans.push_back(vec3(cobble_x, cobble_y, cobble_z));
	}
	// 森林参数
	tree_Trans.push_back(vec3(0.0f, 0.0f, 0.0f));
	tree_Trans.push_back(vec3(3.74f, 0.0f, 60.12f));
	tree_Trans.push_back(vec3(-22.86f, -6.31f, 115.01f));
	tree_Trans.push_back(vec3(-71.31f, -5.31f, 147.02f));
	tree_Trans.push_back(vec3(-126.48f, -2.12f, 137.04f));
	tree_Trans.push_back(vec3(-162.50f, -1.91f, 97.52f));
	tree_Trans.push_back(vec3(-189.99f, -1.91f, 54.56f));
	tree_Trans.push_back(vec3(-177.32f, -2.75f, 1.09f));
	// 鸟群参数
	birdRotationAxis = vec3(0.0f, 1.0f, 0.0f);
	bird_agl = 0.0f;
	bird_o_Tran = vec3(44.74f, 52.10f, 55.74f);
	bird_t_Tran = vec3(0.0f, 0.0f, 0.0f);
	bird_Trans.push_back(vec3(0.0f, 0.0f, 0.0f));
	bird_Trans.push_back(vec3(7.68f, 0.4f, -8.06f));
	bird_Trans.push_back(vec3(14.11f, 0.0f, -19.09f));
	bird_Trans.push_back(vec3(-13.30f, 0.4f, 3.29f));
	bird_Trans.push_back(vec3(-24.67f, 0.7f, -3.33f));
	// 青蛙参数
	frog_Trans = vec3(0.0f, 0.0f, 0.0f);
	frog_hide = false;
	// 公鹿参数
	deerRotationAxis = vec3(0.0f, 0.0f, 1.0f);
	deer_front_Trans = vec3(5.0f, 28.9f, -30.0f);
	deer_behind_Trans = vec3(-10.0f, 30.0f, -20.0f);
	deer_agl = 0.0f;
	deer_part = 1.0f;

	// 初始化OpenGL
	initializeOpenGLFunctions();

	// 固定分辨率
	setFixedWidth(1024);
	setFixedHeight(768);
	glViewport(0, 0, width(), height());

	// 开启深度测试
	glEnable(GL_DEPTH_TEST);

	// 加载着色器
	program_DepthShader = LoadShaders("./shaders/DepthShader.vs", "./shaders/DepthShader.fs");
	program_debugDepthQ = LoadShaders("./shaders/debugDepthQ.vs", "./shaders/debugDepthQ.fs");
	program_shader = LoadShaders("./shaders/shader.vs", "./shaders/shader.fs");


	// 初始化

	//program_DepthShader
	lightSpaceMatrixID = glGetUniformLocation(program_DepthShader, "lightSpaceMatrix");
	modelID = glGetUniformLocation(program_DepthShader, "model");


	//program_debugDepthQ
	depthMapID = glGetUniformLocation(program_debugDepthQ, "depthMap");
	near_planeID = glGetUniformLocation(program_debugDepthQ, "near_plane");
	far_planeID = glGetUniformLocation(program_debugDepthQ, "far_planeid");

	//program_shader

	projectionID = glGetUniformLocation(program_shader, "projection");
	viewID = glGetUniformLocation(program_shader, "view");
	modelID_shader = glGetUniformLocation(program_shader, "model");
	lightSpaceMatrixID_shader = glGetUniformLocation(program_shader, "lightSpaceMatrix");


	diffuseTextureID = glGetUniformLocation(program_shader, "diffuseTexture");
	shadowMapID = glGetUniformLocation(program_shader, "shadowMapID");
	lightPosID = glGetUniformLocation(program_shader, "lightPos");
	viewPosID = glGetUniformLocation(program_shader, "viewPos");



	// 加载OBJ
	loadOBJ3("./objs/skybox.obj", vertices, uvs, normals, skybox_size);
	loadOBJ3("./objs/ground.obj", vertices, uvs, normals, ground_size);
	loadOBJ4("./objs/flower.obj", vertices, uvs, normals, flower_size);
	loadOBJ3("./objs/mushroom.obj", vertices, uvs, normals, mushroom_size);
	loadOBJ4("./objs/rock.obj", vertices, uvs, normals, rock_size);
	loadOBJ3("./objs/branch.obj", vertices, uvs, normals, branch_size);
	loadOBJ3("./objs/leaf.obj", vertices, uvs, normals, leaf_size);
	loadOBJ3("./objs/grass.obj", vertices, uvs, normals, grass_size);
	loadOBJ4("./objs/cobble.obj", vertices, uvs, normals, cobble_size);
	loadOBJ3("./objs/frog.obj", vertices, uvs, normals, frog_size);
	loadOBJ3("./objs/tree.obj", vertices, uvs, normals, tree_size);
	loadOBJ3("./objs/trunk.obj", vertices, uvs, normals, trunk_size);
	loadOBJ3("./objs/bird.obj", vertices, uvs, normals, bird_size);
	loadOBJ3("./objs/bird_wing_left.obj", vertices, uvs, normals, bird_wing_left_size);
	loadOBJ3("./objs/bird_wing_right.obj", vertices, uvs, normals, bird_wing_right_size);
	loadOBJ4("./objs/bird2.obj", vertices, uvs, normals, bird2_size);
	loadOBJ4("./objs/bird3.obj", vertices, uvs, normals, bird3_size);
	loadOBJ3("./objs/deer_body.obj", vertices, uvs, normals, deer_body_size);
	loadOBJ3("./objs/deer_head.obj", vertices, uvs, normals, deer_head_size);
	loadOBJ3("./objs/deer_front_left.obj", vertices, uvs, normals, deer_front_left_size);
	loadOBJ3("./objs/deer_front_right.obj", vertices, uvs, normals, deer_front_right_size);
	loadOBJ3("./objs/deer_behind_left.obj", vertices, uvs, normals, deer_behind_left_size);
	loadOBJ3("./objs/deer_behind_right.obj", vertices, uvs, normals, deer_behind_right_size);
	
	// 第1个缓冲区 -- 顶点缓冲
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
	// 第2个缓冲区 -- UV缓冲
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), &uvs[0], GL_STATIC_DRAW);
	// 第3个缓冲区 -- 法向量缓冲
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);
	// 清理本地空间
	vertices.clear();
	uvs.clear();
	normals.clear();

	// 加载纹理
	skybox_texture = loadBMP("./textures/skybox.bmp");
	ground_texture = loadBMP("./textures/ground.bmp");
	flower_texture = loadBMP("./textures/flower.bmp");
	mushroom_texture = loadBMP("./textures/mushroom.bmp");
	rock_texture = loadBMP("./textures/rock.bmp");
	branch_texture = loadBMP("./textures/branch.bmp");
	leaf_texture = loadBMP("./textures/leaf.bmp");
	grass_texture = loadBMP("./textures/grass.bmp");
	cobble_texture = loadBMP("./textures/cobble.bmp");
	frog_texture = loadBMP("./textures/frog.bmp");
	tree_texture = loadBMP("./textures/tree.bmp");
	trunk_texture = loadBMP("./textures/trunk.bmp");
	bird_blue_texture = loadBMP("./textures/bluebird.bmp");
	bird_yellow_texture = loadBMP("./textures/yellowbird.bmp");
	deer_texture = loadBMP("./textures/deer.bmp");
}

/*###################################################
##  函数: paintGL
##  函数描述： 绘图函数，实现图形绘制，会被update()函数调用
##  参数描述： 无
#####################################################*/
void MyGLWidget::paintGL()
{
	Move();

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// 摄像机位置
	vec3 position = vec3(posX, posY, posZ);
	vec3 center = vec3(centerX, centerY, centerZ);
	vec3 up = vec3(0, 1, 0);
	/*
	// 模型、观察、投影矩阵
	projection = perspective(field, (float)width() / height(), 0.001f, 1000.0f);
	view = lookAt(position, center, up);
	model = mat4(1.0);

	mat3 normal_matrix = mat3(transpose(inverse(view * model)));

	glUniform3f(cameraId, posX, posY, posZ);
	glUniformMatrix4fv(modelId, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(viewId, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionId, 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix3fv(normalId, 1, GL_FALSE, &normal_matrix[0][0]);

	// 光源（太阳）位置
	light_pos = vec3(0.0f, 100.0f, 20.0f);
	glUniform3f(lightId, light_pos.x, light_pos.y, light_pos.z);
	glUniform3f(lightMatId, 1.0, 1.0, 1.0);

	// 初始化光照.
	glUniform3f(ambientMatId, 1.0, 1.0, 1.0);
	glUniform1f(specularPowerId, 1.0);


	// 激活纹理单元
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(textureId, 0);
	*/
	// 第1个缓冲区 -- 顶点缓冲
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 第2个缓冲区 -- UV缓冲
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 第3个缓冲区 -- 法向量缓冲
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	GLint offset = 0;

	//阴影部分 从光源绘制
	//生成深度贴图
	glGenFramebuffers(1, &depthMapFBO);
	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 768;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//生成的深度纹理作为帧缓冲的深度缓冲
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);

	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// shader configuration
	glUseProgram(program_shader);
	glUniform1i(diffuseTextureID, 0);
	glUniform1i(shadowMapID, 1);

	glUseProgram(program_debugDepthQ);
	glUniform1i(depthMapID, 0);


	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	//初始化光源位置
	vec3 lightPos(-2.0f, 20.0f, -1.0f);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//求取光源相关矩阵
	mat4 lightProjection, lightView;
	mat4 lightSpaceMatrix;

	lightProjection = ortho(move_light, -move_light, 420.0f, -420.0f, -100.0f, 240.5f);
	lightView = lookAt(lightPos, center, up);
	lightSpaceMatrix = lightProjection * lightView;


	glUseProgram(program_DepthShader);
	glUniformMatrix4fv(lightSpaceMatrixID, 1, GL_FALSE, &lightSpaceMatrix[0][0]);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	offset = 0;
	// 绘制天空盒
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, skybox_texture);
	glDrawArrays(GL_TRIANGLES, offset, skybox_size);
	offset += skybox_size;
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);


	// 绘制草地
	glDepthMask(GL_FALSE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ground_texture);
	glDrawArrays(GL_TRIANGLES, offset, ground_size);
	offset += ground_size;
	glDepthMask(GL_TRUE);


	// 绘制花丛
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, flower_texture);
	glDrawArrays(GL_QUADS, offset, flower_size);
	offset += flower_size;
	// 绘制蘑菇
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mushroom_texture);
	glDrawArrays(GL_TRIANGLES, offset, mushroom_size);
	offset += mushroom_size;

	// 绘制岩石
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, rock_texture);
	glDrawArrays(GL_QUADS, offset, rock_size);
	offset += rock_size;

	// 绘制灌木
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, branch_texture);
	glDrawArrays(GL_TRIANGLES, offset, branch_size);
	offset += branch_size;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, leaf_texture);
	glDrawArrays(GL_TRIANGLES, offset, leaf_size);
	offset += leaf_size;

	// 绘制草
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grass_texture);
	glDrawArrays(GL_TRIANGLES, offset, grass_size);
	offset += grass_size;

	// 绘制卵石路
	for (int i = 0; i < cobble_Trans.size(); i++) {
		model = translate(model, -cobble_Trans[i]);
		glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cobble_texture);
		glDrawArrays(GL_QUADS, offset, cobble_size);
		model = translate(model, cobble_Trans[i]);
	}
	offset += cobble_size;

	// 绘制小青蛙
	model = translate(model, -frog_Trans);
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, frog_texture);
	if (!frog_hide) glDrawArrays(GL_TRIANGLES, offset, frog_size);
	offset += frog_size;
	model = translate(model, frog_Trans);

	// 绘制森林
	for (int i = 0; i < tree_Trans.size(); i++) {
		model = translate(model, -tree_Trans[i]);
		glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tree_texture);
		glDrawArrays(GL_TRIANGLES, offset, tree_size);
		offset += tree_size;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, trunk_texture);
		glDrawArrays(GL_TRIANGLES, offset, trunk_size);
		offset -= tree_size;
		model = translate(model, tree_Trans[i]);
	}
	offset += tree_size;
	offset += trunk_size;

	// 绘制蓝色小鸟
	model = translate(model, -bird_t_Tran);
	for (int i = 0; i < bird_Trans.size(); i++) {
		model = translate(model, -bird_Trans[i]);
		glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bird_blue_texture);
		glDrawArrays(GL_TRIANGLES, offset, bird_size);
		offset += bird_size;
		model = translate(model, bird_Trans[i]);

		model = translate(model, -bird_Trans[i]);
		model = translate(model, bird_o_Tran);
		model = rotate(model, bird_agl, birdRotationAxis);
		model = translate(model, -bird_o_Tran);
		glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bird_blue_texture);
		glDrawArrays(GL_TRIANGLES, offset, bird_wing_left_size);
		offset += bird_wing_left_size;
		model = translate(model, bird_o_Tran);
		model = rotate(model, -bird_agl, birdRotationAxis);
		model = translate(model, -bird_o_Tran);
		model = translate(model, bird_Trans[i]);

		model = translate(model, -bird_Trans[i]);
		model = translate(model, bird_o_Tran);
		model = rotate(model, -bird_agl, birdRotationAxis);
		model = translate(model, -bird_o_Tran);

		glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bird_blue_texture);
		glDrawArrays(GL_TRIANGLES, offset, bird_wing_right_size);
		//offset += bird_wing_right_size;
		offset -= bird_wing_left_size;
		offset -= bird_size;
		model = translate(model, bird_o_Tran);
		model = rotate(model, bird_agl, birdRotationAxis);
		model = translate(model, -bird_o_Tran);
		model = translate(model, bird_Trans[i]);
	}
	offset += bird_size + bird_wing_left_size + bird_wing_right_size;
	model = translate(model, bird_t_Tran);

		// 绘制黄色小鸟1

		glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bird_yellow_texture);
		glDrawArrays(GL_QUADS, offset, bird2_size);
		offset += bird2_size;

		// 绘制黄色小鸟2
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bird_yellow_texture);
		glDrawArrays(GL_QUADS, offset, bird3_size);
		offset += bird3_size;

		// 绘制公鹿
		model = translate(model, deer_Trans);
		// 绘制公鹿身体
		glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, deer_texture);
		glDrawArrays(GL_TRIANGLES, offset, deer_body_size);
		offset += deer_body_size;

		// 绘制公鹿头部

		glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, deer_texture);
		glDrawArrays(GL_TRIANGLES, offset, deer_head_size);
		offset += deer_head_size;

		//绘制鹿的左前腿
		model = translate(model, deer_front_Trans);
		model = rotate(model, -deer_agl, deerRotationAxis);
		model = translate(model, -deer_front_Trans);

		glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, deer_texture);
		glDrawArrays(GL_TRIANGLES, offset, deer_front_left_size);
		offset += deer_front_left_size;
		model = translate(model, deer_front_Trans);
		model = rotate(model, deer_agl, deerRotationAxis);
		model = translate(model, -deer_front_Trans);

		//右前腿
		model = translate(model, deer_front_Trans);
		model = rotate(model, deer_agl, deerRotationAxis);
		model = translate(model, -deer_front_Trans);

		glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, deer_texture);
		glDrawArrays(GL_TRIANGLES, offset, deer_front_right_size);
		offset += deer_front_right_size;
		model = translate(model, deer_front_Trans);
		model = rotate(model, -deer_agl, deerRotationAxis);
		model = translate(model, -deer_front_Trans);

		//左后腿
		model = translate(model, deer_behind_Trans);
		model = rotate(model, deer_agl, deerRotationAxis);
		model = translate(model, -deer_behind_Trans);

		glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, deer_texture);
		glDrawArrays(GL_TRIANGLES, offset, deer_behind_left_size);
		offset += deer_behind_left_size;
		model = translate(model, deer_behind_Trans);
		model = rotate(model, -deer_agl, deerRotationAxis);
		model = translate(model, -deer_behind_Trans);

		//右后腿
		model = translate(model, deer_behind_Trans);
		model = rotate(model, -deer_agl, deerRotationAxis);
		model = translate(model, -deer_behind_Trans);

		glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, deer_texture);
		glDrawArrays(GL_TRIANGLES, offset, deer_behind_right_size);
		offset += deer_behind_right_size;
		model = translate(model, deer_behind_Trans);
		model = rotate(model, deer_agl, deerRotationAxis);
		model = translate(model, -deer_behind_Trans);

		// 绘制结束
		model = translate(model, -deer_Trans);
	
	// 从相机位置绘制
	model = mat4(1.0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glUseProgram(program_shader);
	projection = perspective(field, (float)width() / height(), 0.001f, 1000.0f);
	view = lookAt(vec3(posX, posY, posZ), vec3(centerX, centerY, centerZ), up);

	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &view[0][0]);
	vec3 viewPos = vec3(posX, posY, posZ);
	glUniform3f(viewPosID, posX, posY, posZ);

	glUniform3f(lightPosID, lightPos.x, lightPos.y, lightPos.z);
	glUniformMatrix4fv(lightSpaceMatrixID_shader, 1, GL_FALSE, &lightSpaceMatrix[0][0]);

	glUniformMatrix4fv(modelID_shader, 1, GL_FALSE, &model[0][0]);
	offset = 0;
	// 绘制天空盒  暂时禁止写入深度缓冲
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, skybox_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glDrawArrays(GL_TRIANGLES, offset, skybox_size);
	offset += skybox_size;
	glDepthMask(GL_TRUE);
	// 开启背面剔除
	glEnable(GL_CULL_FACE);
	
	// 绘制草地
	glDepthMask(GL_FALSE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ground_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glDrawArrays(GL_TRIANGLES, offset, ground_size);
	offset += ground_size;
	glDepthMask(GL_TRUE);

	// 绘制花丛
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, flower_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glDrawArrays(GL_QUADS, offset, flower_size);
	offset += flower_size;
	// 绘制蘑菇
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mushroom_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glDrawArrays(GL_TRIANGLES, offset, mushroom_size);
	offset += mushroom_size;

	// 绘制岩石
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, rock_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glDrawArrays(GL_QUADS, offset, rock_size);
	offset += rock_size;

	// 绘制灌木
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, branch_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glDrawArrays(GL_TRIANGLES, offset, branch_size);
	offset += branch_size;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, leaf_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glDrawArrays(GL_TRIANGLES, offset, leaf_size);
	offset += leaf_size;

	// 绘制草
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grass_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glDrawArrays(GL_TRIANGLES, offset, grass_size);
	offset += grass_size;

	// 绘制卵石路
	for (int i = 0; i < cobble_Trans.size(); i++) {
		model = translate(model, -cobble_Trans[i]);
		glUniformMatrix4fv(modelID_shader, 1, GL_FALSE, &model[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cobble_texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glDrawArrays(GL_QUADS, offset, cobble_size);
		model = translate(model, cobble_Trans[i]);
	}
	offset += cobble_size;

	// 绘制小青蛙
	model = translate(model, -frog_Trans);
	glUniformMatrix4fv(modelID_shader, 1, GL_FALSE, &model[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, frog_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	if (!frog_hide) glDrawArrays(GL_TRIANGLES, offset, frog_size);
	offset += frog_size;
	model = translate(model, frog_Trans);

	// 绘制森林
	for (int i = 0; i < tree_Trans.size(); i++) {
		model = translate(model, -tree_Trans[i]);
		glUniformMatrix4fv(modelID_shader, 1, GL_FALSE, &model[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tree_texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glDrawArrays(GL_TRIANGLES, offset, tree_size);
		offset += tree_size;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, trunk_texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glDrawArrays(GL_TRIANGLES, offset, trunk_size);
		offset -= tree_size;
		model = translate(model, tree_Trans[i]);
	}
	offset += tree_size;
	offset += trunk_size;
	
	// 绘制蓝色小鸟
	model = translate(model, -bird_t_Tran);
	for (int i = 0; i < bird_Trans.size(); i++) {
		model = translate(model, -bird_Trans[i]);
		glUniformMatrix4fv(modelID_shader, 1, GL_FALSE, &model[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bird_blue_texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glDrawArrays(GL_TRIANGLES, offset, bird_size);
		offset += bird_size;
		model = translate(model, bird_Trans[i]);

		model = translate(model, -bird_Trans[i]);
		model = translate(model, bird_o_Tran);
		model = rotate(model, bird_agl, birdRotationAxis);
		model = translate(model, -bird_o_Tran);
		glUniformMatrix4fv(modelID_shader, 1, GL_FALSE, &model[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bird_blue_texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glDrawArrays(GL_TRIANGLES, offset, bird_wing_left_size);
		offset += bird_wing_left_size;
		model = translate(model, bird_o_Tran);
		model = rotate(model, -bird_agl, birdRotationAxis);
		model = translate(model, -bird_o_Tran);
		model = translate(model, bird_Trans[i]);

		model = translate(model, -bird_Trans[i]);
		model = translate(model, bird_o_Tran);
		model = rotate(model, -bird_agl, birdRotationAxis);
		model = translate(model, -bird_o_Tran);

		glUniformMatrix4fv(modelID_shader, 1, GL_FALSE, &model[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bird_blue_texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glDrawArrays(GL_TRIANGLES, offset, bird_wing_right_size);
		//offset += bird_wing_right_size;
		offset -= bird_wing_left_size;
		offset -= bird_size;
		model = translate(model, bird_o_Tran);
		model = rotate(model, bird_agl, birdRotationAxis);
		model = translate(model, -bird_o_Tran);
		model = translate(model, bird_Trans[i]);
	}
	offset += bird_size + bird_wing_left_size + bird_wing_right_size;
	model = translate(model, bird_t_Tran);

	// 绘制黄色小鸟1

	glUniformMatrix4fv(modelID_shader, 1, GL_FALSE, &model[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bird_yellow_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glDrawArrays(GL_QUADS, offset, bird2_size);
	offset += bird2_size;

	// 绘制黄色小鸟2
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bird_yellow_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glDrawArrays(GL_QUADS, offset, bird3_size);
	offset += bird3_size;

	// 绘制公鹿
	model = translate(model, deer_Trans);
	// 绘制公鹿身体
	glUniformMatrix4fv(modelID_shader, 1, GL_FALSE, &model[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, deer_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glDrawArrays(GL_TRIANGLES, offset, deer_body_size);
	offset += deer_body_size;

	// 绘制公鹿头部

	glUniformMatrix4fv(modelID_shader, 1, GL_FALSE, &model[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, deer_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glDrawArrays(GL_TRIANGLES, offset, deer_head_size);
	offset += deer_head_size;

	//绘制鹿的左前腿
	model = translate(model, deer_front_Trans);
	model = rotate(model, -deer_agl, deerRotationAxis);
	model = translate(model, -deer_front_Trans);

	glUniformMatrix4fv(modelID_shader, 1, GL_FALSE, &model[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, deer_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glDrawArrays(GL_TRIANGLES, offset, deer_front_left_size);
	offset += deer_front_left_size;
	model = translate(model, deer_front_Trans);
	model = rotate(model, deer_agl, deerRotationAxis);
	model = translate(model, -deer_front_Trans);

	//右前腿
	model = translate(model, deer_front_Trans);
	model = rotate(model, deer_agl, deerRotationAxis);
	model = translate(model, -deer_front_Trans);

	glUniformMatrix4fv(modelID_shader, 1, GL_FALSE, &model[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, deer_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glDrawArrays(GL_TRIANGLES, offset, deer_front_right_size);
	offset += deer_front_right_size;
	model = translate(model, deer_front_Trans);
	model = rotate(model, -deer_agl, deerRotationAxis);
	model = translate(model, -deer_front_Trans);

	//左后腿
	model = translate(model, deer_behind_Trans);
	model = rotate(model, deer_agl, deerRotationAxis);
	model = translate(model, -deer_behind_Trans);

	glUniformMatrix4fv(modelID_shader, 1, GL_FALSE, &model[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, deer_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glDrawArrays(GL_TRIANGLES, offset, deer_behind_left_size);
	offset += deer_behind_left_size;
	model = translate(model, deer_behind_Trans);
	model = rotate(model, -deer_agl, deerRotationAxis);
	model = translate(model, -deer_behind_Trans);

	//右后腿
	model = translate(model, deer_behind_Trans);
	model = rotate(model, -deer_agl, deerRotationAxis);
	model = translate(model, -deer_behind_Trans);

	glUniformMatrix4fv(modelID_shader, 1, GL_FALSE, &model[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, deer_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glDrawArrays(GL_TRIANGLES, offset, deer_behind_right_size);
	offset += deer_behind_right_size;
	model = translate(model, deer_behind_Trans);
	model = rotate(model, deer_agl, deerRotationAxis);
	model = translate(model, -deer_behind_Trans);

	// 绘制结束
	model = translate(model, -deer_Trans);
	
	// 结束绘制
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}


/*###################################################
##  函数: resizeGL
##  函数描述： 当窗口大小改变时调整视窗尺寸
##  参数描述： 无
#####################################################*/
void MyGLWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	printf("Changing viewport:%d, %d\n", width, height);
	update();
}

/*###################################################
##  函数: Move
##  函数描述： 在每次绘制时，根据绘制次数设置矩阵参数，
##			   静止时不进行计算
##  参数描述： 无
#####################################################*/
void MyGLWidget::Move()
{
	if (silent)
		return;

	// 绘制次数
	paintTimes++;

	// 阴影参数
	GLfloat light_move = 1;
	if (move_light < 480) {
		move_light += light_move;
	}

	// 青蛙的跳动
	GLfloat frog_move_x = 0.2;
	GLfloat frog_move_z = -0.1;
	GLfloat frog_move_y = 0.0f;
	GLint frog_part = paintTimes % 20;
	GLfloat frog_step = GLfloat(paintTimes % 10000);
	if (frog_part <= 10) {
		frog_move_y = 0.4 * frog_part;
	}
	else {
		frog_move_y = 8.0 - 0.4 * frog_part;
	}

	if (frog_step > 200.0f) {
		frog_hide = true;
	}
	frog_Trans = vec3(frog_move_x * frog_step, -frog_move_y, frog_move_z * frog_step);

	// 鹿的运动
	GLfloat deer_move = 0.5;
	GLfloat deer_step = GLfloat(paintTimes % 10000);
	deer_agl += deer_part;
	if (deer_agl > 15.0) {
		deer_part = -deer_part;
	}
	else if (deer_agl < -13.0) {
		deer_part = -deer_part;
	}
	if (deer_step >= 200.0f) {
		deer_step = 200.0f;
		deer_agl = 0.0f;
	}
	deer_Trans = vec3(deer_step / 5.0, 0.0f, -deer_move * deer_step);

	// 鸟的飞舞
	GLfloat bird_speed = 2.0f;
	GLfloat bird_move_x = 0.5 * bird_speed;
	GLfloat bird_move_y = 0.325 * bird_speed;
	GLfloat bird_move_z = 1.1 * bird_speed;
	GLint bird_part = paintTimes % 10;
	GLfloat bird_step = GLfloat(paintTimes % 10000);

	if (bird_part <= 5) {
		bird_agl = -15.0 + 8.0 * bird_part;
	}
	else {
		bird_agl = 65 - 8.0 * bird_part;
	}
	if (bird_step >= 1000.0f) {
		bird_step = 1000.0f;
		bird_agl = 0.0f;
	}
	bird_t_Tran = vec3(bird_move_x * bird_step, -bird_move_y * bird_step, bird_move_z * bird_step);

}

/*###################################################
##  函数: keyPressEvent
##  函数描述： 根据键盘输入调整视角和切换光照模型
##  参数描述： 无
#####################################################*/
void MyGLWidget::keyPressEvent(QKeyEvent* e)
{
	// 步长
	GLfloat move = 1.0f;
	GLfloat move_x = (centerX - posX) / sqrt(pow((centerX - posX), 2) + pow((centerZ - posZ), 2)) * move;
	GLfloat move_z = (centerZ - posZ) / sqrt(pow((centerZ - posZ), 2) + pow((centerZ - posZ), 2)) * move;

	// 调整观察者位置
	if (e->key() == Qt::Key_Q) {
		posY += move;
	}
	else if (e->key() == Qt::Key_E) {
		posY -= move;
	}
	else if (e->key() == Qt::Key_W) {
		posX += move_x;
		posZ += move_z;
	}
	else if (e->key() == Qt::Key_S) {
		posX -= move_x;
		posZ -= move_z;
	}
	else if (e->key() == Qt::Key_A) {
		posX += move_z;
		posZ += move_x;
	}
	else if (e->key() == Qt::Key_D) {
		posX -= move_z;
		posZ -= move_x;
	}
	// 调整观察目标位置
	else if (e->key() == Qt::Key_U) {
		centerY += move;
	}
	else if (e->key() == Qt::Key_O) {
		centerY -= move;
	}
	else if (e->key() == Qt::Key_I) {
		centerX += move_x;
		centerZ += move_z;
	}
	else if (e->key() == Qt::Key_K) {
		centerX -= move_x;
		centerZ -= move_z;
	}
	else if (e->key() == Qt::Key_J) {
		centerX += move_z;
		centerZ += move_x;
	}
	else if (e->key() == Qt::Key_L) {
		centerX -= move_z;
		centerZ -= move_x;
	}
	// 运动静止
	else if (e->key() == Qt::Key_R) {
		if (silent == true)
			silent = false;
		else silent = true;
	}
	// 恢复默认视角
	else if (e->key() == Qt::Key_F) {
		field = 75.0f;
		posX = 0.0f;
		posY = 80.0f;
		posZ = 130.0f;
		centerX = 0.0f;
		centerY = 10.0f;
		centerZ = 0.0f;
	}
	// 打印当前视角，便于调试
	else if (e->key() == Qt::Key_G) {
		printf("pos:%f %f %f\n", posX, posY, posZ);
		printf("center:%f %f %f\n", centerX, centerY, centerZ);
		printf("\n");
	}
	// 恢复初始状态
	else if (e->key() == Qt::Key_Y) {
		paintTimes = 0;

		frog_Trans = vec3(0.0f, 0.0f, 0.0f);
		frog_hide = false;

		deer_agl = 0.0f;
		deer_part = 1.0f;
		move_light = 350.0f;
	}
	// 切换光照模型
	// 切换光照模型
	else if (e->key() == Qt::Key_0) {
	}
	else if (e->key() == Qt::Key_1) {
	}
	else if (e->key() == Qt::Key_2) {
	}
	else if (e->key() == Qt::Key_3) {
	}
	else if (e->key() == Qt::Key_4) {
		//programID = LoadShaders("./shaders/shading.vs", "./shaders/shading.fs");
	}
	else if (e->key() == Qt::Key_5) {
	}
	else if (e->key() == Qt::Key_6) {
	}
	else if (e->key() == Qt::Key_7) {
	}
	else if (e->key() == Qt::Key_8) {
	}
	update();
}

/*###################################################
##  函数: wheelEvent
##  函数描述： 根据鼠标滚轮调整视野
##  参数描述： 无
#####################################################*/
void MyGLWidget::wheelEvent(QWheelEvent* event)
{
	GLfloat move = 1.0f;
	if (event->delta() < 0) {
		field += move;
	}
	else if (event->delta() > 0) {
		field -= move;
	}
	update();
}

/*###################################################
##  函数: loadShader
##  函数描述： 创建和编译着色器
##  参数描述： 顶点着色器，片元着色器
#####################################################*/
GLuint MyGLWidget::LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
	// 创建着色器
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// 读取顶点着色器
	string VertexShaderCode;
	ifstream VertexShaderStream(vertex_file_path, ios::in);
	if (VertexShaderStream.is_open()) {
		stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Failed to open the vertex file, please check again.\n");
		return 0;
	}

	// 读取片元着色器
	string FragmentShaderCode;
	ifstream FragmentShaderStream(fragment_file_path, ios::in);
	if (FragmentShaderStream.is_open()) {
		stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}
	else {
		printf("Failed to open the fragment file, please check again.\n");
		return 0;
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// 编译顶点着色器
	printf("Compiling shader %s...\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// 检查顶点着色器
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// 编译片元着色器
	printf("Compiling shader %s...\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// 检查片元着色器
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// 创建并链接着色器程序
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// 检查着色器程序
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	// 释放着色器
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	printf("Load shaders successfully.\n\n");
	// 返回着色器程序ID
	return ProgramID;
}


/*###################################################
##  函数: loadBMP
##  函数描述： 导入纹理
##  参数描述： 纹理文件名
#####################################################*/
GLuint MyGLWidget::loadBMP(const char* path)
{
	printf("Loading image %s...\n", path);

	// BMP 包含的信息
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	unsigned int bpp;
	unsigned int inRGB;
	unsigned int outRGB;
	// RGB 数据
	unsigned char* data;

	FILE* file = fopen(path, "rb");
	if (!file)
	{
		printf("Failed to open file %s, please check again.\n", path);
		getchar();
		return 0;
	}

	// 检查 BMP 头部是否正确
	if (fread(header, 1, 54, file) != 54)
	{
		printf("Not a correct BMP file.\n");
		fclose(file);
		return 0;
	}
	// BMP 文件以"BM"开头
	if (header[0] != 'B' || header[1] != 'M')
	{
		printf("Not a correct BMP file.\n");
		fclose(file);
		return 0;
	}
	// 确保是 24bpp 或 32bpp 文件，前者对应BGR，后者对应BGRA
	if (*(int*)&(header[0x1C]) == 24) {
		inRGB = GL_BGR;
		outRGB = GL_RGB;
		bpp = 3;
	}
	else if (*(int*)&(header[0x1C]) == 32) {
		bpp = 4;
		inRGB = GL_BGRA;
		outRGB = GL_RGBA;
	}
	else {
		printf("Not a correct BMP file..\n");
		fclose(file);
		return 0;
	}

	// 根据头部获得图片的信息
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// 部分BMP文件缺失了imageSize和datapos，需要手动补全信息
	if (imageSize == 0)
		imageSize = width * height * bpp; // 24bpp对应3通道， 32bpp对应4通道
	if (dataPos == 0)
		dataPos = 54;					  // 头部结束位置 

	// 读取数据
	data = new unsigned char[imageSize];
	fread(data, 1, imageSize, file);
	fclose(file);

	// 创建和绑定纹理
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, outRGB, width, height, 0, inRGB, 
		GL_UNSIGNED_BYTE, data);

	// 粗糙的过滤，纹理质量不好
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// 使用线性过滤
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// mipmap采样
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// 创建mipmap
	glGenerateMipmap(GL_TEXTURE_2D);

	// 返回生成的纹理ID
	printf("Load BMP file %s successfully.\n\n", path);
	delete[] data;
	return textureID;
}


/*###################################################
##  函数: loadOBJ3
##  函数描述： 导入三角形OBJ模型
##  参数描述： OBJ文件名，顶点坐标，纹理坐标，顶点法向量，顶点数
#####################################################*/
void MyGLWidget::loadOBJ3(
	const char* path,
	vector<vec3>& out_vertices,
	vector<vec2>& out_uvs,
	vector<vec3>& out_normals,
	int& obj_size)
{
	printf("Loading OBJ file %s...\n", path);

	vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	vector<vec3> temp_vertices;
	vector<vec2> temp_uvs;
	vector<vec3> temp_normals;

	FILE* file = fopen(path, "r");
	if (file == NULL)
	{
		printf("Failed to open file %s, please check again.\n", path);
		exit(-1);
	}

	while (1)
	{
		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;

		if (strcmp(lineHeader, "v") == 0)
		{
			vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			//uv.y = 1.0 - uv.y;
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0],
				&vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9)
			{
				printf("There is something wrong with this file.\n");
				fclose(file);
				exit(-1);
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else
		{	// 只考虑obj文件中的v, vt, vn和f数据块
			char trashBuffer[1000];
			fgets(trashBuffer, 1000, file);
		}
	}

	// 对于每个面（三角形）上的每个顶点
	obj_size = vertexIndices.size();
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		// 获得参数的索引
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// obj文件中索引以1开始，数组索引以0开始，需要-1
		vec3 vertex = temp_vertices[vertexIndex - 1];
		vec2 uv = temp_uvs[uvIndex - 1];
		vec3 normal = temp_normals[normalIndex - 1];

		// 将参数存入输出中
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);
	}
	fclose(file);
	printf("Load OBJ file %s successfully.\n\n", path);
	return;
}


/*###################################################
##  函数: loadOBJ4
##  函数描述： 导入三角形OBJ模型
##  参数描述： OBJ文件名，顶点坐标，纹理坐标，顶点法向量，顶点数
#####################################################*/
void MyGLWidget::loadOBJ4(
	const char* path,
	vector<vec3>& out_vertices,
	vector<vec2>& out_uvs,
	vector<vec3>& out_normals,
	int& obj_size)
{
	printf("Loading OBJ file %s...\n", path);

	vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	vector<vec3> temp_vertices;
	vector<vec2> temp_uvs;
	vector<vec3> temp_normals;

	FILE* file = fopen(path, "r");
	if (file == NULL)
	{
		printf("Failed to open file %s, please check again.\n", path);
		exit(-1);
	}

	while (1)
	{
		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;

		if (strcmp(lineHeader, "v") == 0)
		{
			vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			//uv.y = 1.0 - uv.y;
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0],
				&vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2],
				&vertexIndex[3], &uvIndex[3], &normalIndex[3]);
			if (matches != 12)
			{
				printf("There is something wrong with this file.\n");
				fclose(file);
				exit(-1);
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			vertexIndices.push_back(vertexIndex[3]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			uvIndices.push_back(uvIndex[3]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
			normalIndices.push_back(normalIndex[3]);
		}
		else
		{	// 只考虑obj文件中的v, vt, vn和f数据块
			char trashBuffer[1000];
			fgets(trashBuffer, 1000, file);
		}
	}

	// 对于每个面（四边形）上的每个顶点
	obj_size = vertexIndices.size();
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		// 获得参数的索引
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// obj文件中索引以1开始，数组索引以0开始，需要-1
		vec3 vertex = temp_vertices[vertexIndex - 1];
		vec2 uv = temp_uvs[uvIndex - 1];
		vec3 normal = temp_normals[normalIndex - 1];

		// 将参数存入输出中
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);
	}
	fclose(file);
	printf("Load OBJ file %s successfully.\n\n", path);
	return;
}
