#ifndef _SPHERE_3D_H
#define _SPHERE_3D_H

#include <math.h>
#include <stdio.h>
#include "Vector3D.h"
#include "Vector4D.h"
#include "Object3D.h"

namespace GLScene
{

/**
 * TSphere class
 * Params: radius of the sphere or the 3 radius of the ellipsoïd over x, y and z
 * Params: position (default (0,0,0))
 * The sphere/ellipsoïd is centered to it's position
 */
class TSphere: public TObject3D
{
	public:
		TSphere(GLfloat _radius, GLuint _sectorCount = 36, GLuint _stackCount = 18, const TVector3D &pos = GLDefaultPosition);
		TSphere(TVector3D _radius, GLuint _sectorCount = 36, GLuint _stackCount = 18, const TVector3D &pos = GLDefaultPosition);
		virtual ~TSphere();

		void SetRadius(GLfloat _radius)
		{
			radius = _radius;
			IsSphere = true;
			InitializeArray();
		}

		void SetRadius(TVector3D _radius)
		{
			radius3D = _radius;
			IsSphere = false;
			InitializeArray();
		}

	protected:
		void virtual DoDisplay(TDisplayMode mode = dmRender);

	private:
		GLfloat radius;
		TVector3D radius3D;
		GLuint sectorCount;     // longitude, # of slices
		GLuint stackCount;      // latitude, # of stacks
		bool IsSphere;

		TVertex *sphere = NULL;
		GLuint *indices = NULL;
		GLuint sphereLength;
		GLuint indiceLength;

		// For low resolution
		TVertex *sphere_low = NULL;
		GLuint *indices_low = NULL;
		GLuint sphereLength_low;
		GLuint indiceLength_low;

#ifdef USE_VBO
		GLuint vboId = 0;  // ID of VBO for vertex and normal interleaved arrays
		GLuint iboId = 0;  // ID of VBO for index array

		// For low resolution
		GLuint vboId_low = 0;  // ID of VBO for vertex and normal interleaved arrays
		GLuint iboId_low = 0;  // ID of VBO for index array
#endif

		void InitializeArray(void);
		void FreeArray(void);
		void FreeVBO(void);
		void CreateSphere(GLuint sectorCount, GLuint stackCount, TVertex *sph, GLuint *ind);
};

} // namespace GLScene

//---------------------------------------------------------------------------
#endif // _SPHERE_3D_H
