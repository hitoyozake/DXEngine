#include "shader.h"

#include <sdkddkver.h>
#include <D3D11.h>


namespace shader
{
	HRESULT compile_shader( TCHAR * sz_file_name, LPCSTR sz_entry_point, LPCSTR sz_shader_model, ID3DBlob ** pp_blob_out )
	{
		HRESULT hr = S_OK;

		DWORD dw_shader_flags = D3DCOMPILE_ENABLE_STRICTNESS;

		ID3DBlob * p_error_blob;
		
		hr = D3DX11CompileFromFile( sz_file_name, NULL, NULL, sz_entry_point, sz_shader_model, dw_shader_flags, 0, NULL, pp_blob_out,
			std::addressof( p_error_blob ), NULL );

		if( FAILED( hr ) )
		{
			if( p_error_blob != NULL )
			{
				//ƒGƒ‰[•\Ž¦
			}
		}

		if( p_error_blob )
		{
			p_error_blob->Release();
			p_error_blob = nullptr;
		}

		return hr;
		
	}
}

