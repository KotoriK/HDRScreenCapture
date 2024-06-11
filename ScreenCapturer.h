// require desktop duplication via DXGI
#include <d3d11.h>
#include <dxgi1_5.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
namespace HDRScreenCapture {
	class ScreenCapturer {
	public:
		ScreenCapturer();
		~ScreenCapturer();
		bool Init();
		bool Capture(ID3D11Texture2D** texture);
		bool Inited();
		void Release();
	private:
		bool m_inited;
#pragma region DXGIResources
		Microsoft::WRL::ComPtr<IDXGIOutput5> m_DXGIOutput;
		Microsoft::WRL::ComPtr<IDXGIOutputDuplication> m_DXGIOutputDuplication;
#pragma endregion


	};
}