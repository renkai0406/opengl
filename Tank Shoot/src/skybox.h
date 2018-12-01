#pragma once
#include "stdafx.h"
#include "unicorn.h"
#include "opengl.h"
#include "shader.h"

class Skybox : UnicornBase
{
public:
	void init(vector<string> &faces);
	void draw(ShaderManager *shader);
protected:
private:
	GLuint texId, vao, vbo, ibo;
	vector<float> vertices = {
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f
	};

vector<GLuint> indices = {
		5, 6, 2, 1,//����
		0, 3, 7, 4,//����
		3, 2, 6, 7,//����
		4, 5, 1, 0,//����
		0, 1, 2, 3,//ǰ�� 
		7, 6, 5, 4,//����
	};


	void initVertex();
	void loadCubeTexture(vector<string> &faces);
	
	

};
