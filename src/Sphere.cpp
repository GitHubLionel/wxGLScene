#include "Sphere.h"

// Code for sphere construction inspired from :
// AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// http://www.songho.ca/opengl/gl_sphere.html

// Low resolution for picking
#define stackCount_low	5
#define sectorCount_low 10

TSphere::TSphere(GLfloat _radius, GLuint _sectorCount, GLuint _stackCount, const TVector3D &pos) : TObject3D(pos)
{
	ObjectType = otSphere;
	radius = _radius;
	sectorCount = _sectorCount;
	stackCount = _stackCount;
	IsSphere = true;

	sphereLength = (sectorCount + 1) * (stackCount + 1);
	indiceLength = 6 * (stackCount - 1) * sectorCount;
	sphereLength_low = (sectorCount_low + 1) * (stackCount_low + 1);
	indiceLength_low = 6 * (stackCount_low - 1) * sectorCount_low;
	InitializeArray();

	// Yellow sphere
	material.SetColor(mfFront, msAmbient, {0.647059, 0.164706, 0.164706, 1.0});
	material.SetColor(mfFront, msDiffuse, GLColor_yellow);

	material.SetColor(mfBack, msAmbient, {0.2, 0.2, 0.2, 1.0});
	material.SetColor(mfBack, msDiffuse, {0.8, 0.8, 0.8, 1.0});

	DoPosition = true;
}

TSphere::TSphere(TVector3D _radius, GLuint _sectorCount, GLuint _stackCount, const TVector3D &pos) :
		TSphere(1, _sectorCount, _stackCount, pos)
{
	ObjectType = otEllipsoid;
	radius3D = _radius;
	levelOfDetail = 25;
	IsSphere = false;
}

TSphere::~TSphere()
{
	FreeArray();
	FreeVBO();
}

void TSphere::InitializeArray(void)
{
	// Clean array
	FreeArray();
	FreeVBO();

	sphere = new TVertex[sphereLength];
	indices = new GLuint[indiceLength];
	CreateSphere(sectorCount, stackCount, sphere, indices);

	// Low resolution
	sphere_low = new TVertex[sphereLength_low];
	indices_low = new GLuint[indiceLength_low];
	CreateSphere(sectorCount_low, stackCount_low, sphere_low, indices_low);

#ifdef USE_VBO
	initOGL();
	vboId = createVBO(sphere, SIZE_VERTEX(sphereLength));
	iboId = createVBO(indices, SIZE_UINT(indiceLength), GL_ELEMENT_ARRAY_BUFFER);
	createVBO_OK = ((vboId != 0) && (iboId != 0));

	// Low resolution
	vboId_low = createVBO(sphere_low, SIZE_VERTEX(sphereLength_low));
	iboId_low = createVBO(indices_low, SIZE_UINT(indiceLength_low), GL_ELEMENT_ARRAY_BUFFER);
	createVBO_OK = createVBO_OK && ((vboId_low != 0) && (iboId_low != 0));

	if (!createVBO_OK)
		std::cout << "[WARNING] VBO array not created for Sphere." << std::endl;

	// Now we can delete free array
	FreeArray();
#endif
}

void TSphere::FreeArray(void)
{
	DeleteAndNull(sphere);
	DeleteAndNull(indices);
	DeleteAndNull(sphere_low);
	DeleteAndNull(indices_low);
}

void TSphere::FreeVBO(void)
{
#ifdef USE_VBO
	DeleteAndNullVBO(1, vboId);
	DeleteAndNullVBO(1, iboId);
	DeleteAndNullVBO(1, vboId_low);
	DeleteAndNullVBO(1, iboId_low);
#endif
}

void TSphere::CreateSphere(GLuint sectorCount, GLuint stackCount, TVertex *sph, GLuint *ind)
{
	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // normal

	float sectorStep = 2.0f * PI / sectorCount;
	float stackStep = PI / stackCount;
	float sectorAngle, stackAngle;

	GLuint count = 0;
	for (GLuint i = 0; i <= stackCount; ++i)
	{
		stackAngle = PI / 2.0f - i * stackStep;     // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);             // r * cos(u)
		z = radius * sinf(stackAngle);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal
		for (GLuint j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			sph[count].SetVertice(x, y, z);

			// normalized vertex normal
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			sph[count].SetNormal(nx, ny, nz);
			count++;
		}
	}

	// indices
	//  k1--k1+1
	//  |  / |
	//  | /  |
	//  k2--k2+1
	GLuint k1, k2;
	count = 0;
	for (GLuint i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1);     // beginning of current stack
		k2 = k1 + sectorCount + 1;      // beginning of next stack

		for (GLuint j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding 1st and last stacks
			if (i != 0) // k1---k2---k1+1
			{
				ind[count++] = k1;
				ind[count++] = k2;
				ind[count++] = k1 + 1;
			}

			if (i != (stackCount - 1)) // k1+1---k2---k2+1
			{
				ind[count++] = k1 + 1;
				ind[count++] = k2;
				ind[count++] = k2 + 1;
			}
		}
	}
}

void TSphere::DoDisplay(TDisplayMode mode)
{
	// Ellipsoïd case, we scale the sphere
	if (!IsSphere)
		glScalef(radius3D.X, radius3D.Y, radius3D.Z);

	// interleaved array
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	if (mode == dmSelect)
	{

#ifdef USE_VBO

		if (createVBO_OK)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vboId_low);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId_low);

			glInterleavedArrays(GL_N3F_V3F, SIZE_VERTEX(1), (GLvoid*) 0);
			glDrawElements(GL_TRIANGLES, indiceLength_low, GL_UNSIGNED_INT, (GLvoid*) 0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

#else

		glInterleavedArrays(GL_N3F_V3F, SIZE_VERTEX(1), &sphere_low[0]);
		glDrawElements(GL_TRIANGLES, indiceLength_low, GL_UNSIGNED_INT, &indices_low[0]);

#endif

	}
	else
	{

#ifdef USE_VBO

		if (createVBO_OK)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vboId);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);

			glInterleavedArrays(GL_N3F_V3F, SIZE_VERTEX(1), (GLvoid*) 0);
			glDrawElements(GL_TRIANGLES, indiceLength, GL_UNSIGNED_INT, (GLvoid*) 0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

#else

		glInterleavedArrays(GL_N3F_V3F, SIZE_VERTEX(1), &sphere[0]);
		glDrawElements(GL_TRIANGLES, indiceLength, GL_UNSIGNED_INT, &indices[0]);

#endif

	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

// ********************************************************************************
// End of file
// ********************************************************************************
