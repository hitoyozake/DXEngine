#include "struct.h"
#include "global.h"


namespace box
{
	void make_box( float const x, float const y, float const z, ID3D11Buffer ** buffer_index, ID3D11Buffer ** vertices_buffer )
	{
		HRESULT hr;
	
			//3D
			VertexData verticespoly[] = {
				{ XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
				{ XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
				{ XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },
				{ XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) },

				{ XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
				{ XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
				{ XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
				{ XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },

				{ XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },
				{ XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
				{ XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
				{ XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT2( 0.0f, 0.0f ) },

				{ XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
				{ XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT2( 0.0f, 1.0f ) },
				{ XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
				{ XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT2( 1.0f, 0.0f ) },

				{ XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT2( 0.0f, 1.0f ) },
				{ XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
				{ XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
				{ XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) },

				{ XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
				{ XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },
				{ XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
				{ XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
			};



			[ x, y, z ]( VertexData * v )
			{
				for( int i = 0; i < 24; ++i )
				{
					v[ i ].pos.x += x;
					v[ i ].pos.y += y;
					v[ i ].pos.z += z;
				}
			}( verticespoly );

			{
				D3D11_BUFFER_DESC bd;
				ZeroMemory( &bd, sizeof( bd ) );
				bd.Usage = D3D11_USAGE_DEFAULT;
				bd.ByteWidth = sizeof( VertexData ) * 24;
				bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				bd.CPUAccessFlags = 0;
				D3D11_SUBRESOURCE_DATA InitData;
				ZeroMemory( &InitData, sizeof( InitData ) );
				InitData.pSysMem = verticespoly;
				hr = cntxt->i_dev_->CreateBuffer( &bd, &InitData, buffer_index );
				if( FAILED( hr ) )
				{
					return;
				}

			}

		{
			// インデックスバッファ作成
			WORD indices[ ] = {
				3, 1, 0,
				2, 1, 3,

				6, 4, 5,
				7, 4, 6,

				11, 9, 8,
				10, 9, 11,

				14, 12, 13,
				15, 12, 14,

				19, 17, 16,
				18, 17, 19,

				22, 20, 21,
				23, 20, 22
			};

			D3D11_BUFFER_DESC bd;
			ZeroMemory( &bd, sizeof( bd ) );
			D3D11_SUBRESOURCE_DATA InitData;
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof( WORD )* 36;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;
			InitData.pSysMem = indices;
			hr = cntxt->i_dev_->CreateBuffer( &bd, &InitData, vertices_buffer );
			if( FAILED( hr ) )
			{
				return;
			}
		}
	}
}