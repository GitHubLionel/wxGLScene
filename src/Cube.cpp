#include "Cube.h"

// unit cube //////////////////////////////////////////////////////////////////
//    v6----- v5
//   /|      /|
//  v1------v0|
//  | |     | |
//  | v7----|-v4
//  |/      |/
//  v2------v3

// vertex coords array for glDrawElements()
// A cube has 6 sides and each side has 4 vertices, therefore, the total number
// of vertices is 24 (6 sides * 4 verts), and 72 floats in the vertex array
// since each vertex has 3 components (x,y,z) (= 24 * 3)
// Vertex coordinates interleaved with normal in format : GL_N3F_V3F
static GLfloat unit_cube[] = {
		 1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,
		 1.0f,  0.0f,  0.0f, -0.5f,  0.5f,  0.5f,
		 1.0f,  0.0f,  0.0f, -0.5f, -0.5f,  0.5f,
		 1.0f,  0.0f,  0.0f,  0.5f, -0.5f,  0.5f,  // v0,v1,v2,v3 (front)
		 0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,
		 0.0f,  1.0f,  0.0f,  0.5f, -0.5f,  0.5f,
		 0.0f,  1.0f,  0.0f,  0.5f, -0.5f, -0.5f,
		 0.0f,  1.0f,  0.0f,  0.5f,  0.5f, -0.5f,  // v0,v3,v4,v5 (right)
		 0.0f,  0.0f,  1.0f,  0.5f,  0.5f,  0.5f,
		 0.0f,  0.0f,  1.0f,  0.5f,  0.5f, -0.5f,
		 0.0f,  0.0f,  1.0f, -0.5f,  0.5f, -0.5f,
		 0.0f,  0.0f,  1.0f, -0.5f,  0.5f,  0.5f,  // v0,v5,v6,v1 (top)
		 0.0f, -1.0f,  0.0f, -0.5f,  0.5f,  0.5f,
		 0.0f, -1.0f,  0.0f, -0.5f,  0.5f, -0.5f,
		 0.0f, -1.0f,  0.0f, -0.5f, -0.5f, -0.5f,
		 0.0f, -1.0f,  0.0f, -0.5f, -0.5f,  0.5f,  // v1,v6,v7,v2 (left)
		 0.0f,  0.0f, -1.0f, -0.5f, -0.5f, -0.5f,
		 0.0f,  0.0f, -1.0f,  0.5f, -0.5f, -0.5f,
		 0.0f,  0.0f, -1.0f,  0.5f, -0.5f,  0.5f,
		 0.0f,  0.0f, -1.0f, -0.5f, -0.5f,  0.5f,  // v7,v4,v3,v2 (bottom)
		-1.0f,  0.0f,  0.0f,  0.5f, -0.5f, -0.5f,
		-1.0f,  0.0f,  0.0f, -0.5f, -0.5f, -0.5f,
		-1.0f,  0.0f,  0.0f, -0.5f,  0.5f, -0.5f,
		-1.0f,  0.0f,  0.0f,  0.5f,  0.5f, -0.5f   // v4,v7,v6,v5 (back)
};

// index array for glDrawElements()
// A cube has 36 indices = 6 sides * 2 tris * 3 verts
static const GLuint indices[] = {
		0, 1, 2, 			2, 3, 0,    // v0-v1-v2, v2-v3-v0 (front)
		4, 5, 6, 			6, 7, 4,    // v0-v3-v4, v4-v5-v0 (right)
		8, 9, 10, 		10, 11, 8,    // v0-v5-v6, v6-v1-v0 (top)
		12, 13, 14, 	14, 15, 12,    // v1-v6-v7, v7-v2-v1 (left)
		16, 17, 18, 	18, 19, 16,    // v7-v4-v3, v3-v2-v7 (bottom)
		20, 21, 22, 	22, 23, 20     // v4-v7-v6, v6-v5-v4 (back)
};

#ifdef USE_VBO
static GLuint vboId = 0;  // ID of VBO for vertex and normal interleaved arrays
static GLuint iboId = 0;  // ID of VBO for index array
#endif

TCube::TCube(GLfloat _size, const TVector3D &pos) :
		TObject3D(pos)
{
	ObjectType = otCube;
	size = _size;
	IsCube = true;

	int n = sizeof(unit_cube) / sizeof(GLfloat);
	cube = new GLfloat[n];
	for (int i = 0; i < n; i++)
		cube[i] = unit_cube[i];

	// Resize the cube
	for (int i = 0; i < 24; i++)
	{
		int j = 6 * i + 3;
		cube[j] *= size;
		cube[j + 1] *= size;
		cube[j + 2] *= size;
	}

#ifdef USE_VBO
	initOGL();
	vboId = createVBO(cube, sizeof(unit_cube));
	iboId = createVBO(&indices, sizeof(indices), GL_ELEMENT_ARRAY_BUFFER);
	createVBO_OK = ((vboId != 0) && (iboId != 0));
	if (!createVBO_OK)
		std::cout << "[WARNING] VBO array not created for Cube." << std::endl;

	// Now we can delete cube array
	delete[] cube;
	cube = NULL;
#endif

	material.SetColor(mfFront, msAmbient, {0.1, 0.1, 0.8, 1.0});
	material.SetColor(mfFront, msDiffuse, GLColor_blue);

	material.SetColor(mfBack, msAmbient, {0.2, 0.2, 0.2, 1.0});
	material.SetColor(mfBack, msDiffuse, {0.8, 0.8, 0.8, 1.0});

	direction = {0.0, 0.0, 1.0};

	ComputeParameters();
}

TCube::TCube(TVector3D _size, const TVector3D &pos) :
		TCube(1, pos)
{
	ObjectType = otCuboid;
	size3D = _size;
	IsCube = false;
}

TCube::~TCube()
{
#ifdef USE_VBO
	glDeleteBuffers(1, &vboId);
	vboId = 0;
	glDeleteBuffers(1, &iboId);
	iboId = 0;
#endif
	if (cube)
		delete[] cube;
}

void TCube::ComputeParameters(bool normalize)
{
	reduced_pos = direction;
	if (normalize)
		reduced_pos.Normalize();
	axe_angle = {-reduced_pos.Y, reduced_pos.X, 0.0};
	angle = RADtoDEG * acos(reduced_pos.Z / size);
}

void TCube::DoDisplay(TDisplayMode mode __attribute__((unused)))
{
	glTranslated(position.X, position.Y, position.Z);
	if (!axe_angle.IsNull())
		glRotated(angle, axe_angle.X, axe_angle.Y, axe_angle.Z);
	// Parallelepiped case, we scale the cube
	if (!IsCube)
		glScalef(size3D.X, size3D.Y, size3D.Z);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

#ifdef USE_VBO

	if (createVBO_OK)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);

		glInterleavedArrays(GL_N3F_V3F, SIZE_VERTEX(1), (GLvoid*) 0);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*) 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

#else

	glInterleavedArrays(GL_N3F_V3F, SIZE_VERTEX(1), cube);
	glDrawElements(GL_TRIANGLES, 36,  GL_UNSIGNED_INT, (void*) indices);

#endif

	glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
	glDisableClientState(GL_NORMAL_ARRAY);
}

// ********************************************************************************
// End of file
// ********************************************************************************
