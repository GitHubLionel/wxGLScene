#ifndef _Vector_4D_H
#define _Vector_4D_H

// we need OpenGL headers for GLfloat/GLint types used below
#if defined(__WXMAC__)
	#ifdef __DARWIN__
		#include <OpenGL/gl.h>
	#else
		#include <gl.h>
	#endif
#else
	#include <GL/gl.h>
#endif

#include "Vector3D.h"
#include <cmath>
#include <initializer_list> // for std::initializer_list

namespace GLScene
{

/* ================================================================== */
/*                          Compilation option                        */
/* ================================================================== */

/// If we need to have explicitly the assigment operator for the structure
/// This avoid the warning "Wdeprecated-copy"
//#define ASSIGN_OPERATOR
#define Epsilon 1e-8

typedef union TVector4D
{
	public:
		GLfloat array[4];
		struct
		{
				GLfloat X;
				GLfloat Y;
				GLfloat Z;
				GLfloat alpha;
		};
		struct
		{
				GLfloat R;
				GLfloat G;
				GLfloat B;
				GLfloat Alpha;
		};

	public:
		/// Default constructor. Initialize to (0, 0, 0, 0)
		TVector4D()
		{
			X = 0.0f;
			Y = 0.0f;
			Z = 0.0f;
			alpha = 0.0f;
		}

		TVector4D(const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat a)
		{
			X = x;
			Y = y;
			Z = z;
			alpha = a;
		}

		// This is implicitly declared (-Wdeprecated-copy)
#if ASSIGN_OPERATOR
		TVector4D(const GLfloat vect[])
		{
			X = vect[0];
			Y = vect[1];
			Z = vect[2];
			alpha = vect[3];
		}
#endif

		TVector4D(const TVector3D vect, const GLfloat a)
		{
			X = vect.X;
			Y = vect.Y;
			Z = vect.Z;
			alpha = a;
		}

		~TVector4D()
		{
			;
		}

		// Operator overloads
		// This is implicitly declared (-Wdeprecated-copy)
#if ASSIGN_OPERATOR
		inline TVector4D& operator =(const TVector4D &vect)
		{
			X = vect.X;
			Y = vect.Y;
			Z = vect.Z;
			alpha = vect.alpha;
			return *this;
		}
#endif

		inline TVector4D& operator =(const GLfloat *vect)
		{
			X = vect[0];
			Y = vect[1];
			Z = vect[2];
			alpha = vect[3];
			return *this;
		}

		inline TVector4D& operator =(const std::initializer_list<GLfloat> list)
		{
			int count = 0;
			for (auto element : list)
			{
				array[count] = element;
				if (++count > 3)
					break;
			}
			return *this;
		}

		inline bool operator ==(const TVector4D &vect) const
		{
			return (fabs(X - vect.X) + fabs(Y - vect.Y) + fabs(Z - vect.Z) + fabs(alpha - vect.alpha)) < Epsilon;
		}

		inline TVector4D& operator -=(const TVector4D &vect)
		{
			X -= vect.X;
			Y -= vect.Y;
			Z -= vect.Z;
			alpha -= vect.alpha;
			return *this;
		}

		inline TVector4D& operator -=(const TVector3D &vect)
		{
			X -= vect.X;
			Y -= vect.Y;
			Z -= vect.Z;
			return *this;
		}

		inline TVector4D& operator -=(const GLfloat val)
		{
			X -= val;
			Y -= val;
			Z -= val;
			return *this;
		}

		inline TVector4D& operator +=(const TVector4D &vect)
		{
			X += vect.X;
			Y += vect.Y;
			Z += vect.Z;
			alpha += vect.alpha;
			return *this;
		}

		inline TVector4D& operator +=(const TVector3D &vect)
		{
			X += vect.X;
			Y += vect.Y;
			Z += vect.Z;
			return *this;
		}

		inline TVector4D& operator +=(const GLfloat val)
		{
			X += val;
			Y += val;
			Z += val;
			return *this;
		}

		inline TVector4D& operator *=(const GLfloat val)
		{
			X *= val;
			Y *= val;
			Z *= val;
			alpha *= val;
			return *this;
		}

		inline TVector4D& operator /=(const GLfloat val)
		{
			GLfloat inv = 1.0f / val;
			X *= inv;
			Y *= inv;
			Z *= inv;
			alpha *= inv;
			return *this;
		}

		inline TVector4D operator -(void)
		{
			return TVector4D(-X, -Y, -Z, -alpha);
		}

		inline TVector4D operator -(const TVector4D &vect) const
		{
			return TVector4D(X - vect.X, Y - vect.Y, Z - vect.Z, alpha - vect.alpha);
		}

		inline TVector4D operator +(const TVector4D &vect) const
		{
			return TVector4D(X + vect.X, Y + vect.Y, Z + vect.Z, alpha + vect.alpha);
		}

		/// Scalar product like dot function
		inline GLfloat operator *(const TVector4D &vect) const
		{
			return vect.X * X + vect.Y * Y + vect.Z * Z + vect.alpha * alpha;
		}

		inline TVector4D operator *(const GLfloat val) const
		{
			return TVector4D(X * val, Y * val, Z * val, alpha * val);
		}

		inline TVector4D operator /(const GLfloat val) const
		{
			GLfloat inv = 1.0f / val;
			return TVector4D(X * inv, Y * inv, Z * inv, alpha * inv);
		}

		// Setter/Getter
		inline GLfloat* Array(void)
		{
			return &array[0];
		}

		// Only getter
		inline GLfloat operator[](const int index) const
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

		inline void TranslateTo(GLfloat x, GLfloat y, GLfloat z, GLfloat a)
		{
			X = x;
			Y = y;
			Z = z;
			alpha = a;
		}

		// Other functions
		/// Scalar product
		inline GLfloat Dot(const TVector4D &vect) const
		{
			return vect.X * X + vect.Y * Y + vect.Z * Z + vect.alpha * alpha;
		}

		/// Vector Length
		inline GLfloat Length(void) const
		{
			return sqrtf(X * X + Y * Y + Z * Z + alpha * alpha);
		}

		/// Test if vector is nul
		inline bool IsNull(void) const
		{
			return ((fabs(X) + fabs(Y) + fabs(Z) + fabs(alpha)) < Epsilon);
		}

		/// Distance to a point
		inline GLfloat Distance(const TVector4D &vect) const
		{
			return (*this - vect).Length();
		}

		/// Normalize
		inline TVector4D& Normalize()
		{
			GLfloat norm = X * X + Y * Y + Z * Z + alpha * alpha;
			if (fabs(norm) < Epsilon)
				*this = TVector4D(0.0f, 0.0f, 0.0f, 0.0f);
			else
				*this /= sqrtf(norm);
			return *this;
		}

		/// Simple print function
		inline void Print(void) const
		{
			printf("(%f, %f, %f, %f)\n", X, Y, Z, alpha);
			fflush(stdout);
		}
} TVector4D;

/**
 * Some test operation
 */
inline void Vector4D_Test(void)
{
	TVector4D a;
	TVector4D b(1, 2, 3, 4);
	TVector4D c, e;

	GLfloat d;

	printf("\n******** Vector 4D Test ***********\n");
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

	d = b.Distance(TVector4D(1, 2, 3, 4));
	printf("d = %f\r\n", d);

	e = TVector4D(1, 1, 1, 1);
	e.Normalize();
	e.Print();

	GLfloat *tab;
	TVector3D color(2, 2, 2);
	tab = (TVector4D(color, 1)).Array();
	printf("tab = (%f, %f, %f, %f)\r\n", tab[0], tab[1], tab[2], tab[3]);

	fflush(stdout);
}

} // namespace GLScene

using namespace GLScene;

//---------------------------------------------------------------------------
#endif // _Vector_4D_H
