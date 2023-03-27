#include "Cone.h"

// Code for sphere construction inspired from :
// AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// http://www.songho.ca/opengl/gl_cylinder.html

TCone::TCone(GLfloat _len, GLfloat _radius, const TVector3D &pos) :
		TBaseCylinder(_len, _radius, pos)
{
	ObjectType = otCone;

	ConeDetail = 20;

	// + 2 : +1 pour le point de départ, +1 pour fermer le cone
	ConeSize = ConeDetail + 2;
	coneLength = (ConeDetail + 2) + (ConeDetail + 1);

	InitializeArray();

	material.SetColor(mfFront, msAmbient, GLColor_red);
	material.SetColor(mfFront, msDiffuse, GLColor_red);
}

TCone::~TCone()
{
	FreeArray();
#ifdef USE_VBO
	glDeleteBuffers(1, &vboId);
#endif
}

void TCone::InitializeArray(void)
{
	FreeArray();
	cone = new TVertex[coneLength];

	CreateCone();

#ifdef USE_VBO
	initOGL();
	vboId = createVBO(cone, SIZE_VERTEX(coneLength));

	createVBO_OK = (vboId != 0);
	if (!createVBO_OK)
		std::cout << "[WARNING] VBO array not created for Cone." << std::endl;

	// Now we can delete free array
	FreeArray();
#endif
}

void TCone::FreeArray(void)
{
	DeleteAndNull(cone);
}

void TCone::CreateCone(void)
{
	double phi = atan2((double) radius, (double) len2);
	double cos_phi = cos(phi);
	double sin_phi = sin(phi);
	double dtheta = (2.0 * PI / ConeDetail);
	double theta = 0.0;

	// Cone
	cone[0].SetVertice(0.0f, 0.0f, len);
	cone[0].SetNormal(0.0f, 0.0f, 1.0f);

	for (GLuint i = 1; i < ConeSize; ++i)
	{
		const double cos_theta = cos(theta);
		const double sin_theta = sin(theta);
		const float x = (float) (radius * cos_theta);
		const float y = (float) (radius * sin_theta);

		cone[i].SetVertice(x, y, 0.0f);

		cone[i].normal.X = (float) (cos_phi * cos_theta);
		cone[i].normal.Y = (float) (cos_phi * sin_theta);
		cone[i].normal.Z = sin_phi;

		cone[coneLength - i].SetVertice(x, y, 0.0f);
		cone[coneLength - i].SetNormal(0.0f, 0.0f, -1.0f);

		theta += dtheta;
	}
}

void TCone::DoDisplay(TDisplayMode mode __attribute__((unused)))
{
	glTranslated(position.X, position.Y, position.Z);
	glRotated(angle, axe_angle.X, axe_angle.Y, axe_angle.Z);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

#ifdef USE_VBO

	if (createVBO_OK)
	{
		// cone
		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glInterleavedArrays(GL_N3F_V3F, SIZE_VERTEX(1), (GLvoid*) 0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, ConeSize);
		glDrawArrays(GL_POLYGON, ConeSize, ConeSize - 1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

#else

	// cone
	glInterleavedArrays(GL_N3F_V3F, SIZE_VERTEX(1), &cone[0]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, ConeSize);
	glDrawArrays(GL_POLYGON, ConeSize, ConeSize-1);

#endif

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

// ********************************************************************************
// End of file
// ********************************************************************************
