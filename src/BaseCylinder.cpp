#include "BaseCylinder.h"

TBaseCylinder::TBaseCylinder(GLfloat _len, GLfloat _radius, const TVector3D &pos) : TObject3D(pos)
{
	len = _len;
	len2 = _len / 2.0;
	radius = _radius;

	direction = {0.0, 0.0, 1.0};

	ComputeParameters();
}

TBaseCylinder::~TBaseCylinder()
{

}

void TBaseCylinder::ComputeParameters(bool normalize)
{
	reduced_pos = direction;
	if (normalize)
		reduced_pos.Normalize();
	reduced_pos *= len2;
	axe_angle = {-reduced_pos.Y, reduced_pos.X, 0.0};
	angle = RADtoDEG * acos(reduced_pos.Z / len2);
	reduced_pos = position - reduced_pos;
}

// ********************************************************************************
// End of file
// ********************************************************************************
