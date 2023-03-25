#ifndef _CUBE_3D_H
#define _CUBE_3D_H

#include "Vector3D.h"
#include "Vector4D.h"
#include "Object3D.h"

namespace GLScene
{

/**
 * TCube class
 * Params: size of the cube or the 3 size of a parallelepiped over x, y and z
 * Params: position (default (0,0,0))
 * The cube is centered to it's position and initial direction is (0,0,1)
 */
class TCube: public TObject3D
{
	public:
		TCube(GLfloat _size, const TVector3D &pos = GLDefaultPosition);
		TCube(TVector3D _size, const TVector3D &pos = GLDefaultPosition);
		virtual ~TCube();

	protected:
		void DoDisplay(TDisplayMode mode = dmRender);
		virtual void ComputeParameters(bool normalize = true);

	private:
		GLfloat *cube;
		GLfloat size;
		TVector3D size3D;
		bool IsCube;
		GLfloat angle;
		TVector3D axe_angle;
		TVector3D reduced_pos;
};

} // namespace GLScene

//---------------------------------------------------------------------------
#endif // _CUBE_3D_H
