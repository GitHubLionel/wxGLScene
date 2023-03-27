#ifndef _CONE_3D_H
#define _CONE_3D_H

#include "Object3D.h"
#include "BaseCylinder.h"

namespace GLScene
{

/**
 * TCone class
 * Params: length
 * Params: radius
 * Params: position (default (0,0,0))
 * The cone is centered to it's base position and initial direction is (0,0,1)
 */
class TCone: public TBaseCylinder
{
	public:
		TCone(GLfloat _len = 1.0f, GLfloat _radius = 0.5f, const TVector3D &pos = GLDefaultPosition);
		virtual ~TCone();

		void DoDisplay(TDisplayMode mode = dmRender);

	private:
		GLint ConeDetail;

		GLuint coneLength;
		GLuint ConeSize;
		TVertex *cone = NULL;    // "cone" triangles stored as triangle fan and polygon
		GLuint vboId = 0;

		void InitializeArray(void);
		void FreeArray(void);
		void CreateCone(void);
};

} // namespace GLScene

//---------------------------------------------------------------------------
#endif // _CONE_3D_H
