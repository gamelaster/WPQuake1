/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// d3d_vid.cpp

#include "quakedef.h"
#include "../../winquake.h"
#ifdef ID_PC
#include "../../resource.h"
#endif

#include "../../newui/ui_main.h"

#ifdef ID_PC
#define	BASEWIDTH	1024
#define	BASEHEIGHT	768
#else
int BASEWIDTH = 0;
int BASEHEIGHT = 0;
#endif
unsigned short	d_8to16table[256];
unsigned	d_8to24table[256];
unsigned char d_15to8table[65536];

int window_center_x = BASEWIDTH / 2;
int window_center_y = BASEHEIGHT / 2;

float		gldepthmin, gldepthmax;
int currenttexture = 0;
int m_state = 0;
RECT window_rect;
modestate_t	modestate = MS_UNINIT;

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

//int		texture_mode = GL_NEAREST;
//int		texture_mode = GL_NEAREST_MIPMAP_NEAREST;
//int		texture_mode = GL_NEAREST_MIPMAP_LINEAR;
int		texture_mode = 0;
//int		texture_mode = GL_LINEAR_MIPMAP_NEAREST;
//int		texture_mode = GL_LINEAR_MIPMAP_LINEAR;

int		texture_extension_number = 1;

byte    *vid_buffer;

extern idImage     *currentBoundTexture[64];

#ifndef ID_PC
float ConvertDipsToPixels(float dips)
{
	static const float dipsPerInch = 96.0f;
	return floor(dips * DisplayProperties::LogicalDpi / dipsPerInch + 0.5f); // Round to nearest integer.
}
#endif

idRender render;
void	idRender::SetPalette (unsigned char *palette)
{
	byte	*pal;
	unsigned r,g,b;
	unsigned v;
	int     r1,g1,b1;
	int		j,k,l,m;
	unsigned short i;
	unsigned	*table;
	FILE *f;
	char s[255];
	HWND hDlg, hProgress;
	float gamma;

//
// 8 8 8 encoding
//
	pal = palette;
	table = d_8to24table;
	for (i=0 ; i<256 ; i++)
	{
		r = pal[0];
		g = pal[1];
		b = pal[2];
		pal += 3;
		
//		v = (255<<24) + (r<<16) + (g<<8) + (b<<0);
//		v = (255<<0) + (r<<8) + (g<<16) + (b<<24);
		v = (255<<24) + (r<<0) + (g<<8) + (b<<16);
		*table++ = v;
	}
	d_8to24table[255] &= 0xffffff;	// 255 is transparent

	// JACK: 3D distance calcs - k is last closest, l is the distance.
	// FIXME: Precalculate this and cache to disk.
	for (i=0; i < (1<<15); i++) {
		/* Maps
			000000000000000
			000000000011111 = Red  = 0x1F
			000001111100000 = Blue = 0x03E0
			111110000000000 = Grn  = 0x7C00
		*/
		r = ((i & 0x1F) << 3)+4;
		g = ((i & 0x03E0) >> 2)+4;
		b = ((i & 0x7C00) >> 7)+4;
		pal = (unsigned char *)d_8to24table;
		for (v=0,k=0,l=10000*10000; v<256; v++,pal+=4) {
			r1 = r-pal[0];
			g1 = g-pal[1];
			b1 = b-pal[2];
			j = (r1*r1)+(g1*g1)+(b1*b1);
			if (j<l) {
				k=v;
				l=j;
			}
		}
		d_15to8table[i]=k;
	}
}

void	idRender::ShiftPalette (unsigned char *palette)
{
}

void idRender::Init( unsigned char *palette )
{
	Sys_Printf("---------------- R_Init ----------------\n");
	
	vid.aspect = 1.0;
	vid.numpages = 1;
	vid.colormap = host_colormap;
	vid.fullbright = 256 - LittleLong (*((int *)vid.colormap + 2048));
	

#ifdef ID_PC
	Sys_Printf("Using PC Build...\n");
#endif

#ifdef ID_PC
	Sys_Printf("Creating window....\n");
	InitWindow();
#endif

	Sys_Printf("Init Device....\n");
	InitDevice();

	Sys_Printf("Loading RenderProgs....\n");
	CreateRenderProgs();

	SetPalette( palette );

	newUI = new idNewHud();
}

#ifdef ID_PC
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch( message )
    {
        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}

void idRender::InitWindow( void ) {
	// Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetModuleHandle( NULL );
    wcex.hIcon = LoadIcon(  wcex.hInstance, ( LPCTSTR )IDI_ICON2 );
    wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "Quake";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_ICON2 );

	if( !RegisterClassEx( &wcex ) ) {
		Sys_Error( "Failed to register window" );
	}

	hwnd = CreateWindow( "Quake", "Quake", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, BASEWIDTH, BASEHEIGHT, NULL, NULL, wcex.hInstance, NULL );
	if(!hwnd) {
		Sys_Error( "Failed to create window" );
	}

	ShowWindow( hwnd, SW_SHOW );
}
#endif

void idRender::InitDevice( void ) {
	UINT width = BASEWIDTH;
	UINT height = BASEHEIGHT;

	UINT createDeviceFlags = 0;

	HRESULT hr;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] =
    {
#ifdef ID_PC
		D3D_FEATURE_LEVEL_11_0
#else
        D3D_FEATURE_LEVEL_9_3
#endif
    };

	D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE
    };
#ifdef ID_PC
	DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
#endif
	Sys_Printf("...Aquiring D3D 11 device...\n");
	
#ifdef ID_PC
	hr = D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevels, 1,
                                            D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext );
#else
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	hr = D3D11CreateDevice( nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevels, 1,
                            D3D11_SDK_VERSION, &device, &g_featureLevel, &context );
	device.As(&g_pd3dDevice);
	context.As(&g_pImmediateContext);
#endif
	if(FAILED(hr)) {
		Sys_Error("Failed to create device and swap chain");
	}

#ifndef ID_PC

	// Calculate the necessary swap chain and render target size in pixels.
	BASEWIDTH = ConvertDipsToPixels(CoreWindow::GetForCurrentThread()->Bounds.Width);
	BASEHEIGHT = ConvertDipsToPixels(CoreWindow::GetForCurrentThread()->Bounds.Height);

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {0};
	swapChainDesc.Width = static_cast<UINT>(BASEWIDTH); // Match the size of the window.
	swapChainDesc.Height = static_cast<UINT>(BASEHEIGHT);
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // This is the most common swap chain format.
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1; // Don't use multi-sampling.
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1; // On phone, only single buffering is supported.
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH; // On phone, only stretch and aspect-ratio stretch scaling are allowed.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // On phone, no swap effects are supported.
	swapChainDesc.Flags = 0;


	//BASEWIDTH = 800;
	//BASEHEIGHT = 480;

	Sys_Printf("Creating dxgi factory and swapchain\n");
	ComPtr<IDXGIDevice1> dxgiDevice;
	GetDevice()->QueryInterface(__uuidof(IDXGIDevice1), &dxgiDevice);

	ComPtr<IDXGIAdapter> dxgiAdapter;
	dxgiDevice->GetAdapter(&dxgiAdapter);

	ComPtr<IDXGIFactory2> dxgiFactory;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), &dxgiFactory);

	Windows::UI::Core::CoreWindow^ window = CoreWindow::GetForCurrentThread();
	hr = dxgiFactory->CreateSwapChainForCoreWindow(GetDevice(),reinterpret_cast<IUnknown*>(window),&swapChainDesc,nullptr, &g_pSwapChain );
	if(FAILED(hr)) {
		Sys_Printf("Create swap chain failed\n");
	}

#endif
	vid_buffer = new byte[BASEWIDTH * BASEHEIGHT];

	vid.maxwarpwidth = vid.width = vid.conwidth = BASEWIDTH;
	vid.maxwarpheight = vid.height = vid.conheight = BASEHEIGHT;
	vid.buffer = vid.conbuffer = vid_buffer;
	vid.rowbytes = vid.conrowbytes = BASEWIDTH;

	Sys_Printf("...Creating RenderTargetView...\n");
	
	depthImage = new idImage();
	depthImage->Init( BASEWIDTH, BASEHEIGHT, FMT_DEPTH_STENCIL );

	nativeRenderDestinationReal = new idRenderTexture();
	nativeRenderDestinationReal->CreateFromImages( NULL, depthImage );
	//nativeRenderDestination->MakeCurrent();

	idImage *depthImage2 = new idImage();
	depthImage2->Init( BASEWIDTH, BASEHEIGHT, FMT_DEPTH_STENCIL );

	
	frameBufferImage = new idImage();
	frameBufferImage->Init( BASEWIDTH, BASEHEIGHT, FMT_RGBA_RT );

	nativeRenderDestination = new idRenderTexture();
	nativeRenderDestination->CreateFromImages( frameBufferImage, depthImage2 );
	nativeRenderDestination->MakeCurrent();

	GL_Viewport( 0, 0, BASEWIDTH, BASEHEIGHT );

}

void	VID_Shutdown (void)
{
}

void	VID_Update (vrect_t *rects)
{
}

void VID_SetDefaultMode (void)
{
	//IN_DeactivateMouse ();
}



/*
=================
GL_BeginRendering

=================
*/
void GL_BeginRendering (int *x, int *y, int *width, int *height)
{
	extern cvar_t gl_clear;

	*x = *y = 0;
	*width = BASEWIDTH;
	*height = BASEHEIGHT;

	CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

	render.GetNativeRenderDestination()->MakeCurrent();
	GL_Clear( true, true, false, false, 0, 0,0,0 );
	
	GL_SetAlphaBlend( true );
//    if (!wglMakeCurrent( maindc, baseRC ))
//		Sys_Error ("wglMakeCurrent failed");

//	glViewport (*x, *y, *width, *height);
}


void GL_EndRendering (void)
{
	// Whats the best way to handle screen orientation transforms? TO NOT DO IT ALL :)
	// We render into the non "real" native render destination and just transform here...tadda...
	render.GetNativeRenderDestinationReal()->MakeCurrent();
	GL_Clear( true, true, false, false, 0, 0,0,0 );
	GL_Set2D();
	
	GL_SetAlphaBlend( false );
	
	idDrawVert verts[4];

	verts[0].SetST( 1, 0);
	verts[0].SetPosition( 0, 0, 0 );

	verts[1].SetST( 1, 1);
	verts[1].SetPosition( vid.width, 0, 0 );

	verts[2].SetST( 0, 1);
	verts[2].SetPosition( vid.width, vid.height, 0 );

	verts[3].SetST( 0 , 0);
	verts[3].SetPosition( 0, vid.height, 0 );

	currentBoundTexture[0] = render.GetFrameBufferImage();
	GL_DrawImageWithVerts2D( verts );

	render.GetSwapChain()->Present(1, 0 );
}
