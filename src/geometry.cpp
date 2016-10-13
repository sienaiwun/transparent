#include "geometry.h"

Geometry::Geometry(const char* fileName/*const GLMmodel * glmodel*/) :_mainModel(true), _baseVertexID(0)
{
	_glmodel = glmReadOBJ(fileName);
	
	


	if (!_glmodel)
	{
		printf("cpu model [%s] is bad!\n", fileName);
		exit(-1);
	}
	/*glmFacetNormals(_glmodel);
	glmVertexNormals(_glmodel, 50.0f);*/

	

	float *positions = _glmodel->vertices;
	float *normals = _glmodel->normals;
	float *texCoords = _glmodel->texcoords;

	_posSize = 3;
	_normalSize = 3;
	_tcSize = 2;
	_pOffset = 0;
	_nOffset = _pOffset + _posSize;
	_tcOffset = _nOffset + _normalSize;
	_vtxSize = _posSize + _normalSize + _tcSize;
	
	float temp[4] = { 0, 0, 0, 0 };

	_groupIndices.clear();
	GLMgroup * group = _glmodel->groups;
	GLMtriangle * triangle = _glmodel->triangles;
	std::map<Triple<GLuint>, int> vertexIdx;
	int cnt = 0;
	_indices.clear();
	int groupId = 0;
	_groupNum = 0;
	while (group){
		if( group->numtriangles > 0)
			_groupNum++;
		group = group->next;
	}

	group = _glmodel->groups;

	for (; group; group = group->next)
	{
		if (group->numtriangles <=0)
			continue;
		_groupIndices.resize(cnt+1);
		_groupMaterial.push_back(group->material);

		for (int i = 0; i < group->numtriangles; i++)
		{
			unsigned int tid = group->triangles[i];
			GLMtriangle curTriangle = triangle[tid];
			for (int j = 0; j < 3; j++)
			{
				Triple<GLuint> key(curTriangle.vindices[j], curTriangle.nindices[j], curTriangle.tindices[j]);
				if (!vertexIdx.count(key))
				{
					// make vertex;
					int index = curTriangle.vindices[j];
					if (positions != NULL)
						_vertices.insert(_vertices.end(), positions + index*_posSize, positions + index* _posSize + _posSize);
					else
						_vertices.insert(_vertices.end(), temp, temp + _posSize);
					index = curTriangle.nindices[j];
					if (normals != NULL)
						_vertices.insert(_vertices.end(), normals + index* _normalSize, normals + index * _normalSize + _normalSize);
					else
						_vertices.insert(_vertices.end(), temp, temp + _normalSize);
					index = curTriangle.tindices[j];
					if (texCoords != NULL)
						_vertices.insert(_vertices.end(), texCoords + index * _tcSize, texCoords + index * _tcSize + _tcSize);
					else
						_vertices.insert(_vertices.end(), temp, temp+ _tcSize);

					int value = (_vertices.size() / _vtxSize) - 1;
					vertexIdx[key] = value;
					_groupIndices[cnt].push_back(value);
					_indices.push_back(value);
				}
				else
				{
					int value = vertexIdx[key];
					_groupIndices[cnt].push_back(value);
					_indices.push_back(value);
				}
			}
		}

		cnt++;
		// show process status;
		groupId++;
		printf("\r                                               ");
		printf("\rconvert to GPU format: %.1f%%", (float)groupId / _groupNum * 100);
	}
	printf("\n");

	printf("mode have [%d] triangle, [%d] vertex\n", _indices.size() / 3, _vertices.size() / _vtxSize);


	// copy materials;
	for (int i = 0; i < _glmodel->nummaterials; i++)
		_materials.push_back(_glmodel->materials[i]);


	// compute model AABB on cpu;
	glmBoundingBox(_glmodel, &_modelAABBmin.x, &_modelAABBmax.x);
	nv::vec3f middle = (_modelAABBmax + _modelAABBmin) *0.5f;
	_modelCenter = glm::vec3(middle.x, middle.y, middle.z);


	// delte _glmodel;
	glmDelete(_glmodel);
}

const float* Geometry::getCompiledVertices() const
{
	return &_vertices[0];
}

const GLuint* Geometry::getCompiledGroupIndices(int gID) const
{
	return &_groupIndices[gID][0];
}

int Geometry::getCompiledPositionOffset() const
{
	return _pOffset;
}

int Geometry::getCompiledNormalOffset() const
{
	return _nOffset;
}

int Geometry::getCompiledTexCoordOffset() const
{
	return _tcOffset;
}

int Geometry::getCompiledVertexSize() const
{
	return _vtxSize;
}

int Geometry::getCompiledVertexCount() const
{
	return _vertices.size() / _vtxSize;
}

int Geometry::getCompiledGroupIndexCount(int gID) const
{
	return _groupIndices[gID].size();
}

const GLuint* Geometry::getCompiledIndices() const
{
	return &_indices[0];
}

int Geometry::getCompiledIndexCount() const
{
	return _indices.size();
}

#define CHECK_ERRORS()         \
	do {                         \
	GLenum err = glGetError(); \
	if (err) {                                                       \
	printf( "GL Error %d at line %d of FILE %s\n", (int)err, __LINE__,__FILE__);       \
	exit(-1);                                                      \
			}                                                                \
			} while(0)
void Geometry::render(glslShader & shader,textureManager& manager)
{
	glBindBuffer(GL_ARRAY_BUFFER, getVBO());

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	CHECK_ERRORS();
	glVertexAttribPointer(0,
		getPositionSize(),
		GL_FLOAT,
		GL_FALSE,
		getCompiledVertexSize()*sizeof(float),
		(void*)(getCompiledPositionOffset()*sizeof(float)));

	glVertexAttribPointer(1,
		getNormalSize(),
		GL_FLOAT,
		GL_TRUE,
		getCompiledVertexSize()*sizeof(float),
		(void*)(getCompiledNormalOffset()*sizeof(float)));

	glVertexAttribPointer(2,
		getTexCoordSize(),
		GL_FLOAT,
		GL_FALSE,
		getCompiledVertexSize()*sizeof(float),
		(void*)(getCompiledTexCoordOffset()*sizeof(float)));

	CHECK_ERRORS();


	for (int gID = 0; gID < getGroupCount(); gID++)
	{	
		if (_materials.size()>0)
		shader.setMaterial(getGroupMaterial(gID), manager);	
		CHECK_ERRORS();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, getGroupIBO(gID));
		glDrawElements(GL_TRIANGLES, getCompiledGroupIndexCount(gID), GL_UNSIGNED_INT, 0);
		CHECK_ERRORS();
	}

	CHECK_ERRORS();
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	CHECK_ERRORS();
	
}
int Geometry::Create_GPU_Buffer()
{
	if (!_mainModel)
	{
		printf("can not create all gpu buffer for model except main model\n");
		return 1;
	}
	printf("transfer to gpu memory...\n");
	_groupIBO.resize(_groupIndices.size());

	glGenBuffers(1, &_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER,
		_vertices.size()*sizeof(float),
		&_vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		_indices.size()*sizeof(GLuint),
		&_indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	for (int i = 0; i < _groupIndices.size(); i++)
	{
		glGenBuffers(1, &_groupIBO[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _groupIBO[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			_groupIndices[i].size()*sizeof(GLuint),
			&_groupIndices[i][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	return 0;
}

int Geometry::CreateDynamicGPU_Buffer()
{
	glGenBuffers(1, &_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER,
		_vertices.size()*sizeof(float),
		&_vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return 0;
}

GLuint Geometry::getVBO() const
{
	return _VBO;
}

GLuint Geometry::getIBO() const
{
	return _IBO;

}

GLuint Geometry::getGroupIBO(int gID) const
{
	return _groupIBO[gID];
}

int Geometry::getPositionSize() const
{
	return 3;
}

int Geometry::getNormalSize() const
{
	return 3;
}

int Geometry::getTexCoordSize() const
{
	return 2;
}

const GLMmaterial& Geometry::getGroupMaterial(int gID) const
{
	int id = _groupMaterial[gID];
	const GLMmaterial& meterial = (_materials[id]);
	printf("%s\n", meterial.diffuse_map);
	return meterial;
}

int Geometry::getGroupCount() const
{
	return _groupIndices.size();
}

void Geometry::adjustIndicesAndGroupMaterial(int baseVerticesID, int baseGroupMaterialID)
{
	// this means not main model;
	_mainModel = false;
	_baseVertexID = baseVerticesID;

	for (int gid = 0; gid < _groupIndices.size(); gid++)
		for (int i = 0; i < _groupIndices[gid].size(); i++)
			_groupIndices[gid][i] += baseVerticesID;

	for (int i = 0; i < _indices.size(); i++)
		_indices[i] += baseVerticesID;

	for (int i = 0; i < _groupMaterial.size(); i++)
		_groupMaterial[i] += baseGroupMaterialID;

}
void Geometry::drawQuad(glslShader& shader, bool addition, nv::vec2f beginPoint, nv::vec2f endPoint)
{
	if (addition)
		glDisable(GL_DEPTH_TEST);
	beginPoint = beginPoint * 2 - nv::vec2f(1, 1);
	endPoint = endPoint * 2 - nv::vec2f(1, 1);
	shader.begin();
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glVertex2fv(beginPoint);
	glVertex2f(endPoint.x, beginPoint.y);
	glVertex2fv(endPoint);
	glVertex2f(beginPoint.x, endPoint.y);
	glEnd();
	shader.end();
	if (addition)
		glEnable(GL_DEPTH_TEST);
}
void Geometry::drawQuad(glslShader& shader)
{
	shader.begin();
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(1.0f, -1.0f);
	glVertex2f(1.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glEnd();
	shader.end();
}
void Geometry::drawTriangle(nv::vec3f newOrigin, glslShader& shader)
{
	CHECK_ERRORS();
	nv::vec3f point1 = nv::vec3f(-4.3123, -10.4843, -51.6457);
	nv::vec3f point2 = nv::vec3f(-4.3123, -18.6958, -51.6457);

	const float farDis = 150;

	nv::vec3f vec1 = point1 + farDis* normalize(point1 - newOrigin);
	nv::vec3f vec2 = point2 + farDis* normalize(point2 - newOrigin);
	CHECK_ERRORS();
	shader.begin();
	CHECK_ERRORS();
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3fv(vec1);
	glVertex3fv(vec2);
	glVertex3fv(point1);
	glVertex3fv(point2);
	glEnd();
	CHECK_ERRORS();
	shader.end();
	CHECK_ERRORS();

}
void Geometry::appendDynamicObject(Geometry& dynamicObject)
{
	if (_pOffset != dynamicObject._pOffset ||
		_nOffset != dynamicObject._nOffset ||
		_tcOffset != dynamicObject._tcOffset ||
		_vtxSize != dynamicObject._vtxSize ||
		_posSize != dynamicObject._posSize ||
		_normalSize != dynamicObject._normalSize ||
		_tcSize != dynamicObject._tcSize)
	{
		printf("can not append dynamicObject, format not match!!\n");
		return;
	}
	_groupIndices.insert(_groupIndices.end(), dynamicObject._groupIndices.begin(), dynamicObject._groupIndices.end());
	_indices.insert(_indices.end(), dynamicObject._indices.begin(), dynamicObject._indices.end());
	
	_vertices.insert(_vertices.end(), dynamicObject._vertices.begin(), dynamicObject._vertices.end());
	_materials.insert(_materials.end(), dynamicObject._materials.begin(), dynamicObject._materials.end());

	_groupMaterial.insert(_groupMaterial.end(), dynamicObject._groupMaterial.begin(), dynamicObject._groupMaterial.end());

	_groupNum += dynamicObject._groupNum;

}





