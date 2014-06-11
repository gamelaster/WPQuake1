// d3d_image.cpp
//

#include "quakedef.h"

idImage::idImage() {
	textureHandle = NULL;
	_width = -1;
	_height = -1;
}

idImage::~idImage() {
	Release();
}

/*
===========
idImage::Init
===========
*/
void idImage::Init( int width, int height, ImageFormat_t format ) {
	D3D11_TEXTURE2D_DESC desc;

	_width = width;
	_height = height;

	_xcrop = _ycrop = 0;

	switch(format)
	{
		case FMT_RGBA:
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
		break;
		case FMT_RGBA_RT:
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
		break;

		case FMT_DEPTH_STENCIL:
			desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			desc.CPUAccessFlags = 0;
			break;

		default:
			Sys_Error("idImage::Init: Unknown format\n");
			break;
	}
    

	desc.Width = _width;
	desc.Height = _height;
	desc.MipLevels = 1;
    desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.MiscFlags = 0;

	render.GetDevice()->CreateTexture2D( &desc, NULL, &textureHandle );
	if(textureHandle == NULL)
		Sys_Error("Failed to allocate texture");

	if(format != FMT_DEPTH_STENCIL)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC rv;

		rv.Format = desc.Format;
		rv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		rv.Texture2D.MostDetailedMip = 0;
		rv.Texture2D.MipLevels = 1;
		render.GetDevice()->CreateShaderResourceView( textureHandle, &rv, &textureHandleRV );
		if( textureHandleRV== NULL)
			Sys_Error("Failed to allocate texture RV" );
	}

	    // Create the sample state
    D3D11_SAMPLER_DESC sampDesc;
	HRESULT hr;
	ZeroMemory( &sampDesc, sizeof(sampDesc) );
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = render.GetDevice()->CreateSamplerState( &sampDesc, &g_pSamplerLinear );
    if( FAILED( hr ) ) {
        Sys_Error("Failed to create texture sample state");
	}
}

/*
===========
idImage::UploadBuffer
===========
*/
void idImage::UploadBuffer( byte *buffer ) {
	D3D11_BOX destRegion;
	destRegion.left = _ycrop;
	destRegion.right = _width;
	destRegion.top = _xcrop;
	destRegion.bottom = _height;
	destRegion.front = 0;
	destRegion.back = 1;

	_xcrop = _ycrop = 0;

	render.GetDeviceContext()->UpdateSubresource( textureHandle, 0, &destRegion, buffer, _width * 4, 0 ); 
}
/*
===========
idImage::UploadBuffer
===========
*/
void idImage::UploadBuffer( byte *buffer, float width, float height ) {
	D3D11_BOX destRegion;
	destRegion.left = _ycrop;
	destRegion.right = width;
	destRegion.top = _xcrop;
	destRegion.bottom = height;
	destRegion.front = 0;
	destRegion.back = 1;

	_xcrop = _ycrop = 0;

	render.GetDeviceContext()->UpdateSubresource( textureHandle, 0, &destRegion, buffer, width * 4, 0 ); 
}

/*
===========
idImage::Release
===========
*/
void idImage::Release( void ) {
	if( textureHandle == NULL )
		return;

	 textureHandle->Release();
	 textureHandle = NULL;
	_width = -1;
	_height = -1;
}

/*
===========
idImage::Resize
===========
*/
void idImage::Resize( int width, int height ) {
	Release();

	Init( width, height );
}

/*
===========
idImage::Bind
===========
*/
void idImage::Bind( int texNum ) {
	render.GetDeviceContext()->PSSetShaderResources( texNum, 1, &textureHandleRV );
	render.GetDeviceContext()->PSSetSamplers( texNum, 1, &g_pSamplerLinear );
}