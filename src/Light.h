#ifndef _LIGHT_H
#define _LIGHT_H

#include <stdio.h>
#include "Object3D.h"

namespace GLScene
{

#define CUTOFF 10
#define EXPO 8

// Light style : Omni, Spot
typedef enum
{
  Omni,
  Spot
} TLightStyle;

// Color
typedef enum
{
  lcAmbient,
  lcDiffuse,
  lcSpecular,
  lcModelAmbient
} TLightColor;

extern const TVector3D GLDefaultLightPosition;

/**
 * TLight class
 * By default, GL_LIGHT0 is used
 */
class TLight : public TObject3D
{
  public:
		TLight(GLenum _light = GL_LIGHT0, TLightStyle _style = Omni, const TVector3D &pos = GLDefaultLightPosition);
    virtual ~TLight() { ; }

    // Attenuation
    GLfloat const_at;
    GLfloat lin_at;
    GLfloat quad_at;

    // Spot
    TVector3D spot_direction;
    GLfloat spot_expo;
    GLfloat spot_cutoff;

    TVector4D model_ambient;
    TVector4D local_view;

    // Ligth render
    void Render();

    void Disable();
    void Enable();

    // Setter
    void SetStyle(TLightStyle _style);
    virtual void SetDefaultColor();
    void SetLightColor(const TLightColor _light, const TVector4D _color);
    void SetLightColor(const TLightColor _light, const GLfloat _inc);

    // To draw something to the light position
    void DoDisplay(TDisplayMode mode = dmRender);

  private:
    GLenum num_light;
    TLightStyle style;

    TVector4D Ambient;
		TVector4D Diffuse;
		TVector4D Emission;
		TVector4D Specular;

    bool enabled;
    bool CheckIncrement(GLfloat x, GLfloat _inc);
};

} // namespace GLScene

//---------------------------------------------------------------------------
#endif // _LIGHT_H
