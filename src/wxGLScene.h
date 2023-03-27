#ifndef _WX_GLSCENE_H_
#define _WX_GLSCENE_H_

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_GLCANVAS
    #error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

#include "Object3D.h" // IMPORTANT, must be declared before glcanvas !
#include "wx/glcanvas.h"
#include <initializer_list> // for std::initializer_list
#include <vector>  // for std::vector

#include "Light.h"
#include "Camera.h"

#ifdef USE_FFMPEG
#include "ffmpeg_encoder.h"
#endif

/**
 * GLScene is a small OpenGL implementation to create some simples objects.
 * It use wxGLCanvas from wxWidgets to draw 3D scene.
 */
namespace GLScene
{

#define MAX_OBJECT 20000

typedef enum __gl_Projection
{
	glpFrustum,
	glpPerspective,
	glpOrtho,
	glpLookAt
} glProjection;

typedef std::vector<TObject3D*> TObject3DVector;

class wxGLScene: public wxGLCanvas
{
	public:
		wxGLScene(wxWindow *parent, const wxGLAttributes &canvasAttrs, wxWindowID id = wxID_ANY);
		virtual ~wxGLScene();

		void InitGL(bool blending = true, TVector4D backcolor = GLColor_white);

		// Events
		void OnPaint(wxPaintEvent &event);
		void OnSize(wxSizeEvent &event);
		void OnChar(wxKeyEvent &event);

		void OnMouseLeftDown(wxMouseEvent &event);
		void OnMouseLeftUp(wxMouseEvent &event);
		void OnMouseMoved(wxMouseEvent &event);
		void OnMouseWheelMoved(wxMouseEvent &event);

		void OnMouseRightClick(wxMouseEvent &event);
		void OnMouseLeaveWindow(wxMouseEvent &event);
		void OnKeyPressed(wxKeyEvent &event);
		void OnKeyReleased(wxKeyEvent &event);

		void OnScreenShot(wxCommandEvent &event);
		void OnFullScreen(wxCommandEvent &event);
		void OnInfoGL(wxCommandEvent &event);
		virtual void OnUserMenu(wxCommandEvent &event);

		virtual void OnTimer(wxTimerEvent &event);

	private:
		wxFrame *m_parent;
		wxGLContext *m_glRC;
		wxMenu m_popmenu;
		wxStaticText *m_InfoClick;
		wxTimer m_timer;

		bool PolygonModeLine = false;
		bool IsFullScreen = false;

		TVector3D Center_Translation;
		int mouse_x = 0, mouse_y = 0;

		bool timmerRunning = false;

#ifdef USE_FFMPEG
		bool FFMpeg_OK = false;
		int FFMpeg_LastError;
#endif

	protected:
		bool IsSizeLocked = false;
		TDisplayMode display_mode;
		// For mouse move
		bool mouse_dragging = false;
		int mouse_x0 = 0, mouse_y0 = 0;
		GLfloat mouse_Xrot = 0.0, mouse_Yrot = 0.0;
		GLfloat mouse_scale = 0.0;

		TLight *light;
//    TCamera *camera;

		size_t ObjectCount;
		TObject3DVector Object3DList;

		glProjection Projection;

		/// restore initial parameters
		void InitialView(void);
		virtual void UpdateProjection(int width, int height);

		// Picking
		GLint PickedObject;
		GLint OldPickedObject;
		void DisplayMousePicking(int x, int y);
		virtual void DoPicking(int mod);
		void PickList(GLuint *buffer, GLint hits);

		virtual bool OnPicking(int mod, TObject3D *Old, TObject3D *New);
		virtual wxString GetInfoPicking(void);

	public:
		// set mode rendering to GL_LINE if true else to GL_FILL (default)
		void SetPolygonMode(bool line)
		{
			PolygonModeLine = line;
			if (line)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDisable(GL_LIGHTING);
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glEnable(GL_LIGHTING);
			}
		}

		void Zoom(const int inc = 1);
		void CenterUpDown(const GLfloat incZ);
		void UpDown(const GLfloat incY);
		void LeftRight(const GLfloat incX);

		// Set the center of the scene
		void SetCenter(const TVector3D &pos)
		{
			Center_Translation = pos;
		}

		int GetWidth() const
		{
			return GetSize().GetWidth();
		}

		int GetHeight() const
		{
			return GetSize().GetHeight();
		}

		// Object gestion
		int AddObject3D(TObject3D *obj, bool autoFree = true);
		void DeleteObject3D(const int index);
		TObject3D* GetObject3D(const int index);
		TObject3D* GetObject3D(TVector3D position, TObjectType type);
		void ClearObject3D(bool ForceFree = true);

		// Scene gestion
		void SetProjection(glProjection proj)
		{
			Projection = proj;
			Refresh();
		}
		virtual void Create_GLScene(void);
		virtual void Update_GLScene(bool Repaint = true);
		void LockSize(bool lock, bool ForceEvenSize = true);

		void SetInfoClick(wxStaticText *text)
		{
			m_InfoClick = text;
		}

		void Screenshot(void);

		wxMenu* GetPopupMenu()
		{
			return &m_popmenu;
		}

		// Creation of a menu
		// If not insert then menu is appended
		int Create_GLMenu(std::initializer_list<const wxString> menu, bool insert = true);
		// Get info
		void Get_GLInfo(bool show_EXT = false);
		wxString GetGLVersion()
		{
			return wxString::FromUTF8(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
		}
		wxString GetGLVendor()
		{
			return wxString::FromUTF8(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
		}
		wxString GetGLRenderer()
		{
			return wxString::FromUTF8(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
		}

		// Timer method
		void StartTimer(int milliseconds);
		void StopTimer(void);

		// FFMPEG encoder
#ifdef USE_FFMPEG
    bool MPEG_Initialize(const wxString &filename, const wxString &codec_name = _("mpeg2video"), int fps = 25, int bit_rate = 400);
    bool MPEG_Initialize(const wxString &filename, int codec_id = AV_CODEC_ID_MPEG2VIDEO, int fps = 25, int bit_rate = 400);
    void MPEG_Close(void);
    void MPEG_Update(void);
    wxString MPEG_LastError(void);
#endif

#ifdef USE_GLEW
    void InitGlew();
    const char *GetGlewVersion();
#endif

	wxDECLARE_NO_COPY_CLASS(wxGLScene);
};

wxGLAttributes GetGLAttributes(bool *success);

} // namespace GLScene

//---------------------------------------------------------------------------
#endif // _WX_GLSCENE_H_
