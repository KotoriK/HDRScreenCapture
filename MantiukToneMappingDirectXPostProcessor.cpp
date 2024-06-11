#include "IDirectXPostProcessor.h"
#include <d3d11.h>
class MantiukToneMappingDirectXPostProcessor : public IDirectXPostProcessor {
public:
	int Process(ID3D11Texture2D* input, ID3D11Texture2D* output) {}
	int ProcessInPlace(ID3D11Texture2D* texture) {
	}
private:
	const char* pixelShaderSrc = "()";
};