#ifndef _CAMERA_H
#define _CAMERA_H

#include <stdio.h>
#include "Vector3D.h"

namespace GLScene
{

class TCamera
{
  public:
    TCamera();
    ~TCamera();
    GLfloat x = 5, y = 1.8, z = 4;
    GLfloat vel = 0.4;
    GLfloat tX = 0.0, tY = 1.8, tZ = 0.0;
    int mode = 0;
    void move(GLfloat angle);
    void rotate(GLfloat angle, bool horizontal);
    void draw();
};

} // namespace GLScene

//---------------------------------------------------------------------------
#endif // _CAMERA_H
