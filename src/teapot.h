#include "scene.h"
#include "Camera.h"
#ifndef TEAPOT_H
#define TEAPOT_H

class teapotScene :public Scene
{
public:
	teapotScene()
	{
		m_fileName.resize(1);
		m_fileName[0] = "./model/leave.obj";
		extern Camera g_Camera;
		g_Camera.loadToFIle("./metaData/flower.txt");
	};
};
#endif