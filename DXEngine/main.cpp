#include "DirectXTex.h"

#include <sdkddkver.h>
#include <D3D11.h>
#include <DXGIType.h>
#include <d3dcompiler.h>
#include <DirectXMath.h> //namespace DirectX空間使用
#include <DXGIFormat.h>


#include <directxpackedvector.h>
#include <xnamath.h>
#include <string>
#include <memory>
#include <vector>
#include <locale>
#include <Windows.h>
#include <boost/format.hpp>

#include "struct.h"

#include "shader.h"

#include "common.h"

#include "time_manager.h"

#include "global.h"

#include "box.h"


ID3D11ShaderResourceView* g_pShaderResView = NULL;
ID3D11Texture2D*		g_pDepthStencil = NULL;
ID3D11DepthStencilView*	g_pDepthStencilView = NULL;
ID3D11RenderTargetView*	g_pRenderTargetView = NULL;
ID3D11Buffer*			g_pConstBuffer = NULL;


//****************************************//
//プロトタイプ宣言
HRESULT init_dx11( HWND hwnd );
void exit_dx11();
void render_dx11();

HRESULT init_window( HINSTANCE h_instance, int n_cmd_show );
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

HRESULT create_buffer( custom_vertex const * const vertices, std::size_t const vertices_num );
std::array< custom_vertex, 4 > create_vertices( int const x, int const y, int const width, int const height );
//****************************************//
HWND global_h_wnd;
//****************************************//
ID3D11SamplerState *		g_pSamplerLinear = nullptr;


std::vector< ID3D11Buffer * > p_vertex_buffers;
ID3D11Buffer * poly_test_buffer[ 3 ];
ID3D11Buffer * poly_test_buffer_index[ 3 ];


void release_buffer( ID3D11Buffer ** buf )
{
	( *buf )->Release();
	*buf = nullptr;
}

HRESULT create_index_buffer( custom_vertex const * const vertices, std::size_t const index_num )
{
	HRESULT hr;

	//頂点バッファの設定
	D3D11_BUFFER_DESC bd;
	ZeroMemory( std::addressof( bd ), sizeof bd );
	bd.Usage = D3D11_USAGE_DYNAMIC;

	bd.ByteWidth = ( sizeof custom_vertex ) * index_num;//sizeof XMFLOAT3
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;//D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//0

	//サブリソースの設定
	D3D11_SUBRESOURCE_DATA init_data;
	ZeroMemory( std::addressof( init_data ), sizeof init_data );
	init_data.pSysMem = vertices;
	
	ID3D11Buffer * p_vertex_buffer = nullptr;

	//頂点バッファ生成
	hr = cntxt->i_dev_->CreateBuffer( std::addressof( bd ), std::addressof( init_data ), std::addressof( p_vertex_buffer ) );

	if( FAILED( hr ) )
	{
		return hr;
	}

	//入力アセンブラに頂点バッファを設定
	UINT stride = sizeof custom_vertex;
	UINT offset = 0;

	//入力アセンブラに頂点バッファを設定
	p_vertex_buffers.push_back( p_vertex_buffer );
	cntxt->i_dev_context_->IASetVertexBuffers( 0, 1, std::addressof( p_vertex_buffer ), std::addressof( stride ), std::addressof( offset ) );

	return hr;
}


HRESULT create_index_buffer( custom_vertex const * const vertices, std::size_t const index_num, int const index )
{
	HRESULT hr;

	//頂点バッファの設定
	D3D11_BUFFER_DESC bd;
	ZeroMemory( std::addressof( bd ), sizeof bd );
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = ( sizeof custom_vertex ) * index_num;//sizeof XMFLOAT3
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;//D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	//サブリソースの設定
	D3D11_SUBRESOURCE_DATA init_data;
	ZeroMemory( std::addressof( init_data ), sizeof init_data );
	init_data.pSysMem = vertices;

	ID3D11Buffer * p_vertex_buffer = nullptr;

	//頂点バッファ生成
	hr = cntxt->i_dev_->CreateBuffer( std::addressof( bd ), std::addressof( init_data ), std::addressof( p_vertex_buffer ) );

	if( FAILED( hr ) )
	{
		return hr;
	}

	//入力アセンブラに頂点バッファを設定
	UINT stride = sizeof custom_vertex;
	UINT offset = 0;

	//入力アセンブラに頂点バッファを設定
	p_vertex_buffers[ index ] = p_vertex_buffer;

	cntxt->i_dev_context_->IASetVertexBuffers( 0, 1, std::addressof( p_vertex_buffer ), std::addressof( stride ), std::addressof( offset ) );

	return hr;
}


float convert_cordinate( int const position, int window_length )
{
	return static_cast< float >( position - window_length / 2 ) / ( ( window_length != 0 ? window_length : 100000 ) / 2.0 );
}

void move_texture( int const index, int x, int y, int width, int height )
{
	auto const v = create_vertices( x, y, width, height );
	
	D3D11_MAPPED_SUBRESOURCE msr;
	cntxt->i_dev_context_->Map( p_vertex_buffers[ index ], 0, D3D11_MAP_WRITE_DISCARD, 0, std::addressof( msr ) );
	memcpy( msr.pData, v.data(), sizeof( v[ 0 ] ) * 4 );
	cntxt->i_dev_context_->Unmap( p_vertex_buffers[ index ], 0 );

}


//頂点の定義
std::array< custom_vertex, 4 > create_vertices( int const x, int const y, int const width, int const height )
{
	std::array< custom_vertex, 4 > const ret =
	{
		XMFLOAT3( convert_cordinate( x + width, 640 ), convert_cordinate( y + height, 480 ), 0.5f ),
		XMFLOAT3( convert_cordinate( x + width, 640 ), convert_cordinate( y, 480 ), 0.5f ),
		XMFLOAT3( convert_cordinate( x, 640 ), convert_cordinate( y + height, 480 ), 0.5f ),
		XMFLOAT3( convert_cordinate( x, 640 ), convert_cordinate( y, 480 ), 0.5f )
	};

	return ret;
};

HRESULT create_buffer( custom_vertex const * const vertices, std::size_t const vertices_num )
{
	HRESULT hr;

	//頂点バッファの設定
	D3D11_BUFFER_DESC bd;
	ZeroMemory( std::addressof( bd ), sizeof bd );
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = ( sizeof custom_vertex ) * vertices_num;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	//サブリソースの設定
	D3D11_SUBRESOURCE_DATA init_data;
	ZeroMemory( std::addressof( init_data ), sizeof init_data );
	init_data.pSysMem = vertices;

	ID3D11Buffer * p_vertex_buffer = nullptr;

	//頂点バッファ生成
	hr = cntxt->i_dev_->CreateBuffer( std::addressof( bd ), std::addressof( init_data ), std::addressof( p_vertex_buffer ) );
	

	if( FAILED( hr ) )
	{
		return hr;
	}


	//入力アセンブラに頂点バッファを設定
	UINT stride = sizeof custom_vertex;
	UINT offset = 0;

	//入力アセンブラに頂点バッファを設定
	cntxt->i_dev_context_->IASetVertexBuffers( 0, 1, std::addressof( p_vertex_buffer ), std::addressof( stride ), std::addressof( offset ) );

	cntxt->i_vertex_buffer_.reset( p_vertex_buffer );

	return hr;
}


HRESULT init_dx11( HWND hwnd )
{
	HRESULT hr = S_OK;

	tagRECT rect;

	GetClientRect( hwnd, std::addressof( rect ) );
	auto const width = rect.right - rect.left;
	auto const height = rect.bottom - rect.top;

	cntxt =  new context();

	D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;
	cntxt->driver_type_ = D3D_DRIVER_TYPE_HARDWARE;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( std::addressof( sd ), sizeof sd );

	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60; //fps設定．refresh Rate
	sd.BufferDesc.RefreshRate.Denominator = 1; //1/60 = 60fps
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd; //出力先ウインドウにハンドルをセット
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE; //WINDOWにするかどうか

	//一時変数
	IDXGISwapChain * swap_chain_ptr = nullptr;
	ID3D11Device * dev_ptr = nullptr;
	ID3D11DeviceContext * dev_context_ptr = nullptr;

	hr = D3D11CreateDeviceAndSwapChain( NULL, cntxt->driver_type_, NULL, 
		false, std::addressof( feature_level ), 1, D3D11_SDK_VERSION, std::addressof( sd ),
		std::addressof( swap_chain_ptr ), std::addressof( dev_ptr ), std::addressof( cntxt->feature_level_ ), std::addressof( dev_context_ptr ) );

	//for文で成功したら脱出みたいな処理のほうがスマートそう(このコードだとネストが深くなっていく

	if( FAILED( hr ) )
	{
		//feature_levelを切り替えて再度試みる
		feature_level = D3D_FEATURE_LEVEL_10_1;

		hr = D3D11CreateDeviceAndSwapChain( NULL, cntxt->driver_type_, NULL,
			false, std::addressof( feature_level ), 1, D3D11_SDK_VERSION, std::addressof( sd ),
			std::addressof( swap_chain_ptr ), std::addressof( dev_ptr ), std::addressof( cntxt->feature_level_ ), std::addressof( dev_context_ptr ) );

		if( FAILED( hr ) )
		{
			feature_level = D3D_FEATURE_LEVEL_10_0;

			hr = D3D11CreateDeviceAndSwapChain( NULL, cntxt->driver_type_, NULL,
				false, std::addressof( feature_level ), 1, D3D11_SDK_VERSION, std::addressof( sd ),
				std::addressof( swap_chain_ptr ), std::addressof( dev_ptr ), std::addressof( cntxt->feature_level_ ), std::addressof( dev_context_ptr ) );

			if( FAILED( hr ) )
				return hr;
		}
	}
	
	//初期化に成功したので一時変数を代入する
	cntxt->i_swap_chain_.reset( swap_chain_ptr );
	cntxt->i_dev_.reset( dev_ptr );
	cntxt->i_dev_context_.reset( dev_context_ptr );

	//スワップチェインに用意されたバッファ(2Dテクスチャ)を取得
	ID3D11Texture2D * back_buff = nullptr;

	hr = cntxt->i_swap_chain_->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast< LPVOID * >( std::addressof( back_buff ) ) );
	
	if( FAILED( hr ) || back_buff == nullptr )
	{
		return hr;
	}

	ID3D11RenderTargetView * render_target_view_ptr = nullptr;

	hr = cntxt->i_dev_->CreateRenderTargetView( back_buff, nullptr, std::addressof( render_target_view_ptr ) );
	
	if( FAILED( hr ) )
	{
		return hr;
	}
	back_buff->Release( );

	std::array< ID3D11RenderTargetView *, 1 > rtv = { cntxt->i_render_target_view_.get() };
	cntxt->i_dev_context_->OMSetRenderTargets( 1, rtv.data(), nullptr );

	//depth stencil buffer
	// デプスバッファ作成
	D3D11_TEXTURE2D_DESC decs_depth;
	ZeroMemory( &decs_depth, sizeof( decs_depth ) );
	decs_depth.Width = width;
	decs_depth.Height = height;
	decs_depth.MipLevels = 1;
	decs_depth.ArraySize = 1;
	decs_depth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	decs_depth.SampleDesc.Count = 1;
	decs_depth.SampleDesc.Quality = 0;
	decs_depth.Usage = D3D11_USAGE_DEFAULT;
	decs_depth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	decs_depth.CPUAccessFlags = 0;
	decs_depth.MiscFlags = 0;

	ID3D11DepthStencilView * tmp_depth_stencil_view_ = nullptr;

	hr = cntxt->i_dev_->CreateTexture2D( &decs_depth, NULL, & cntxt->g_pDepthStencil );

	if( FAILED( hr ) )
	{
		return hr;
	}

	// DepthStencilView作成
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory( &descDSV, sizeof( descDSV ) );
	descDSV.Format = decs_depth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	hr = cntxt->i_dev_->CreateDepthStencilView( cntxt->g_pDepthStencil, &descDSV, &g_pDepthStencilView );
	if( FAILED( hr ) )
	{
		return hr;
	}

	// ターゲットビュー登録
	cntxt->i_dev_context_->OMSetRenderTargets( 1, & render_target_view_ptr, g_pDepthStencilView );
	//これでレンダリング結果がスワップチェインのバッファに書き込まれる

	cntxt->i_render_target_view_.reset( render_target_view_ptr );

	cntxt->i_depth_stencil_view_.reset( g_pDepthStencilView );

	//setting of view port
	D3D11_VIEWPORT vp;
	vp.Width = static_cast< FLOAT >( width );
	vp.Height = static_cast< FLOAT >( height );
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	cntxt->i_dev_context_->RSSetViewports( 1, std::addressof( vp ) );

	//頂点シェーダをコンパイル
	ID3DBlob * p_vs_blob = nullptr;

	hr = shader::compile_shader( TEXT( "hoge3D.fx" ), "vsMain", "vs_4_0", std::addressof( p_vs_blob ) );

	if( FAILED( hr ) )
	{
		return hr;
	}

	//頂点シェーダ生成
	ID3D11VertexShader * p_vertex_shader = nullptr;

	hr = cntxt->i_dev_->CreateVertexShader( p_vs_blob->GetBufferPointer(), p_vs_blob->GetBufferSize(), nullptr, std::addressof( p_vertex_shader ) );

	if( FAILED( hr ) )
	{
		p_vs_blob->Release();
		p_vs_blob = nullptr;
		return hr;
	}

	cntxt->i_vertex_shader_.reset( p_vertex_shader );


	//入力レイアウトの定義
	//cntxt->i_dev_context_->IASetInputLayout( cntxt->i_vertex_layout_.get() );

	std::array< D3D11_INPUT_ELEMENT_DESC, 2 > layout = {
		D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	/*std::array< D3D11_INPUT_ELEMENT_DESC, 2 > layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};*/
	//入力レイアウトを生成
	ID3D11InputLayout * p_vertex_layout = nullptr;
	hr = cntxt->i_dev_->CreateInputLayout( layout.data(), layout.size(), p_vs_blob->GetBufferPointer(), p_vs_blob->GetBufferSize(), std::addressof( p_vertex_layout ) );

	if( FAILED( hr ) )
	{
		return hr;
	}

	cntxt->i_vertex_layout_.reset( p_vertex_layout );

	cntxt->i_dev_context_->IASetInputLayout( cntxt->i_vertex_layout_.get() );


	//ジオメトリシェーダコンパイル
	ID3DBlob * p_gs_blob = nullptr;

	hr = shader::compile_shader( TEXT( "./hoge.fx" ), "GSFunc", "gs_4_0", std::addressof( p_gs_blob ) );//"GSFunc", "gs_4_0", std::addressof( p_gs_blob ) );

	if( FAILED( hr ) )
	{
		return hr;
	}

	ID3D11GeometryShader * p_gs = nullptr;
	//ジオメトリシェーダを生成
	hr = cntxt->i_dev_->CreateGeometryShader( p_gs_blob->GetBufferPointer(), p_gs_blob->GetBufferSize(), nullptr, std::addressof( p_gs ) );	
	cntxt->i_geometry_shader_.reset( p_gs );

	p_gs_blob->Release( );
	p_gs_blob = nullptr;

	if( FAILED( hr ) )
	{
		return hr;
	}

	//ピクセルシェーダのコンパイル
	ID3DBlob * p_ps_blob = nullptr;

	hr = shader::compile_shader( TEXT( "./hoge3D.fx" ), "psMain", "ps_4_0", std::addressof( p_ps_blob ) );

	if( FAILED( hr ) )
	{
		return hr;
	}

	ID3D11PixelShader * p_shader_ps = nullptr;

	//ピクセルシェーダ生成
	hr = cntxt->i_dev_->CreatePixelShader( p_ps_blob->GetBufferPointer(),
		p_ps_blob->GetBufferSize(),
		nullptr,
		std::addressof( p_shader_ps ) );

	cntxt->i_pixel_shader_.reset( p_shader_ps );

	p_ps_blob->Release();
	p_ps_blob = nullptr;

	if( FAILED( hr ) )
	{
		return hr;
	}

	cntxt->inited_flag_dx11_ = true;

	auto const v = convert_cordinate( 310, 640 );

	//頂点の定義
	std::array< custom_vertex, 4 > vertices =
		create_vertices( 310, 280, 20, 20 );

	//頂点の定義
	std::array< custom_vertex, 4 > vertices2 =
	{
		XMFLOAT3( 0.0f, 0.2f, 0.5f ),
		XMFLOAT3( 0.0f, 0.0f, 0.5f ),
		XMFLOAT3( -0.1f,  0.2f, 0.5f ),
		XMFLOAT3( -0.1f, 0.0f, 0.5f )

	};

	create_index_buffer( vertices.data( ), vertices.size( ) );

	create_index_buffer( vertices2.data(), vertices2.size() );

	//プリミティブの種類を設定
	cntxt->i_dev_context_->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	//cntxt->i_dev_context_->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );


	box::make_box( -1.5f, -1.2f, 0, & poly_test_buffer[ 0 ], & poly_test_buffer_index[ 0 ] );
	box::make_box( 0.0f, 1.1f , 0, &poly_test_buffer[ 1 ], &poly_test_buffer_index[ 1 ] );
	box::make_box( 1.6f, 4.2f, 0, &poly_test_buffer[ 2 ], &poly_test_buffer_index[ 2 ] );

	
	//テクスチャ作成
	// 画像ファイル読み込み DirectXTex
	
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage image;
	hr = LoadFromWICFile( L"texture.png", 0, &metadata, image );

	if( FAILED( hr ) )
	{
		return hr;
	}

	// 画像からシェーダリソースView DirectXTex
	hr = CreateShaderResourceView( cntxt->i_dev_.get(), image.GetImages(), image.GetImageCount(), metadata, & g_pShaderResView );
	if( FAILED( hr ) )
	{
		return hr;
	}

	// SamplerState作成
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof( sampDesc ) );
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = cntxt->i_dev_->CreateSamplerState( &sampDesc, &g_pSamplerLinear );
	if( FAILED( hr ) )
	{
		return hr;
	}

	//定数バッファ作成
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof( bd ) );
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof( ConstBuff );
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;


		hr = cntxt->i_dev_->CreateBuffer( std::addressof( bd ), nullptr, std::addressof( cntxt->const_buffer_ ) );


		if( FAILED( hr ) )
		{
			return hr;
		}

		bd.ByteWidth = sizeof CBuffObject;

		hr = cntxt->i_dev_->CreateBuffer( std::addressof( bd ), nullptr, std::addressof( cntxt->cbuff_obj_ ) );

		if( FAILED( hr ) )
		{
			return hr;
		}

	}



	// 定数バッファ
	ConstBuff cbuff;
	XMVECTOR Eye = XMVectorSet( 0.0f, 3.0f, -6.0f, 0.0f );
	XMVECTOR At = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	cbuff.mtxView = XMMatrixTranspose( XMMatrixLookAtLH( Eye, At, Up ) );
	cbuff.mtxProj = XMMatrixTranspose( XMMatrixPerspectiveFovLH( XM_PIDIV4, width / ( FLOAT )height, 0.01f, 100.0f ) );
	cntxt->i_dev_context_->UpdateSubresource( cntxt->const_buffer_, 0, NULL, std::addressof( cbuff ), 0, 0 );

	return S_OK;

}

void exit_dx11()
{
	//delete cntxt;
	//cntxt = nullptr;
	//cntxt.reset();
}

void render_dx11()
{
	if( !cntxt || !cntxt->inited_flag_dx11_ ) return;

	//指定色で画面をクリアにする．r = 0, g = 0.125, b = 0.3, alpha = 1.0f;rgb = {};
	std::array< float, 4 > clear_color = { 0.1f, 0.0f, 0.125f , 1.0f };


	//デプスバッファをクリア
	cntxt->i_dev_context_->ClearDepthStencilView( cntxt->i_depth_stencil_view_.get(), D3D11_CLEAR_DEPTH, 1.0f, 0 );

	cntxt->i_dev_context_->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	//入力レイアウト

	cntxt->i_dev_context_->ClearRenderTargetView( cntxt->i_render_target_view_.get(), clear_color.data() );
	cntxt->i_dev_context_->IASetInputLayout( cntxt->i_vertex_layout_.get( ) );


	UINT offset = 0;
	UINT stride = sizeof VertexData;

	for( int i = 0; i < 3; ++i )
	{
		cntxt->i_dev_context_->IASetIndexBuffer( poly_test_buffer_index[ i ], DXGI_FORMAT_R16_UINT, 0 );
		cntxt->i_dev_context_->IASetVertexBuffers( 0, 1, std::addressof( poly_test_buffer[ i ] ), std::addressof( stride ), std::addressof( offset ) );


		cntxt->i_dev_context_->VSSetShader( cntxt->i_vertex_shader_.get( ), nullptr, 0 );
		//cntxt->i_dev_context_->GSSetShader( cntxt->i_geometry_shader_.get(), nullptr, 0 );
		cntxt->i_dev_context_->PSSetShader( cntxt->i_pixel_shader_.get( ), nullptr, 0 );

		//回転
		static float g_fRotY = 0.0f;
		static DWORD dwTimeStart = 0;
		DWORD dwTimeCur = GetTickCount( );
		if( dwTimeStart == 0 ) dwTimeStart = dwTimeCur;
		g_fRotY = ( dwTimeCur - dwTimeStart ) / 1000.0f;



		// 姿勢行列と色を更新
		CBuffObject cbobj;
		cbobj.mtxWorld = XMMatrixTranspose( XMMatrixRotationY( g_fRotY ) );
		cbobj.v4MeshColor = XMFLOAT4( sinf( g_fRotY )*0.5f + 0.5f, 1.0f, 1.0f, 1.0f );
		cntxt->i_dev_context_->UpdateSubresource( cntxt->cbuff_obj_, 0, NULL, &cbobj, 0, 0 );

		cntxt->i_dev_context_->VSSetConstantBuffers( 1, 1, std::addressof( cntxt->cbuff_obj_ ) );

		cntxt->i_dev_context_->VSSetConstantBuffers( 0, 1, &cntxt->const_buffer_ );

		cntxt->i_dev_context_->PSSetConstantBuffers( 1, 1, std::addressof( cntxt->cbuff_obj_ ) );
		cntxt->i_dev_context_->PSSetShaderResources( 0, 1, &g_pShaderResView );
		cntxt->i_dev_context_->PSSetSamplers( 0, 1, &g_pSamplerLinear );

		cntxt->i_dev_context_->DrawIndexed( 36, 0, 0 );
	}





	

	/*static int counter = 0;

	for( int i = 0; i < p_vertex_buffers.size(); ++i )
	{
		UINT stride = sizeof custom_vertex;
		UINT offset = 0;

		if( i == 0 )
		{
			move_texture( i, 300 + ++counter, 120, 10, 10 );
		}

		cntxt->i_dev_context_->IASetVertexBuffers( 0, 1, std::addressof( p_vertex_buffers[ i ] ), std::addressof( stride ), std::addressof( offset ) );
	
		cntxt->i_dev_context_->Draw( 4, 0 );



	}*/



	
	//結果をWindowに反映(バックバッファから表へコピー)
	cntxt->i_swap_chain_->Present( 0, 0 );
}

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
	)
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );

	setlocale( LC_ALL, "Japanese" );

	if( FAILED( init_window( hInstance, nCmdShow ) ) )
	{
		return 0;
	}

	if( FAILED( init_dx11( global_h_wnd ) ) )
	{
		exit_dx11();
		return 0;
	}

	MSG msg = { 0 };

	time_m::time_manager tm;
	int counter = 0;



	while( WM_QUIT != msg.message )
	{
		tm.update();
		auto const frame = tm.get_fps();
		auto const fpsstr = boost::format( "%0.3f fps" ) % frame;
		
		SetWindowText( global_h_wnd, static_cast< LPCSTR >( fpsstr.str().c_str() ) );
		

		if( PeekMessage( std::addressof( msg ), NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( std::addressof( msg ) );
			DispatchMessage( std::addressof( msg ) );
		}
		else
		{
			render_dx11();

			tm.wait_auto( );
		}


	}

	exit_dx11();
	return 0;
}

HRESULT init_window( HINSTANCE h_instance, int n_cmd_show )
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof WNDCLASSEX;
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = h_instance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground = reinterpret_cast< HBRUSH >( COLOR_WINDOW + 1 );
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "dx11WindowClass";
	wcex.hIconSm = NULL;

	if( !RegisterClassEx( std::addressof( wcex ) ) )
	{
		return E_FAIL;
	}

	RECT rc = { 0, 0, 640, 480 };

	AdjustWindowRect( std::addressof( rc ), WS_OVERLAPPEDWINDOW, FALSE );

	global_h_wnd = CreateWindow( "dx11WindowClass", "Direct11 Test", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, h_instance, NULL );

	if( ! global_h_wnd )
	{
		return E_FAIL;
	}

	ShowWindow( global_h_wnd, n_cmd_show );
	UpdateWindow( global_h_wnd );

	return S_OK;
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch( message )
	{
	case WM_PAINT:
		hdc = BeginPaint( hWnd, std::addressof( ps ) );
		EndPaint( hWnd, std::addressof( ps ) );
		break;
	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}

	return 0;
}