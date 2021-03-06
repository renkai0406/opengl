// Assimp.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "opengl.h"

#include "camera.h"
#include "shader.h"
#include "model.h"
#include "light.h"

EditCamera camera;
ShaderManager *shader_scene;
ModelLoader *loader;
LightManager lights;

mat4 projection;

#pragma region 资源路径
char shader_scene_path[3][27] = {
	"shader/scene_lighting.vert",
	"",
	"shader/scene_lighting.frag" };
//string source_path = "model/nanosuit/nanosuit.obj";
string source_path = "model/Tank.fbx";
#pragma endregion

#pragma region screen
GLint window_width = 600, window_height = 600;
GLint view_width = 600, view_height = 600;
#pragma endregion

void init()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.5, 0.5, 0.5, 1.0);

	shader_scene = ShaderManager::NewInstance();
	shader_scene->load(shader_scene_path[0], shader_scene_path[1], shader_scene_path[2]);
	shader_scene->use();

	loader = new AssimpModel();
	loader->loadScene(source_path);

	lights.initData();

}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mat4 view = camera.setCamera();
	shader_scene->use();
	lights.refresh(shader_scene);
	shader_scene->setUniformMatrix4fv("view", 1, value_ptr(view));
	shader_scene->setUniformMatrix4fv("projection", 1, value_ptr(projection));
	mat4 model = mat4(1.0f);
	model = scale(model, vec3(0.1f));
	shader_scene->setUniformMatrix4fv("model", 1, value_ptr(model));
	loader->draw(shader_scene);
	glutSwapBuffers();
}


void reshape(int w, int h)
{
	window_width = w;
	window_height = h;
	projection = perspective(radians(45.0f), float(view_width / view_height), 0.1f, 300.0f);
	//projection = ortho(-2.0f, 2.0f, -2.0f, 2.0f, 1.0f, 20.0f);
}

void idle() {
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
	camera.updateMouse(button, state, x, y);
}

void mouse_motion(int x, int y) {
	camera.updateMouseMotion(x, y);
}

void special(int key, int x, int y) {
	camera.updateSpecial(key, x, y);
}

void keyboard(unsigned char button, int x, int y) {
	camera.updateKeyBoard(button, x, y);
	switch (button)
	{
	case '1':
		lights.setDirectionalEnable("ld0", !(lights.getDirectional("ld0")->getEnable()));
		break;
	case '2':
		lights.setPointEnable("lp0", !lights.getPoint("lp0")->getEnable());
		break;
	case '3':
		lights.setSpotEnable("ls0", !lights.getSpot("ls0")->getEnable());
		break;
	case '4':
		break;
	default:
		break;
	}
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("冯氏光照模型");
	GLuint result = glewInit();
	if (result != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(result));
		return 1;
	}
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	glutMotionFunc(mouse_motion);
	glutSpecialFunc(special);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}