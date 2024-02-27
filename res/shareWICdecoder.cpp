// shareWICdecoder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <wrl.h>
#include <wincodec.h>
#pragma comment(lib, "Windowscodecs.lib")

int main()
{
	HRESULT hr = S_OK;
	CoInitialize(NULL);
	Microsoft::WRL::ComPtr<IWICBitmapSource> bitmap;

	Microsoft::WRL::ComPtr<IWICImagingFactory> wicFactory;
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(
			CLSID_WICImagingFactory, nullptr,
			CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));
	}
	// create a decoder for the stream
	Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> source;
	Microsoft::WRL::ComPtr<IWICFormatConverter> converter;

	std::wstring path(L"620_400.png");
	auto h = CreateFileW(
		path.c_str(), GENERIC_READ, FILE_SHARE_READ,
		nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0
	);

	if (hr = HRESULT_FROM_WIN32(GetLastError()))
	{
		std::cout << "hr=" << std::hex << hr << std::endl;
		return hr;
	}
	if (SUCCEEDED(hr))
	{
		/*hr = wicFactory->CreateDecoderFromFilename(
			path.c_str(),
			NULL,
			GENERIC_READ,
			WICDecodeMetadataCacheOnLoad,
			&decoder
		);*/
		hr = wicFactory->CreateDecoderFromFileHandle(
			reinterpret_cast<ULONG_PTR>(h), nullptr,
			WICDecodeMetadataCacheOnLoad, &decoder
		);
	}

	// Create the initial frame.
	if (SUCCEEDED(hr))
	{
		hr = decoder->GetFrame(0, &source);
	}

	// Convert format to 32bppPBGRA - which D2D expects.
	if (SUCCEEDED(hr))
	{
		hr = wicFactory->CreateFormatConverter(&converter);
	}

	if (SUCCEEDED(hr))
	{
		hr = converter->Initialize(
			source.Get(),
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut
		);
	}

	if (SUCCEEDED(hr))
	{
		//*bitmap = SafeDetach(&converter);
		bitmap = converter.Detach();
	}
	std::cout << "hr=" << std::hex << hr << std::endl;
	//SafeRelease(&converter);
	//SafeRelease(&source);
	//SafeRelease(&decoder);
	int res = CloseHandle(h);
	if (res == 0) std::cout << "CloseHandle returns " << res << std::endl;
	std::string str;
	std::cin >> str;
	return 0;
}
