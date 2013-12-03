
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cmath>
#include <Windows.h>

#include "HDRImage.h"
#include "Shader.h"
#include "Texture2D.h"
#include "Framebuffer2D.h"

bool initGL(int width, int height);
void destroyGL();

void initGeom(GLuint &vao, GLuint &vbo);
void destroyGeom(GLuint &vao, GLuint &vbo);

void createFramebuffer(GLuint &fbo);
void getMinMax1f(float &min, float &max, const Texture2D &tex, GLint level);
void getMinMax2f(float &min, float &max, const Texture2D &tex, GLint level);

void scrollFun(GLFWwindow * win, double sx, double sy);
void keyFun(GLFWwindow * win, int key, int scancode, int action, int mods);

void setWindowTitle();

GLFWwindow *g_window;
float		g_value;
bool		g_separable;
int			g_method;

int main(int argc, char **argv)
{
	g_value = 0.5f;
	g_separable = false;
	g_method = 0;

	GLuint vao;
	GLuint vbo;
	int width, height;

	if(argc < 2)
	{
		printf("No argument... \n");
		return -1;
	}

	const char * file = argv[1];
	//const char * file = "samples/memorial.hdr";
	//const char * file = "samples/AtriumNight.hdr";

	HDRImage img(file);

	if(!img.getSize())
		return -1;

	if(!initGL(img.getWidth(), img.getHeight()))
		return -1;

	width = img.getWidth();
	height = img.getHeight();

	// CONSTANTS TO TINKER WITH
	const float sigmaI = 0.2f;							// Intensity sigma (Called Range in article), recommended value 0.4
	const float sigmaS = (float)width * 0.02f;			// Spacial sigma, should be 2% of the width of the image
	const int	dsf = 1;								// DownSampleFactor
	const int	kernelRadius = (int)(sigmaS * 2.0f);	// Radius of the bilateral kernel, 2 * sigmaS
	const float targetContrast = 5.0f;

	Texture2D texture(width, height, img.getData(), GL_RGB32F, GL_RGB, GL_FLOAT);

	Texture2D texRGB(width, height, NULL, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);		// RGB / intensity
	Texture2D texLogI(width, height, NULL, GL_R32F, GL_RED, GL_FLOAT);				// log10(intensity)
	Texture2D texLogBase(width/dsf, height/dsf, NULL, GL_R32F, GL_RED, GL_FLOAT);	// log10(base)
	Texture2D texTemp(width/dsf, height/dsf, NULL, GL_R32F, GL_RED, GL_FLOAT);		// middle storage for separable
	Texture2D texMinMax(texLogBase.getWidth()/4, texLogBase.getHeight()/4,
		NULL, GL_RG32F, GL_RG, GL_FLOAT);											// Reduction minmax

	printf("nice width %i height %i \n", width, height);

	Framebuffer2D fboRGBI;
	fboRGBI.attachTexture(GL_COLOR_ATTACHMENT0, texRGB);
	fboRGBI.attachTexture(GL_COLOR_ATTACHMENT1, texLogI);

	Framebuffer2D fboTemp;
	fboTemp.attachTexture(GL_COLOR_ATTACHMENT0, texTemp);

	Framebuffer2D fboBase;
	fboBase.attachTexture(GL_COLOR_ATTACHMENT0, texLogBase);

	Framebuffer2D fboMinMax;
	fboMinMax.attachTexture(GL_COLOR_ATTACHMENT0, texMinMax, 0);

	Shader shadBasic("src/shaders/basic_vert.glsl", "src/shaders/basic_frag.glsl");
	Shader shadIntensity("src/shaders/basic_vert_flip_t.glsl", "src/shaders/intensity_frag.glsl");
	Shader shadBilateral("src/shaders/basic_vert.glsl", "src/shaders/bilateral_frag.glsl");
	Shader shadBilateralX("src/shaders/basic_vert.glsl", "src/shaders/bilateral_x_frag.glsl");
	Shader shadBilateralY("src/shaders/basic_vert.glsl", "src/shaders/bilateral_y_frag.glsl");
	Shader shadDownsampleFirst("src/shaders/basic_vert.glsl", "src/shaders/down_first_frag.glsl");
	Shader shadDownsample("src/shaders/basic_vert.glsl", "src/shaders/down_frag.glsl");
	Shader shadCompose("src/shaders/basic_vert.glsl", "src/shaders/compose_frag.glsl");
	Shader shadComposeReinhard("src/shaders/basic_vert.glsl", "src/shaders/compose_reinhard_frag.glsl");
	Shader shadComposeDurandDorsey("src/shaders/basic_vert.glsl", "src/shaders/compose_durand_dorsey_frag.glsl");
	Shader shadComposeUncharted2("src/shaders/basic_vert.glsl", "src/shaders/compose_uncharted2_frag.glsl");

	initGeom(vao, vbo);

	glDisable(GL_DEPTH_TEST);

	GLenum drawBuffer1[1] = {GL_COLOR_ATTACHMENT0};
	GLenum drawBuffer2[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};

    while (!glfwWindowShouldClose(g_window))
    {
    	setWindowTitle();
		glViewport(0, 0, width, height);

		// Calculate logIntensity and RGB / Intensity
		texture.bind(GL_TEXTURE0);
		shadIntensity.bind();
		fboRGBI.bind();
		glDrawBuffers(2, drawBuffer2);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);	// Draw fullscreen-Quad

		// Calculate logBase from Bilateral(Intensity)
		if(g_separable)
		{
			// X-pass
			texLogI.bind(GL_TEXTURE0);
			shadBilateralX.bind();
			glUniform2f(shadBilateralX.getUniformLocation("invRes"), 1.0f/texLogI.getWidth(), 1.0f/texLogI.getHeight());
			glUniform1f(shadBilateralX.getUniformLocation("sigmaI"), sigmaI);
			glUniform1f(shadBilateralX.getUniformLocation("sigmaS"), sigmaS);
			glUniform1i(shadBilateralX.getUniformLocation("kernelRadius"), kernelRadius);
			fboTemp.bind();
			glDrawBuffers(1, drawBuffer1);
			glViewport(0, 0, texTemp.getWidth(), texTemp.getHeight());

			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, 6);	// Draw fullscreen-Quad

			// Y-pass
			texTemp.bind(GL_TEXTURE0);
			shadBilateralY.bind();
			glUniform2f(shadBilateralY.getUniformLocation("invRes"), 1.0f/texTemp.getWidth(), 1.0f/texTemp.getHeight());
			glUniform1f(shadBilateralY.getUniformLocation("sigmaI"), sigmaI);
			glUniform1f(shadBilateralY.getUniformLocation("sigmaS"), sigmaS);
			glUniform1i(shadBilateralY.getUniformLocation("kernelRadius"), kernelRadius);
			fboBase.bind();
			glDrawBuffers(1, drawBuffer1);
			glViewport(0, 0, texLogBase.getWidth(), texLogBase.getHeight());

			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, 6);	// Draw fullscreen-Quad
		}
		else
		{
			texLogI.bind(GL_TEXTURE0);
			shadBilateral.bind();
			glUniform2f(shadBilateral.getUniformLocation("invRes"), 1.0f/texLogI.getWidth(), 1.0f/texLogI.getHeight());
			glUniform1f(shadBilateral.getUniformLocation("sigmaI"), sigmaI);
			glUniform1f(shadBilateral.getUniformLocation("sigmaS"), sigmaS);
			glUniform1i(shadBilateral.getUniformLocation("kernelRadius"), kernelRadius);
			fboBase.bind();
			glDrawBuffers(1, drawBuffer1);
			glViewport(0, 0, texLogBase.getWidth(), texLogBase.getHeight());

			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, 6);	// Draw fullscreen-Quad

		}

		// Reduction to find min/max of logBase
		texLogBase.bind(GL_TEXTURE0);
		shadDownsampleFirst.bind();
		fboMinMax.bind();
		glDrawBuffers(1, drawBuffer1);
		glViewport(0, 0, texMinMax.getWidth(), texMinMax.getHeight());

		glUniform1i(shadDownsampleFirst.getUniformLocation("width"), texLogBase.getWidth());
		glUniform1i(shadDownsampleFirst.getUniformLocation("height"), texLogBase.getHeight());

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);	// Draw fullscreen-Quad

		glFlush();

		float min, max;
		getMinMax2f(min, max, texMinMax, 0);

		Framebuffer2D::bindDefault();
		glViewport(0, 0, width, height);

		glClear(GL_COLOR_BUFFER_BIT);


		Shader * composeShad = &shadComposeDurandDorsey;
		if(g_method == 1)
			composeShad = &shadComposeReinhard;
		else if(g_method == 2)
			composeShad = &shadComposeUncharted2;
		else if(g_method == 3)
			composeShad = &shadCompose;

		// Calculate and render final value to screen
		texRGB.bind(GL_TEXTURE0);
		texLogI.bind(GL_TEXTURE1);
		texLogBase.bind(GL_TEXTURE2);
		composeShad->bind();

		glUniform1f(composeShad->getUniformLocation("minLogBase"), min);
		glUniform1f(composeShad->getUniformLocation("maxLogBase"), 0);
		glUniform1f(composeShad->getUniformLocation("targetContrast"), targetContrast);
		glUniform1f(composeShad->getUniformLocation("value"), g_value);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Swap and poll
        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }

	destroyGeom(vao, vbo);

    destroyGL();
    return 0;
}

bool initGL(int width, int height)
{
    /* Initialize the library */
    if (!glfwInit())
        return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    /* Create a windowed mode window and its OpenGL context */
    g_window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!g_window)
    {
		printf("glfw error: window could not be created \n");
        glfwTerminate();
        return false;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(g_window);
	glfwSetScrollCallback(g_window, scrollFun);
	glfwSetKeyCallback(g_window, keyFun);

    glewExperimental = GL_TRUE;
	GLenum error = glewInit();
	if(GLEW_OK != error)
	{
		printf("glew error: init returned an error \n");
		glfwTerminate();
		return false;
	}

	return true;
}

void destroyGL()
{
	glfwTerminate();
}

void initGeom(GLuint &vao, GLuint &vbo)
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	static const GLfloat vbo_data[] = {
	    -1.0f, -1.0f, 0.0f,
	    1.0f, -1.0f, 0.0f,
	    -1.0f,  1.0f, 0.0f,
	    -1.0f,  1.0f, 0.0f,
	    1.0f, -1.0f, 0.0f,
	    1.0f,  1.0f, 0.0f,
	};

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vbo_data), vbo_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	
	glBindVertexArray(0);
}

void destroyGeom(GLuint &vao, GLuint &vbo)
{
	if(glIsBuffer(vbo))
		glDeleteBuffers(1, &vbo);
}


// Find min max in a texture
void getMinMax1f(float &min, float &max, const Texture2D &tex, GLint level)
{
	min = 1.0e20f;
	max = -1.0e20f;
	GLint width=0, height=0;

	tex.bind();

	glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_HEIGHT, &height);

	if(width > 0 && height > 0)
	{
		float * data = new float[width * height];
		glGetTexImage(GL_TEXTURE_2D, level, GL_RED, GL_FLOAT, data);

		glFlush();

		for(int y=0; y<height; ++y)
		{
			for(int x=0; x<width; ++x)
			{
				int i = (y*width + x);
				float value = data[i];

				min = value < min ? value : min;
				max = value > max ? value : max;
			}
		}
		delete[] data;
	}
	else
	{
		min = 0.0f;
		max = 1.0f;
	}
}

// Find min max in a texture
void getMinMax2f(float &min, float &max, const Texture2D &tex, GLint level)
{
	min = 1.0e20f;
	max = -1.0e20f;
	GLint width=0, height=0;

	tex.bind();

	glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_HEIGHT, &height);

	if(width > 0 && height > 0)
	{
		float * data = new float[width * height * 2];
		glGetTexImage(GL_TEXTURE_2D, level, GL_RG, GL_FLOAT, data);

		glFlush();

		for(int y=0; y<height; ++y)
		{
			for(int x=0; x<width; ++x)
			{
				int i = (y*width + x)*2;
				float readMin = data[i];
				float readMax = data[i+1];

				min = readMin < min ? readMin : min;
				max = readMax > max ? readMax : max;
			}
		}
		delete[] data;
	}
	else
	{
		min = 0.0f;
		max = 1.0f;
	}
}

float clamp(float val, float min, float max)
{
	val = val < min ? min : val;
	val = val > max ? max : val;
	return val;
}

void scrollFun(GLFWwindow * win, double sx, double sy)
{
	g_value += (float)sy * 0.05f;
	g_value = clamp(g_value, 0.0f, 1.0f);
}

void keyFun(GLFWwindow * win, int key, int scancode, int action, int mods)
{
	if(action == GLFW_RELEASE)
	{
		if(key == GLFW_KEY_1)
			g_method = 0;
		else if(key == GLFW_KEY_2)
			g_method = 1;
		else if(key == GLFW_KEY_3)
			g_method = 2;
		else if(key == GLFW_KEY_4)
			g_method = 3;
		else if(key == GLFW_KEY_S)
			g_separable = !g_separable;
	}
}

void setWindowTitle()
{
	char title[256];
	char * method[] =
	{
		"Durand Dorsey, Local",
		"Reinhard, Global",
		"Fimlic Games, Uncharted 2, Global",
		"Fimlic Games, Uncharted 2, Local"
	};

	sprintf(title, "Tonemap: %s, Detail: %.2f, Separable: %i", method[g_method], g_value, (int)g_separable);
	glfwSetWindowTitle(g_window, title);
}