#ifndef _ARROW_3D_H
#define _ARROW_3D_H

#include "Vector3D.h"
#include "Vector4D.h"
#include "Object3D.h"
#include "Cone.h"
#include "Cylinder.h"

namespace GLScene
{

/**
 * TArrow class
 * Params: length
 * Params: thickness
 * Params: position (default (0,0,0))
 * The arrow is centered to it's position and initial direction is (0,0,1)
 */
class TArrow: public TBaseCylinder
{
	public:
		TArrow(GLfloat _len, GLfloat _radius, const TVector3D &pos = GLDefaultPosition);
		virtual ~TArrow();

	protected:
		void DoDisplay(TDisplayMode mode = dmRender);

	private:
		TCylinder *myCylinder;
		TCone *myCone;
};

} // namespace GLScene

//---------------------------------------------------------------------------
#endif // _ARROW_3D_H
