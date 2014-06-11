// d3d_buffer.cpp
//

#include "quakedef.h"

/*
================
idDrawVert::CreateDeviceVert
================
*/
ID3D11InputLayout *idDrawVert::CreateDeviceVert( ID3DBlob* pVSBlob ) {
	ID3D11InputLayout * g_pVertexLayout;
	HRESULT hr;

	// Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    UINT numElements = ARRAYSIZE( layout );

    // Create the input layout
    hr = render.GetDevice()->CreateInputLayout( layout, numElements, pVSBlob->GetBufferPointer(),
                                          pVSBlob->GetBufferSize(), &g_pVertexLayout );
  //  pVSBlob->Release();
    if( FAILED( hr ) ) {
		Sys_Error("CreateDeviceVert: Failed");
	}

    // Set the input layout
    //render.GetDeviceContext()->IASetInputLayout( g_pVertexLayout );

	return  g_pVertexLayout;
}

idVertexBuffer::~idVertexBuffer() {
	if(g_pVertexBuffer != NULL)
	{
		g_pVertexBuffer->Release();
	}
}

/*
================
idVertexBuffer::Init
================
*/
void idVertexBuffer::Init( int numVerts ) {
	HRESULT hr;
	int Size = sizeof(idDrawVert) * numVerts;

	//Sys_Printf("Creating Vertex Buffer(%d)...\n", Size );

	D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = Size;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    
    hr = render.GetDevice()->CreateBuffer( &bd, NULL, &g_pVertexBuffer );
    if( FAILED( hr ) ) {
		Sys_Error("Failed to create vertex buffer");
	}
}

/*
================
idVertexBuffer::Upload
================
*/
void idVertexBuffer::Upload( idDrawVert *verts, int numVerts ) {
	D3D11_MAPPED_SUBRESOURCE resource;
	render.GetDeviceContext()->Map( g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &resource );
	memcpy( resource.pData, verts, sizeof(idDrawVert) * numVerts );
	render.GetDeviceContext()->Unmap( g_pVertexBuffer, 0 );

	_numVerts = numVerts;
}

/*
================
idVertexBuffer::Bind
================
*/
void idVertexBuffer::Bind( void ) {
	// Set vertex buffer
    UINT stride = sizeof( idDrawVert );
    UINT offset = 0;
    render.GetDeviceContext()->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &stride, &offset );
}

idIndexBuffer::~idIndexBuffer() {
	g_pIndexBuffer->Release();
}

/*
================
idIndexBuffer::Init
================
*/
void idIndexBuffer::Init( int numIndexes ) {
	HRESULT hr;
	int Size = sizeof(int) * numIndexes;

	Sys_Printf("Creating Index Buffer(%d)...\n", Size );

	D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = Size;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = render.GetDevice()->CreateBuffer( &bd, NULL, &g_pIndexBuffer );
    if( FAILED( hr ) ) {
		Sys_Error("Failed to create index buffer");
	}
}

/*
================
idIndexBuffer::Upload
================
*/
void idIndexBuffer::Upload( int *indexes, int numIndexes ) {
	D3D11_MAPPED_SUBRESOURCE resource;
	render.GetDeviceContext()->Map( g_pIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &resource );
	memcpy( resource.pData, indexes, sizeof(int) * numIndexes );
	render.GetDeviceContext()->Unmap( g_pIndexBuffer, 0 );
}

/*
================
idIndexBuffer::Bind
================
*/
void idIndexBuffer::Bind( void ) {
	// Set vertex buffer
    UINT stride = sizeof( idDrawVert );
    UINT offset = 0;
    render.GetDeviceContext()->IASetIndexBuffer(  g_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );

	render.GetDeviceContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}

/*
================
idConstantBuffer::Init
================
*/
void idConstantBuffer::Init( int size, int num ) {
	HRESULT hr;
	int Size = size * num;

	_size = size;

	Sys_Printf("Creating Constant Buffer(%d)...\n", Size );

	D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = Size;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = render.GetDevice()->CreateBuffer( &bd, NULL, &g_pBuffer );
    if( FAILED( hr ) ) {
		Sys_Error("Failed to create index buffer");
	}
}

/*
================
idConstantBuffer::Upload
================
*/
void idConstantBuffer::Upload( void *buffer, int num ) {
	D3D11_MAPPED_SUBRESOURCE resource;
	render.GetDeviceContext()->Map( g_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &resource );
	memcpy( resource.pData, buffer, _size * num );
	render.GetDeviceContext()->Unmap( g_pBuffer, 0 );
}

/*
================
idConstantBuffer::VSBind
================
*/
void idConstantBuffer::VSBind( int constantBufferSlot ) {
	 render.GetDeviceContext()->VSSetConstantBuffers( constantBufferSlot, 1, &g_pBuffer );
}