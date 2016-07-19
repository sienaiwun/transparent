#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include "Camera.h"
#include "Fbo.h"
#include "macro.h"
#include "gbuffershader.h"
#include "teapot.h"
#include "time.h"
#include "transparentRender.h"
Timer g_time;
Camera g_Camera;
GbufferShader g_bufferShader;
Scene* g_scene;
OITrender * pOit;
textureManager texManager("");
bool drawFps = true;



void drawTex(GLuint mapId)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mapId);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();


	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(-1.0, -1.0);
	glTexCoord2f(1, 0);
	glVertex2f(1.0, -1.0);
	glTexCoord2f(1, 1);
	glVertex2f(1.0, 1.0);
	glTexCoord2f(0, 1);
	glVertex2f(-1.0, 1.0);
	glEnd();
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, 0);

}

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
	freopen("stdout.txt", "w", stdout);
	freopen("stderr.txt", "w", stderr);
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

	pOit = new OITrender(SCREEN_WIDTH, SCREEN_HEIGHT, K);
	pOit->setScene(g_scene);

}




void Display()
{
	CHECK_ERRORS();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	g_Camera.cameraControl();
	//g_scene->render(g_bufferShader, texManager, &g_Camera);            // diffuse rendering
	
	pOit->render(&g_Camera, texManager);
	drawTex(pOit->getRenderImage());
	
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
	freopen("stdout.txt","w", stdout);
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

