// sys_winphone8app.cpp
//

#include "quakedef.h"
#include "sys_winphone8app.h"
#include "winquake.h"
#include "newui/ui_main.h"

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace concurrency;

void Sys_Init( void );

QuakeApp::QuakeApp() :
	m_windowClosed(false),
	m_windowVisible(true)
{
}

void QuakeApp::Initialize(CoreApplicationView^ applicationView)
{
	applicationView->Activated +=
		ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &QuakeApp::OnActivated);

	CoreApplication::Suspending +=
		ref new EventHandler<SuspendingEventArgs^>(this, &QuakeApp::OnSuspending);

	CoreApplication::Resuming +=
		ref new EventHandler<Platform::Object^>(this, &QuakeApp::OnResuming);

	
}

void QuakeApp::SetWindow(CoreWindow^ window)
{

	
	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &QuakeApp::OnVisibilityChanged);

	window->Closed += 
		ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &QuakeApp::OnWindowClosed);

	window->PointerPressed +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &QuakeApp::OnPointerPressed);

	window->PointerMoved +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &QuakeApp::OnPointerMoved);

	window->PointerReleased +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &QuakeApp::OnPointerReleased);

	static quakeparms_t    parms;

	parms.memsize = 8*1024*1024;
	parms.membase = malloc (parms.memsize);
	parms.basedir = ".";

	COM_InitArgv (0, NULL);

	parms.argc = com_argc;
	parms.argv = com_argv;

	Sys_Init ();

// because sound is off until we become active
	S_BlockSound ();

	Sys_Printf ("Host_Init\n");
	Host_Init (&parms);
}

void QuakeApp::Load(Platform::String^ entryPoint)
{
}

void QuakeApp::Run()
{
	double time = 0, newtime = 0, oldtime = 0;

	while (!m_windowClosed)
	{
		if (m_windowVisible)
		{
			newtime = Sys_FloatTime ();
			time = newtime - oldtime;
			oldtime = time;

			Host_Frame( time );
		//	int x, y, z, w;
		//	GL_BeginRendering( &x, &y, &z, &w );
		//	GL_EndRendering();
		}
		else
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}
}

void QuakeApp::Uninitialize()
{
}

void QuakeApp::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	m_windowVisible = args->Visible;
}

void QuakeApp::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
	m_windowClosed = true;
}

void QuakeApp::OnPointerPressed(CoreWindow^ sender, PointerEventArgs^ args)
{
	float x = args->CurrentPoint->RawPosition.X;
	float y = args->CurrentPoint->RawPosition.Y;

	newUI->OnPressEvent( x, y );
}

void QuakeApp::OnPointerMoved(CoreWindow^ sender, PointerEventArgs^ args)
{
	// Insert your code here.
}

void QuakeApp::OnPointerReleased(CoreWindow^ sender, PointerEventArgs^ args)
{
	float x = args->CurrentPoint->RawPosition.X;
	float y = args->CurrentPoint->RawPosition.Y;

	newUI->OnPressRelease(x, y);
}

void QuakeApp::OnOrientationChanged(CoreWindow^ sender, PointerEventArgs^ args) {

}

void QuakeApp::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
	 DisplayProperties::AutoRotationPreferences = Windows::Graphics::Display::DisplayOrientations::Landscape;
	// DisplayProperties::OrientationChanged += 
	//	ref new Windows::Graphics::Display::DisplayPropertiesEventHandler(&QuakeApp::OnOrientationChanged);
	CoreWindow::GetForCurrentThread()->Activate();
}

void QuakeApp::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
	// Save app state asynchronously after requesting a deferral. Holding a deferral
	// indicates that the application is busy performing suspending operations. Be
	// aware that a deferral may not be held indefinitely. After about five seconds,
	// the app will be forced to exit.
	SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();
	//m_renderer->ReleaseResourcesForSuspending();

	create_task([this, deferral]()
	{
		// Insert your code here.

		deferral->Complete();
	});
}
 
void QuakeApp::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
	// Restore any data or state that was unloaded on suspend. By default, data
	// and state are persisted when resuming from suspend. Note that this event
	// does not occur if the app was previously terminated.
	// m_renderer->CreateWindowSizeDependentResources();
}

IFrameworkView^ Direct3DApplicationSource::CreateView()
{
	return ref new QuakeApp();
}

[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
	auto direct3DApplicationSource = ref new Direct3DApplicationSource();
	CoreApplication::Run(direct3DApplicationSource);
	return 0;
}
