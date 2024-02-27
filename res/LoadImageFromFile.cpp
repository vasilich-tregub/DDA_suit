STDMETHODIMP CSLD2SimpleTextObject::LoadImageFromFile(
	std::wstring fileName,
	Microsoft::WRL::ComPtr<IWICImagingFactory>& wicFactory,
	std::map<std::wstring, Microsoft::WRL::ComPtr<IWICBitmapSource>> inlineImages,
	_Out_ Microsoft::WRL::ComPtr<IWICBitmapSource>& bitmap
)
{
	// Loads an image from a file into the given bitmap.

	HRESULT hr = S_OK;

	if (inlineImages.find(fileName) != inlineImages.end())
	{
		bitmap = inlineImages[fileName];
		return hr;
	}

	wchar_t ext[_MAX_EXT];
	wchar_t fname[_MAX_FNAME];
	_wsplitpath_s(fileName.c_str(), nullptr, 0, nullptr, 0, fname, _MAX_FNAME, ext, _MAX_EXT);
	if (_wcsicmp(ext, L".tga") == 0)
	{
		DirectX::TexMetadata metadata;
		std::unique_ptr<DirectX::ScratchImage> image(new DirectX::ScratchImage);
		if (FAILED(hr = DirectX::LoadFromTGAFile(fileName.c_str(), &metadata, *image))) return hr;
		auto srcImage = *image->GetImage(0, 0, 0);
		std::unique_ptr<DirectX::ScratchImage> pmImage(new DirectX::ScratchImage);
		if (!metadata.IsPMAlpha())
		{
			hr = DirectX::PremultiplyAlpha(*image->GetImage(0, 0, 0), DirectX::TEX_PMALPHA_DEFAULT, *(pmImage.get()));
		}
		if (SUCCEEDED(hr))
		{
			srcImage = *pmImage->GetImage(0, 0, 0);
			Microsoft::WRL::ComPtr<IWICBitmap> wicBitmap;
			WICPixelFormatGUID pfGuid;
			if (!DirectX::_DXGIToWIC(srcImage.format, pfGuid))
				return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
			hr = wicFactory->CreateBitmapFromMemory(static_cast<UINT>(srcImage.width), static_cast<UINT>(srcImage.height), GUID_WICPixelFormat32bppPRGBA/*sourceFormat*/,
				static_cast<UINT>(srcImage.rowPitch), static_cast<UINT>(srcImage.slicePitch),
				srcImage.pixels, wicBitmap.GetAddressOf());
			if (SUCCEEDED(hr))
				bitmap = wicBitmap.Detach();
			return hr;
		}
		else
			hr = S_OK;
	}
	// create a decoder for the stream
	Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> source;
	Microsoft::WRL::ComPtr<IWICFormatConverter> converter;

	auto imfhandle = CreateFileW(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0
	);
	if (SUCCEEDED(hr))
	{
		/*hr = wicFactory->CreateDecoderFromFilename(
			fileName.c_str(),
			NULL,
			GENERIC_READ,
			WICDecodeMetadataCacheOnLoad,
			&decoder
		);*/
		hr = wicFactory->CreateDecoderFromFileHandle(reinterpret_cast<ULONG_PTR>(imfhandle), nullptr,
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
		CloseHandle(imfhandle);
		return hr;
	}
	else
	{
		CloseHandle(imfhandle);
		hr = S_OK;
		DirectX::TexMetadata metadata;
		std::unique_ptr<DirectX::ScratchImage> image(new DirectX::ScratchImage);
		if (FAILED(hr = DirectX::LoadFromTGAFile(fileName.c_str(), &metadata, *image))) return hr;
		auto srcImage = *image->GetImage(0, 0, 0);
		std::unique_ptr<DirectX::ScratchImage> pmImage(new DirectX::ScratchImage);
		if (!metadata.IsPMAlpha())
		{
			hr = DirectX::PremultiplyAlpha(*image->GetImage(0, 0, 0), DirectX::TEX_PMALPHA_DEFAULT, *(pmImage.get()));
		}
		if (SUCCEEDED(hr))
		{
			srcImage = *pmImage->GetImage(0, 0, 0);
			Microsoft::WRL::ComPtr<IWICBitmap> wicBitmap;
			WICPixelFormatGUID pfGuid;
			if (!DirectX::_DXGIToWIC(srcImage.format, pfGuid))
				return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
			hr = wicFactory->CreateBitmapFromMemory(static_cast<UINT>(srcImage.width), static_cast<UINT>(srcImage.height), GUID_WICPixelFormat32bppPRGBA/*sourceFormat*/,
				static_cast<UINT>(srcImage.rowPitch), static_cast<UINT>(srcImage.slicePitch),
				srcImage.pixels, wicBitmap.GetAddressOf());
			if (SUCCEEDED(hr))
				bitmap = wicBitmap.Detach();
		}
	}

	//SafeRelease(&converter);
	//SafeRelease(&source);
	//SafeRelease(&decoder);

	return hr;
}
