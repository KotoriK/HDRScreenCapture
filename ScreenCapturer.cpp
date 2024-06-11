#include "ScreenCapturer.h"
using namespace HDRScreenCapture;
bool ScreenCapturer::Inited() {
	return m_inited;
}
ScreenCapturer::ScreenCapturer() {
	m_inited = false;
}

void ScreenCapturer::Release() {
	{
		if (m_DXGIOutput) {
			m_DXGIOutput->Release();
		}
		if (m_DXGIOutputDuplication) {
			m_DXGIOutputDuplication->Release();
		}
		m_inited = false;
	}
}
ScreenCapturer::~ScreenCapturer() {
	Release();
}
bool ScreenCapturer::Init() {
	ComPtr<IDXGIFactory1> _factory;
	IDXGIAdapter1* _adapter;
	HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&_factory));
	if (FAILED(hr)) {
		return false;
	}
	hr = _factory->EnumAdapters1(0, &_adapter);
	if (FAILED(hr)) {
		Release();

		return false;
	}
	ComPtr<ID3D11Device> device;
	hr = D3D11CreateDevice(_adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, 0, NULL, 0, D3D11_SDK_VERSION, device.GetAddressOf(), NULL, NULL);
	if (FAILED(hr)) {
		return false;
	}
	IDXGIOutput* _output;
	hr = _adapter->EnumOutputs(0, &_output);
	if (FAILED(hr)) {
		Release();
		return false;
	}
	hr = _output->QueryInterface(IID_PPV_ARGS(&m_DXGIOutput));
	if (FAILED(hr)) {
		_output->Release();
		Release();
		return false;
	}
	_output->Release();
	hr = m_DXGIOutput->DuplicateOutput(device.Get(), &m_DXGIOutputDuplication);
	if (FAILED(hr)) {
		Release();
		return false;
	}
	m_inited = true;
	return true;
}

bool ScreenCapturer::Capture(ID3D11Texture2D** texture) {
	if (!m_inited) {
		return false;
	}
	DXGI_OUTDUPL_FRAME_INFO frame_info{};
	IDXGIResource* res;
	auto hr = m_DXGIOutputDuplication->AcquireNextFrame(1000, &frame_info, &res);
	// TODO: exception handling
	if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
		return false;
	}
	else if (hr == DXGI_ERROR_ACCESS_LOST) {
		return false;
	}
	else	if (FAILED(hr)) {
		return false;
	}
	ID3D11Texture2D* tex;
	hr = res->QueryInterface(IID_PPV_ARGS(
		&tex
	));
	if (FAILED(hr)) {
		m_DXGIOutputDuplication->ReleaseFrame();
		return false;
	}
	*texture = tex;
	// m_DXGIOutputDuplication->ReleaseFrame();

	return true;
}