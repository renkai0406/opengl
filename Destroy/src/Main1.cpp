#include "stdafx.h"
#include "timeutils.h"
#include "utils.h"
#include "opengl.h"
#include "polygon.h"
#include "model.h"
#include "light.h"
#include "camera.h"
#include "shadow.h"

GLint explode_times = 0;
GLboolean exploded = false;

Cube cube;
GLfloat cube_length = 3.0f;
GLfloat space = 0.01f;
Panel panel;

AssimpModel obj;

EditCamera camera;

Shadow shadow_direct;

ShaderManager *shader_model;
ShaderManager *shader_shadow;

mat4 projection;

#pragma region 光照
LightManager lights;
//Blinn-Phong or Phong
bool blinn = false;
//平行光
GLboolean ld_enabled = false;

//点光源
GLboolean lp_enabled = false;

//聚光灯
GLboolean ls_enabled = false;

#pragma endregion

#pragma region 资源路径
char shader_model_path[3][18] = { 
	"shader/model.vert", 
	"shader/model.geom", 
	"shader/model.frag"};
char texture_dif_path[] = {"texture/container.png"};
char texture_spe_path[] = { "texture/container_specular.png" };
char wood_texture_spe[] = "texture/wood-specular.jpg";
string source_path = "model/nanosuit/nanosuit.obj";

#pragma endregion

#pragma region screen
GLint window_width = 600, window_height = 600;
GLint view_width = 600, view_height = 600;
#pragma endregion

void renderScene(ShaderManager * shader) 
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

	//model = scale(model, vec3(0.01f));
	shader_model->setUniformMatrix4fv("model", 1, value_ptr(model));
	obj.draw(shader);

	/*shader_model->setUniformMatrix4fv("model", 1, value_ptr(model));
	for (int i = 0; i < model_count; i++)
	{
	scene[i]->draw(shader);
	}*/
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.5, 0.5, 0.5, 1.0);

	string log_path = "log/" + TimeUtils::getCompleteTime() + ".log";
	LogUtils::createInstance()->init(log_path.c_str());

	shader_model = ShaderManager::NewInstance();
	shader_model->load(shader_model_path[0], shader_model_path[1], shader_model_path[2]);
	shader_model->use();
	lights.initData();

	shadow_direct.init(DIRECTIONAL_LIGHT, lights.getDirectionl(0)->direction);

	cube.init(cube_length);
	cube.initCubeTexture(texture_dif_path, texture_spe_path, true);

	panel.init(40.0f, 40.0f);
	panel.initPanelTexture(wood_texture_spe, wood_texture_spe, true);

	obj.load_model(source_path, false);

	/*for (int i = 0; i < model_count; i++)
	{
		scene[i]->load_model("model/" + scene_path[i]);
	}*/

}

void display()
{
	explode_times = exploded ? (explode_times + 1) : 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mat4 model = mat4(1.0f);
	
	mat4 view = camera.setCamera();
	shader_model->use();
	lights.setAllLightEnable(ld_enabled, lp_enabled, ls_enabled);
	lights.refresh(shader_model);
	shader_model->setUniformMatrix4fv("view", 1, value_ptr(view));
	shader_model->setUniformMatrix4fv("projection", 1, value_ptr(projection));
	vec3 camera_position = camera.getCameraPosition();
	shader_model->setUniform3fv("camera_position", 1, value_ptr(camera_position));
	shader_model->setUniform1i("blinn", blinn);
	shader_model->setUniform1i("explode_times", explode_times);
	shader_model->setUniformMatrix4fv("light_view", 1, value_ptr(*shadow_direct.getView()));
	shader_model->setUniformMatrix4fv("light_projection", 1, value_ptr(*shadow_direct.getProjection()));

	renderScene(shader_model);
	
	glutSwapBuffers();
}


void reshape(int w, int h)
{
	window_width = w;
	window_height = h;
	projection = perspective(radians(45.0f), float(view_width / view_height), 0.1f, 300.0f);
	glViewport(0, 0, w, h);
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
		ld_enabled = !ld_enabled;
		break;
	case '2':
		lp_enabled = !lp_enabled;
		break;
	case '3':
		ls_enabled = !ls_enabled;
		break;
	case '4':
		blinn = !blinn;
		break;
	case '5':
		exploded = !exploded;
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
	glutCreateWindow("openGL 顶点破坏");
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