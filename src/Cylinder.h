#ifndef _CYLINDER_3D_H
#define _CYLINDER_3D_H

#include "Object3D.h"
#include "BaseCylinder.h"

namespace GLScene
{

/**
 * TCylinder class
 * Params: length
 * Params: radius
 * Params: position (default (0,0,0))
 * The cylinder is centered to it's position and initial direction is (0,0,1)
 */
class TCylinder: public TBaseCylinder
{
	public:
		TCylinder(GLfloat _len = 1.0f, GLfloat _radius = 0.5f, const TVector3D &pos = GLDefaultPosition);
		virtual ~TCylinder();

		void DoDisplay(TDisplayMode mode = dmRender);

	private:
		GLint CylinderDetail;

		GLuint cylinderLength;
		GLuint baseLength;

		TVertex *cylinder;   // "cylinder walls" stored as quad strip
		TVertex *base_up;    // "up cylinder" stored as polygon
		TVertex *base_down;  // "down cylinder" stored as polygon

#ifdef USE_VBO    
		GLuint arrowObjects[3] = {0};
#endif

		void InitializeArray(void);
		void FreeArray(void);
		void CreateCylinder(void);
};

} // namespace GLScene

//---------------------------------------------------------------------------
#endif // _CYLINDER_3D_H
