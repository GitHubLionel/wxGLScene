#ifndef _FACEPROPERTIES_H
#define _FACEPROPERTIES_H

#include "Vector4D.h"
#include <initializer_list> // for std::initializer_list

namespace GLScene
{

typedef enum TMaterialFace
{
	mfFront = 1,
	mfBack = 2,
	mfFrontAndBack = 3
} TMaterialFace;

typedef enum TMaterialSource
{
	msAmbient,
	msDiffuse,
	msEmission,
	msSpecular
} TMaterialSource;

typedef struct TGLFaceProperties
{
	public:
		TVector4D Ambient;
		TVector4D Diffuse;
		TVector4D Emission;
		TVector4D Specular;
		GLfloat Shininess;

		/// Default constructor. Initialize to (0, 0, 0, 0)
		TGLFaceProperties()
		{
			Shininess = 0.0;
		}

		TGLFaceProperties(const TVector4D am, const TVector4D di, const TVector4D em, const TVector4D sp, const GLfloat sh)
		{
			Ambient = am;
			Diffuse = di;
			Emission = em;
			Specular = sp;
			Shininess = sh;
		}

		~TGLFaceProperties()
		{
			;
		}

		inline TGLFaceProperties& operator =(const TVector4D *vect)
		{
			Ambient = vect[0];
			Diffuse = vect[1];
			Emission = vect[2];
			Specular = vect[3];
			Shininess = vect[4][0];
			return *this;
		}

		inline TGLFaceProperties& operator =(const std::initializer_list<TVector4D> list)
		{
			int count = 0;
			for (auto element : list)
			{
				switch (count)
				{
					case 0:
						Ambient = element;
						break;
					case 1:
						Diffuse = element;
						break;
					case 2:
						Emission = element;
						break;
					case 3:
						Specular = element;
						break;
					case 4:
						Shininess = element[0];
						break;
					default:
						;
				}
				if (++count > 4)
					break;
			}
			return *this;
		}

} TGLFaceProperties;

} // namespace GLScene

using namespace GLScene;

//---------------------------------------------------------------------------
#endif // _FACEPROPERTIES_H
