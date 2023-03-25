#include "Material.h"
#include "Object3D.h"

TMaterial::TMaterial()
{
	SetDefaultColor();
}

TMaterial::~TMaterial()
{

}

/**
 * The default material color
 * https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glMaterial.xml
 */
void TMaterial::SetDefaultColor(void)
{
	FrontProperties.Ambient = {0.2, 0.2, 0.2, 1.0};
	FrontProperties.Diffuse = {0.8, 0.8, 0.8, 1.0};
	FrontProperties.Emission = {0.0, 0.0, 0.0, 1.0};
	FrontProperties.Specular = {0.0, 0.0, 0.0, 1.0};
	FrontProperties.Shininess = 0.0;

	BackProperties.Ambient = {0.2, 0.2, 0.2, 1.0};
	BackProperties.Diffuse = {0.8, 0.8, 0.8, 1.0};
	BackProperties.Emission = {0.0, 0.0, 0.0, 1.0};
	BackProperties.Specular = {0.0, 0.0, 0.0, 1.0};
	BackProperties.Shininess = 0.0;
}

TVector4D TMaterial::GetBackProperties(TMaterialSource source)
{
	TVector4D mat;

	switch (source)
	{
		case msAmbient:
			mat = BackProperties.Ambient;
			break;
		case msDiffuse:
			mat = BackProperties.Diffuse;
			break;
		case msEmission:
			mat = BackProperties.Emission;
			break;
		case msSpecular:
			mat = BackProperties.Specular;
			break;
	}
	return mat;
}

TVector4D TMaterial::GetFrontProperties(TMaterialSource source)
{
	TVector4D mat;

	switch (source)
	{
		case msAmbient:
			mat = FrontProperties.Ambient;
			break;
		case msDiffuse:
			mat = FrontProperties.Diffuse;
			break;
		case msEmission:
			mat = FrontProperties.Emission;
			break;
		case msSpecular:
			mat = FrontProperties.Specular;
			break;
	}
	return mat;
}
void TMaterial::SetColor(const TMaterialFace _face, const TVector4D _ambiant_diffuse, GLfloat _alpha)
{
	TGLFaceProperties *face;

	if ((_face & mfFront) == mfFront)
	{
		face = &FrontProperties;
		face->Ambient = _ambiant_diffuse;
		face->Ambient.alpha = _alpha;
		face->Diffuse = _ambiant_diffuse;
		face->Diffuse.alpha = _alpha;
	}
	if ((_face & mfBack) == mfBack)
	{
		face = &BackProperties;
		face->Ambient = _ambiant_diffuse;
		face->Ambient.alpha = _alpha;
		face->Diffuse = _ambiant_diffuse;
		face->Diffuse.alpha = _alpha;
	}
}

void TMaterial::SetColor(const TMaterialFace _face, const TMaterialSource _mat, const TVector4D _color)
{
	switch (_mat)
	{
		case msAmbient:
		{
			if ((_face & mfFront) == mfFront)
				FrontProperties.Ambient = _color;
			if ((_face & mfBack) == mfBack)
				BackProperties.Ambient = _color;
			break;
		}
		case msDiffuse:
		{
			if ((_face & mfFront) == mfFront)
				FrontProperties.Diffuse = _color;
			if ((_face & mfBack) == mfBack)
				BackProperties.Diffuse = _color;
			break;
		}
		case msEmission:
		{
			if ((_face & mfFront) == mfFront)
				FrontProperties.Emission = _color;
			if ((_face & mfBack) == mfBack)
				BackProperties.Emission = _color;
			break;
		}
		case msSpecular:
		{
			if ((_face & mfFront) == mfFront)
				FrontProperties.Specular = _color;
			if ((_face & mfBack) == mfBack)
				BackProperties.Specular = _color;
			break;
		}
	}
}

void TMaterial::SetColor(const TMaterialFace _face, const TMaterialSource _mat, const GLfloat _inc)
{
	switch (_mat)
	{
		case msAmbient:
		{
			if ((_face & mfFront) == mfFront)
				FrontProperties.Ambient += _inc;
			if ((_face & mfBack) == mfBack)
				BackProperties.Ambient += _inc;
			break;
		}
		case msDiffuse:
		{
			if ((_face & mfFront) == mfFront)
				FrontProperties.Diffuse += _inc;
			if ((_face & mfBack) == mfBack)
				BackProperties.Diffuse += _inc;
			break;
		}
		case msEmission:
		{
			if ((_face & mfFront) == mfFront)
				FrontProperties.Emission += _inc;
			if ((_face & mfBack) == mfBack)
				BackProperties.Emission += _inc;
			break;
		}
		case msSpecular:
		{
			if ((_face & mfFront) == mfFront)
				FrontProperties.Specular += _inc;
			if ((_face & mfBack) == mfBack)
				BackProperties.Specular += _inc;
			break;
		}
	}
}

void TMaterial::SetAlpha(const TMaterialFace _face, const TMaterialSource _mat, const GLfloat _alpha)
{
	switch (_mat)
	{
		case msAmbient:
		{
			if ((_face & mfFront) == mfFront)
				FrontProperties.Ambient.Alpha = _alpha;
			if ((_face & mfBack) == mfBack)
				BackProperties.Ambient.Alpha = _alpha;
			break;
		}
		case msDiffuse:
		{
			if ((_face & mfFront) == mfFront)
				FrontProperties.Diffuse.Alpha = _alpha;
			if ((_face & mfBack) == mfBack)
				BackProperties.Diffuse.Alpha = _alpha;
			break;
		}
		case msEmission:
		{
			if ((_face & mfFront) == mfFront)
				FrontProperties.Emission.Alpha = _alpha;
			if ((_face & mfBack) == mfBack)
				BackProperties.Emission.Alpha = _alpha;
			break;
		}
		case msSpecular:
		{
			if ((_face & mfFront) == mfFront)
				FrontProperties.Specular.Alpha = _alpha;
			if ((_face & mfBack) == mfBack)
				BackProperties.Specular.Alpha = _alpha;
			break;
		}
	}
}

void TMaterial::ApplyMaterial(void)
{
	glMaterialfv(GL_BACK, GL_AMBIENT, BackProperties.Ambient.Array());
	glMaterialfv(GL_BACK, GL_DIFFUSE, BackProperties.Diffuse.Array());
	glMaterialfv(GL_BACK, GL_EMISSION, BackProperties.Emission.Array());
	glMaterialfv(GL_BACK, GL_SPECULAR, BackProperties.Specular.Array());
	glMaterialfv(GL_BACK, GL_SHININESS, &BackProperties.Shininess);

	glMaterialfv(GL_FRONT, GL_AMBIENT, FrontProperties.Ambient.Array());
	glMaterialfv(GL_FRONT, GL_DIFFUSE, FrontProperties.Diffuse.Array());
	glMaterialfv(GL_FRONT, GL_EMISSION, FrontProperties.Emission.Array());
	glMaterialfv(GL_FRONT, GL_SPECULAR, FrontProperties.Specular.Array());
	glMaterialfv(GL_FRONT, GL_SHININESS, &FrontProperties.Shininess);
}

TVector4D GLScene::RandomColor(void)
{
	return TVector4D(GLRand(1.0), GLRand(1.0), GLRand(1.0), 1.0);
}

// ********************************************************************************
// End of file
// ********************************************************************************
