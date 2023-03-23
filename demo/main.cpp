#include <wx/wx.h>
#include <wx/image.h>
#include "GLSceneDialog.h"

class MyApp: public wxApp
{
	public:
		virtual bool OnInit() wxOVERRIDE;
};

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
  if ( !wxApp::OnInit() )
      return false;

	wxInitAllImageHandlers();
	GLSceneDialog *dialog = new GLSceneDialog(NULL, wxID_ANY, _T("wxGLScene demo"));
	dialog->ShowModal();
	dialog->Destroy();

	return true;
}
