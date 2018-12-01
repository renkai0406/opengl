
#include "opengl.h"
#include "shader.h"

class Polygon {
public:
	void init();

protected:
	GLuint vao, vbo, ibo;
	GLuint texture_dif, texture_spe;
	void initTexture(const char* path, GLuint *texture, bool gamma);

};

/*������*/
class Cube : public Polygon {

public:
	void init(float length);
	void draw(ShaderManager * shader, mat4 model);
	void initCubeTexture(const char * dif_path, const char * spe_path, bool gamma = false);
private:
	float length;
	void initCubeVertex();
};

/*�ذ�*/
class Panel : public Polygon {
public:
	void init(float width, float height);
	void draw(ShaderManager * shader, mat4 model);
	void initPanelTexture(const char * dif_path, const char * spe_path, bool gamma = false);
private:
	float width, height;
	void initPanelVertex();
};

