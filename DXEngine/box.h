#pragma once

#include "global.h"

namespace box
{
	void make_box( float x, float y, float z, ID3D11Buffer ** buffer_index, ID3D11Buffer ** vertices_buffer );
}