#ifndef _AXIS_3D_H
#define _AXIS_3D_H

#include "Vector3D.h"
#include "Vector4D.h"
#include "Object3D.h"

namespace GLScene
{

/**
 * TAxis class
 * Params: len  : length of the axe
 * Params: detail : detail of the cone
 */
class TAxis: public TObject3D
{
	public:
		TAxis(GLfloat len = 10, GLuint detail = 20);
		virtual ~TAxis();

	protected:
		void virtual DoDisplay(TDisplayMode mode = dmRender);

	private:
		TVertex *arrow;
		GLuint vertexSize;
		GLuint arrowDetail;
		GLuint arrowSize;
		GLfloat thickness;
		GLfloat size;
		GLfloat colorRed[4] = {1.0, 0.0, 0.0, 1.0};
		GLfloat colorGreen[4] = {0.0, 1.0, 0.0, 1.0};
		GLfloat colorBlue[4] = {0.0, 0.0, 1.0, 1.0};

		void CreateArrow(void);
};

} // namespace GLScene

//---------------------------------------------------------------------------
#endif // _AXIS_3D_H
