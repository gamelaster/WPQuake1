// d3d_wrapper.cpp
//

#include "quakedef.h"

idImage		imageStack[ 40000 ];   // Image stack, were emulating the opengl image stack system.
idImage     *currentBoundTexture[64];

GL_MatrixMode _mode;

int			currentTMU = 0;
idVertexBuffer  sceneVertexBuffer;
idIndexBuffer	quad2DIndexBuffer;

idRenderMatrix  currentRenderMatrix;
idConstantBuffer currentRenderMatrixConstantBuffer;
idConstantBuffer currentFrameBuffer;

const float	M_DEG2RAD		= M_PI / 180.0f;
#define DEG2RAD( a )		(a * M_DEG2RAD)
idCurrentFrameData			frameData;

/*
======================
idRender::CreateRenderProgs
======================
*/
void idRender::CreateRenderProgs( void ) {
	lightmapRenderProgram = new idRenderProg();
	lightmapRenderProgram->InitFromFile( "renderprogs/lightmappass_vs.cso", "renderprogs/lightmappass_ps.cso" );

	albedo2DRenderProgram = new idRenderProg();
	albedo2DRenderProgram->InitFromFile( "renderprogs/2d_albedo_vs.cso", "renderprogs/2d_albedo_ps.cso" );

	albedoRenderProgram = new idRenderProg();
	albedoRenderProgram->InitFromFile( "renderprogs/albedoonly_vs.cso", "renderprogs/albedoonly_ps.cso" );

	liquidRenderProgram = new idRenderProg();
	liquidRenderProgram->InitFromFile( "renderprogs/liquid_vs.cso", "renderprogs/albedoonly_ps.cso" );

	skyRenderProgram = new idRenderProg();
	skyRenderProgram->InitFromFile( "renderprogs/sky_vs.cso", "renderprogs/albedoonly_ps.cso" );

	sceneVertexBuffer.Init( 7000 );
	quad2DIndexBuffer.Init( 6 );

	int indexes[6] = { 0, 1, 2, 0, 2, 3 };
	quad2DIndexBuffer.Upload( &indexes[0], 6 );

	currentRenderMatrixConstantBuffer.Init( sizeof( idRenderMatrix ), 1 );
	currentFrameBuffer.Init( sizeof(idCurrentFrameData), 1 );
}


void GL_SelectTexture (int target) 
{
	if (target == currentTMU) 
		return;

	currentTMU = target;
}

void GL_Bind (int texnum)
{
//	if (currenttexture == texnum)
	//	return;
	currenttexture = texnum;

	if(texnum == 0)
	{
		currentBoundTexture[currentTMU] = NULL;
		return;
	}

    currentBoundTexture[currentTMU] = &imageStack[currenttexture];
}

idImage *GL_GetImage( void ) {
	return currentBoundTexture[currentTMU];
}

/*
===============
GL_SetCropForImage
===============
*/
void GL_SetCropForImage( float x, float y ) {
	currentBoundTexture[currentTMU]->SetUploadCrop( x, y );
}

/*
===============
GL_Upload32
===============
*/
void GL_Upload32 (unsigned *data, int width, int height,  qboolean mipmap, qboolean alpha)
{
	if(currentBoundTexture[currentTMU] == NULL) {
		Sys_Error("GL_Upload32: No texture bound for upload");
	}

	if(currentBoundTexture[currentTMU]->GetWidth() != width || currentBoundTexture[currentTMU]->GetHeight() != height) {
		currentBoundTexture[currentTMU]->Resize( width, height );
	}

	currentBoundTexture[currentTMU]->UploadBuffer( (byte *)data );
}
/*
===============
GL_DrawImage2D
===============
*/
void GL_DrawImage2D( float x, float y, float width, float height, float red, float green, float blue, float alpha ) {

}

/*
===============
GL_DrawImage2D
===============
*/
void GL_DrawImageWithVerts2D( idDrawVert verts[4] ) {

	sceneVertexBuffer.Upload( &verts[0], 4 );
	currentRenderMatrixConstantBuffer.Upload( &currentRenderMatrix, 1 );

	sceneVertexBuffer.Bind();
	quad2DIndexBuffer.Bind();

	render.GetAlbedo2DRenderProgram()->Bind();
	currentRenderMatrixConstantBuffer.VSBind( 0 );

	if(currentBoundTexture[0] != NULL)
	{
		currentBoundTexture[0]->Bind( 0 );
	}

	if(currentBoundTexture[1] != NULL)
	{
		currentBoundTexture[1]->Bind( 1 );
	}

	render.GetDeviceContext()->DrawIndexed( 6, 0, 0 );
}

/*
================
GL_Set2D

Setup as if the screen was 320*200
================
*/
void GL_Set2D (void)
{
	GL_Cull( CULL_NONE );
	GL_DepthTest( false );
	render.GetDeviceContext()->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
#ifdef ID_PC
	currentRenderMatrix.mProjection = XMMatrixOrthographicLH( vid.width, vid.height, 0, 1 );
#else
	currentRenderMatrix.mProjection = XMMatrixOrthographicLH( vid.width, vid.height, 0, 1 );
#endif
	currentRenderMatrix.mView = XMMatrixIdentity();
		
	currentRenderMatrix.mWorld = XMMatrixIdentity();
}

/*
================
GL_Translate
================
*/
void GL_Translate( float x, float y, float z ) {
	currentRenderMatrix.mWorld *= XMMatrixTranspose(XMMatrixTranslation( x, y, z ));
}

/*
================
GL_Rotate
================
*/
void GL_Rotate( float angle, float yaw, float pitch, float roll ) {
	angle = DEG2RAD( angle );

	currentRenderMatrix.mWorld *= XMMatrixRotationRollPitchYaw( yaw * angle, pitch * angle, roll * angle );

}
/*
================
GL_Identity
================
*/
void GL_Identity( void ) {
	if(_mode == GL_MATRIX_PROJECTION)
	{
		currentRenderMatrix.mProjection = XMMatrixIdentity();
	}
	else
	{
		currentRenderMatrix.mView = currentRenderMatrix.mWorld = XMMatrixIdentity();
	}
}

/*
================
GL_SetMatrixMode
================
*/
void GL_SetMatrixMode( GL_MatrixMode mode ) {
	_mode = mode;
}

/*
================
GL_Viewport
================
*/
void GL_Viewport( float x, float y, float width, float height ) {
	    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = x;
    vp.TopLeftY = y;
    render.GetDeviceContext()->RSSetViewports( 1, &vp );
}

/*
================
MYgluPerspective
================
*/
void MYgluPerspective( double fovy, double aspect, double zNear, double zFar )
{

	float	xmin, xmax, ymin, ymax;
	float	width, height, depth;


	ymax = zNear * tan( fovy * M_PI / 360.0f );
	ymin = -ymax;

	xmax = zNear * tan( fovy * M_PI / 360.0f );
	xmin = -xmax;

	width = xmax - xmin;
	height = ymax - ymin;
	depth = zFar - zNear;

	float *projection = &currentRenderMatrix.mProjection.r[0].n128_f32[0];

	projection[0] = 2 * zNear / width;
	projection[4] = 0;
	projection[8] = ( xmax + xmin ) / width;	// normally 0
	projection[12] = 0;

	projection[1] = 0;
	projection[5] = 2 * zNear / height;
	projection[9] = ( ymax + ymin ) / height;	// normally 0
	projection[13] = 0;

	projection[2] = 0;
	projection[6] = 0;
	projection[10] = -( zFar + zNear ) / depth;
	projection[14] = -2 * zFar * zNear / depth;

	projection[3] = 0;
	projection[7] = 0;
	projection[11] = -1;
	projection[15] = 0;


	currentRenderMatrix.mProjection = XMMatrixTranspose( currentRenderMatrix.mProjection );
}

/*
================
GL_Scale
================
*/
void GL_Scale( float x, float y, float z ) {
	currentRenderMatrix.mWorld *= XMMatrixScaling( x, y, z );
}

/*
================
GL_DepthTest
================
*/
void GL_DepthTest( bool active, GL_DepthFunc depthFunc ) {
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	// Depth test parameters
	if(active)
	{
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	}
	else {
		dsDesc.DepthEnable = false;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	}
	

	// Stencil test parameters
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	ID3D11DepthStencilState * pDSState;
	render.GetDevice()->CreateDepthStencilState(&dsDesc, &pDSState);

	render.GetDeviceContext()->OMSetDepthStencilState(pDSState, 1);

}
/*
================
GL_SetAddBlend
================
*/
void GL_SetAddBlend( bool enable ) {
		ID3D11BlendState* g_pBlendState = NULL;
	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
	BlendState.RenderTarget[0].BlendEnable = enable;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	render.GetDevice()->CreateBlendState(&BlendState, &g_pBlendState);
	render.GetDeviceContext()->OMSetBlendState(g_pBlendState, 0, 0xffffffff);
}

/*
================
GL_SetAlphaBlend
================
*/
void GL_SetAlphaBlend( bool enable ) {
	ID3D11BlendState* g_pBlendState = NULL;
	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
	BlendState.RenderTarget[0].BlendEnable = enable;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	render.GetDevice()->CreateBlendState(&BlendState, &g_pBlendState);
	render.GetDeviceContext()->OMSetBlendState(g_pBlendState, 0, 0xffffffff);


}

/*
================
GL_GetMatrix
================
*/
void GL_GetMatrix( GL_MatrixMode mode, idRenderMatrix &m ) {
	m = currentRenderMatrix;
}
/*
================
GL_GetMatrix
================
*/
void GL_SetMatrix( idRenderMatrix &m ) {
	currentRenderMatrix = m;
}

/*
================
GL_Cull
================
*/
void GL_Cull( GL_CullType cullType ) {
	D3D11_RASTERIZER_DESC rasterDesc;

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;

	switch(cullType)
	{
		case CULL_NONE:
			rasterDesc.CullMode = D3D11_CULL_NONE;
		break;

		case CULL_BACK:
			rasterDesc.CullMode = D3D11_CULL_BACK;
		break;

		case CULL_FRONT:
			rasterDesc.CullMode = D3D11_CULL_FRONT;
		break;
	}
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	ID3D11RasterizerState* m_rasterState;

	// Create the rasterizer state from the description we just filled out.
	HRESULT hr = render.GetDevice()->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if(FAILED(hr))
	{
		Sys_Error("Failed to create rasterizerstate");
	}

	// Now set the rasterizer state.
	render.GetDeviceContext()->RSSetState(m_rasterState);

	m_rasterState->Release();
}

/*
================
GL_Clear
================
*/
void GL_Clear( bool colorBuffer, bool depthBuffer, bool stencilBuffer, float stencil, float r, float g, float b, float a ) {
#ifdef ID_PC
	float ClearColor[4] = {r, g, b, a }; // red, green, blue, alpha
#else
	float ClearColor[4] = {0.098f, 0.098f, 0.439f, 1.000f};
#endif
	if(colorBuffer)
	{
		render.GetDeviceContext()->ClearRenderTargetView( idRenderTexture::GetCurrentRenderTarget()->GetRenderTargetView(), ClearColor );
	}

	if(depthBuffer) {
		render.GetDeviceContext()->ClearDepthStencilView( idRenderTexture::GetCurrentRenderTarget()->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0 );
	}
}

/*
================
GL_SetMatrix
================
*/
void GL_SetMatrix( float *m ) {
	memcpy( &currentRenderMatrix.mView.r[0].n128_f32[0], &m[0], sizeof(float) * 16 );
}

/*
================
GL_SetMatrix
================
*/
void GL_GetMatrix( GL_MatrixMode mode, float *m ) {
	memcpy( &m[0], &currentRenderMatrix.mView.r[0].n128_f32[0], sizeof(float) * 16 );
}

/*
================
GL_Finish
================
*/
void GL_Finish( void ) {
	render.GetDeviceContext()->Flush();
}

/*
================
GL_DrawQuad
================
*/
void GL_DrawQuad( idDrawVert *verts ) {


	sceneVertexBuffer.Upload( &verts[0], 4 );
	currentRenderMatrixConstantBuffer.Upload( &currentRenderMatrix, 1 );

	sceneVertexBuffer.Bind();
	quad2DIndexBuffer.Bind();

	render.GetAlbedoRenderProgram()->Bind();
	currentRenderMatrixConstantBuffer.VSBind( 0 );

	if(currentBoundTexture[0] != NULL)
	{
		currentBoundTexture[0]->Bind( 0 );
	}

	if(currentBoundTexture[1] != NULL)
	{
		currentBoundTexture[1]->Bind( 1 );
	}
	render.GetDeviceContext()->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	render.GetDeviceContext()->DrawIndexed( 6, 0, 0);
}
/*
================
GL_DrawBuffer
================
*/
void GL_DrawBuffer( idDrawVert *verts, int numVerts, idRenderProg *renderProg, idVertexBuffer *vertexpool, idIndexBuffer *indexbuffer, int startVertex, int numIndexes  ) {
	bool useIndexBuffer = false;

	frameData.rendertime = realtime;
	
	currentFrameBuffer.Upload( &frameData, 1 );

	if(vertexpool == NULL)
	{
		sceneVertexBuffer.Upload( &verts[0], numVerts );
		sceneVertexBuffer.Bind();
	}
	else {
		vertexpool->Bind();
		numVerts = vertexpool->GetNumVerts();
	}

	//if(numVerts == 4)
	//	useIndexBuffer = true;

	currentRenderMatrixConstantBuffer.Upload( &currentRenderMatrix, 1 );

	
	if(useIndexBuffer && !indexbuffer)
		quad2DIndexBuffer.Bind();

	if(renderProg == NULL)
		render.GetAlbedoRenderProgram()->Bind();
	else
		renderProg->Bind();

	currentRenderMatrixConstantBuffer.VSBind( 0 );
	currentFrameBuffer.VSBind( 1 );

	if(currentBoundTexture[0] != NULL)
	{
		currentBoundTexture[0]->Bind( 0 );
	}

	if(currentBoundTexture[1] != NULL)
	{
		currentBoundTexture[1]->Bind( 1 );
	}
	render.GetDeviceContext()->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	if(useIndexBuffer || indexbuffer != NULL)
	{
		if(indexbuffer) {
			indexbuffer->Bind();

			render.GetDeviceContext()->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
			render.GetDeviceContext()->DrawIndexed( numIndexes, 0, startVertex );

			//
		}
		else 
		{
			render.GetDeviceContext()->DrawIndexed( 6, 0, 0);
		}
	}
	else
	{
		render.GetDeviceContext()->Draw( numVerts, 0 );
	}

	if(vertexpool != NULL)
	{
		sceneVertexBuffer.Bind();
	}
}
/*
================
GL_DrawBufferTri
================
*/
void GL_DrawBufferTri( idDrawVert *verts, int numVerts ) {
		bool useIndexBuffer = false;
	if(numVerts == 4) {
		useIndexBuffer = true;
	}

	sceneVertexBuffer.Upload( &verts[0], numVerts );
	currentRenderMatrixConstantBuffer.Upload( &currentRenderMatrix, 1 );

	sceneVertexBuffer.Bind();
	if(useIndexBuffer)
		quad2DIndexBuffer.Bind();

	render.GetAlbedoRenderProgram()->Bind();
	currentRenderMatrixConstantBuffer.VSBind( 0 );

	if(currentBoundTexture[0] != NULL)
	{
		currentBoundTexture[0]->Bind( 0 );
	}

	if(currentBoundTexture[1] != NULL)
	{
		currentBoundTexture[1]->Bind( 1 );
	}
	render.GetDeviceContext()->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	if(useIndexBuffer)
	{
		render.GetDeviceContext()->DrawIndexed( 6, 0, 0);
	}
	else
	{
		render.GetDeviceContext()->Draw( numVerts, 0 );
	}
}