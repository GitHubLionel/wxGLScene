#include "Spin.h"

#ifdef USE_VBO

enum
{
	arrow_all = 0,
	cylinder_all,
	base_all,
	ARRAY_COUNT
};

GLuint arrowObjects[ARRAY_COUNT] = {0};

#endif

static TVertex *arrow;    // "arrow" triangles stored as triangle fan and polygon
static TVertex *cylinder; // "cylinder walls" stored as quad strip
static TVertex *base;     // "bottom cylinder" stored as polygon

/**
 * Initialization of common array for all instances of TSpin (same len, same thickness)
 * Only made when we create the first spin
 */
bool TSpin::FirstInitialization = true;

TSpin::TSpin(GLfloat _len, GLfloat _radius, const TVector3D &pos) : TBaseCylinder(_len, _radius, pos)
{
	ObjectType = otSpin;

	ArrowDetail = 20;
	CylinderDetail = 10;

	// + 2 : +1 pour le point de départ, +1 pour fermer le cone
	ArrowSize = ArrowDetail + 2;
	arrowLength = (ArrowDetail + 2) + (ArrowDetail + 1);

	// + 1 pour fermer le cyclindre
	cylinderLength = 2 * (CylinderDetail + 1);

	// Base
	baseLength = CylinderDetail + 1;

	if (FirstInitialization)
	{
		InitializeArray();
		FirstInitialization = false;
	}

	material.SetColor(mfFront, msAmbient, GLColor_red);
	material.SetColor(mfFront, msDiffuse, GLColor_red);
}

TSpin::~TSpin()
{
	FirstInitialization = true;
	FreeArray();
#ifdef USE_VBO
	glDeleteBuffers(ARRAY_COUNT, arrowObjects);
#endif
}

void TSpin::InitializeArray(void)
{
	arrow = new TVertex[arrowLength];
	cylinder = new TVertex[cylinderLength];
	base = new TVertex[baseLength];

	CreateSpin();

#ifdef USE_VBO
	initOGL();
	arrowObjects[arrow_all] = createVBO(arrow, SIZE_VERTEX(arrowLength));
	arrowObjects[cylinder_all] = createVBO(cylinder, SIZE_VERTEX(cylinderLength));
	arrowObjects[base_all] = createVBO(base, SIZE_VERTEX(baseLength));

	for (int i = 0; i < ARRAY_COUNT; i++)
		createVBO_OK &= (arrowObjects[i] != 0);
	if (!createVBO_OK)
		std::cout << "[WARNING] VBO array not created for Spin." << std::endl;

	// Now we can delete free array
	FreeArray();
#endif
}

void TSpin::FreeArray(void)
{
	if (arrow)
		delete[] arrow;
	arrow = NULL;
	if (cylinder)
		delete[] cylinder;
	cylinder = NULL;
	if (base)
		delete[] base;
	base = NULL;
}

void TSpin::CreateSpin(void)
{
	double R_Arrow = radius * 4.0;
	double phi = atan2(R_Arrow, (double) len2);
	double cos_phi = cos(phi);
	double sin_phi = sin(phi);
	double dtheta = (2.0 * PI / ArrowDetail);
	double theta = 0.0;

	// Arrow
	arrow[0].SetVertice(0.0f, 0.0f, len2);
	arrow[0].SetNormal(0.0f, 0.0f, 1.0f);

	for (GLuint i = 1; i < ArrowSize; ++i)
	{
		const double cos_theta = cos(theta);
		const double sin_theta = sin(theta);
		const float x = (float) (R_Arrow * cos_theta);
		const float y = (float) (R_Arrow * sin_theta);

		arrow[i].SetVertice(x, y, 0.0f);

		arrow[i].normal.X = (float) (cos_phi * cos_theta);
		arrow[i].normal.Y = (float) (cos_phi * sin_theta);
		arrow[i].normal.Z = sin_phi;

		arrow[arrowLength - i].SetVertice(x, y, 0.0f);
		arrow[arrowLength - i].SetNormal(0.0f, 0.0f, -1.0f);

		theta += dtheta;
	}

	// Body + base
	dtheta = (2.0 * PI / CylinderDetail);
	theta = 0.0;

	for (GLuint i = 0; i < baseLength; ++i)
	{
		const double cos_theta = cos(theta);
		const double sin_theta = sin(theta);
		const float x = (float) (radius * cos_theta);
		const float y = (float) (radius * sin_theta);

		const GLuint j = 2 * i;
		const GLuint k = j + 1;

		cylinder[j].SetVertice(x, y, 0.0f);
		cylinder[k].SetVertice(x, y, -len2);

		cylinder[j].normal.X = cylinder[k].normal.X = (float) cos_theta;
		cylinder[j].normal.Y = cylinder[k].normal.Y = (float) sin_theta;
		cylinder[j].normal.Z = cylinder[k].normal.Z = 0.0f;

		base[baseLength - 1 - i].SetVertice(x, y, -len2);
		base[baseLength - 1 - i].SetNormal(0.0f, 0.0f, -1.0f);

		theta += dtheta;
	}
}

void TSpin::DoDisplay(TDisplayMode mode __attribute__((unused)))
{
	glTranslated(position.X, position.Y, position.Z);
	glRotated(angle, axe_angle.X, axe_angle.Y, axe_angle.Z);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

#ifdef USE_VBO

	if (createVBO_OK)
	{
		// arrow
		glBindBuffer(GL_ARRAY_BUFFER, arrowObjects[arrow_all]);
		glInterleavedArrays(GL_N3F_V3F, SIZE_VERTEX(1), (GLvoid*) 0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, ArrowSize);
		glDrawArrays(GL_POLYGON, ArrowSize, ArrowSize - 1);

		// cylindre
		glBindBuffer(GL_ARRAY_BUFFER, arrowObjects[cylinder_all]);
		glInterleavedArrays(GL_N3F_V3F, SIZE_VERTEX(1), (GLvoid*) 0);
		glDrawArrays(GL_QUAD_STRIP, 0, cylinderLength);

		// base cyclindre
		glBindBuffer(GL_ARRAY_BUFFER, arrowObjects[base_all]);
		glInterleavedArrays(GL_N3F_V3F, SIZE_VERTEX(1), (GLvoid*) 0);
		glDrawArrays(GL_POLYGON, 0, baseLength);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

#else

	// arrow
	glInterleavedArrays(GL_N3F_V3F, SIZE_VERTEX(1), &arrow[0]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, ArrowSize);
	glDrawArrays(GL_POLYGON, ArrowSize, ArrowSize-1);

	// cylindre
	glInterleavedArrays(GL_N3F_V3F, SIZE_VERTEX(1), &cylinder[0]);
	glDrawArrays(GL_QUAD_STRIP, 0, cylinderLength);

	// Base
	glInterleavedArrays(GL_N3F_V3F, SIZE_VERTEX(1), &base[0]);
	glDrawArrays(GL_POLYGON, 0, baseLength);

#endif

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

// ********************************************************************************
// End of file
// ********************************************************************************
