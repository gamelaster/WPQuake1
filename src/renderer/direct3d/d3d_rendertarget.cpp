// d3d_rendertarget.cpp
//

#include "quakedef.h"

idRenderTexture		*idRenderTexture::currentRenderDestination = NULL;

void idRenderTexture::MakeCurrent( void ) {
	currentRenderDestination = this;
	render.GetDeviceContext()->OMSetRenderTargets( 1, &pRenderTargetView, pDepthStencilView );
	//render.GetDeviceContext()->OMSetRenderTargets(1, NULL, NULL);
	//render.GetDeviceContext()->Release();
}

void idRenderTexture::CreateFromImages( idImage *colorImage, idImage *depthStencilImage ) {
	ID3D11Texture2D* albedoImage;
	HRESULT hr;
	if(colorImage == NULL) {
		// Create a render target view
		hr = render.GetSwapChain()->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&albedoImage );
		if( FAILED( hr ) ) {
			Sys_Error("CreateFromImages: Failed to get backbuffer\n");
		}
	}
	else {
		albedoImage = colorImage->GetTextureHandle();
	}

	hr = render.GetDevice()->CreateRenderTargetView( albedoImage, NULL, &pRenderTargetView );
	if( FAILED( hr ) ) {
		Sys_Error("CreateFromImages: Failed to create render target view\n");
	}

	if(colorImage == NULL) {
		albedoImage->Release();
	}

	if(depthStencilImage != NULL)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory( &descDSV, sizeof(descDSV) );
		descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		hr = render.GetDevice()->CreateDepthStencilView( depthStencilImage->GetTextureHandle(), &descDSV, &pDepthStencilView );
		if( FAILED( hr ) )
			Sys_Error("Failed to create depth image");
	}
	else {
		pDepthStencilView = NULL;
	}
}