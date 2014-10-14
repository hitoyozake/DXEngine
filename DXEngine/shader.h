#pragma once

#include <D3Dcompiler.h> //D3DCOMPILE_ENABLE_STRICTNESS...
#include <d3dx11async.h> //D3DXCompileFromFile
#include <DirectXMath.h> //namespace DirectX‹óŠÔŽg—p

#include <windows.h>
#include "common.h"


namespace shader
{
	HRESULT compile_shader( TCHAR * sz_file_name, LPCSTR sz_entry_point, LPCSTR sz_shader_moidel, ID3DBlob ** pp_blobout );
}