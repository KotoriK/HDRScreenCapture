#pragma once
#include <d3d11.h>
// input a DirectX texture, output a processed texture
class IDirectXPostProcessor {
	virtual int Process(ID3D11Texture2D* input, ID3D11Texture2D* output);
	virtual int ProcessInPlace(ID3D11Texture2D* texture);
};