#include <wx/wx.h>
#include "GLSceneDialog.h"

#ifndef RUN_SAMPLE
#include "Axis.h"
#include "Sphere.h"
#endif

/* ================================================================== */
/*                          Main dialog windows                       */
/* ================================================================== */

GLSceneDialog::GLSceneDialog(wxWindow *parent, wxWindowID id, const wxString &title,
		const wxPoint &pos, const wxSize &size, long WXUNUSED(style)) :
		wxDialog(parent, id, title, pos, size, wxDEFAULT_FRAME_STYLE)
{
	// Set dialog size
	if (size == wxDefaultSize)
	  SetSize(wxSize(400, 300));
	else
		SetSize(size);

	// Set title
	if (title.IsEmpty())
		SetTitle(wxT("GLScene dialog"));
	else
		SetTitle(title);

	// Set Icon
	wxIcon _icon;
	_icon.CopyFromBitmap(wxBitmap(wxT(".\\GLScene.ico"), wxBITMAP_TYPE_ANY));
	SetIcon(_icon);

	// Create and test GLAttributes
	bool success;
	wxGLAttributes glAttrs = GLScene::GetGLAttributes(&success);
	if (!success)
	{
		// Exit
		return;
	}

	sizerMain = new wxBoxSizer(wxVERTICAL);
#ifdef RUN_SAMPLE
	winGLScene = new wxGLScene(this, glAttrs);
#else
	winGLScene = new myScene(this, glAttrs);
#endif
	sizerMain->Add(winGLScene, 1, wxEXPAND, 0);
	pControl = new wxPanel(this, wxID_ANY);
	pControl->SetMinSize(wxSize(-1, 30));
	pControl->SetBackgroundColour(wxColour(196, 251, 255));
	sizerMain->Add(pControl, 0, wxEXPAND, 0);
	wxBoxSizer *sizerInfo = new wxBoxSizer(wxVERTICAL);
	wxStaticText *lInfo = new wxStaticText(pControl, wxID_ANY, wxT("Info"));
	sizerInfo->Add(lInfo, 0, wxALL | wxEXPAND, 5);

	pControl->SetSizer(sizerInfo);
	SetSizer(sizerMain);
	Layout();
	Centre();

	// Very important to show the frame BEFORE initGL especially for Linux
	Show(true);

	winGLScene->InitGL(true);
	winGLScene->Create_GLScene();
	winGLScene->SetInfoClick(lInfo);

	winGLScene->Get_GLInfo();
//	winGLScene->SetPolygonMode(true);

#ifdef RUN_SAMPLE
	winGLScene->StartTimer(500);
#endif
}

GLSceneDialog::~GLSceneDialog()
{
	// Not need because "this" is the owner of winGLScene
//	delete winGLScene;
}

#ifndef RUN_SAMPLE

/* ================================================================== */
/*                          User scene implementation                 */
/* ================================================================== */

// Sphere at random position
void myScene::Create_GLScene(void)
{
  TVector3D p;

  // Clear existing object
  ClearObject3D();

  // Restaure initial position
  InitialView();

  // Axis
  TAxis *axis = new TAxis(5);
  AddObject3D(axis);

  // Sphere at random position, random color
  for (int i=0; i<200; i++)
  {
  	p = {10.0f * GLRand(), 10.0f * GLRand(), 10.0f * GLRand()};
  	GLfloat r = 0.5; // Sphere
//  	TVector3D r = TVector3D(GLRand(2.0), GLRand(2.0), GLRand(2.0)); // Ellipse
  	TSphere *sphere = new TSphere(r, 36, 18, p);
  	sphere->GetMaterial().SetColor(mfFront, msDiffuse, RandomColor());
  	AddObject3D(sphere);
  }
}

void myScene::OnUserMenu(wxCommandEvent &event)
{
  int i;
  TVector3D p;
  TObject3D *obj;

  if (event.GetId() == userMenuID)
  {
		for (i=0; i<ObjectCount; i++)
		{
			if (i % 2)
			{
				obj = GetObject3D(i);
				obj->Visible = !obj->Visible;
			}
		}
		Refresh(false);
  }
}

#endif
