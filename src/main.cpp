#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include "Camera.h"
#include "Fbo.h"
#include "macro.h"
#include "gbuffershader.h"
#include "teapot.h"
Camera g_Camera;
GbufferShader g_bufferShader;
Scene* g_scene;
textureManager texManager(".//model//");
bool drawFps = true;

void key(unsigned char k, int x, int y)
{
	k = (unsigned char)tolower(k);
	switch (k)
	{
	case 'p':
		g_Camera.printToFile("./metaData/flower.txt");
		break;
	}
	glutPostRedisplay();
}

void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 0.01, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void Init()
{
	glewInit();
	if (!glewIsSupported(
		"GL_VERSION_2_0 "
		"GL_ARB_vertex_program "
		"GL_ARB_fragment_program "
		"GL_ARB_texture_float "
		"GL_NV_gpu_program4 " // include GL_NV_geometry_program4
		"GL_ARB_texture_rectangle "
		))
	{
		printf("Unable to load extension()s:\n  GL_ARB_vertex_program\n  GL_ARB_fragment_program\n"
			"  GL_ARB_texture_float\n  GL_NV_gpu_program4\n  GL_ARB_texture_rectangle\n  OpenGL Version 2.0\nExiting...\n");
		exit(-1);
	}
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)1.0, 0.01, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	wglSwapIntervalEXT(0);
	g_scene = new teapotScene();
	g_scene->init();
	g_bufferShader.init();


}



#include "time.h"
Timer g_time;
void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	g_Camera.cameraControl();
	g_scene->render(g_bufferShader, texManager, &g_Camera);
	CHECK_ERRORS();

	if (drawFps ) {
		static char fps_text[32];
		float fps = g_time.getFps();
		sprintf(fps_text, "fps: %6.1f", fps);
		drawText(fps_text, 10.0f, 10.0f, GLUT_BITMAP_8_BY_13);
	}
	glutSwapBuffers();
	glutReportErrors();
}
void idle()
{
	g_Camera.Update();
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("¿ò¼Ü");
	Init();
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(key);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}

