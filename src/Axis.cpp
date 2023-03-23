#include "Axis.h"
#include <string.h>
#include <math.h>

// unit axis //////////////////////////////////////////////////////////////////
//    v3
//    |
//    |
//    v0------v2
//   /
//  v1

// Vertex coordinates interleaved in format : GL_C4F_N3F_V3F
static GLfloat axis[] = {
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // v0 red normal z
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, // v1 red normal z

		0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // v0 green normal z
		0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // v2 green normal z

		0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // v0 blue normal z
		0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f  // v3 blue normal z
		};

/**
 * TAxis class
 * Params: len  : length of the axe
 * Params: detail : detail of the cone
 */
TAxis::TAxis(GLfloat len, GLuint detail) :
		TObject3D()
{
	ObjectType = otAxis;
	size = len;
	arrowDetail = detail;
	arrowSize = detail + 2;
	thickness = 0.02f;

	// axis length
	axis[17] = size;
	axis[38] = size;
	axis[59] = size;

	// arrow = cone + base = (arrowDetail + 1 for top + 1 to close circle) + (arrowDetail + 1 to close circle)
	vertexSize = (arrowDetail + 2) + (arrowDetail + 1);

	arrow = new TVertex[vertexSize];
	CreateArrow();

	material.SetColor(mfFront, msAmbient, {0.0, 0.0, 0.0, 1.0});
	material.SetColor(mfFront, msDiffuse, {0.5, 0.5, 1.0, 1.0});
	material.SetColor(mfFront, msEmission, {0.2, 0.2, 0.2, 1.0});
	material.SetColor(mfFront, msSpecular, {0.0, 0.0, 0.0, 1.0});
}

TAxis::~TAxis()
{
	delete arrow;
}

void TAxis::CreateArrow(void)
{
#define len	0.5
	double R_Arrow = thickness * 4.0;
	double phi = atan2(R_Arrow, len);
	double cos_phi = cos(phi);
	double sin_phi = sin(phi);
	double dtheta = (2.0 * PI / arrowDetail);
	double theta = 0.0;

	// Top
	arrow[0].SetVertice(0.0f, 0.0f, len);
	arrow[0].SetNormal(0.0f, 0.0f, 1.0f);

	// Cone + base
	for (GLuint i = 1; i < arrowSize; ++i)
	{
		const double cos_theta = cos(theta);
		const double sin_theta = sin(theta);
		const float x = (float) (R_Arrow * cos_theta);
		const float y = (float) (R_Arrow * sin_theta);

		// Cone
		arrow[i].SetVertice(x, y, 0.0f);

		arrow[i].normal.X = (float) (cos_phi * cos_theta);
		arrow[i].normal.Y = (float) (cos_phi * sin_theta);
		arrow[i].normal.Z = sin_phi;

		// Base
		arrow[vertexSize - i].SetVertice(x, y, 0.0f);
		arrow[vertexSize - i].SetNormal(0.0f, 0.0f, -1.0f);

		theta += dtheta;
	}
}

void TAxis::DoDisplay(TDisplayMode mode)
{
	// Don't draw axing in select mode
	if (mode == dmSelect)
		return;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	// Load axis
	glInterleavedArrays(GL_C4F_N3F_V3F, 10 * sizeof(GLfloat), &axis[0]);
	glEnable(GL_COLOR_MATERIAL);  // Important !
	glLineWidth(3.0f);
	glDrawArrays(GL_LINES, 0, 6);
	glLineWidth(1.0f);
	glDisable(GL_COLOR_MATERIAL);

	// Load arrow
	glInterleavedArrays(GL_N3F_V3F, SIZE_VERTEX(1), &arrow[0]);

	// X axis red
	glPushMatrix();
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colorRed);
		glTranslated(size, 0.0, 0.0);
		glRotatef(90, 1.0, 0.0, 0.0);
		glRotatef(90, 0.0, 1.0, 0.0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, arrowSize);
		glDrawArrays(GL_POLYGON, arrowSize, arrowSize - 1);
	glPopMatrix();

	// Y axis green
	glPushMatrix();
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colorGreen);
		glTranslated(0.0, size, 0.0);
		glRotatef(90, -1.0, 0.0, 0.0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, arrowSize);
		glDrawArrays(GL_POLYGON, arrowSize, arrowSize - 1);
	glPopMatrix();

	// Z axis blue
	glPushMatrix();
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colorBlue);
		glTranslated(0.0, 0.0, size);
		glDrawArrays(GL_TRIANGLE_FAN, 0, arrowSize);
		glDrawArrays(GL_POLYGON, arrowSize, arrowSize - 1);
	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

// ********************************************************************************
// End of file
// ********************************************************************************
