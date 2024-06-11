#include "ScreenCapturer.h"
#include <stdio.h>
#include <iostream>
#include <opencv2/imgcodecs.hpp>
using namespace HDRScreenCapture;
using namespace Microsoft::WRL;
int GetAdequateMatType(DXGI_FORMAT format) {
	switch (format) {
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return CV_8UC4;
	case DXGI_FORMAT_R8G8B8A8_UINT:
		return CV_8UC4;
	case DXGI_FORMAT_R8G8B8A8_SNORM:
		return CV_8UC4;
	case DXGI_FORMAT_R8G8B8A8_SINT:
		return CV_8UC4;
	case DXGI_FORMAT_R16G16B16A16_UNORM:
		return CV_16UC4;
	case DXGI_FORMAT_R16G16B16A16_UINT:
		return CV_16UC4;
	case DXGI_FORMAT_R16G16B16A16_SNORM:
		return CV_16SC4;
	case DXGI_FORMAT_R16G16B16A16_SINT:
		return CV_16SC4;
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		return CV_32FC4;
	case DXGI_FORMAT_R32G32B32A32_UINT:
		return CV_32SC4;
	case DXGI_FORMAT_R32G32B32A32_SINT:
		return CV_32SC4;
	case DXGI_FORMAT_R32G32B32_FLOAT:
		return CV_32FC3;
	case DXGI_FORMAT_R32G32B32_UINT:
		return CV_32SC3;
	case DXGI_FORMAT_R32G32B32_SINT:
		return CV_32SC3;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
		return CV_16FC4;
	case DXGI_FORMAT_R32G32_FLOAT:
		return CV_32FC2;
	case DXGI_FORMAT_R32G32_UINT:
		return CV_32SC2;
	case DXGI_FORMAT_R32G32_SINT:
		return CV_32SC2;
	case DXGI_FORMAT_R10G10B10A2_UNORM:
		return CV_16UC4;

	}
};
HRESULT getNextCapture(ScreenCapturer* capturer, cv::Mat* mat) {
	ComPtr<ID3D11Texture2D> texture;
	if (capturer->Capture(&texture)) {
		// Transform Texture to Bitmap
		ComPtr< ID3D11Device> device;
		texture->GetDevice(&device);

		ComPtr< ID3D11DeviceContext> context;
		device->GetImmediateContext(&context);

		D3D11_TEXTURE2D_DESC desc;
		texture->GetDesc(&desc);
		printf("Texture Format: %d\n", desc.Format);
		printf("CPU Access Flags: %d\n", desc.CPUAccessFlags);

		D3D11_MAPPED_SUBRESOURCE mapped;

		HRESULT hr;
		ComPtr<ID3D11Texture2D> newTexture;
		if ((desc.CPUAccessFlags & D3D10_CPU_ACCESS_READ) == D3D10_CPU_ACCESS_READ) {
			hr = context->Map(texture.Get(), 0u, D3D11_MAP_READ, 0u, &mapped);
		}
		else {
			D3D11_TEXTURE2D_DESC newDesc = desc;
			newDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			newDesc.MiscFlags = 0; // cannot use miscflag with CPU access flag
			newDesc.Usage = D3D11_USAGE_STAGING;
			newDesc.BindFlags = 0;
			hr = device->CreateTexture2D(&newDesc, NULL, &newTexture);
			if (FAILED(hr)) {
				printf("Failed to create texture: 0x%x \n", hr);
				return hr;
			}
			context->CopyResource(newTexture.Get(), texture.Get());
			hr = context->Map(newTexture.Get(), 0u, D3D11_MAP_READ, 0u, &mapped);

		}
		if (FAILED(hr)) {
			printf("Failed to extract texture: 0x%x \n", hr);
			return hr;
		}
		auto matType = GetAdequateMatType(desc.Format);
		char* arr = (char*)mapped.pData;
		std::vector<char> vec(arr, arr + desc.Width * desc.Height);
		// *mat = cv::Mat(desc.Height, desc.Width, matType, mapped.pData);
		if (newTexture) {
			context->Unmap(newTexture.Get(), 0u);
		}
		else {
			context->Unmap(texture.Get(), 0u);
		}
		return S_OK;

	}
	return E_FAIL;
}
int main() {
	auto capturer = new ScreenCapturer();
	if (capturer->Init()) {
		unsigned int i = 0;
		for (i = 0; i < 5; i++) {
			//cv::Mat mat;
			HRESULT hr = getNextCapture(capturer,nullptr);
			if (FAILED(hr)) {
				printf("Failed to capture screen: 0x%x\n", hr);
				break;
			}
			
		//	cv::imwrite("capture" + std::to_string(i) + ".png", mat);

			Sleep(1000);
		}
	}

}
