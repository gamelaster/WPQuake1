// d3d_renderprog.cpp
//

#include "quakedef.h"

#ifdef ID_PC
#include <d3dcompiler.h>
#endif

idRenderProg *idRenderProg::currentRenderProg = NULL;

struct idD3DBlob : public ID3D10Blob {
public:
	idD3DBlob( byte *ptr, int ptrsize ) { blob = ptr; blobsize = ptrsize; }

	byte *blob;
	int blobsize;

	virtual LPVOID STDMETHODCALLTYPE GetBufferPointer( void) { return blob; }
        
	virtual SIZE_T STDMETHODCALLTYPE GetBufferSize( void) { return blobsize; }

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject) { return 0; }

	virtual ULONG STDMETHODCALLTYPE AddRef( void) { return 0; }

	virtual ULONG STDMETHODCALLTYPE Release( void) { free(blob); return 0; }
};

/*
======================
idRenderProg::InitFromFile
======================
*/
#ifdef ID_PC
void idRenderProg::InitFromFile( const char *path ) {
#else
void idRenderProg::InitFromFile( const char *vertpath, const char *pixelpath ) {
#endif
	HRESULT hr;

	

	// Load the renderprog from the file system.
#ifdef ID_PC
	Sys_Printf("Loading RenderProg %s\n", path );
	byte *buffer = COM_LoadHunkFile( (char *)path );
	if(buffer == NULL) {
		Sys_Error("idRenderProg::initFromfile: failed to open %s\n", path );
	}
#else
	Sys_Printf("Loading RenderProg %s\n", vertpath );
	byte *vertbuffer = COM_LoadHunkFile( (char *)vertpath );
	int vertbufferlen = COM_GetLoadedFileLen();
	if(vertbuffer == NULL) {
		Sys_Error("idRenderProg::initFromfile: failed to open %s\n", vertpath );
	}

	Sys_Printf("Loading RenderProg %s\n", pixelpath );
	byte *pixelbuffer = COM_LoadHunkFile( (char *)pixelpath );
	int pixelbufferlen = COM_GetLoadedFileLen();
	if(pixelbuffer == NULL) {
		Sys_Error("idRenderProg::initFromfile: failed to open %s\n", pixelpath );
	}
#endif

	
	ID3DBlob* pErrorBlob;
	ID3DBlob* pVertexShaderBlob;
	ID3DBlob* pPixelShaderBlob;

#ifdef ID_PC
	DWORD dwShaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	Sys_Printf("...Compiling Vertex Shader\n");
	hr = D3DCompile( (const char *)buffer, strlen( (const char *)buffer ), path, NULL, NULL, "VSMain", "vs_4_0", dwShaderFlags, 0, &pVertexShaderBlob, &pErrorBlob );
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
            Sys_Error( (char*)pErrorBlob->GetBufferPointer() );
    }

	Sys_Printf("...Compiling Pixel Shader\n");

	hr = D3DCompile( (const char *)buffer, strlen( (const char *)buffer ), path, NULL, NULL, "PSMain", "ps_4_0", dwShaderFlags, 0,  &pPixelShaderBlob, &pErrorBlob );
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
            Sys_Error( (char*)pErrorBlob->GetBufferPointer() );
    }
#else
	pVertexShaderBlob = (ID3DBlob *)new idD3DBlob(vertbuffer, vertbufferlen );
	pPixelShaderBlob = (ID3DBlob *)new idD3DBlob(pixelbuffer, pixelbufferlen );

#endif
	hr = render.GetDevice()->CreateVertexShader( pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), NULL, &g_pVertexShader );
	if( FAILED(hr) )
    {
		Sys_Error("Failed to create vertex shader");
	}

	inputLayout = idDrawVert::CreateDeviceVert( pVertexShaderBlob );

	hr = render.GetDevice()->CreatePixelShader( pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), NULL, &g_pPixelShader );
	if( FAILED(hr) )
    {
		Sys_Error("Failed to create pixel shader");
	}

	//pVertexShaderBlob->Release();
	//pPixelShaderBlob->Release();
}

/*
======================
idRenderProg::Set2DViewProjectionMatrix
======================
*/
void idRenderProg::Set2DViewProjectionMatrix( void ) {

}

/*
======================
idRenderProg::Bind
======================
*/
void idRenderProg::Bind( void ) {
	if(currentRenderProg == this)
		return;

	 // Set the input layout
    render.GetDeviceContext()->IASetInputLayout( inputLayout );
	render.GetDeviceContext()->VSSetShader( g_pVertexShader, NULL, 0 );
	render.GetDeviceContext()->PSSetShader( g_pPixelShader, NULL, 0 );

	currentRenderProg = this;
}