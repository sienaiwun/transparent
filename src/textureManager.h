
#pragma once

#include <gl/glew.h>
#include <vector>
#include <map>
#include <string>

using namespace std;
class textureManager{
public:
	textureManager(char * texDir){ m_textDir = string(texDir); };
	~textureManager();
	int getTexId(const char * texPath);
private:
	map<string, GLuint> m_nameToTexId;
	string m_textDir;

};