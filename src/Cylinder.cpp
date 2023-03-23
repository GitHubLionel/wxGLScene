#include "Cylinder.h"

// Code for sphere construction inspired from :
// AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// http://www.songho.ca/opengl/gl_cylinder.html

#ifdef USE_VBO

enum
{
	cylinder_all = 0,
	base_up_id,
	base_down_id,
	ARRAY_COUNT
};

#endif

TCylinder::TCylinder(GLfloat _len, GLfloat _radius, const TVector3D &pos) : TBaseCylinder(_len, _radius, pos)
{
	ObjectType = otCylinder;

	CylinderDetail = 20;

	// + 1 pour fermer le cyclindre
	cylinderLength = 2 * (CylinderDetail + 1);

	// Base
	baseLength = CylinderDetail + 1;

	InitializeArray();

	material.SetColor(mfFront, msAmbient, GLColor_red);
	material.SetColor(mfFront, msDiffuse, GLColor_red);
}

TCylinder::~TCylinder()
{
	FreeArray();
#ifdef USE_VBO
	glDeleteBuffers(ARRAY_COUNT, arrowObjects);
#endif
}

void TCylinder::InitializeArray(void)
{
	cylinder = new TVertex[cylinderLength];
	base_up = new TVertex[baseLength];
	base_down = new TVertex[baseLength];

	CreateCylinder();

#ifdef USE_VBO
	initOGL();
	arrowObjects[cylinder_all] = createVBO(cylinder, SIZE_VERTEX(cylinderLength));
	arrowObjects[base_up_id] = createVBO(base_up, SIZE_VERTEX(baseLength));
	arrowObjects[base_down_id] = createVBO(base_down, SIZE_VERTEX(baseLength));

	for (int i = 0; i < ARRAY_COUNT; i++)
		createVBO_OK &= (arrowObjects[i] != 0);
	if (!createVBO_OK)
		std::cout << "[WARNING] VBO array not created for Cylinder." << std::endl;

	// Now we can delete free array
	FreeArray();
#endif
}

void TCylinder::FreeArray(void)
{
	if (cylinder)
		delete[] cylinder;
	cylinder = NULL;
	if (base_up)
		delete[] base_up;
	base_up = NULL;
	if (base_down)
		delete[] base_down;
	base_down = NULL;
}

void TCylinder::CreateCylinder(void)
{
	// Body + base
	double dtheta = (2.0 * PI / CylinderDetail);
	double theta = 0.0;

	for (GLuint i = 0; i < baseLength; ++i)
	{
		const double cos_theta = cos(theta);
		const double sin_theta = sin(theta);
		const float x = (float) (radius * cos_theta);
		const float y = (float) (radius * sin_theta);

		const GLuint j = 2 * i;
		const GLuint k = j + 1;

		cylinder[j].SetVertice(x, y, len2);
		cylinder[k].SetVertice(x, y, -len2);

		cylinder[j].normal.X = cylinder[k].normal.X = (float) cos_theta;
		cylinder[j].normal.Y = cylinder[k].normal.Y = (float) sin_theta;
		cylinder[j].normal.Z = cylinder[k].normal.Z = 0.0f;

		base_up[i].SetVertice(x, y, len2);
		base_up[i].SetNormal(0.0f, 0.0f, 1.0f);

		base_down[baseLength - 1 - i].SetVertice(x, y, -len2);
		base_down[baseLength - 1 - i].SetNormal(0.0f, 0.0f, -1.0f);

		theta += dtheta;
	}
}

void TCylinder::DoDisplay(TDisplayMode mode __attribute__((unused)))
{
	glTranslated(position.X, position.Y, position.Z);
	glRotated(angle, axe_angle.X, axe_angle.Y, axe_angle.Z);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

#ifdef USE_VBO

	if (createVBO_OK)
	{
		// cylindre
		glBindBuffer(GL_ARRAY_BUFFER, arrowObjects[cylinder_all]);
		glInterleavedArrays(GL_N3F_V3F, SIZE_VERTEX(1), (GLvoid*) 0);
		glDrawArrays(GL_QUAD_STRIP, 0, cylinderLength);

		// base up cyclindre
		glBindBuffer(GL_ARRAY_BUFFER, arrowObjects[base_up_id]);
		glInterleavedArrays(GL_N3F_V3F, SIZE_VERTEX(1), (GLvoid*) 0);
		glDrawArrays(GL_POLYGON, 0, baseLength);

		// base down cyclindre
		glBindBuffer(GL_ARRAY_BUFFER, arrowObjects[base_down_id]);
		glInterleavedArrays(GL_N3F_V3F, SIZE_VERTEX(1), (GLvoid*) 0);
		glDrawArrays(GL_POLYGON, 0, baseLength);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

#else

	// cylindre
	glInterleavedArrays(GL_N3F_V3F, SIZE_VERTEX(1), &cylinder[0]);
	glDrawArrays(GL_QUAD_STRIP, 0, cylinderLength);

	// Base up
	glInterleavedArrays(GL_N3F_V3F, SIZE_VERTEX(1), &base_up[0]);
	glDrawArrays(GL_POLYGON, 0, baseLength);

	// Base down
	glInterleavedArrays(GL_N3F_V3F, SIZE_VERTEX(1), &base_down[0]);
	glDrawArrays(GL_POLYGON, 0, baseLength);

#endif

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

// ********************************************************************************
// End of file
// ********************************************************************************
