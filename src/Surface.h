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
		TSurface(GLuint dimX, GLuint dimY, bool useMatColor = true, const TVector3D &pos = GLDefaultPosition);
		virtual ~TSurface();

		void SetDimension(GLuint dimX, GLuint dimY, bool recompute = false);
		void SetColor(const TVector4D &begin, const TVector4D &end);
		void SetNormal(bool use);

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
		TVector3D *colors = NULL;
		GLuint *indices = NULL;
		GLuint sizeLength;
		GLuint indiceLength;
		bool SurfaceComputed;
		bool useNormal;

#ifdef USE_VBO
		GLuint vboId = 0;  // ID of VBO for vertex arrays
		GLuint nboId = 0;  // ID of VBO for normal arrays
		GLuint cboId = 0;  // ID of VBO for normal arrays
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

		// For color
		double Maximum, Minimum;
		TVector4D colorBegin;
		TVector4D colorEnd;
		TVector4D colorDelta;
		bool colorUpdated;
		bool useColor;

		void InitializeArray(void);
		void ComputeIndices(void);
		void ComputeNormals(void);
		void ComputeColors(void);
		void FreeArray(void);
		TVector3D CreateColor(double Value);
};

} // namespace GLScene

//---------------------------------------------------------------------------
#endif // _SURFACE_3D_H
