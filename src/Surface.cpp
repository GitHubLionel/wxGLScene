#include "Surface.h"
#include "Vector2D.h"
#include <cstring>

TSurface::TSurface(GLuint dimX, GLuint dimY, const TVector3D &pos) : TObject3D(pos)
{
	ObjectType = otSurface;

	sizeX = dimX;
	sizeY = dimY;
	_xMin = _xMax = _yMin = _yMax = 0.0;
	min_max = false;

	ComputeIndices();

	material.SetColor(mfFront, msAmbient, {0.1, 0.1, 0.8, 1.0});
	material.SetColor(mfFront, msDiffuse, GLColor_blue);

	material.SetColor(mfBack, msAmbient, {0.2, 0.2, 0.2, 1.0});
	material.SetColor(mfBack, msDiffuse, {0.8, 0.8, 0.8, 1.0});

	direction = {0.0, 0.0, 1.0};

	ComputeParameters();
}

TSurface::~TSurface()
{
	FreeArray();
#ifdef USE_VBO
	FreeVBO();
#endif
}

void TSurface::SetDimension(GLuint dimX, GLuint dimY, bool recompute)
{
	FreeArray();
#ifdef USE_VBO
	FreeVBO();
#endif
	sizeX = dimX;
	sizeY = dimY;

	ComputeIndices();

	if (recompute && min_max)
		InitializeSurface(_xMin, _xMax, _yMin, _yMax);
}

/**
 * Compute the indices of the vertex in triangle that map a surface sizeX x sizeY
 *
 * V(sizeY-2)*(sizeX-1)         ....    V(vsizeY-1)*(sizeX-1)
 *    .
 *    .
 *    |
 * VsizeX			VsizeX+1
 * 		|          |
 * 	 V0          V1         V2  ....   VsizeX-1
 * 	 Triangle vertex coordinates are :
 * 	 (V0, V1, VsizeX), (V1, VsizeX+1, VsizeX), (V1, V2, VsizeX+1), ....
 */
void TSurface::ComputeIndices(void)
{
	sizeLength = sizeX * sizeY;
	indiceLength = 6 * (sizeX - 1) * (sizeY - 1);
	SurfaceComputed = false;

	DeleteAndNull(indices);
	indices = new GLuint[indiceLength];

	GLuint k = 0;
	for (GLuint j = 0; j < sizeY - 1; j++)
	{
		for (GLuint i = 0; i < sizeX - 1; i++)
		{
			indices[k++] = j * sizeX + i;
			indices[k++] = j * sizeX + i + 1;
			indices[k++] = (j + 1) * sizeX + i;
			indices[k++] = j * sizeX + i + 1;
			indices[k++] = (j + 1) * sizeX + i + 1;
			indices[k++] = (j + 1) * sizeX + i;
		}
	}
}

void TSurface::InitializeArray(void)
{
#ifdef USE_VBO
	initOGL();
	FreeVBO();
	vboId = createVBO(surface, SIZE_FLOAT3D(sizeLength));
	nboId = createVBO(normals, SIZE_FLOAT3D(sizeLength));
	iboId = createVBO(indices, SIZE_UINT(indiceLength), GL_ELEMENT_ARRAY_BUFFER);
	createVBO_OK = ((vboId != 0) && (nboId != 0) && (iboId != 0));

	if (!createVBO_OK)
		std::cout << "[WARNING] VBO array not created for Surface." << std::endl;

	// Now we can delete free array
	FreeArray();
#endif
}

void TSurface::FreeArray(void)
{
	DeleteAndNull(surface);
	DeleteAndNull(normals);
	DeleteAndNull(indices);
}

#ifdef USE_VBO
void TSurface::FreeVBO(void)
{
	DeleteAndNullVBO(1, vboId);
	DeleteAndNullVBO(1, nboId);
	DeleteAndNullVBO(1, iboId);
}
#endif

void TSurface::ComputeParameters(bool normalize)
{
	reduced_pos = direction;
	if (normalize)
		reduced_pos.Normalize();
	axe_angle = {-reduced_pos.Y, reduced_pos.X, 0.0};
	angle = RADtoDEG * acos(reduced_pos.Z / sizeX);
}

void TSurface::InitializeSurface(double xMin, double xMax, double yMin, double yMax)
{
	if (FunctionZ == NULL)
	{
		SurfaceComputed = false;
		return;
	}
	_xMin = xMin;
	_xMax = xMax;
	_yMin = yMin;
	_yMax = yMax;
	min_max = true;

	DeleteAndNull(surface);
	surface = new TVector3D[sizeLength];
	SurfaceComputed = false;

	const double stepX = (xMax - xMin) / (sizeX - 1);
	const double stepY = (yMax - yMin) / (sizeY - 1);
	double x = xMin;
	double y = yMin;

	try
	{
		int k = 0;
		for (GLuint j = 0; j < sizeY; j++)
		{
			x = xMin;
			for (GLuint i = 0; i < sizeX; i++)
			{
				surface[k++] = TVector3D(x, y, FunctionZ(x, y));
				//			std::cout << x << "\t" << y << "\t" << surface[k-1].Z << std::endl;
				x += stepX;
			}
			y += stepY;
		}

		ComputeNormals();
		InitializeArray(); // For VBO
		SurfaceComputed = true;
	} catch (...)
	{

	}
}

bool TSurface::LoadSurface(const char *filename)
{
	FILE *file = fopen(filename, "r");
	if (file == NULL)
		return false;

	DeleteAndNull(surface);
	surface = new TVector3D[sizeLength];
	SurfaceComputed = false;

	// Max line length
	char line[128];
	// Data separator : space or ; or tab
	char seps[] = " ;\t\n";
	char *token;
	// 3 data by line : x, y, z
	double data[3];
	unsigned int i = 0, k = 0;
	do
	{
		if (fgets(line, 128, file) == NULL)
			continue; // EOF

		token = strtok(line, seps);
		i = 0;
		while ((token != NULL) && (i < 3))
		{
			data[i++] = atof(token);
			token = strtok(NULL, seps);
		}

		if (i == 3)
			surface[k++] = TVector3D(data[0], data[1], data[2]);

	} while ((!feof(file)) && (k < sizeLength));

	fclose(file);

	if (k == sizeLength)
	{
		ComputeNormals();
		InitializeArray(); // For VBO
		SurfaceComputed = true;
	}

	return (k == sizeLength);
}

void TSurface::ComputeNormals(void)
{
	DeleteAndNull(normals);
	normals = new TVector3D[sizeLength];

	GLuint l = 0;
	TVector3D norm;
	do
	{
		norm = (surface[indices[l + 1]] - surface[indices[l]]) ^ (surface[indices[l + 2]] - surface[indices[l]]);
		norm.Normalize();
		normals[indices[l]] = normals[indices[l + 1]] = normals[indices[l + 2]] = norm;
		l += 6;
	} while (l < indiceLength);
	// Same norm for the last vertex
	normals[sizeLength - 1] = norm;
}

void TSurface::DoDisplay(TDisplayMode mode __attribute__((unused)))
{
	if (!SurfaceComputed)
		return;

	glTranslated(position.X, position.Y, position.Z);
	if (!axe_angle.IsNull())
		glRotated(angle, axe_angle.X, axe_angle.Y, axe_angle.Z);

	glDisable(GL_CULL_FACE);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

#ifdef USE_VBO

	if (createVBO_OK)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glVertexPointer(3, GL_FLOAT, 0, (GLvoid*) 0);

		glBindBuffer(GL_ARRAY_BUFFER, nboId);
		glNormalPointer(GL_FLOAT, 0, (GLvoid*) 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
		glDrawElements(GL_TRIANGLES, indiceLength, GL_UNSIGNED_INT, (GLvoid*) 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

#else

		glVertexPointer(3, GL_FLOAT, 0, &surface[0]);
		glNormalPointer(GL_FLOAT, 0, &normals[0]);
		glDrawElements(GL_TRIANGLES, indiceLength, GL_UNSIGNED_INT, &indices[0]);

#endif

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

// ********************************************************************************
// End of file
// ********************************************************************************
