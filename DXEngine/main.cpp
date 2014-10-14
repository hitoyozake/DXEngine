#include <sdkddkver.h>
#include <D3D11.h>
#include <DirectXMath.h> //namespace DirectX��Ԏg�p
#include <xnamath.h>
#include <memory>
#include <locale>
#include <Windows.h>
#include "shader.h"

#include "common.h"

//****************************************//
//�v���g�^�C�v�錾
HRESULT init_dx11( HWND hwnd );
void exit_dx11();
void render_dx11();

HRESULT init_window( HINSTANCE h_instance, int n_cmd_show );
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
//****************************************//
HWND global_h_wnd;
//****************************************//

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

	bool inited_flag_dx11_ = false;
	D3D_DRIVER_TYPE driver_type_ = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL feature_level_ = D3D_FEATURE_LEVEL_11_0;
};

//���_���W
struct custom_vertex
{
	XMFLOAT3 position_;
};

//�������O���
namespace
{
	context* cntxt;
	//std::unique_ptr< context > cntxt;
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
	sd.BufferDesc.RefreshRate.Numerator = 60; //fps�ݒ�Drefresh Rate
	sd.BufferDesc.RefreshRate.Denominator = 1; //1/60 = 60fps
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd; //�o�͐�E�C���h�E�Ƀn���h�����Z�b�g
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE; //WINDOW�ɂ��邩�ǂ���

	//�ꎞ�ϐ�
	IDXGISwapChain * swap_chain_ptr = nullptr;
	ID3D11Device * dev_ptr = nullptr;
	ID3D11DeviceContext * dev_context_ptr = nullptr;

	hr = D3D11CreateDeviceAndSwapChain( NULL, cntxt->driver_type_, NULL, 
		false, std::addressof( feature_level ), 1, D3D11_SDK_VERSION, std::addressof( sd ),
		std::addressof( swap_chain_ptr ), std::addressof( dev_ptr ), std::addressof( cntxt->feature_level_ ), std::addressof( dev_context_ptr ) );

	if( FAILED( hr ) )
	{
		return hr;
	}
	
	//�������ɐ��������̂ňꎞ�ϐ���������
	cntxt->i_swap_chain_.reset( swap_chain_ptr );
	cntxt->i_dev_.reset( dev_ptr );
	cntxt->i_dev_context_.reset( dev_context_ptr );

	//�X���b�v�`�F�C���ɗp�ӂ��ꂽ�o�b�t�@(2D�e�N�X�`��)���擾
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

	cntxt->i_render_target_view_.reset( render_target_view_ptr );

	std::array< ID3D11RenderTargetView *, 1 > rtv = { cntxt->i_render_target_view_.get() };
	cntxt->i_dev_context_->OMSetRenderTargets( 1, rtv.data(), nullptr );


	//setting of view port
	D3D11_VIEWPORT vp;
	vp.Width = static_cast< FLOAT >( width );
	vp.Height = static_cast< FLOAT >( height );
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	cntxt->i_dev_context_->RSSetViewports( 1, std::addressof( vp ) );

	//���_�V�F�[�_���R���p�C��
	ID3DBlob * p_vs_blob = nullptr;

	hr = shader::compile_shader( TEXT( "hoge.fx" ), "VSFunc", "vs_4_0", std::addressof( p_vs_blob ) );

	if( FAILED( hr ) )
	{
		return hr;
	}

	//���_�V�F�[�_����
	ID3D11VertexShader * p_vertex_shader = nullptr;

	hr = cntxt->i_dev_->CreateVertexShader( p_vs_blob->GetBufferPointer(), p_vs_blob->GetBufferSize(), nullptr, std::addressof( p_vertex_shader ) );

	if( FAILED( hr ) )
	{
		p_vs_blob->Release();
		p_vs_blob = nullptr;
		return hr;
	}

	cntxt->i_vertex_shader_.reset( p_vertex_shader );


	//���̓��C�A�E�g�̒�`
	cntxt->i_dev_context_->IASetInputLayout( cntxt->i_vertex_layout_.get() );

	std::array< D3D11_INPUT_ELEMENT_DESC, 1 > layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	//���̓��C�A�E�g�𐶐�
	ID3D11InputLayout * p_vertex_layout = nullptr;
	hr = cntxt->i_dev_->CreateInputLayout( layout.data(), layout.size(), p_vs_blob->GetBufferPointer(), p_vs_blob->GetBufferSize(), std::addressof( p_vertex_layout ) );

	if( FAILED( hr ) )
	{
		return hr;
	}

	cntxt->i_vertex_layout_.reset( p_vertex_layout );

	cntxt->i_dev_context_->IASetInputLayout( cntxt->i_vertex_layout_.get() );


	//�W�I���g���V�F�[�_�R���p�C��
	ID3DBlob * p_gs_blob = nullptr;

	hr = shader::compile_shader( TEXT( "./hoge.fx" ), "GSFunc", "gs_4_0", std::addressof( p_gs_blob ) );

	if( FAILED( hr ) )
	{
		return hr;
	}

	ID3D11GeometryShader * p_gs = nullptr;
	//�W�I���g���V�F�[�_�𐶐�
	hr = cntxt->i_dev_->CreateGeometryShader( p_gs_blob->GetBufferPointer(), p_gs_blob->GetBufferSize(), nullptr, std::addressof( p_gs ) );	
	cntxt->i_geometry_shader_.reset( p_gs );

	p_gs_blob->Release( );
	p_gs_blob = nullptr;

	if( FAILED( hr ) )
	{
		return hr;
	}

	//�s�N�Z���V�F�[�_�̃R���p�C��
	ID3DBlob * p_ps_blob = nullptr;

	hr = shader::compile_shader( TEXT( "./hoge.fx" ), "PSFunc", "ps_4_0", std::addressof( p_ps_blob ) );

	if( FAILED( hr ) )
	{
		return hr;
	}

	ID3D11PixelShader * p_shader_ps = nullptr;

	//�s�N�Z���V�F�[�_����
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

	//���_�̒�`
	std::array< custom_vertex, 3 > vertices =
	{
		XMFLOAT3( 0.0f, 0.5f, 0.5f ),
		XMFLOAT3( 0.5f, -0.5f, 0.5f ),
		XMFLOAT3( -0.5f, -0.5f, 0.5f )
	};

	//���_�o�b�t�@�̐ݒ�
	D3D11_BUFFER_DESC bd;
	ZeroMemory( std::addressof( bd ), sizeof bd );
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = ( sizeof custom_vertex ) * 3;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	
	//�T�u���\�[�X�̐ݒ�
	D3D11_SUBRESOURCE_DATA init_data;
	ZeroMemory( std::addressof( init_data ), sizeof init_data );
	init_data.pSysMem = vertices.data();

	ID3D11Buffer * p_vertex_buffer = nullptr;

	//���_�o�b�t�@����
	hr = cntxt->i_dev_->CreateBuffer( std::addressof( bd ), std::addressof( init_data ), std::addressof( p_vertex_buffer ) );

	if( FAILED( hr ) )
	{
		return hr;
	}

	//���̓A�Z���u���ɒ��_�o�b�t�@��ݒ�
	UINT stride = sizeof custom_vertex;
	UINT offset = 0;

	cntxt->i_dev_context_->IASetVertexBuffers( 0, 1, std::addressof( p_vertex_buffer ), std::addressof( stride ), std::addressof( offset ) );

	cntxt->i_vertex_buffer_.reset( p_vertex_buffer );

	//�v���~�e�B�u�̎�ނ�ݒ�
	cntxt->i_dev_context_->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

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

	//�w��F�ŉ�ʂ��N���A�ɂ���Dr = 0, g = 0.125, b = 0.3, alpha = 1.0f;rgb = {};
	std::array< float, 4 > clear_color = { 0.1f, 0.0f, 0.125f , 1.0f };
	cntxt->i_dev_context_->ClearRenderTargetView( cntxt->i_render_target_view_.get(), clear_color.data() );

	cntxt->i_dev_context_->VSSetShader( cntxt->i_vertex_shader_.get(), nullptr, 0 );
	cntxt->i_dev_context_->GSSetShader( cntxt->i_geometry_shader_.get(), nullptr, 0 );
	cntxt->i_dev_context_->PSSetShader( cntxt->i_pixel_shader_.get(), nullptr, 0 );
	cntxt->i_dev_context_->Draw( 3, 0 );

	//���ʂ�Window�ɔ��f
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

	while( WM_QUIT != msg.message )
	{
		if( PeekMessage( std::addressof( msg ), NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( std::addressof( msg ) );
			DispatchMessage( std::addressof( msg ) );
		}
		else
		{
			render_dx11();
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
	wcex.lpszClassName = L"dx11WindowClass";
	wcex.hIconSm = NULL;

	if( !RegisterClassEx( std::addressof( wcex ) ) )
	{
		return E_FAIL;
	}

	RECT rc = { 0, 0, 640, 480 };

	AdjustWindowRect( std::addressof( rc ), WS_OVERLAPPEDWINDOW, FALSE );

	global_h_wnd = CreateWindow( L"dx11WindowClass", L"Direct11 Test", WS_OVERLAPPEDWINDOW,
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