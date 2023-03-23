#ifndef _BASE_CYLINDER_3D_H
#define _BASE_CYLINDER_3D_H

#include <math.h>
#include <stdio.h>
#include "Vector3D.h"
#include "Vector4D.h"
#include "Object3D.h"

namespace GLScene
{

/**
 * TBaseCylinder class
 * This class draw nothing, just the base for cylinder and cone
 */
class TBaseCylinder: public TObject3D
{
	public:
		TBaseCylinder(GLfloat _len, GLfloat _radius, const TVector3D &pos = GLDefaultPosition);
		virtual ~TBaseCylinder();

	protected:
		GLfloat len;
		GLfloat len2;
		GLfloat radius;

		GLfloat angle;
		TVector3D axe_angle;
		TVector3D reduced_pos;

		virtual void ComputeParameters(bool normalize = true);
};

} // namespace GLScene

//---------------------------------------------------------------------------
#endif // _BASE_CYLINDER_3D_H
