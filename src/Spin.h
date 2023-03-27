#ifndef _SPIN_3D_H
#define _SPIN_3D_H

#include "Object3D.h"
#include "BaseCylinder.h"

namespace GLScene
{

/**
 * TSpin class
 * Params: length
 * Params: thickness
 * Params: position (default (0,0,0))
 * The arrow is centered to it's position and initial direction is (0,0,1)
 */
class TSpin: public TBaseCylinder
{
	public:
		TSpin(GLfloat _len = 1.0f, GLfloat _radius = 0.03f, const TVector3D &pos = GLDefaultPosition);
		virtual ~TSpin();

		static bool FirstInitialization;

	protected:
		void DoDisplay(TDisplayMode mode = dmRender);

	private:
		GLint ArrowDetail;
		GLint CylinderDetail;

		GLuint arrowLength;
		GLuint ArrowSize;
		GLuint cylinderLength;
		GLuint baseLength;

		void InitializeArray(void);
		void FreeArray(void);
		void CreateSpin(void);
};

} // namespace GLScene

//---------------------------------------------------------------------------
#endif // _SPIN_3D_H
