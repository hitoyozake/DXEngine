#pragma once

#include <vector>

#include <sdkddkver.h>
#include <D3D11.h>
#include <DXGIType.h>
#include <d3dcompiler.h>
#include <DirectXMath.h> //namespace DirectX��Ԏg�p
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
		uint32_t count_;						//���_�J�E���g��
		uint32_t start_index_;					//�J�n�ԍ�
		vector4 diffuse_;						//�g�U���ːF
		vector3 specular_;						//���ʔ��ːF
		vector3 emissive_;						//���Ȕ����F
		ID3D11ShaderResourceView * srv_;		//�e�N�X�`���f�[�^
	};


	class meshdata
	{
	public:
		void release();

		bool createbuffer( ID3D11Device * device );
		void draw( ID3D11DeviceContext * context );
		uint32_t getnumsubsets();
		subsetdata & subset( int const index );

		ID3D11Buffer * vertex_buffer_;		//���_�o�b�t�@
		ID3D11Buffer * material_buffer_;	//�}�e���A���o�b�t�@
		ID3D11Buffer * index_buffer_;		//�C���f�b�N�X�o�b�t�@

		uint32_t offset_;					//���_�o�b�t�@�̃I�t�Z�b�g
		uint32_t stride_;					//���_�o�b�t�@�̃X�g���C�h

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
			D3DXVECTOR3 camera_position = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );	//�J�����̈ʒu
			D3DXVECTOR3 camera_target = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );	//�J�����̒����_
			D3DXVECTOR3	camera_upvector = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );	//�J�����̏�������ǂ̃x�N�g���ɂ��邩

			double const pi_over4 = boost::math::constants::pi< double >();	//���p

			//�s��v�Z
			D3DXMatrixIdentity( &world_ );
			D3DXMatrixLookAtLH( &view_, &camera_position, &camera_target, &camera_upvector );
			D3DXMatrixPerspectiveFovLH( &projection_, pi_over4, 0.0f, 0.1f, 1000.0f );//��3�����͏C���̕K�v���肩��
		}

		void on_release();
		bool on_create_device( ID3D11Device * device, DXGI_SURFACE_DESC const * desc );
		void on_destroy_device();
		bool on_resize_swap_chain( ID3D11Device * device, IDXGISwapChain * swap_chain, DXGI_SURFACE_DESC const * desc );
		void on_update( double time, float elapsed_time );
		void on_render( ID3D11Device * device, ID3D11DeviceContext * context, double time, float elapsed_time );

	private:
		modeldata model_;			//���f���f�[�^
		D3DXMATRIX world_;			//���[���h�s��
		D3DXMATRIX view_;			//�r���[�s��
		D3DXMATRIX projection_;		//�ˉe�s��
		D3DXVECTOR3 view_dir_;		//�����x�N�g��
		D3DXVECTOR3 light_dir_;		//���C�g�̕����x�N�g��



	};


}






