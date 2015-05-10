#pragma once

#include "DirectXTex.h"

#include <sdkddkver.h>
#include <D3D11.h>
#include <DXGIType.h>
#include <d3dcompiler.h>
#include <DirectXMath.h> //namespace DirectX空間使用

#include <directxpackedvector.h>
#include <xnamath.h>
#include <string>
#include <memory>
#include <vector>
#include <locale>
#include <Windows.h>
#include <boost/format.hpp>
#include <DXGIFormat.h>


struct context
{
	boost::shared_ptr< ID3D11Device > i_dev_;
	boost::shared_ptr< ID3D11DeviceContext > i_dev_context_;
	boost::shared_ptr< IDXGISwapChain > i_swap_chain_;
	boost::shared_ptr< ID3D11RenderTargetView > i_render_target_view_;
	boost::shared_ptr< ID3D11DepthStencilView > i_depth_stencil_view_;
	boost::shared_ptr< ID3D11VertexShader > i_vertex_shader_;
	boost::shared_ptr< ID3D11InputLayout > i_vertex_layout_;
	boost::shared_ptr< ID3D11GeometryShader > i_geometry_shader_;
	boost::shared_ptr< ID3D11PixelShader > i_pixel_shader_;
	boost::shared_ptr< ID3D11Buffer > i_vertex_buffer_;
	ID3D11Buffer * const_buffer_;
	ID3D11Buffer * cbuff_obj_;

	ID3D11Texture2D*		g_pDepthStencil;


	bool inited_flag_dx11_ = false;
	D3D_DRIVER_TYPE driver_type_ = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL feature_level_ = D3D_FEATURE_LEVEL_11_0;
};

//頂点座標
struct custom_vertex
{
	XMFLOAT3 position_;
};

//無名名前空間
extern context * cntxt;



struct VertexData
{
	XMFLOAT3 pos;
	XMFLOAT2 tex;
};

//シェーダ定数バッファ
struct ConstBuff
{
	XMMATRIX mtxProj;
	XMMATRIX mtxView;
};

struct CBuffObject
{
	XMMATRIX mtxWorld;
	XMFLOAT4 v4MeshColor;
};


struct input_layout
{

};
