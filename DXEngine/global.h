
#pragma once

#include "DirectXTex.h"

#include <sdkddkver.h>
#include <D3D11.h>
#include <DXGIType.h>
#include <d3dcompiler.h>
#include <DirectXMath.h> //namespace DirectX‹óŠÔŽg—p

#include <directxpackedvector.h>
#include <xnamath.h>
#include <string>
#include <memory>
#include <vector>
#include <locale>
#include <Windows.h>
#include <boost/format.hpp>
#include <DXGIFormat.h>

#include "struct.h"

#include "shader.h"

#include "common.h"

#include "time_manager.h"

//****************************************//
extern ID3D11SamplerState *		g_pSamplerLinear;

extern std::vector< ID3D11Buffer * > p_vertex_buffers;
extern ID3D11Buffer * poly_test_buffer[ 3 ];
extern ID3D11Buffer * poly_test_buffer_index[ 3 ];

extern ID3D11ShaderResourceView * g_pShaderResView;
extern ID3D11Texture2D *		g_pDepthStencil;
extern ID3D11DepthStencilView *	g_pDepthStencilView;
extern ID3D11RenderTargetView *	g_pRenderTargetView;
extern ID3D11Buffer *			g_pConstBuffer;
