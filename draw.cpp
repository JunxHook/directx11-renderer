#include "draw.h"
#include <d3dcompiler.h>

namespace
{
	ID3D11InputLayout* g_pInputLayout = nullptr;
	ID3D11VertexShader* g_pVertexShader = nullptr;
	ID3D11PixelShader* g_pPixelShader = nullptr;
	ID3D11Buffer* g_pVertexBuffer = nullptr;
	ID3D11Buffer* g_pConstantBuffer = nullptr;
	ID3D11BlendState* g_pBlendState = nullptr;
	ID3D11RasterizerState* g_pRasterizerState = nullptr;
	ID3D11DepthStencilState* g_pDepthStencilState = nullptr;
	std::vector<Vertex> g_vertices;
	const size_t MAX_VERTICES = 10000;
	struct ConstantBuffer { float mvp [ 4 ][ 4 ]; };
}

const char* shader_code = R"(
    cbuffer ConstantBuffer : register(b0)
    {
        float4x4 mvp;
    }
    struct VS_INPUT
    {
        float4 pos : POSITION;
        float4 col : COLOR0;
    };
    struct PS_INPUT
    {
        float4 pos : SV_POSITION;
        float4 col : COLOR0;
    };
    PS_INPUT VS(VS_INPUT input)
    {
        // Como a matriz agora é row-major, a multiplicação correta é mul(vector, matrix)
        PS_INPUT output;
        output.pos = mul(input.pos, mvp);
        output.col = input.col;
        return output;
    }
    float4 PS(PS_INPUT input) : SV_Target
    {
        return input.col;
    }
)";

bool Draw::Initialize ( )
{
	UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3DCompile ( shader_code , strlen ( shader_code ) , nullptr , nullptr , nullptr , "VS" , "vs_4_0" , compileFlags , 0 , &vsBlob , &errorBlob );
	if ( FAILED ( hr ) ) { if ( errorBlob ) OutputDebugStringA ( ( char* ) errorBlob->GetBufferPointer ( ) ); return false; }

	hr = g_pd3dDevice->CreateVertexShader ( vsBlob->GetBufferPointer ( ) , vsBlob->GetBufferSize ( ) , nullptr , &g_pVertexShader );
	if ( FAILED ( hr ) ) { vsBlob->Release ( ); return false; }

	hr = D3DCompile ( shader_code , strlen ( shader_code ) , nullptr , nullptr , nullptr , "PS" , "ps_4_0" , compileFlags , 0 , &psBlob , &errorBlob );
	if ( FAILED ( hr ) ) { if ( errorBlob ) OutputDebugStringA ( ( char* ) errorBlob->GetBufferPointer ( ) ); vsBlob->Release ( ); return false; }

	hr = g_pd3dDevice->CreatePixelShader ( psBlob->GetBufferPointer ( ) , psBlob->GetBufferSize ( ) , nullptr , &g_pPixelShader );
	if ( FAILED ( hr ) ) { vsBlob->Release ( ); psBlob->Release ( ); return false; }

	D3D11_INPUT_ELEMENT_DESC layout [ ] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	hr = g_pd3dDevice->CreateInputLayout ( layout , 2 , vsBlob->GetBufferPointer ( ) , vsBlob->GetBufferSize ( ) , &g_pInputLayout );
	vsBlob->Release ( ); psBlob->Release ( ); if ( FAILED ( hr ) ) return false;
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC; bd.ByteWidth = MAX_VERTICES * sizeof ( Vertex ); bd.BindFlags = D3D11_BIND_VERTEX_BUFFER; bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = g_pd3dDevice->CreateBuffer ( &bd , nullptr , &g_pVertexBuffer ); if ( FAILED ( hr ) ) return false;
	bd.Usage = D3D11_USAGE_DEFAULT; bd.ByteWidth = sizeof ( ConstantBuffer ); bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER; bd.CPUAccessFlags = 0;
	hr = g_pd3dDevice->CreateBuffer ( &bd , nullptr , &g_pConstantBuffer ); if ( FAILED ( hr ) ) return false;
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget [ 0 ].BlendEnable = TRUE; blendDesc.RenderTarget [ 0 ].SrcBlend = D3D11_BLEND_SRC_ALPHA; blendDesc.RenderTarget [ 0 ].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; blendDesc.RenderTarget [ 0 ].BlendOp = D3D11_BLEND_OP_ADD; blendDesc.RenderTarget [ 0 ].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA; blendDesc.RenderTarget [ 0 ].DestBlendAlpha = D3D11_BLEND_ZERO; blendDesc.RenderTarget [ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD; blendDesc.RenderTarget [ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = g_pd3dDevice->CreateBlendState ( &blendDesc , &g_pBlendState ); if ( FAILED ( hr ) ) return false;
	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D11_FILL_SOLID; rasterDesc.CullMode = D3D11_CULL_NONE; rasterDesc.ScissorEnable = true;
	hr = g_pd3dDevice->CreateRasterizerState ( &rasterDesc , &g_pRasterizerState ); if ( FAILED ( hr ) ) return false;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = false; depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS; depthStencilDesc.StencilEnable = false;
	hr = g_pd3dDevice->CreateDepthStencilState ( &depthStencilDesc , &g_pDepthStencilState ); if ( FAILED ( hr ) ) return false;
	g_vertices.reserve ( MAX_VERTICES );
	return true;
}


void Draw::Shutdown ( ) { if ( g_pInputLayout ) g_pInputLayout->Release ( ); if ( g_pVertexShader ) g_pVertexShader->Release ( ); if ( g_pPixelShader ) g_pPixelShader->Release ( ); if ( g_pVertexBuffer ) g_pVertexBuffer->Release ( ); if ( g_pConstantBuffer ) g_pConstantBuffer->Release ( ); if ( g_pBlendState ) g_pBlendState->Release ( ); if ( g_pRasterizerState ) g_pRasterizerState->Release ( ); if ( g_pDepthStencilState ) g_pDepthStencilState->Release ( ); }
void Draw::Begin ( ) { g_vertices.clear ( ); }
void Draw::End ( ) {
	auto ctx = g_pd3dDeviceContext;
	if ( g_vertices.empty ( ) ) return;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if ( FAILED ( ctx->Map ( g_pVertexBuffer , 0 , D3D11_MAP_WRITE_DISCARD , 0 , &mappedResource ) ) ) return;
	memcpy ( mappedResource.pData , g_vertices.data ( ) , g_vertices.size ( ) * sizeof ( Vertex ) );
	ctx->Unmap ( g_pVertexBuffer , 0 );
	float L = 0.0f , R = ( float ) WINDOW_WIDTH , T = 0.0f , B = ( float ) WINDOW_HEIGHT;
	float mvp [ 4 ][ 4 ] = { { 2.0f / ( R - L ),0,0,0 },{ 0,2.0f / ( T - B ),0,0 },{ 0,0,0.5f,0 },{ ( R + L ) / ( L - R ),( T + B ) / ( B - T ),0.5f,1.0f } };
	ctx->UpdateSubresource ( g_pConstantBuffer , 0 , NULL , &mvp , 0 , 0 );
	UINT stride = sizeof ( Vertex ) , offset = 0;
	ctx->IASetInputLayout ( g_pInputLayout );
	ctx->IASetVertexBuffers ( 0 , 1 , &g_pVertexBuffer , &stride , &offset );
	ctx->IASetPrimitiveTopology ( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	ctx->VSSetShader ( g_pVertexShader , nullptr , 0 );
	ctx->VSSetConstantBuffers ( 0 , 1 , &g_pConstantBuffer );
	ctx->PSSetShader ( g_pPixelShader , nullptr , 0 );
	ctx->GSSetShader ( nullptr , nullptr , 0 );
	ctx->HSSetShader ( nullptr , nullptr , 0 );
	ctx->DSSetShader ( nullptr , nullptr , 0 );
	ID3D11ShaderResourceView* nullSRV [ 1 ] = { nullptr };
	ctx->PSSetShaderResources ( 0 , 1 , nullSRV );
	const float blend_factor [ 4 ] = { 0.f, 0.f, 0.f, 0.f };
	ctx->OMSetBlendState ( g_pBlendState , blend_factor , 0xffffffff );
	ctx->OMSetDepthStencilState ( g_pDepthStencilState , 0 );
	ctx->RSSetState ( g_pRasterizerState );
	D3D11_VIEWPORT vp = { 0.0f, 0.0f, ( float ) WINDOW_WIDTH, ( float ) WINDOW_HEIGHT, 0.0f, 1.0f };
	ctx->RSSetViewports ( 1 , &vp );
	D3D11_RECT r = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	ctx->RSSetScissorRects ( 1 , &r );
	ctx->Draw ( ( UINT ) g_vertices.size ( ) , 0 );
}
void Draw::AddFilledRect ( float x , float y , float w , float h , Color color ) { DWORD col = color; g_vertices.push_back ( { x,y,0.5f,col } ); g_vertices.push_back ( { x + w,y,0.5f,col } ); g_vertices.push_back ( { x,y + h,0.5f,col } ); g_vertices.push_back ( { x + w,y,0.5f,col } ); g_vertices.push_back ( { x + w,y + h,0.5f,col } ); g_vertices.push_back ( { x,y + h,0.5f,col } ); }
void Draw::AddRect ( float x , float y , float w , float h , Color color , float thickness ) { AddFilledRect ( x , y , w , thickness , color ); AddFilledRect ( x , y + h - thickness , w , thickness , color ); AddFilledRect ( x , y , thickness , h , color ); AddFilledRect ( x + w - thickness , y , thickness , h , color ); }