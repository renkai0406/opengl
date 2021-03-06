// ShaderJoy.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "maincfg.h"

void framebuffer_resize_callback(GLFWwindow*, int, int);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double x, double y);
void scroll_callback(GLFWwindow* window, double x, double y);
void init();
void mainloop(GLFWwindow*);

bool loadTexture(std::string, GLuint&, bool gamma = false);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(win_width, win_height, "OpenGL Shader Joy", NULL, NULL); 
	
	if (window == NULL)
	{
		CLog::error("Failed to create GLFW window");
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwMakeContextCurrent(window);
	GLuint result = glewInit();
	if (result != GLEW_OK) {
		CLog::error(glewGetErrorString(result));
		return -1;
	}
	init();
	framebuffer_resize_callback(window, win_width, win_height);
	while (!glfwWindowShouldClose(window))
	{
		
		if (rendering) {
			GLfloat currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			curFrame++;
			mainloop(window);

			glfwSwapBuffers(window);
		}
		glfwPollEvents();
	}
	
	glfwTerminate();
    return 0;
}

void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
	win_width = width;
	win_height = height;
	projection = glm::ortho(0.0f, (float)win_width, (float)win_height, 0.0f, -1.0f, 10.0f);
	glViewport(0, 0, win_width, win_height);
	iResolution.x = width;
	iResolution.y = height;
}

void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS)
		return;
	switch (key)
	{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_SPACE:
			rendering = !rendering;
			break;
		default:
			break;
	}
}

void mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
}

void cursor_position_callback(GLFWwindow * window, double x, double y)
{
	iMouse.x = x;
	iMouse.y = y;
}

void scroll_callback(GLFWwindow * window, double x, double y)
{
}

void init()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	shader.load(
		(shader_path[0]).c_str(), 
		(shader_path[1]).c_str(),
		(shader_path[2]).c_str()
	);

	GLuint vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//loadTexture(img1_path, tex1);
	//loadTexture(img2_path, tex2);
	//loadTexture(img3_path, tex3);
}

void mainloop(GLFWwindow* window) 
{
	glClear(GL_COLOR_BUFFER_BIT);
	mat4 model = mat4(1.0f);
	shader.use();
	model = scale(model, vec3(win_width, win_height, 1.0f));
	shader.setUniformMatrix4fv("model", 1, value_ptr(model));
	shader.setUniformMatrix4fv("projection", 1, value_ptr(projection));
	mat4 view = lookAt(
		vec3(0.0f, 0.0f, 5.0f), 
		vec3(0.0f), 
		vec3(0.0f, 1.0f, 0.0f)
	);
	shader.setUniformMatrix4fv("view", 1, value_ptr(view));
	shader.setUniform1f("iTime", lastFrame);
	shader.setUniform1f("iTimeDelta", deltaTime); 
	shader.setUniform1i("iFrame", curFrame);
	shader.setUniform3fv("iResolution", 1, value_ptr(iResolution));
	shader.setUniform4fv("iMouse", 1, value_ptr(iMouse));

	/*glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex1);
	shader.setUniform1i("iChannel2", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tex2);
	shader.setUniform1i("iChannel3", 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, tex2);
	shader.setUniform1i("iChannel1", 3);*/
	
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	
}

bool loadTexture(std::string path, GLuint& texture, bool gamma)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;

	unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format_img;
		GLenum format_buf;
		if (nrChannels == 1) {
			format_img = GL_RED;
			format_buf = GL_RED;
		}


		else if (nrChannels == 3) {
			format_img = gamma ? GL_SRGB : GL_RGB;
			format_buf = GL_RGB;
		}


		else if (nrChannels == 4) {
			format_img = gamma ? GL_SRGB_ALPHA : GL_RGBA;
			format_buf = GL_RGBA;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, format_img, width, height, 0, format_buf, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		CLog::error("Failed to load texture: " + path);
		return false;
	}
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}


