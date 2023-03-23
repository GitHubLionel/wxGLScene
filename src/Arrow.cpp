#include "Arrow.h"
#include <math.h>

TArrow::TArrow(GLfloat _len, GLfloat _radius, const TVector3D &pos) :
		TBaseCylinder(_len, _radius, pos)
{
	ObjectType = otArrow;

	TVector3D arrowPos = GLDefaultPosition;
	arrowPos.Z -= len2 / 2;
	myCylinder = new TCylinder(len2, _radius, arrowPos);

	arrowPos = GLDefaultPosition;
	arrowPos.Z += len2 / 2;
	myCone = new TCone(len2, _radius * 4, arrowPos);

	material.SetColor(mfFront, msAmbient, GLColor_fuchsia);
	material.SetColor(mfFront, msDiffuse, GLColor_fuchsia);

	myCylinder->SetUseMaterial(false);
	myCone->SetUseMaterial(false);
}

TArrow::~TArrow()
{
	delete myCylinder;
	delete myCone;
}

void TArrow::DoDisplay(TDisplayMode mode __attribute__((unused)))
{
	glPushMatrix();
		glTranslated(position.X, position.Y, position.Z);
		glRotated(angle, axe_angle.X, axe_angle.Y, axe_angle.Z);
		myCylinder->DoDisplay(mode);
		myCone->DoDisplay(mode);
	glPopMatrix();
}

// ********************************************************************************
// End of file
// ********************************************************************************
