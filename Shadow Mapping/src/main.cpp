// Assimp.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "opengl.h"
#include "camera.h"
#include "shader.h"
#include "light.h"
#include "polygon.h"

EditCamera camera;
ShaderManager *shader_scene;
LightManager lights;

Cube cube;
GLfloat cube_length = 3.0f;
GLfloat space = 0.01f;
Panel panel;

mat4 projection;

#pragma region 资源路径
char shader_scene_path[3][27] = {
	"shader/scene_lighting.vert",
	"",
	"shader/scene_lighting.frag" };

char texture_dif_path[] = { "texture/container.png" };
char texture_spe_path[] = { "texture/container_specular.png" };
char wood_texture_spe[] = "texture/wood-specular.jpg";
#pragma endregion

#pragma region screen
GLint window_width = 1024, window_height = 768;
GLint view_width = 1024, view_height = 768;
#pragma endregion

void renderScene(ShaderManager* shader);

void init()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.5, 0.5, 0.5, 1.0);

	shader_scene = ShaderManager::NewInstance();
	shader_scene->load(shader_scene_path[0], shader_scene_path[1], shader_scene_path[2]);
	shader_scene->use();

	cube.init(cube_length);
	cube.initCubeTexture(texture_dif_path, texture_spe_path, true);

	panel.init(40.0f, 40.0f);
	panel.initPanelTexture(wood_texture_spe, wood_texture_spe, true);

	lights.initData();
	lights.setRenderFunc(renderScene);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mat4 view = camera.setCamera();

	shader_scene->use();
	lights.refresh(shader_scene);
	shader_scene->setUniform1i("blinn", 1);
	shader_scene->setUniformMatrix4fv("view", 1, value_ptr(view));
	shader_scene->setUniformMatrix4fv("projection", 1, value_ptr(projection));
	vec3 camera_position = camera.getCameraPosition();
	shader_scene->setUniform3fv("camera_position", 1, value_ptr(camera_position));

	glViewport(0, 0, window_width, window_width);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderScene(shader_scene);

	glutSwapBuffers();
}

void reshape(int w, int h)
{
	window_width = w;
	window_height = h;
	projection = perspective(radians(90.0f), float(window_width / window_height), 0.1f, 300.0f);
	//projection = ortho(-20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 200.0f);
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

void renderScene(ShaderManager* shader)
{
	mat4 model = mat4(1.0f);
	model = rotate(model, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	panel.draw(shader, model);

	model = mat4(1.0f);
	model = translate(model, vec3(0.0f, cube_length / 2 + space, 0.0f));
	cube.draw(shader, model);

	model = translate(model, vec3(5.0f, 1.0f, -5.0f));
	cube.draw(shader, model);

	model = translate(model, vec3(10.0f, 0.0f, -10.0f));
	cube.draw(shader, model);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("OpenGL 阴影贴图");
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