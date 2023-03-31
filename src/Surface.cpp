#include "Surface.h"
#include "Vector2D.h"
#include <cstring>
#include "GLColor.h"

#define SETMINMAX(v)	{ \
	if ((v) > Maximum) Maximum = (v); \
	else if ((v) < Minimum) Minimum = (v);}

TSurface::TSurface(GLuint dimX, GLuint dimY, bool useMatColor, const TVector3D &pos) : TObject3D(pos)
{
	ObjectType = otSurface;

	sizeX = dimX;
	sizeY = dimY;
	_xMin = _xMax = _yMin = _yMax = 0.0;
	min_max = false;
	useNormal = false;

	ComputeIndices();

	material.SetColor(mfFront, msAmbient, {0.1, 0.1, 0.8, 1.0});
	material.SetColor(mfFront, msDiffuse, GLColor_blue);

	material.SetColor(mfBack, msAmbient, {0.2, 0.2, 0.2, 1.0});
	material.SetColor(mfBack, msDiffuse, {0.8, 0.8, 0.8, 1.0});

	colorBegin = GLColor_blue;
	colorEnd = GLColor_red;
	colorDelta = colorEnd - colorBegin;
	colorUpdated = false;
	useColor = !useMatColor;

	direction = {0.0, 0.0, 1.0};

	SetUseMaterial(useMatColor);

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

void TSurface::SetColor(const TVector4D &begin, const TVector4D &end)
{
	material.SetColor(mfFront, msAmbient, begin);
	material.SetColor(mfFront, msDiffuse, begin);
	colorBegin = begin;
	useColor = !(end == TVector4D(GLColor_NULL));
	SetUseMaterial(!useColor);
	if (useColor)
	{
		colorEnd = end;
		colorDelta = colorEnd - colorBegin;
	}
	colorUpdated = false;
}

void TSurface::SetNormal(bool use)
{
	useNormal = use;
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
	cboId = createVBO(colors, SIZE_FLOAT3D(sizeLength));
	iboId = createVBO(indices, SIZE_UINT(indiceLength), GL_ELEMENT_ARRAY_BUFFER);
	createVBO_OK = ((vboId != 0) && (nboId != 0) && (cboId != 0) && (iboId != 0));

	if (!createVBO_OK)
		std::cout << "[WARNING] VBO array not created for Surface." << std::endl;

	// Now we can delete indices, normals and colors array but NOT surface !
	DeleteAndNull(normals);
	DeleteAndNull(colors);
	DeleteAndNull(indices);
#endif
}

void TSurface::FreeArray(void)
{
	DeleteAndNull(surface);
	DeleteAndNull(normals);
	DeleteAndNull(colors);
	DeleteAndNull(indices);
}

#ifdef USE_VBO
void TSurface::FreeVBO(void)
{
	DeleteAndNullVBO(1, vboId);
	DeleteAndNullVBO(1, nboId);
	DeleteAndNullVBO(1, cboId);
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
	double z;

	Minimum = Maximum = 0.0;
	try
	{
		int k = 0;
		for (GLuint j = 0; j < sizeY; j++)
		{
			x = xMin;
			for (GLuint i = 0; i < sizeX; i++)
			{
				z = FunctionZ(x, y);
				SETMINMAX(z);
				surface[k++] = TVector3D(x, y, z);
				//			std::cout << x << "\t" << y << "\t" << surface[k-1].Z << std::endl;
				x += stepX;
			}
			y += stepY;
		}

		ComputeNormals();
		ComputeColors();
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
	Minimum = Maximum = 0.0;

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
		{
			SETMINMAX(data[2]);
			surface[k++] = TVector3D(data[0], data[1], data[2]);
		}

	} while ((!feof(file)) && (k < sizeLength));

	// try read one more line to verify eof
	fgets(line, 128, file);
	// Ok if eof is reached and we have sizeLength = dimX*dimY datas
	bool success = ((feof(file)) && (k == sizeLength));

	fclose(file);

	if (success)
	{
		ComputeNormals();
		ComputeColors();
		InitializeArray(); // For VBO
		SurfaceComputed = true;
	}
	else
		DeleteAndNull(surface);

	return success;
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

TVector3D TSurface::CreateColor(double Value)
{
	double Factor = ((Value - Minimum)  / (Maximum - Minimum));

	return TVector3D(colorBegin.R + (Factor * colorDelta.R),
			colorBegin.G + (Factor * colorDelta.G),
			colorBegin.B + (Factor *colorDelta.B));
}

void TSurface::ComputeColors(void)
{
	DeleteAndNull(colors);
	colors = new TVector3D[sizeLength];

	for (GLuint i = 0; i < sizeLength; i++)
	{
		colors[i] = CreateColor(surface[i].Z);
//		std::cout << colors[i].X << "\t" << colors[i].Y << "\t" << colors[i].Z << std::endl;
	}
	colorUpdated = true;
}

void TSurface::DoDisplay(TDisplayMode mode __attribute__((unused)))
{
	if (!SurfaceComputed)
		return;

	if (useColor)
	{
		if (!colorUpdated)
		{
			ComputeColors();
#ifdef USE_VBO
			if (createVBO_OK)
			{
				updateVBO(cboId, colors, SIZE_FLOAT3D(sizeLength));
				DeleteAndNull(colors);
			}
#endif
		}
	}

	glTranslated(position.X, position.Y, position.Z);
	if (!axe_angle.IsNull())
		glRotated(angle, axe_angle.X, axe_angle.Y, axe_angle.Z);

	glDisable(GL_CULL_FACE);

	glEnableClientState(GL_VERTEX_ARRAY);
	if (useNormal)
  	glEnableClientState(GL_NORMAL_ARRAY);
	if (useColor)
	{
		glEnable(GL_COLOR_MATERIAL);
		glEnableClientState(GL_COLOR_ARRAY);
	}

#ifdef USE_VBO

	if (createVBO_OK)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glVertexPointer(3, GL_FLOAT, 0, (GLvoid*) 0);

		if (useNormal)
		{
			glBindBuffer(GL_ARRAY_BUFFER, nboId);
			glNormalPointer(GL_FLOAT, 0, (GLvoid*) 0);
		}

		if(useColor)
		{
			glBindBuffer(GL_ARRAY_BUFFER, cboId);
			glColorPointer(3, GL_FLOAT, 0, (GLvoid*) 0);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
		glDrawElements(GL_TRIANGLES, indiceLength, GL_UNSIGNED_INT, (GLvoid*) 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

#else

		glVertexPointer(3, GL_FLOAT, 0, &surface[0]);
		if (useNormal)
			glNormalPointer(GL_FLOAT, 0, &normals[0]);
		if (useColor)
			glColorPointer(3, GL_FLOAT, 0, &colors[0]);
		glDrawElements(GL_TRIANGLES, indiceLength, GL_UNSIGNED_INT, &indices[0]);

#endif

	glDisableClientState(GL_VERTEX_ARRAY);
	if (useNormal)
		glDisableClientState(GL_NORMAL_ARRAY);
	if (useColor)
		glDisableClientState(GL_COLOR_ARRAY);
}

// ********************************************************************************
// End of file
// ********************************************************************************
