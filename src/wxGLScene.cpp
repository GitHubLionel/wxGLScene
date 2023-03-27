#include "wxGLScene.h"
#include <climits>
#ifdef RUN_SAMPLE
#include "Axis.h"
#include "Sphere.h"
#include "Arrow.h"
#include "Cube.h"
#include "Cylinder.h"
#include "Spin.h"
#include "Cone.h"
#endif

#if (defined( __WXMSW__) || defined( __WXGTK__) || defined(__WXMAC__))
#undef _UNICODE
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include <wx/clipbrd.h>
#endif

// If defined, minimal sample is running with only wxGLScene created
//#define RUN_SAMPLE

// Angle rotation for the arrow keyboard
#define ROTATION_ANGLE	5.0f
#define ZOOM_FACTOR_PLUS 1.1f
#define ZOOM_FACTOR_MINUS 0.9f

#define glIDMENU (wxID_HIGHEST + 1)

enum
{
	mpID_FIRST = glIDMENU,
	mpID_SCREENSHOT,       // Copy a screen shot to the clipboard
	mpID_FULLSCREEN,			 // Toggle fullscreen
	mpID_INFO,					 	 // Info about OpenGL
	mpID_USER              // Always last for user
};

/* ================================================================== */
/*                          Function to get wxGLAttributes            */
/* ================================================================== */

wxGLAttributes GLScene::GetGLAttributes(bool *success)
{
	wxGLAttributes vAttrs;

	// Defaults should be accepted
	vAttrs.PlatformDefaults().Defaults().EndList();
	*success = wxGLCanvas::IsDisplaySupported(vAttrs);

	if (!(*success))
	{
		// Try again without sample buffers
		vAttrs.Reset();
		vAttrs.PlatformDefaults().RGBA().DoubleBuffer().Depth(16).EndList();
		*success = wxGLCanvas::IsDisplaySupported(vAttrs);

		if (!(*success))
		{
			vAttrs.Reset();
			wxMessageBox(_T("Visual attributes for OpenGL are not accepted."), _T("Error with OpenGL"), wxOK | wxICON_ERROR);
		}
		else
		{
			// Second try is good, nothing to do more
		}
	}
	return vAttrs;
}

/* ================================================================== */
/*                          wxGLScene class                            */
/* ================================================================== */

wxGLScene::wxGLScene(wxWindow *parent, const wxGLAttributes &canvasAttrs, wxWindowID id) :
		wxGLCanvas(parent, canvasAttrs, id, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS)
{
	// wxWANTS_CHARS : very important for the arrow keyboard

	// Keep the parent for the fullscreen
	m_parent = (wxFrame*) parent;

	// Explicitly create a new rendering context instance for this canvas.
	m_glRC = new wxGLContext(this);

	//
	m_popmenu.Append(mpID_SCREENSHOT, _T("Screen shot"), _T("Copy a screen shot to the clipboard"));
	m_popmenu.Append(mpID_FULLSCREEN, _T("Full Screen"), _T("Toggle fullscreen"));
	m_popmenu.Append(mpID_INFO, _T("Info"), _T("Info about OpenGL"));

	ObjectCount = 0;
	PickedObject = -1;
	OldPickedObject = -1;
	display_mode = dmRender;
	Center_Translation = TVector3D();
	Projection = glpFrustum;

	InitialView();

	// The default light
	light = new TLight();
//  camera = new TCamera();

	// Bind events
	Bind(wxEVT_SIZE, &wxGLScene::OnSize, this);
	Bind(wxEVT_PAINT, &wxGLScene::OnPaint, this);
	Bind(wxEVT_CHAR, &wxGLScene::OnChar, this);

	Bind(wxEVT_LEFT_DOWN, &wxGLScene::OnMouseLeftDown, this);
	Bind(wxEVT_LEFT_UP, &wxGLScene::OnMouseLeftUp, this);
	Bind(wxEVT_MOTION, &wxGLScene::OnMouseMoved, this);
	Bind(wxEVT_RIGHT_DOWN, &wxGLScene::OnMouseRightClick, this);
	Bind(wxEVT_LEAVE_WINDOW, &wxGLScene::OnMouseLeaveWindow, this);
	Bind(wxEVT_MOUSEWHEEL, &wxGLScene::OnMouseWheelMoved, this);

	Bind(wxEVT_MENU, &wxGLScene::OnScreenShot, this, mpID_SCREENSHOT);
	Bind(wxEVT_MENU, &wxGLScene::OnFullScreen, this, mpID_FULLSCREEN);
	Bind(wxEVT_MENU, &wxGLScene::OnInfoGL, this, mpID_INFO);
}

//---------------------------------------------------------------------------
wxGLScene::~wxGLScene()
{
	if (timmerRunning)
		StopTimer();

	ClearObject3D(false);
	delete light;
//  delete camera;
	delete m_glRC;
}

/**
 * Add object to the scene and return his index
 * Return -1 if we over MAX_OBJECT
 * If autofree = true (default), the object delete itself when object is suppressed from the scene
 */
int wxGLScene::AddObject3D(TObject3D *obj, bool autoFree)
{
	if (ObjectCount == MAX_OBJECT)
	{
		fprintf(stderr, "Error: Too many object\n");
		fflush(stderr);
		return -1;
	}
	obj->AutoFree = autoFree;
	Object3DList.push_back(obj);
	ObjectCount++;
	return ObjectCount - 1;
}

//---------------------------------------------------------------------------
void wxGLScene::DeleteObject3D(const int index)
{
	TObject3D *obj = GetObject3D(index);
	if (obj != NULL)
	{
		// Free object itself if autofree
		if (obj->AutoFree)
			delete obj;
		// Remove object from list
		Object3DList.erase(Object3DList.begin() + index);
		if (PickedObject == index)
			PickedObject = -1;
		if (OldPickedObject == index)
			OldPickedObject = -1;
		ObjectCount--;
	}
}

//---------------------------------------------------------------------------
TObject3D* wxGLScene::GetObject3D(const int index)
{
	if ((index < 0) || (index >= (int)ObjectCount))
		return NULL;
	return Object3DList[(size_t)index];
}

//---------------------------------------------------------------------------
TObject3D* wxGLScene::GetObject3D(TVector3D position, TObjectType type)
{
	for (size_t i = 0; i < ObjectCount; i++)
	{
		if ((Object3DList[i]->GetObjectType() == type) && (Object3DList[i]->GetPosition() == position))
			return Object3DList[i];
	}
	return NULL;
}

//---------------------------------------------------------------------------
void wxGLScene::ClearObject3D(bool ForceFree)
{
	for (size_t i = 0; i < ObjectCount; i++)
	{
		if (ForceFree || (Object3DList[i]->AutoFree))
			delete Object3DList[i];
	}
	Object3DList.clear();
	ObjectCount = 0;
	PickedObject = -1;
	OldPickedObject = -1;
}

//---------------------------------------------------------------------------
void wxGLScene::Get_GLInfo(bool show_EXT)
{
	std::cout << "OpenGL: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "OpenGL: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;
	if (show_EXT)
		std::cout << "OpenGL: " << glGetString(GL_EXTENSIONS) << std::endl;
#ifdef USE_GLEW
  std::cout << "GLEW: Using GLEW " << GetGlewVersion() << std::endl;
#endif
}

void wxGLScene::OnInfoGL(wxCommandEvent &WXUNUSED(event))
{
	wxString str = _T("Keyboard action :\n");
	str += _T("Left - Right : rotate left, right\n");
	str += _T("Up - Down : move scene up, down\n");
	str += _T("Page Up, Page Down : zoom in, out\n");
	str += _T("Char U, D : move scene up, down\n");
	str += _T("Char L, M : increase, decrease brightness\n");
	str += _T("ESC : quit full screen\n");
	str += _T("------------------------------\n");
	// Get the GL version for the current OGL context
	wxString info = _T("Using OpenGL version: ");
	info += GetGLVersion();
	info += _T("\nVendor: ");
	info += GetGLVendor();
	info += _T("\nRenderer: ");
	info += GetGLRenderer();
	wxMessageBox(str + info, _T("OpenGL information"), wxOK | wxICON_INFORMATION, m_parent);
}

/**
 * Main function that create the scene. Must be overloaded in new wxGLScene class.
 * The GLContextWindow must be created before because we use some glut function
 * MUST BE call AFTER InitGL function
 */
void wxGLScene::Create_GLScene(void)
{
#ifdef RUN_SAMPLE
	TVector3D p(0.0, 0.0, 0.0);

	// Clear existing object
	ClearObject3D();

	// Restaure initial position
	InitialView();

	// Clean sub windows
//  CleanSubDisplay();

	// For example, add an axis and a sphere
	TAxis *axis = new TAxis(5);
	AddObject3D(axis);

	TSphere *sphere = new TSphere(0.5, 36, 18, p);
	AddObject3D(sphere);

	// Add cylinder
	TCylinder *cylinder = new TCylinder(1, 0.5, {0.0, 0.0, 1.0});
	cylinder->SetDirection( {1.0, 0.0, 0.0});
	AddObject3D(cylinder);

	// Add cone
	TCone *cone = new TCone(1, 0.3, {-1.0, 1.0, 0.0});
	cone->SetDirection( {0.0, 1.0, 0.0});
	AddObject3D(cone);

	// Add cube
	TCube *cube = new TCube(2.0, {0.0, 4.0, 0.0});
	cube->SetDirection( {0.0, 1.0, 1.0});
	AddObject3D(cube);

	// Add arrow
	TArrow *arrow = new TArrow(1, 0.05, {1.0, 0.0, 0.0});
	arrow->SetDirection( {0.0, 1.0, 0.0});
	AddObject3D(arrow);

	// Add arrow
	TArrow *arrow2 = new TArrow(5, 0.1, {2.0, -2.0, 0.0});
	arrow2->SetDirection( {1.0, 0.0, -1.0});
	AddObject3D(arrow2);

	// Add spin
	TSpin *spin = new TSpin(1, 0.05, {1.0, 1.0, 0.0});
	spin->SetDirection( {0.0, -1.0, 0.0});
	spin->GetMaterial().SetColor(mfFront, msAmbient, GLColor_green);
	spin->GetMaterial().SetColor(mfFront, msDiffuse, GLColor_green);
	AddObject3D(spin);

	// Add another spin
	TSpin *spin2 = new TSpin(1, 0.05, {0.0, 0.0, 3.0});
	spin2->SetDirection( {-1.0, 0.0, 0.0});
	spin2->GetMaterial().SetColor(mfFront, msAmbient, GLColor_yellow);
	spin2->GetMaterial().SetColor(mfFront, msDiffuse, GLColor_yellow);
	AddObject3D(spin2);

	// Add Parallelepiped
	TCube *para = new TCube( {1.0, 2.0, 3.0}, {0.0, 0.0, -2.0});
//	para->SetDirection( {0.0, 1.0, 1.0});
	AddObject3D(para);

#endif
}

/**
 * Function that update an existing scene. Must be overloaded in new wxGLScene class.
 */
void wxGLScene::Update_GLScene(bool WXUNUSED(Repaint))
{
#ifdef RUN_SAMPLE
	// For example, just move the sphere (index = 1)
	TObject3D *obj = GetObject3D(1);
	if ((obj != NULL) && (obj->GetObjectType() == otSphere))
		obj->SetPosition( {GLRand() * 5, GLRand() * 3, GLRand() * 3});

	// false because we don't need to rescale the scene (we just move a sphere)
	Refresh(false);
#endif
}

//---------------------------------------------------------------------------
void wxGLScene::OnTimer(wxTimerEvent &WXUNUSED(event))
{
#ifdef RUN_SAMPLE
	// For example, redraw the scene
	Update_GLScene();
#endif
}

void wxGLScene::StartTimer(int milliseconds)
{
	m_timer.Bind(wxEVT_TIMER, &wxGLScene::OnTimer, this);
	m_timer.Start(milliseconds);
	timmerRunning = true;
}

void wxGLScene::StopTimer(void)
{
	m_timer.Stop();
	m_timer.Unbind(wxEVT_TIMER, &wxGLScene::OnTimer, this, m_timer.GetId());
	timmerRunning = false;
}

/* ================================================================== */
/*                          Display method                            */
/* ================================================================== */

//---------------------------------------------------------------------------
// Adjust the projection you want : glFrustum, gluPerspective, glOrtho, gluLookAt, ...
void wxGLScene::UpdateProjection(int width, int height)
{
	GLdouble ar = (GLdouble) width / (GLdouble) height;

	switch (Projection)
	{
		case glpFrustum:
			glFrustum(-ar, ar, -1.0, 1.0, 10.0, 100.0);
			break;
		case glpPerspective:
			gluPerspective(45.0f, ar, 1.0, 300.0f);
			break;
		case glpOrtho:
			glOrtho(-ar, ar, -1.0, 1.0, 0.1, 1000.0);
			break;
		case glpLookAt:
			gluLookAt(0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
			break;
		default:
			;
	}
}

//---------------------------------------------------------------------------
void wxGLScene::InitialView(void)
{
	mouse_dragging = false;
	mouse_x = 0, mouse_y = 0;
	mouse_x0 = 0, mouse_y0 = 0;
	// First orientation : (1, 1, 1) en face
	mouse_Xrot = -135.0, mouse_Yrot = -45.0;
	mouse_scale = 1.0;
	Center_Translation = {0.0, 0.0, 0.0};
	display_mode = dmRender;
}

//---------------------------------------------------------------------------
void wxGLScene::LockSize(bool lock, bool ForceEvenSize)
{
	if (lock && ForceEvenSize)
	{
		int w = GetWidth();
		int h = GetHeight();
		w = (w / 2) * 2;
		h = (h / 2) * 2;
		SetSize(w, h);
		Refresh();
	}

	IsSizeLocked = lock;
}

//---------------------------------------------------------------------------
void wxGLScene::OnSize(wxSizeEvent &event)
{
	if (IsSizeLocked)
	{
	  event.Skip();
	  return;
	}

	if (!IsShownOnScreen())
		return;
	// This is normally only necessary if there is more than one wxGLCanvas
	// or more than one wxGLContext in the application.
	SetCurrent(*m_glRC);

	// It's up to the application code to update the OpenGL viewport settings.
	// This is OK here only because there is only one canvas that uses the
	// context. See the cube sample for that case that multiple canvases are
	// made current with one context.
	const wxSize size = event.GetSize() * GetContentScaleFactor();

	glViewport(0, 0, size.x, size.y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	UpdateProjection(size.x, size.y);
	Refresh(false);
}

//---------------------------------------------------------------------------
void wxGLScene::OnPaint(wxPaintEvent &WXUNUSED(event))
{
	static bool display_running = false;

	// This is a dummy, to avoid an endless succession of paint messages.
	// OnPaint handlers must always create a wxPaintDC.
	wxPaintDC dc(this);

	// This is normally only necessary if there is more than one wxGLCanvas
	// or more than one wxGLContext in the application.
	SetCurrent(*m_glRC);

	if (display_running)
		return;
	display_running = true;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	// Light if needed
	light->Render();

	//camera->draw();

//	if (redraw_view)
	{
		glLoadIdentity();
		glTranslated(0.0, 0.0, -50.0);

		glRotatef(mouse_Yrot, 1.0, 0.0, 0.0);
		glRotatef(mouse_Xrot, 0.0, 0.0, 1.0);

		glScaled(mouse_scale, mouse_scale, mouse_scale);

		glTranslatef(Center_Translation.X, Center_Translation.Y, Center_Translation.Z);
	}

	// Display objects
	if (display_mode == dmSelect)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_LIGHTING);
	}
	for (size_t i = 0; i < ObjectCount; i++)
	{
		Object3DList[i]->Display(i, display_mode);
	}

	if (display_mode == dmSelect)
	{
		// Restaure polygon mode
		if (!PolygonModeLine)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_LIGHTING);
		}
	}

	SwapBuffers();

//	redraw_view = false;
	display_running = false;
}

//---------------------------------------------------------------------------
// On picking action. Here we redraw sub window
void wxGLScene::DoPicking(int mod)
{
	bool DoDefault = true;

	// Callback to do other operations
	DoDefault = OnPicking(mod, GetObject3D(OldPickedObject), GetObject3D(PickedObject));

	// Default behaviour :
	// Reduce emission on picked object
	if (DoDefault)
	{
		if (OldPickedObject > 0)
			GetObject3D(OldPickedObject)->ChangeFrontEmission(-0.2f);
		if (PickedObject > 0)
			GetObject3D(PickedObject)->ChangeFrontEmission(0.2f);
	}

	if (m_InfoClick)
		m_InfoClick->SetLabel(GetInfoPicking());

	Refresh(false);
}

//---------------------------------------------------------------------------
wxString wxGLScene::GetInfoPicking(void)
{
	return wxString::Format(_T("Object: %d"), PickedObject);
}

//---------------------------------------------------------------------------
/**
 * Action to do when an object is picked. To be defined by user.
 * By default, we just reduce emission on picked object
 */
bool wxGLScene::OnPicking(int WXUNUSED(mod), TObject3D *WXUNUSED(Old), TObject3D *WXUNUSED(New))
{
	// Nothing to do here
	return true;
}

//---------------------------------------------------------------------------
// Special display for the picking
void wxGLScene::DisplayMousePicking(int x, int y)
{
#define BUFFER_SIZE 256
	GLuint buff[BUFFER_SIZE] = {0};
	GLint hits, view[4];

	// Choose the buffer where store the values for the selection data
	glSelectBuffer(BUFFER_SIZE, buff);

	// Get info about the viewport
	glGetIntegerv(GL_VIEWPORT, view);

	// Switching in selection mode
	glRenderMode(GL_SELECT);
	display_mode = dmSelect;

	// Clearing and init the name's stack
	glInitNames();

	// Fill the stack with one element (or glLoadName will generate an error)
	glPushName(0);

	// Modify the vieving volume and restricting selection area around the cursor
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	{
		glLoadIdentity();

		// Restrict the viewing volume
		gluPickMatrix(x, view[3] - y, 5.0, 5.0, view);
		// Set same projection as normal view
		UpdateProjection(view[2], view[3]);

		// Draw the objects onto the screen in select mode
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);

		// Display objects
		for (size_t i = 0; i < ObjectCount; i++)
		{
			Object3DList[i]->Display(i, display_mode);
		}

		// DO NOT swap buffer !!
		// Go back to projection mode
		glMatrixMode(GL_PROJECTION);
	}
	glPopMatrix();
	glFlush();

	// Get number of objects drawed in that area and return to render mode
	hits = glRenderMode(GL_RENDER);
	display_mode = dmRender;

	// Check picking object
	PickList(buff, hits);
}

//---------------------------------------------------------------------------
void wxGLScene::PickList(GLuint *buffer, GLint hits)
{
	/**
	 * For each hit in the buffer are allocated 4 bytes:
	 * 1. Number of hits selected (always one, because when we draw each object
	 * we use glLoadName, so we replace the previous name in the stack)
	 * 2. Min Z
	 * 3. Max Z
	 * 4. Name of all the hits (give by glLoadName). Here we have only one.
	 */
	GLint id_min = -1;
	if (hits > 0)
	{
		GLuint *pBuffer = &buffer[0];
		GLuint z_min = UINT_MAX;

		for (int i = 0; i < hits; i++)
		{
			if (*(++pBuffer) < z_min)
			{
				z_min = *pBuffer;
				id_min = *(pBuffer + 2);
			}
			pBuffer += 3;
		}
	}

//	std::cout << "Selected object: " << id_min << std::endl;
	OldPickedObject = PickedObject;
	PickedObject = id_min;
}

/**
 * Zoom the scene plus if inc > 0
 */
void wxGLScene::Zoom(const int inc)
{
	int i;

	if (inc >= 1)
	{
		for (i = 1; i <= inc; i++)
			mouse_scale *= ZOOM_FACTOR_PLUS;
	}
	else
		if (inc <= 1)
		{
			for (i = 1; i <= -inc; i++)
				mouse_scale *= ZOOM_FACTOR_MINUS;
		}
}

/**
 * Move the scene up if incZ > 0; down if incZ < 0
 */
void wxGLScene::CenterUpDown(const GLfloat incZ)
{
	Center_Translation.Z += incZ;
}

/**
 * Rotate the scene up if incY > 0; down if incY < 0
 */
void wxGLScene::UpDown(const GLfloat incY)
{
	mouse_Yrot -= incY;
}

/**
 * Rotate the scene right if incX > 0; left if incX < 0
 */
void wxGLScene::LeftRight(const GLfloat incX)
{
	mouse_Xrot += incX;
}

/* ================================================================== */
/*                          Events method                             */
/* ================================================================== */

void wxGLScene::OnChar(wxKeyEvent &event)
{
	switch (event.GetKeyCode())
	{
		case WXK_ESCAPE: // Escape char
			if (IsFullScreen)
			{
				IsFullScreen = !IsFullScreen;
				m_parent->ShowFullScreen(IsFullScreen);
			}
			break;

		case WXK_LEFT:
			LeftRight(-ROTATION_ANGLE);
			break;

		case WXK_RIGHT:
			LeftRight(ROTATION_ANGLE);
			break;

		case WXK_UP:
			CenterUpDown(0.5);
			break;

		case WXK_DOWN:
			CenterUpDown(-0.5);
			break;

		case WXK_PAGEUP:
			Zoom(1);
			break;

		case WXK_PAGEDOWN:
			Zoom(-1);
			break;

		case 'l':
		case 'L':
			light->SetLightColor(lcModelAmbient, 0.1f);
			break;

		case 'm':
		case 'M':
			light->SetLightColor(lcModelAmbient, -0.1f);
			break;
		case 'u':
		case 'U':
			CenterUpDown(0.5f);
			break;

		case 'd':
		case 'D':
			CenterUpDown(-0.5f);
			break;

		default:
			event.Skip();
			return;
	}
	Refresh(false);
}

//---------------------------------------------------------------------------
void wxGLScene::OnMouseLeftDown(wxMouseEvent &event)
{
	event.Skip();
	mouse_x0 = event.GetX();
	mouse_y0 = event.GetY();
//	std::cout << "Mouse button pressed at (" << mouse_x0 << ", " << mouse_y0 << ") dc " << std::endl;
}

//---------------------------------------------------------------------------
void wxGLScene::OnMouseLeftUp(wxMouseEvent &event)
{
	event.Skip();
	// We just click on an object
	if ((mouse_x0 == event.GetX()) && (mouse_y0 == event.GetY()))
	{
		DisplayMousePicking(mouse_x0, mouse_y0);
		DoPicking(event.GetModifiers());
	}
}

//---------------------------------------------------------------------------
void wxGLScene::OnMouseMoved(wxMouseEvent &event)
{
	static int dragging = 0;
	static int last_x, last_y;

	// Allow default processing to happen, or else the canvas cannot gain focus
	// (for key events).
	event.Skip();

	if (event.LeftIsDown())
	{
		if (!dragging)
		{
			dragging = 1;
		}
		else
		{
			mouse_Xrot += event.GetX() - last_x;
			mouse_Yrot += event.GetY() - last_y;
			Refresh(false);
		}
		last_x = event.GetX();
		last_y = event.GetY();
	}
	else
	{
		dragging = 0;
	}
}

//---------------------------------------------------------------------------
void wxGLScene::OnMouseLeaveWindow(wxMouseEvent &WXUNUSED(event))
{
	// not used
}

//---------------------------------------------------------------------------
void wxGLScene::OnMouseWheelMoved(wxMouseEvent &event)
{
	event.Skip();
	float factor = (event.GetWheelRotation() > 0) ? ZOOM_FACTOR_PLUS : ZOOM_FACTOR_MINUS;

	mouse_scale *= factor;
	Refresh(true);
}

//---------------------------------------------------------------------------
void wxGLScene::OnMouseRightClick(wxMouseEvent &event)
{
	PopupMenu(&m_popmenu, event.GetX(), event.GetY());
}

void wxGLScene::OnKeyPressed(wxKeyEvent &WXUNUSED(event))
{
	// not used
}

//---------------------------------------------------------------------------
void wxGLScene::OnKeyReleased(wxKeyEvent &WXUNUSED(event))
{
	// not used
}

//---------------------------------------------------------------------------
void wxGLScene::OnFullScreen(wxCommandEvent &WXUNUSED(event))
{
	IsFullScreen = !IsFullScreen;
	m_parent->ShowFullScreen(IsFullScreen);
}

// ********************************************************************************
// Screenshot
// ********************************************************************************

void wxGLScene::OnScreenShot(wxCommandEvent&WXUNUSED(event))
{
	Screenshot();
}

//---------------------------------------------------------------------------
void wxGLScene::Screenshot(void)
{
	struct viewport_params
	{
			GLint originx;
			GLint originy;
			GLint width;
			GLint height;
	} viewport;

	// Build image from the 3D buffer
//   wxWindowUpdateLocker noUpdates( this );
	glGetIntegerv( GL_VIEWPORT, (GLint*) &viewport);

	GLubyte *pixelbuffer = (GLubyte *) malloc(viewport.width * viewport.height * 3);
	GLubyte *alphabuffer = (GLubyte *) malloc(viewport.width * viewport.height);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadBuffer(GL_BACK_LEFT);
	glReadPixels(viewport.originx, viewport.originy, viewport.width, viewport.height,
			GL_RGB, GL_UNSIGNED_BYTE, pixelbuffer);
	glReadPixels(viewport.originx, viewport.originy, viewport.width, viewport.height,
			GL_ALPHA, GL_UNSIGNED_BYTE, alphabuffer);

	wxImage image(viewport.width, viewport.height);
	image.SetData(pixelbuffer);
	image.SetAlpha(alphabuffer);
	image = image.Mirror(false);
	wxBitmap bitmap(image);

	if (wxTheClipboard->Open())
	{
		wxBitmapDataObject *dobjBmp = new wxBitmapDataObject;
		dobjBmp->SetBitmap(bitmap);

		if (!wxTheClipboard->SetData(dobjBmp))
			wxMessageBox(_T("Failed to copy image to clipboard"));

		wxTheClipboard->Flush(); /* the data in clipboard will stay
		 * available after the application exits */
		wxTheClipboard->Close();
		// DO NOT free the buffer !!
	}
	else
	{
		free(pixelbuffer);
		free(alphabuffer);
	}
}

// ********************************************************************************
// OpenGL window construction
// ********************************************************************************
/**
 * Create the Context for the OpenGL Window
 * The context will be created only one time for all the life of the window.
 * Blending : if true enable GL_BLEND, default true
 * Backcolor : background color of the scene, default white
 */
void wxGLScene::InitGL(bool blending, TVector4D backcolor)
{
	// Make the new context current (activate it for use) with this canvas.
	SetCurrent(*m_glRC);

	// Initialize Glew to get information about OpenGL
#ifdef USE_GLEW
  InitGlew();
#endif

	// Enable back faces
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); // Default

	// Enable depth test, Accept fragment if it closer to the camera than the former one
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); // Default
	glClearDepth(1.0);

	// Enable blending
	if (blending)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	// Usefull ?
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH, GL_NICEST);

	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH, GL_NICEST);

	// speedups ?
	glEnable(GL_DITHER);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);

	// Default : White
	glClearColor(backcolor.R, backcolor.G, backcolor.B, backcolor.Alpha);

	// Enable light
	light->Enable();
}

/**
 * Add menu
 * Index start to mpID_USER who is the return value
 */
int wxGLScene::Create_GLMenu(std::initializer_list<const wxString> menu, bool insert)
{
	// Create menu entry
	int id = 0;
	int count = mpID_USER;
	for (auto element : menu)
	{
		if (insert)
			m_popmenu.Insert(id++, count, element, element);
		else
			m_popmenu.Append(count, element, element);
		Bind(wxEVT_MENU, &wxGLScene::OnUserMenu, this, count);
		count++;
	}
	return mpID_USER;
}

/**
 * User menu action. Should be overriden.
 */
void wxGLScene::OnUserMenu(wxCommandEvent &WXUNUSED(event))
{
	// Nothing to do here
}

/**
 * GLEW Initialization
 */
#ifdef USE_GLEW
void wxGLScene::InitGlew()
{
  static bool glewInitialized = false;
  if (!glewInitialized)
  {
    glewInitialized = true;

    glewExperimental = true;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
      // Problem: glewInit failed, something is seriously wrong !
      fprintf(stderr, "GLEW: ERROR: GLEW initialization failed! We're going to keep going anyways, but we will most likely crash.\n");
      fprintf(stderr, "GLEW: ERROR: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "GLEW: Using GLEW %s\n", glewGetString(GLEW_VERSION));
  }
}

const char *wxGLScene::GetGlewVersion()
{
  return (const char *)glewGetString(GLEW_VERSION);
}
#endif

// FFMPEG encoder
#ifdef USE_FFMPEG

/**
 * Start and define encoder by name
 * codec_name should be : "mpeg1video" or "mpeg2video" (*.mpg) or "mpeg4" (*.m4v)
 */
bool wxGLScene::MPEG_Initialize(const wxString &filename, const wxString &codec_name, int fps, int bit_rate)
{
	LockSize(true);
	FFMpeg_LastError = ffmpeg_encoder_start(filename.c_str(), codec_name, fps, bit_rate, GetWidth(), GetHeight());
	FFMpeg_OK = (FFMpeg_LastError == ffmpeg_OK);
	if (!FFMpeg_OK)
		LockSize(false);
	return FFMpeg_OK;
}

/**
 * Start and define encoder by id
 * codec_id should be : AV_CODEC_ID_MPEG1VIDEO or AV_CODEC_ID_MPEG2VIDEO (*.mpg) or AV_CODEC_ID_H264 (*.m4v)
 */
bool wxGLScene::MPEG_Initialize(const wxString &filename, int codec_id, int fps, int bit_rate)
{
	LockSize(true);
	FFMpeg_LastError = ffmpeg_encoder_start(filename.c_str(), codec_id, fps, bit_rate, GetWidth(), GetHeight());
	FFMpeg_OK = (FFMpeg_LastError == ffmpeg_OK);
	if (!FFMpeg_OK)
		LockSize(false);
	return FFMpeg_OK;
}

void wxGLScene::MPEG_Close(void)
{
	if (FFMpeg_OK)
	{
		FFMpeg_OK = false;
		ffmpeg_encoder_finish();
		LockSize(false);
	}
}

void wxGLScene::MPEG_Update(void)
{
	if (FFMpeg_OK)
	{
		ffmpeg_encoder_glread_rgb();
		ffmpeg_encoder_encode_frame();
	}
}

wxString wxGLScene::MPEG_LastError(void)
{
	char buffer[255] = {0};
	ffmpeg_encoder_print_error(buffer, (fmpeg_encoder_error)FFMpeg_LastError);
	return wxString(buffer);
}

#endif

// ********************************************************************************
// End of file
// ********************************************************************************
