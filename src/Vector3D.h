#ifndef _VECTOR_3D_H__
#define _VECTOR_3D_H__

#include <stdio.h>
#include <iostream>
#include <cmath>
#include <initializer_list> // for std::initializer_list
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

namespace GLScene
{

/* ================================================================== */
/*                          Compilation option                        */
/* ================================================================== */

/// If we need to have explicitly the assigment operator for the structure
/// This avoid the warning "Wdeprecated-copy"
//#define ASSIGN_OPERATOR
#define Epsilon 1e-8
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif
#define RADtoDEG (180.0/3.1415926535897932384626433832795)

typedef union TVector3D
{
	public:
		GLfloat array[3];
		struct
		{
				GLfloat X;
				GLfloat Y;
				GLfloat Z;
		};

	public:
		/// Default constructor. Initialize to (0, 0, 0)
		TVector3D()
		{
			X = 0.0f;
			Y = 0.0f;
			Z = 0.0f;
		}

		TVector3D(const GLfloat x, const GLfloat y, const GLfloat z)
		{
			X = x;
			Y = y;
			Z = z;
		}

		TVector3D(const GLfloat vect[])
		{
			X = vect[0];
			Y = vect[1];
			Z = vect[2];
		}

		~TVector3D()
		{
			;
		}

		// Operator overloads
		// This is implicitly declared (-Wdeprecated-copy)
#ifdef ASSIGN_OPERATOR
		inline TVector3D& operator =(const TVector3D &vect)
		{
			X = vect.X;
			Y = vect.Y;
			Z = vect.Z;
			return *this;
		}
#endif

		inline TVector3D& operator =(const GLfloat *vect)
		{
			X = vect[0];
			Y = vect[1];
			Z = vect[2];
			return *this;
		}

		inline TVector3D& operator =(double vect[])
		{
			X = (GLfloat) vect[0];
			Y = (GLfloat) vect[1];
			Z = (GLfloat) vect[2];
			return *this;
		}

		inline TVector3D& operator =(const std::initializer_list<GLfloat> list)
		{
			int count = 0;
			for (auto element : list)
			{
				array[count] = element;
				if (++count > 2)
					break;
			}
			return *this;
		}

		inline bool operator ==(const TVector3D &vect) const
		{
			return (fabs(X - vect.X) + fabs(Y - vect.Y) + fabs(Z - vect.Z)) < Epsilon;
		}

		// Unaire
		inline TVector3D& operator -=(const TVector3D &vect)
		{
			X -= vect.X;
			Y -= vect.Y;
			Z -= vect.Z;
			return *this;
		}

		inline TVector3D& operator -=(const GLfloat val)
		{
			X -= val;
			Y -= val;
			Z -= val;
			return *this;
		}

		inline TVector3D& operator +=(const TVector3D &vect)
		{
			X += vect.X;
			Y += vect.Y;
			Z += vect.Z;
			return *this;
		}

		inline TVector3D& operator +=(const GLfloat val)
		{
			X += val;
			Y += val;
			Z += val;
			return *this;
		}

		inline TVector3D& operator *=(const GLfloat val)
		{
			X *= val;
			Y *= val;
			Z *= val;
			return *this;
		}

		inline TVector3D& operator /=(const GLfloat val)
		{
			GLfloat inv = 1.0f / val;
			X *= inv;
			Y *= inv;
			Z *= inv;
			return *this;
		}

		// Binaire
		inline TVector3D operator -(void)
		{
			return TVector3D(-X, -Y, -Z);
		}

		inline TVector3D operator -(const TVector3D &vect) const
		{
			return TVector3D(X - vect.X, Y - vect.Y, Z - vect.Z);
		}

		inline TVector3D operator +(const TVector3D &vect) const
		{
			return TVector3D(X + vect.X, Y + vect.Y, Z + vect.Z);
		}

		/// Scalar product like dot function
		inline GLfloat operator *(const TVector3D &vect) const
		{
			return vect.X * X + vect.Y * Y + vect.Z * Z;
		}

		inline TVector3D operator *(const GLfloat val) const
		{
			return TVector3D(X * val, Y * val, Z * val);
		}

		inline TVector3D operator /(const GLfloat val) const
		{
			GLfloat inv = 1.0f / val;
			return TVector3D(X * inv, Y * inv, Z * inv);
		}

		// Setter/Getter
		inline GLfloat* Array(void)
		{
			return &array[0];
		}

		// Only getter
		inline GLfloat operator [](const int index) const
		{
			return array[index];
		}

		inline GLfloat Get(const int index) const
		{
			return array[index];
		}

		inline void Set(const int index, const GLfloat val)
		{
			array[index] = val;
		}

		inline void TranslateTo(GLfloat x, GLfloat y, GLfloat z)
		{
			X = x;
			Y = y;
			Z = z;
		}

		// Other functions
		/// Scalar product
		inline GLfloat Dot(const TVector3D &vect) const
		{
			return vect.X * X + vect.Y * Y + vect.Z * Z;
		}

		/// Vectorial product
		inline TVector3D Vectorial(const TVector3D &vect) const
		{
			return TVector3D((Y * vect.Z - Z * vect.Y), (Z * vect.X - X * vect.Z), (X * vect.Y - Y * vect.X));
		}

		/// Vector Length
		inline GLfloat Length(void) const
		{
			return sqrtf(X * X + Y * Y + Z * Z);
		}

		/// Test if vector is nul
		inline bool IsNull(void) const
		{
			return ((fabs(X) + fabs(Y) + fabs(Z)) < Epsilon);
		}

		/// Distance to a point
		inline GLfloat Distance(const TVector3D &vect) const
		{
			return (*this - vect).Length();
		}

		/// Normalize
		inline TVector3D& Normalize()
		{
			GLfloat norm = X * X + Y * Y + Z * Z;
			if (fabs(norm) < Epsilon)
				*this = TVector3D(0.0f, 0.0f, 0.0f);
			else
				*this /= sqrtf(norm);
			return *this;
		}

		/// Make a stereographic projection
		inline TVector3D& Stereo(void)
		{
			GLfloat d = 1 - Z;
			if (fabs(d) > Epsilon)
			{
				X /= d;
				Y /= d;
				Z = 0.0f;
			}
			else
			{
				X = Y = Z = 0.0f;
			}
			return *this;
		}

		/// Simple print function
		inline void Print(void) const
		{
			printf("(%.6g, %.6g, %.6g)\n", X, Y, Z);
			fflush(stdout);
		}
} TVector3D;

/**
 * Some test operation
 */
inline void Vector3D_Test(void)
{
	TVector3D a;
	// TVector3D b(1,2,3);
	TVector3D b = TVector3D(1, 2, 3);

	TVector3D c, e;

	GLfloat d;

	printf("\n******** Vector 3D Test ***********\n");
	fflush(stdout);

	b.Print();

	a = -b; // * (-1.0);
	a.Print();

	a *= 3;
	a.Print();

	a.Set(2, 3.14);
	a.Print();

	a.Array()[0] = 1.4142;
	a.Print();

	printf("%f\r\n", a[0]);

	printf("%f\r\n", a.Get(2));
	fflush(stdout);

	c = a;
	b.Print();
	c -= b;
	c.Print();

	d = b.Length();
	printf("d = %f\r\n", d);

	d = b.Distance(TVector3D(1, 2, 3));
	printf("d = %f\r\n", d);

	e = TVector3D(1, 1, 1);
	e.Normalize();
	e.Print();

//  GLfloat tab[] = {0};
	TVector3D g( {1, 2, 3});
	g.Print();

	g = {2, 3, 4};
	g.Print();

	fflush(stdout);
}

} // namespace GLScene

using namespace GLScene;

//---------------------------------------------------------------------------

#endif // _VECTOR_3D_H__
