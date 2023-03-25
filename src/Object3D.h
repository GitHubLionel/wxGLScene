#ifndef _OBJECT_3D_H
#define _OBJECT_3D_H

// we need OpenGL headers for GLfloat/GLint types used below
#if defined(__WXMAC__)
	#ifdef __DARWIN__
		#include <OpenGL/gl.h>
		#include <OpenGL/glu.h>
	#else
		#include <gl.h>
		#include <glu.h>
	#endif
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif
#include "Vector3D.h"
#include "Vector4D.h"
#include "Material.h"
#include <initializer_list> // for std::initializer_list
#include "GLColor.h"

// define for VBO else use vertex array
//#define USE_VBO

#ifdef USE_VBO

#ifndef _WIN32
// For Linux, add this define to the project
#define GL_GLEXT_PROTOTYPES	1
#endif
#include <GL/glext.h>

#ifdef _WIN32
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLBUFFERSUBDATAPROC glBufferSubData;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
#endif

void initOGL();
GLuint createVBO(const void *data, int dataSize, GLenum target = GL_ARRAY_BUFFER, GLenum usage = GL_STATIC_DRAW);

#endif

namespace GLScene
{

typedef enum {
	otAxis, otCube, otCuboid, otCylinder, otCone, otSphere,
	otEllipsoid, otArrow, otGrid, otSpin, otNone
} TObjectType;

typedef enum {
	dmRender, dmSelect
} TDisplayMode;

typedef union TGLfloat3D
{
	public:
		GLfloat array[3];
		struct
		{
			GLfloat X;
			GLfloat Y;
			GLfloat Z;
		};
} TGLfloat3D;

/**
 * A simple Vertex structure with normal and position
 */
typedef union TVertex
{
	public:
		GLfloat array[6];
		struct
		{
			// Normal must be first (GL_N3F_V3F)
			TGLfloat3D normal;
			TGLfloat3D vertice;
		};

		TVertex()
		{
			normal.X = normal.Y = normal.Z = 0.0f;
			vertice.X = vertice.Y = vertice.Z = 0.0f;
		}

		void SetVertice(const GLfloat x, const GLfloat y, const GLfloat z)
		{
			vertice.X = x;
			vertice.Y = y;
			vertice.Z = z;
		}

		void SetVertice(const TGLfloat3D &vert)
		{
			vertice.X = vert.X;
			vertice.Y = vert.Y;
			vertice.Z = vert.Z;
		}

		void SetNormal(const GLfloat x, const GLfloat y, const GLfloat z)
		{
			normal.X = x;
			normal.Y = y;
			normal.Z = z;
		}

		void SetNormal(const TGLfloat3D &norm)
		{
			normal.X = norm.X;
			normal.Y = norm.Y;
			normal.Z = norm.Z;
		}

} TVertex;

#define SIZE_VERTEX(size)	((size) * 6 * sizeof(GLfloat))
#define SIZE_FLOAT3D(size)	((size) * 3 * sizeof(GLfloat))
#define SIZE_USHORT(size)	((size) * sizeof(GLushort))
#define SIZE_UINT(size)	((size) * sizeof(GLuint))

extern const TVector3D GLDefaultPosition;

/**
 * TObject3D class
 * A base class for all the object
 * Params: position (default (0,0,0))
 * Params: rotation (default (0,0,0))
 * Params: scale (default (1.0,1.0,1.0))
 * By default, none of the position, rotation and scale are doing
 */
class TObject3D {
	public:
		TObject3D(const TVector3D &pos = GLDefaultPosition);
		TObject3D(TVector3D *pos, TVector3D *rot = NULL, TVector3D *sca = NULL);
		virtual ~TObject3D();

		/// A tag for the user
		int Tag;
		/// Auto free when object is removed from the list
		bool AutoFree;
		/// Visibility of the object
		bool Visible;
		/// A pointer that can be used by user to attach what he want
		void *UserPointer;

		void Display(int id, TDisplayMode mode = dmRender);

		void Select()
		{
			selected = true;
		}
		void Deselect()
		{
			selected = false;
		}

		// setters
		void virtual SetPosition(const TVector3D *pos, bool normalize = true)
		{
			position = *pos;
			ComputeParameters(normalize);
			Changed = true;
		}
		void virtual SetPosition(GLfloat pos[], bool normalize = true)
		{
			position = pos;
			ComputeParameters(normalize);
			Changed = true;
		}
		void virtual SetPosition(std::initializer_list<GLfloat> list, bool normalize = true);
		void virtual SetPosition(GLfloat x, GLfloat y, GLfloat z, bool normalize = true)
		{
			position = { x, y, z };
			ComputeParameters(normalize);
			Changed = true;
		}

		void virtual SetDirection(const TVector3D *pos, bool normalize = true)
		{
			direction = *pos;
			ComputeParameters(normalize);
			Changed = true;
		}
		void virtual SetDirection(GLfloat pos[], bool normalize = true)
		{
			direction = pos;
			ComputeParameters(normalize);
			Changed = true;
		}
		void virtual SetDirection(std::initializer_list<GLfloat> list, bool normalize = true);
		void virtual SetDirection(GLfloat x, GLfloat y, GLfloat z, bool normalize = true)
		{
			direction = { x, y, z };
			ComputeParameters(normalize);
			Changed = true;
		}

		void SetRotation(TVector3D *rot);
		void SetScale(TVector3D *sca);

		TMaterial &GetMaterial(void)
		{
			return material;
		}

		void SetUseMaterial(bool use)
		{
			DoUseMaterial = use;
		}

		void ChangeFrontEmission(GLfloat val);

		// getters
		inline TObjectType GetObjectType()
		{
			return ObjectType;
		}

		inline TVector3D GetPosition()
		{
			return position;
		}
		inline TVector3D GetDirection()
		{
			return direction;
		}
		inline TVector3D GetRotation()
		{
			return rotation;
		}
		inline TVector3D GetScale()
		{
			return scale;
		}

		inline int GetLevelOfDetail()
		{
			return levelOfDetail;
		}

	protected:
		TObjectType ObjectType;
		bool Lighted;
		bool Changed;

		TVector3D z_direction = TVector3D(0.0, 0.0, 1.0);

		// Manipulation
		TVector3D position;
		TVector3D direction;
		TVector3D rotation;
		TVector3D scale;
		bool DoPosition = false;
		bool DoRotation = false;
		bool DoScale = false;

		// Material
		TMaterial material = TMaterial();

		bool DoUseMaterial = true;

		int levelOfDetail;
		bool selected;

#ifdef USE_VBO
    bool createVBO_OK = true;
#endif

    // Operation to do when we change position and direction
		virtual void ComputeParameters(bool normalize = true)
    {
    	(void) normalize;
    }
		virtual void SetLevelOfDetail(int detail);
		virtual void DoDisplay(TDisplayMode mode = dmRender) = 0;
		void PrepareRotate(GLfloat len);

	private:
		void defaultSettings();
		void Draw();
};

// Random functions
GLfloat GLRand(void);
GLfloat GLRand(GLfloat max);

} // namespace GLScene

//---------------------------------------------------------------------------
#endif // _OBJECT_3D_H
