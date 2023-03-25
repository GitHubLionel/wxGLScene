#include "Grid.h"

/**
 * TGrid class
 * Params: _size
 */
TGrid::TGrid(TVector3D _size, bool centered) : TObject3D()
{
	ObjectType = otGrid;
	size = _size;
	ComputeCenterGrid();

	material.SetColor(mfFront, msAmbient, {0.0, 0.0, 0.0, 1.0});
	material.SetColor(mfFront, msDiffuse, {0.5, 0.5, 1.0, 1.0});

	gridX = gridY = gridZ = true;
	planZ = -1;
	Node = false;
	SetCenterGrid(centered);
	SetGridStep(1);
}

TGrid::~TGrid()
{

}

void TGrid::ComputeCenterGrid(void)
{
	Center.X = (size.X - 1.0) / 2.0;
	Center.Y = (size.Y - 1.0) / 2.0;
	Center.Z = (size.Z - 1.0) / 2.0;
}

void TGrid::SetGridStep(int step)
{
	GridStep = step;
	sizeX = ((int) size.X / GridStep) * GridStep;
	sizeY = ((int) size.Y / GridStep) * GridStep;
	sizeZ = ((int) size.Z / GridStep) * GridStep;
}

void TGrid::SetCenterGrid(bool center)
{
	Centered = center;
	CenterX = ((GLfloat) size.X / 2.0);
	CenterY = ((GLfloat) size.Y / 2.0);
	CenterZ = ((GLfloat) size.Z / 2.0);
}

void TGrid::DoDisplay(TDisplayMode mode)
{
	// Don't draw grid in select mode
	if (mode == dmSelect)
		return;

	int i, j, k;

	if (Centered)
		glTranslatef(-CenterX, -CenterY, -CenterZ);

	glLineWidth(1.0f);
	glBegin(GL_LINES);
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colorBlack);
		for (k = 0; k <= size.Z; k += GridStep)
		{
			if ((planZ != -1) && (k != planZ))
				continue;

			if (gridY)
				for (i = 0; i <= size.X; i += GridStep)
				{
					glVertex3d(i, 0, k);
					glVertex3d(i, sizeY, k);
				}

			if (gridX)
				for (j = 0; j <= size.Y; j += GridStep)
				{
					glVertex3d(0, j, k);
					glVertex3d(sizeX, j, k);
				}
		}

		if (gridZ && (planZ == -1))
		{
			for (i = 0; i <= size.X; i += GridStep)
			{
				for (j = 0; j <= size.Y; j += GridStep)
				{
					glVertex3d(i, j, 0);
					glVertex3d(i, j, sizeZ);
				}
			}
		}
	}
	glEnd();

	if (Node)
	{
		for (i = 0; i <= size.X; i += GridStep)
			for (j = 0; j <= size.Y; j += GridStep)
				for (k = 0; k <= size.Z; k += GridStep)
				{
					glPushMatrix();
					glTranslatef(i, j, k);
//					glutSolidSphere(0.08, 10, 10); // Low resolution
					glPopMatrix();
				}
	}
}

// ********************************************************************************
// End of file
// ********************************************************************************
