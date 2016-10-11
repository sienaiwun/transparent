#pragma once
#include "textureManager.h"
#include "glm.h"
#include <map>
#include <vector>
#include <GL/glew.h>
#include <glm.hpp>
#include <algorithm>
#include <nvMath.h>
#include "glslShader.h"
#include "textureManager.h"
template <class TYPE_NAME>
class Triple
{
public:
	TYPE_NAME x, y, z;

public:
	Triple(TYPE_NAME _x, TYPE_NAME _y, TYPE_NAME _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	TYPE_NAME& operator[](int id)
	{
		if (id == 0) return x;
		if (id == 1) return y;
		return z;
	}


	bool operator==(const Triple &p) const
	{
		return x == p.x&&y == p.y && z == p.z;
	}
	bool operator<(const Triple &p) const
	{
		if (x != p.x) return x < p.x;
		if (y != p.y) return y < p.y;
		return z < p.z;
	}


}; 

class Geometry{
public:
	Geometry(const char* fileName);
	int getPositionSize() const;
	int getNormalSize() const;
	int getTexCoordSize() const;
	static void drawQuad(glslShader & shader);
	void render(glslShader & shader, textureManager & manager);
	//
	//compiled data access functions
	//
	const float* getCompiledVertices() const;
	const GLuint* getCompiledGroupIndices(int gID) const;
	const GLuint* getCompiledIndices() const;

	int getCompiledPositionOffset() const;
	int getCompiledNormalOffset() const;
	int getCompiledTexCoordOffset() const;

	// returns the size of the merged vertex in # of floats
	int getCompiledVertexSize() const;

	int getCompiledVertexCount() const;
	int getCompiledGroupIndexCount(int gID) const;
	int getCompiledIndexCount() const;
	int getMaterialsCount() const{ return _materials.size(); }

	GLuint getVBO() const;
	GLuint getIBO() const;
	GLuint getGroupIBO(int gID) const;
	const GLMmaterial & getGroupMaterial(int gID) const;
	int getGroupCount() const;

	void getVertexStartEnd(int & start, int & end)
	{
		start = _baseVertexID;
		end = _baseVertexID + _vertices.size() / _vtxSize;
	}

	void getModelAABB(nv::vec3f& Mmin, nv::vec3f& Mmax)
	{
		Mmin.x = std::min(Mmin.x, _modelAABBmin.x);
		Mmin.y = std::min(Mmin.y, _modelAABBmin.y);
		Mmin.z = std::min(Mmin.z, _modelAABBmin.z);

		Mmax.x = std::max(Mmax.x, _modelAABBmax.x);
		Mmax.y = std::max(Mmax.y, _modelAABBmax.y);
		Mmax.z = std::max(Mmax.z, _modelAABBmax.z);
	}

	inline const glm::vec3 & getModelCenter(){ return _modelCenter; };

	int Create_GPU_Buffer();

	int CreateDynamicGPU_Buffer();

	void adjustIndicesAndGroupMaterial(int baseVerticesID, int baseGroupMaterialID);

	void appendDynamicObject(Geometry& dynamicObject);

protected:
	//Would all this be better done as a channel abstraction to handle more arbitrary data?


	//data structures optimized for rendering, compiled model
	std::vector<std::vector<GLuint> > _groupIndices;
	std::vector<GLuint> _indices;
	
	
	std::vector<float> _vertices;
	std::vector<GLMmaterial> _materials;

	std::vector<GLuint> _groupIBO;
	std::vector<int> _groupMaterial;
	GLuint _VBO;
	GLuint _IBO;

	int _pOffset;
	int _nOffset;
	int _tcOffset;
	int _vtxSize;

	int _posSize;
	int _normalSize;
	int _tcSize;

	int _groupNum;

	bool _mainModel;

	int _baseVertexID;

	nv::vec3f _modelAABBmin, _modelAABBmax;
	glm::vec3 _modelCenter;


private:
	GLMmodel * _glmodel;
};