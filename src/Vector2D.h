#ifndef _VECTOR_2D_H__
#define _VECTOR_2D_H__

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

#define Epsilon 1e-8

typedef union TVector2D
{
	public:
		GLfloat array[2];
		struct
		{
				GLfloat X;
				GLfloat Y;
		};

	public:
		/// Default constructor. Initialize to (0, 0)
		TVector2D()
		{
			X = 0.0f;
			Y = 0.0f;
		}

		TVector2D(const GLfloat x, const GLfloat y)
		{
			X = x;
			Y = y;
		}

		TVector2D(const GLfloat vect[])
		{
			X = vect[0];
			Y = vect[1];
		}

		~TVector2D()
		{
			;
		}

		// Operator overloads
		inline TVector2D& operator =(const TVector2D &vect)
		{
			X = vect.X;
			Y = vect.Y;
			return *this;
		}

		inline TVector2D& operator =(const GLfloat *vect)
		{
			X = vect[0];
			Y = vect[1];
			return *this;
		}

		inline TVector2D& operator =(const std::initializer_list<GLfloat> list)
		{
			int count = 0;
			for (auto element : list)
			{
				array[count] = element;
				if (++count > 1)
					break;
			}
			return *this;
		}

		inline bool operator ==(const TVector2D &vect) const
		{
			return (fabs(X - vect.X) + fabs(Y - vect.Y)) < Epsilon;
		}

		// Unaire
		inline TVector2D& operator -=(const TVector2D &vect)
		{
			X -= vect.X;
			Y -= vect.Y;
			return *this;
		}

		inline TVector2D& operator -=(const GLfloat val)
		{
			X -= val;
			Y -= val;
			return *this;
		}

		inline TVector2D& operator +=(const TVector2D &vect)
		{
			X += vect.X;
			Y += vect.Y;
			return *this;
		}

		inline TVector2D& operator +=(const GLfloat val)
		{
			X += val;
			Y += val;
			return *this;
		}

		inline TVector2D& operator *=(const GLfloat val)
		{
			X *= val;
			Y *= val;
			return *this;
		}

		inline TVector2D& operator /=(const GLfloat val)
		{
			GLfloat inv = 1.0f / val;
			X *= inv;
			Y *= inv;
			return *this;
		}

		// Binaire
		inline TVector2D operator -(void)
		{
			return TVector2D(-X, -Y);
		}

		inline TVector2D operator -(const TVector2D &vect) const
		{
			return TVector2D(X - vect.X, Y - vect.Y);
		}

		inline TVector2D operator +(const TVector2D &vect) const
		{
			return TVector2D(X + vect.X, Y + vect.Y);
		}

		inline TVector2D operator *(const GLfloat val) const
		{
			return TVector2D(X * val, Y * val);
		}

		inline TVector2D operator /(const GLfloat val) const
		{
			GLfloat inv = 1.0f / val;
			return TVector2D(X * inv, Y * inv);
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

		inline void TranslateTo(GLfloat x, GLfloat y)
		{
			X = x;
			Y = y;
		}

		// Other functions

		/// Vector Length
		inline GLfloat Length(void)
		{
			return sqrtf(X * X + Y * Y);
		}

		/// Test if vector is nul
		inline bool IsNull(void) const
		{
			return ((fabs(X) + fabs(Y)) < Epsilon);
		}

		/// Distance to a point
		inline GLfloat Distance(const TVector2D &vect)
		{
			return (*this - vect).Length();
		}

		/// Normalize
		inline TVector2D& Normalize()
		{
			GLfloat norm = X * X + Y * Y;
			if (fabs(norm) < Epsilon)
				*this = TVector2D(0, 0);
			else
				*this /= sqrtf(norm);
			return *this;
		}

		/// Simple print function
		inline void Print(void) const
		{
			printf("(%f, %f)\n", X, Y);
			fflush(stdout);
		}
} TVector2D;

/**
 * Some test operation
 */
inline void Vector2D_Test(void)
{
	TVector2D a;
	// TVector2D b(1,2,3);
	TVector2D b = TVector2D(1, 2);

	TVector2D c, e;

	GLfloat d;

	printf("\n******** Vector 2D Test ***********\n");
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

	d = b.Distance(TVector2D(1, 2));
	printf("d = %f\r\n", d);

	e = TVector2D(1, 1);
	e.Normalize();
	e.Print();

//  GLfloat tab[] = {0};
	TVector2D g( {1, 2});
	g.Print();

	g = {2, 3};
	g.Print();

	fflush(stdout);
}

} // namespace GLScene

using namespace GLScene;

//---------------------------------------------------------------------------
#endif // _VECTOR_2D_H__
