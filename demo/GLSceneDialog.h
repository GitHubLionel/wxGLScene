#ifndef GLSCENEDIALOG_H
#define GLSCENEDIALOG_H

#include <wx/wx.h>
#include <wx/image.h>

#include "wxGLScene.h"

/* ================================================================== */
/*                          Main dialog windows                       */
/* ================================================================== */

class GLSceneDialog: public wxDialog
{
	public:

		GLSceneDialog(wxWindow *parent, wxWindowID id,
				const wxString &title,
				const wxPoint &pos = wxDefaultPosition,
				const wxSize &size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE);

		~GLSceneDialog();

	private:

	protected:
		wxBoxSizer *sizerMain;
		wxGLScene *winGLScene;
		wxPanel *pControl;

	public:
		wxGLScene *GetScene(void)
		{
			return winGLScene;
		}
};

/* ================================================================== */
/*                          User scene declaration                    */
/* ================================================================== */

#ifndef RUN_SAMPLE
/**
 * Exemple d'une scène héritée
 * Une scène toute simple avec juste des sphères
 */
class myScene : public wxGLScene
{
  private:
		int userMenuID;

  protected:
		virtual void OnUserMenu(wxCommandEvent &event);

  public:
		myScene(wxWindow *parent, const wxGLAttributes& canvasAttrs, wxWindowID id = wxID_ANY):
			wxGLScene(parent, canvasAttrs, id)
		{
			// add some menu
			userMenuID = Create_GLMenu({_T("Toggle sphere")});
		}
    virtual ~myScene() {}
    virtual void Create_GLScene(void);
};

#endif

#endif // GLSCENEDIALOG_H
