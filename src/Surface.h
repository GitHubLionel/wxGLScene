#ifndef _SURFACE_3D_H
#define _SURFACE_3D_H

#include "Object3D.h"
#include <functional>

namespace GLScene
{

// The prototype of the function z = f(x, y)
typedef std::function<double(double x, double y)> TFunctionZ;

/**
 * TSurface class
 * Params: dimensions of the plane dimX x dimY. This the number of points along X axis and Y axis.
 * Params: position (default (0,0,0))
 * Two options :
 * 1) you provide a function to plot via setFunctionZ and an interval with InitializeSurface
 * eg : setFunctionZ([this](double x, double y) {return x * x + y * y;});
 *      InitializeSurface(-5, 5, -5, 5);
 * 2) you load a file with (x, y, z) coordinates with dimX x dimY points
 */
class TSurface: public TObject3D
{
	public:
		TSurface(GLuint dimX, GLuint dimY, const TVector3D &pos = GLDefaultPosition);
		virtual ~TSurface();

		void SetDimension(GLuint dimX, GLuint dimY, bool recompute = false);

		void setFunctionZ(TFunctionZ fonc)
		{
			FunctionZ = fonc;
		}
		void InitializeSurface(double xMin, double xMax, double yMin, double yMax);
		bool LoadSurface(const char *filename);

	protected:
		void DoDisplay(TDisplayMode mode = dmRender);
		virtual void ComputeParameters(bool normalize = true);

	private:
		GLuint sizeX, sizeY;
		TVector3D *surface = NULL;
		TVector3D *normals = NULL;
		GLuint *indices = NULL;
		GLuint sizeLength;
		GLuint indiceLength;
		bool SurfaceComputed;

#ifdef USE_VBO
		GLuint vboId = 0;  // ID of VBO for vertex arrays
		GLuint nboId = 0;  // ID of VBO for normal arrays
		GLuint iboId = 0;  // ID of VBO for index array
		void FreeVBO(void);
#endif

		GLfloat angle;
		TVector3D axe_angle;
		TVector3D reduced_pos;

		TFunctionZ FunctionZ = NULL;
		bool min_max;
		double _xMin;
		double _xMax;
		double _yMin;
		double _yMax;

		void InitializeArray(void);
		void ComputeIndices(void);
		void ComputeNormals(void);
		void FreeArray(void);
};

} // namespace GLScene

//---------------------------------------------------------------------------
#endif // _SURFACE_3D_H
