#include "Object3D.h"

#ifdef USE_VBO

#ifndef USE_GLEW

#ifdef _WIN32

PFNGLGENBUFFERSPROC glGenBuffers = NULL;
PFNGLBINDBUFFERPROC glBindBuffer = NULL;
PFNGLBUFFERDATAPROC glBufferData = NULL;
PFNGLBUFFERSUBDATAPROC glBufferSubData = NULL;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = NULL;
PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv = NULL;
PFNGLACTIVETEXTUREPROC glActiveTexture = NULL;

void initOGL()
{
	if (glGenBuffers == NULL)
	{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
		glGenBuffers = (PFNGLGENBUFFERSPROC) wglGetProcAddress("glGenBuffers");
		glBindBuffer = (PFNGLBINDBUFFERPROC) wglGetProcAddress("glBindBuffer");
		glBufferData = (PFNGLBUFFERDATAPROC) wglGetProcAddress("glBufferData");
		glBufferSubData = (PFNGLBUFFERSUBDATAPROC) wglGetProcAddress("glBufferSubData");
		glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) wglGetProcAddress("glDeleteBuffers");
		glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC) wglGetProcAddress("glGetBufferParameteriv");
		glActiveTexture = (PFNGLACTIVETEXTUREPROC) wglGetProcAddress("glActiveTexture");
#pragma GCC diagnostic pop
	}
}
#else

// Ne pas oublier le define GL_GLEXT_PROTOTYPES dans le projet
void initOGL()
{
  // Nothing to do, Linux load library for us
}
#endif // _WIN32

#else
void initOGL()
{
  // Nothing to do, glew make initialization for us
}
#endif // USE_GLEW

GLuint createVBO(const void *data, int dataSize, GLenum target, GLenum usage)
{
	GLuint id = 0;  // 0 is reserved, glGenBuffersARB() will return non-zero id if success

	glGenBuffers(1, &id);                           // create a vbo
	glBindBuffer(target, id);                       // activate vbo id to use
	glBufferData(target, dataSize, data, usage);    // upload data to video card

	// check data size in VBO is same as input array, if not return 0 and delete VBO
	int bufferSize = 0;
	glGetBufferParameteriv(target, GL_BUFFER_SIZE, &bufferSize);
	if (dataSize != bufferSize)
	{
		glDeleteBuffers(1, &id);
		id = 0;
		std::cout << "[createVBO()] Data size is mismatch with input array\n";
	}
	else
		glBindBuffer(target, 0);

	// return VBO id
	return id;
}

#endif // USE_VBO

const TVector3D GLScene::GLDefaultPosition(0.0f, 0.0f, 0.0f);

/**
 * Constructor
 * pos : position
 * rot : rotation
 * sca : scale
 */
TObject3D::TObject3D(TVector3D *pos, TVector3D *rot, TVector3D *sca)
{
	defaultSettings();
	if (pos) SetPosition(pos);
	if (rot) SetRotation(rot);
	if (sca) SetScale(sca);
}

TObject3D::TObject3D(const TVector3D &pos)
{
	defaultSettings();
	position = pos;
}

TObject3D::~TObject3D()
{

}

void TObject3D::defaultSettings()
{
	ObjectType = otNone;
	// By default, you must freeing manualy the object
	AutoFree = false;
	// Position to center world
	position = { 0.0, 0.0, 0.0 };
	// Direction : Z axis
	direction = { 1.0, 0.0, 0.0 };
	// No axis rotation
	rotation = { 0.0, 0.0, 0.0 };
	// No scale
	scale = { 1.0, 1.0, 1.0 };

	UserPointer = NULL;
	Tag = 0;

	Lighted = true;
	Visible = true;
	Changed = true;

	levelOfDetail = 12;
	selected = false;
}

// SETTERS
//-------------------------------------------------

void TObject3D::SetPosition(std::initializer_list<GLfloat> list, bool normalize)
{
	int count = 0;
	for (auto element : list)
	{
		position.Set(count, element);
		++count;
	}
	ComputeParameters(normalize);
	Changed = true;
}

void TObject3D::SetDirection(std::initializer_list<GLfloat> list, bool normalize)
{
	int count = 0;
	for (auto element : list)
	{
		direction.Set(count, element);
		++count;
	}
	ComputeParameters(normalize);
	Changed = true;
}

void TObject3D::SetRotation(TVector3D *rot)
{
	rotation = *rot;
	Changed = true;
}

void TObject3D::SetScale(TVector3D *sca)
{
	scale = *sca;
	Changed = true;
}

// set level of detail
void TObject3D::SetLevelOfDetail(int detail)
{
	if ((detail != levelOfDetail) && (detail > 0) && (detail < 50))
	{
		levelOfDetail = detail;
		Changed = true;
	}
}

void TObject3D::ChangeFrontEmission(GLfloat val)
{
	material.SetColor(mfFront, msEmission, val);
}

// DRAW
//-------------------------------------------------
void TObject3D::Display(int id, TDisplayMode mode)
{
	if (!Visible) return;

	glPushMatrix();

	// translation
	if (DoPosition) glTranslatef(position.X, position.Y, position.Z);

	// rotation
	if (DoRotation)
	{
		glRotatef(rotation.X, 1.0, 0.0, 0.0);
		glRotatef(rotation.Y, 0.0, 1.0, 0.0);
		glRotatef(rotation.Z, 0.0, 0.0, 1.0);
	}

	// scale
	if (DoScale) glScalef(scale.X, scale.Y, scale.Z);

	if (DoUseMaterial && (mode == dmRender))
	{
		material.ApplyMaterial();
	}

	// Set a name in select mode
	if (mode == dmSelect)
	  glLoadName(id);
	DoDisplay(mode);
	glPopMatrix();
}

/**
 * Move the center of the object to the 'position' and make the rotation to
 * be in 'direction' for drawing an object with 'len' size in the direction
 * The original object is over the 'z_direction'
 * Don't forget glPushMatrix(); before call and glPopMatrix(); after draw object
 */
//void TObject3D::PrepareRotate(GLfloat len)
//{
//  TVector3D a, b; // (the two points you want to draw between : low and high)
//  TVector3D p, t;
//  TVector3D d = direction;
//  d.Normalize();
//  d *= (len/2.0);
//
//  a = position - d;
//  b = position + d;
//
//  // Get diff between two points you want cylinder along
//  p = (b - a);
//
//  // Get CROSS product (the axis of rotation)
//  t = z_direction.Vectorial(p);
//
//  // Get angle. LENGTH is magnitude of the vector
//  GLfloat angle = DEGtoRAD * acos(z_direction.Dot(p) / p.Length());
//
//  glTranslated(a.X, a.Y, a.Z);
//  glRotated(angle, t.X, t.Y, t.Z);
//}
void TObject3D::PrepareRotate(GLfloat len)
{
	TVector3D a, p, t;
	TVector3D d = direction;
	d.Normalize();

	p = d * len;
	d *= (len / 2.0);
	a = position - d;
	t = { -p.Y, p.X, 0.0 };
	GLfloat angle = RADtoDEG * acos(p.Z / len);

	glTranslated(a.X, a.Y, a.Z);
	glRotated(angle, t.X, t.Y, t.Z);
}

// ********************************************************************************
// Random functions
// ********************************************************************************

/**
 * Return a random number between [-1, 1]
 */
GLfloat GLScene::GLRand(void)
{
	return (GLfloat) ((1.0 * rand() / RAND_MAX - 0.5) * 2.0);
}

/**
 * Return a random number between [0, max]
 */
GLfloat GLScene::GLRand(GLfloat max)
{
	return (GLfloat)(rand())/(RAND_MAX*1.0) * max;
}

TVector4D GLScene::RandomColor(void)
{
	return TVector4D(GLRand(1.0), GLRand(1.0), GLRand(1.0), 1.0);
}

// ********************************************************************************
// End of file
// ********************************************************************************
