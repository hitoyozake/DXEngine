#pragma once

#include <vector>

#include <sdkddkver.h>
#include <D3D11.h>
#include <DXGIType.h>
#include <d3dcompiler.h>
#include <DirectXMath.h> //namespace DirectX空間使用
#include <DXGIFormat.h>
#include <D3DX10math.h>

#include <boost/math/constants/constants.hpp>

namespace model
{
	struct matrix
	{
		union
		{
			struct
			{
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
				float _41, _42, _43, _44;
			};
			float m[ 4 ][ 4 ];
		};
	};

	struct vector4
	{
		float  x_, y_, z_, w_;
	};

	struct vector3
	{
		float x_, y_, z_;
	};

	struct subsetdata
	{
		uint32_t count_;						//頂点カウント数
		uint32_t start_index_;					//開始番号
		vector4 diffuse_;						//拡散反射色
		vector3 specular_;						//鏡面反射色
		vector3 emissive_;						//自己発光色
		ID3D11ShaderResourceView * srv_;		//テクスチャデータ
	};


	class meshdata
	{
	public:
		void release();

		bool createbuffer( ID3D11Device * device );
		void draw( ID3D11DeviceContext * context );
		uint32_t getnumsubsets();
		subsetdata & subset( int const index );

		ID3D11Buffer * vertex_buffer_;		//頂点バッファ
		ID3D11Buffer * material_buffer_;	//マテリアルバッファ
		ID3D11Buffer * index_buffer_;		//インデックスバッファ

		uint32_t offset_;					//頂点バッファのオフセット
		uint32_t stride_;					//頂点バッファのストライド

	private:
		std::vector< subsetdata > subset_;


	};


	class modeldata
	{
	public:
		modeldata();
		~modeldata();
		void draw();
		bool create_buffer();
		
		std::vector< matrix > get_skin_transform_matrix();
		std::vector< D3DXMATRIX > get_d3dx_skin_transform_matrix();
		void update( bool const stop_flag = false, float const speed = 1.0f );

	private:
		std::vector< meshdata > meshdata_;
		matrix  root_;
		
	};

	class renderer
	{
	public:
		renderer()
		{
			D3DXMatrixIdentity( & world_ );
			D3DXMatrixIdentity( & view_ );
			D3DXMatrixIdentity( & projection_ );

			view_dir_	= D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
			light_dir_ = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

		}

		bool on_init()
		{
			D3DXVECTOR3 camera_position = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );	//カメラの位置
			D3DXVECTOR3 camera_target = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );	//カメラの注視点
			D3DXVECTOR3	camera_upvector = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );	//カメラの上方向をどのベクトルにするか

			double const pi_over4 = boost::math::constants::pi< double >();	//直角

			//行列計算
			D3DXMatrixIdentity( &world_ );
			D3DXMatrixLookAtLH( &view_, &camera_position, &camera_target, &camera_upvector );
			D3DXMatrixPerspectiveFovLH( &projection_, pi_over4, 0.0f, 0.1f, 1000.0f );//第3引数は修正の必要ありかも
		}

		void on_release();
		bool on_create_device( ID3D11Device * device, DXGI_SURFACE_DESC const * desc );
		void on_destroy_device();
		bool on_resize_swap_chain( ID3D11Device * device, IDXGISwapChain * swap_chain, DXGI_SURFACE_DESC const * desc );
		void on_update( double time, float elapsed_time );
		void on_render( ID3D11Device * device, ID3D11DeviceContext * context, double time, float elapsed_time );

	private:
		modeldata model_;			//モデルデータ
		D3DXMATRIX world_;			//ワールド行列
		D3DXMATRIX view_;			//ビュー行列
		D3DXMATRIX projection_;		//射影行列
		D3DXVECTOR3 view_dir_;		//視線ベクトル
		D3DXVECTOR3 light_dir_;		//ライトの方向ベクトル



	};


}






