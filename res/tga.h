// Aseprite TGA Library
// Copyright (C) 2020-2021  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef TGA_TGA_H_INCLUDED
#define TGA_TGA_H_INCLUDED
#pragma once

#include <stdint.h>

#include <cstdio>
#include <string>
#include <vector>

#include <cassert>

namespace tga {

  enum ImageType {
    NoImage = 0,
    UncompressedIndexed = 1,
    UncompressedRgb = 2,
    UncompressedGray = 3,
    RleIndexed = 9,
    RleRgb = 10,
    RleGray = 11,
  };

  typedef uint32_t color_t;

  const color_t color_r_shift  = 0;
  const color_t color_g_shift  = 8;
  const color_t color_b_shift  = 16;
  const color_t color_a_shift  = 24;
  const color_t color_r_mask   = 0x000000ff;
  const color_t color_g_mask   = 0x0000ff00;
  const color_t color_b_mask   = 0x00ff0000;
  const color_t color_rgb_mask = 0x00ffffff;
  const color_t color_a_mask   = 0xff000000;

  inline uint8_t getr(color_t c) { return (c >> color_r_shift) & 0xff; }
  inline uint8_t getg(color_t c) { return (c >> color_g_shift) & 0xff; }
  inline uint8_t getb(color_t c) { return (c >> color_b_shift) & 0xff; }
  inline uint8_t geta(color_t c) { return (c >> color_a_shift) & 0xff; }
  inline color_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
    return ((r << color_r_shift) |
            (g << color_g_shift) |
            (b << color_b_shift) |
            (a << color_a_shift));
  }

  class Colormap {
  public:
    Colormap() { }
    Colormap(const int n) : m_color(n) { }

    int size() const { return int(m_color.size()); }

    const color_t& operator[](int i) const {
      return m_color[i];
    }

    color_t& operator[](int i) {
      return m_color[i];
    }

    bool operator==(const Colormap& o) const {
      for (int i=0; i<int(m_color.size()); ++i) {
        if (m_color[i] != o[i])
          return false;
      }
      return true;
    }

    bool operator!=(const Colormap& o) const {
      return !operator==(o);
    }

  private:
    std::vector<color_t> m_color;
  };

  struct Image {
    uint8_t* pixels;
    uint32_t bytesPerPixel;
    uint32_t rowstride;
  };

  struct Header {
    uint8_t  idLength;
    uint8_t  colormapType;
    uint8_t  imageType;
    uint16_t colormapOrigin;
    uint16_t colormapLength;
    uint8_t  colormapDepth;
    uint16_t xOrigin;
    uint16_t yOrigin;
    uint16_t width;
    uint16_t height;
    uint8_t  bitsPerPixel;
    uint8_t  imageDescriptor;
    std::string imageId;
    Colormap colormap;

    bool leftToRight() const { return !(imageDescriptor & 0x10); }
    bool topToBottom() const { return (imageDescriptor & 0x20); }

    bool hasColormap() const {
      return (colormapLength > 0);
    }

    bool isRgb() const {
      return (imageType == UncompressedRgb ||
              imageType == RleRgb);
    }

    bool isIndexed() const {
      return (imageType == UncompressedIndexed ||
              imageType == RleIndexed);
    }

    bool isGray() const {
      return (imageType == UncompressedGray ||
              imageType == RleGray);
    }

    bool isUncompressed() const {
      return (imageType == UncompressedIndexed ||
              imageType == UncompressedRgb ||
              imageType == UncompressedGray);
    }

    bool isRle() const {
      return (imageType == RleIndexed ||
              imageType == RleRgb ||
              imageType == RleGray);
    }

    bool validColormapType() const {
      return
        // Indexed with palette
        (isIndexed() && bitsPerPixel == 8 && colormapType == 1) ||
        // Grayscale without palette
        (isGray() && bitsPerPixel == 8 && colormapType == 0) ||
        // Non-indexed without palette
        (bitsPerPixel > 8 && colormapType == 0);
    }

    bool valid() const {
      switch (imageType) {
        case UncompressedIndexed:
        case RleIndexed:
          return (bitsPerPixel == 8);
        case UncompressedRgb:
        case RleRgb:
          return (bitsPerPixel == 15 ||
                  bitsPerPixel == 16 ||
                  bitsPerPixel == 24 ||
                  bitsPerPixel == 32);
        case UncompressedGray:
        case RleGray:
          return (bitsPerPixel == 8);
      }
      return false;
    }

    // Returns the number of bytes per pixel needed in an image
    // created with this Header information.
    int bytesPerPixel() const {
      if (isRgb())
        return 4;
      else
        return 1;
    }

  };

  namespace details {

  class ImageIterator {
  public:
    ImageIterator()
    : m_image(nullptr)
    {
    }

    ImageIterator(const Header& header, Image& image)
        : m_image(&image)
        , m_x(header.leftToRight() ? 0 : header.width - 1)
        , m_y(header.topToBottom() ? 0 : header.height - 1)
        , m_w(header.width)
        , m_h(header.height)
        , m_dx(header.leftToRight() ? +1 : -1)
        , m_dy(header.topToBottom() ? +1 : -1)
    {
        m_ptr =
            m_image->pixels
            + m_image->rowstride * m_y
            + m_image->bytesPerPixel * m_x;
    }

    // Put a pixel value into the image and advance the iterator.
    template<typename T>
    bool putPixel(const T value) {
      *((T*)m_ptr) = value;
      return advance();
    }

    // Get one pixel from the image and advance the iterator.
    template<typename T>
    T getPixel() {
      T value = *((T*)m_ptr);
      advance();
      return value;
    }

  public:
    bool advance()
    {
        m_x += m_dx;
        m_ptr += int(m_dx * m_image->bytesPerPixel);

        if ((m_dx < 0 && m_x < 0) ||
            (m_dx > 0 && m_x == m_w)) {
            m_x = (m_dx > 0 ? 0 : m_w - 1);
            m_y += m_dy;
            if ((m_dy < 0 && m_y < 0) ||
                (m_dy > 0 && m_y == m_h)) {
                return true;
            }
            calcPtr();
        }
        return false;
    }
    void calcPtr()
    {
        m_ptr =
            m_image->pixels
            + m_image->rowstride * m_y
            + m_image->bytesPerPixel * m_x;
    }

    Image* m_image;
    int m_x, m_y;
    int m_w, m_h;
    int m_dx, m_dy;
    uint8_t* m_ptr;
  };

  } // namespace details

  class FileInterface {
  public:
    virtual ~FileInterface() { }

    // Returns true if we can read/write bytes from/into the file
    virtual bool ok() const = 0;

    // Current position in the file
    virtual size_t tell() = 0;

    // Jump to the given position in the file
    virtual void seek(size_t absPos) = 0;

    // Returns the next byte in the file or 0 if ok() = false
    virtual uint8_t read8() = 0;

    // Writes one byte in the file (or do nothing if ok() = false)
    virtual void write8(uint8_t value) = 0;
  };

  class StdioFileInterface : public tga::FileInterface {
  public:
    StdioFileInterface(FILE* file)
        : m_file(file)
        , m_ok(true)
    {
    }
    bool ok() const
    {
        return m_ok;
    }
    size_t tell()
    {
        return ftell(m_file);
    }
    void seek(size_t absPos)
    {
        // To detect surprises with the size_t -> long cast.
        assert(absPos <= std::numeric_limits<long>::max());

        fseek(m_file, (long)absPos, SEEK_SET);
    }
    uint8_t read8()
    {
        int value = fgetc(m_file);
        if (value != EOF) {
            // We can safely cast to uint8_t as EOF is the only special
            // non-uint8 value than fgetc() should return.
            return (uint8_t)value;
        }

        m_ok = false;
        return 0;
    }
    void write8(uint8_t value)
    {
        fputc(value, m_file);
    }

  private:
    FILE* m_file;
    bool m_ok;
  };

  class Delegate {
  public:
    virtual ~Delegate() {}
    // Must return true if we should continue the decoding process.
    virtual bool notifyProgress(double progress) = 0;
  };

  static inline uint8_t scale_5bits_to_8bits(uint8_t v) {
      assert(v >= 0 && v < 32);
      return (v << 3) | (v >> 2);
  }

  class Decoder {
  public:
    Decoder(FileInterface* file)
        : m_file(file)
    {
    }

    bool hasAlpha() const { return m_hasAlpha; }

    // Reads the header + colormap (if the file has a
    // colormap). Returns true if the header is valid.
    bool readHeader(Header& header)
    {
        header.idLength = read8();
        header.colormapType = read8();
        header.imageType = read8();
        header.colormapOrigin = read16();
        header.colormapLength = read16();
        header.colormapDepth = read8();
        header.xOrigin = read16();
        header.yOrigin = read16();
        header.width = read16();
        header.height = read16();
        header.bitsPerPixel = read8();
        header.imageDescriptor = read8();

        // Invalid image size
        if (header.width == 0 ||
            header.height == 0)
            return false;

        // Skip ID string (idLength bytes)
        if (header.idLength > 0) {
            uint8_t i = header.idLength;
            while (i--) {
                uint8_t chr = m_file->read8();
                header.imageId.push_back(chr);
            }
        }

#if 0
        // In the best case the "alphaBits" should be valid, but there are
        // invalid TGA files out there which don't indicate the
        // "alphaBits" correctly, so they could be 0 and use the alpha
        // channel anyway on each pixel.
        int alphaBits = (header.imageDescriptor & 15);
        m_hasAlpha =
            (header.bitsPerPixel == 32 && alphaBits == 8) ||
            (header.bitsPerPixel == 16 && alphaBits == 1);
#else
        // So to detect if a 32bpp or 16bpp TGA image has alpha, we'll use
        // the "alpha histogram" in postProcessImage() to check if there are
        // different alpha values. If there is only one alpha value (all 0
        // or all 255), we create an opaque image anyway. The only exception
        // to this rule is when all pixels are black and transparent
        // (RGBA=0), that is the only case when an image is fully
        // transparent.
        //
        // Note: This same heuristic is used in apps like macOS Preview:
        // https://twitter.com/davidcapello/status/1242803110868893697
        m_hasAlpha =
            (header.bitsPerPixel == 32) ||
            (header.bitsPerPixel == 16);
#endif

        // Read colormap
        if (header.colormapType == 1)
            readColormap(header);

        return (header.validColormapType() &&
            header.valid());
    }

    // Reads the image.
    bool readImage(const Header& header,
                   Image& image,
                   Delegate* delegate = nullptr)
    {
        // Bit 4 means right-to-left, else left-to-right
        // Bit 5 means top-to-bottom, else bottom-to-top
        m_iterator = details::ImageIterator(header, image);

        for (int y = 0; y < header.height; ++y) {
            switch (header.imageType) {

            case UncompressedIndexed:
                assert(header.bitsPerPixel == 8);
                if (readUncompressedData<uint8_t>(header.width, &Decoder::read8Color))
                    return true;
                break;

            case UncompressedRgb:
                switch (header.bitsPerPixel) {
                case 15:
                case 16:
                    if (readUncompressedData<uint32_t>(header.width, &Decoder::read16AsRgb))
                        return true;
                    break;
                case 24:
                    if (readUncompressedData<uint32_t>(header.width, &Decoder::read24AsRgb))
                        return true;
                    break;
                case 32:
                    if (readUncompressedData<uint32_t>(header.width, &Decoder::read32AsRgb))
                        return true;
                    break;
                default:
                    assert(false);
                    break;
                }
                break;

            case UncompressedGray:
                assert(header.bitsPerPixel == 8);
                if (readUncompressedData<uint8_t>(header.width, &Decoder::read8Color))
                    return true;
                break;

            case RleIndexed:
                assert(header.bitsPerPixel == 8);
                if (readRleData<uint8_t>(header.width, &Decoder::read8Color))
                    return true;
                break;

            case RleRgb:
                switch (header.bitsPerPixel) {
                case 15:
                case 16:
                    if (readRleData<uint32_t>(header.width, &Decoder::read16AsRgb))
                        return true;
                    break;
                case 24:
                    if (readRleData<uint32_t>(header.width, &Decoder::read24AsRgb))
                        return true;
                    break;
                case 32:
                    if (readRleData<uint32_t>(header.width, &Decoder::read32AsRgb))
                        return true;
                    break;
                default:
                    assert(false);
                    break;
                }
                break;

            case RleGray:
                assert(header.bitsPerPixel == 8);
                if (readRleData<uint8_t>(header.width, &Decoder::read8Color))
                    return true;
                break;
            }

            if (delegate &&
                !delegate->notifyProgress(float(y) / float(header.height))) {
                break;
            }
        }

        return true;
    }

    // Fixes alpha channel for images with invalid alpha values (this
    // is optional, in case you want to preserve the original data
    // from the file, don't use it).
    void postProcessImage(const Header& header,
                          Image& image)
    {
        // The post-processing is only for RGB images with possible invalid
        // alpha information.
        if (!header.isRgb() || !m_hasAlpha)
            return;

        bool transparentImage = true;
        bool blackImage = true;

        for (int y = 0; y < header.height; ++y) {
            auto p = (uint32_t*)(image.pixels + y * image.rowstride);
            for (int x = 0; x < header.width; ++x, ++p) {
                color_t c = *p;
                if (transparentImage &&
                    geta(c) != 0) {
                    transparentImage = false;
                }
                if (blackImage &&
                    (getr(c) != 0 ||
                        getg(c) != 0 ||
                        getb(c) != 0)) {
                    blackImage = false;
                }
            }
        }

        // If the image is fully transparent (all pixels with alpha=0) and
        // there are pixels with RGB != 0 (!blackImage), we have to make the
        // image completely opaque (alpha=255).
        if (transparentImage && !blackImage) {
            for (int y = 0; y < header.height; ++y) {
                auto p = (uint32_t*)(image.pixels + y * image.rowstride);
                for (int x = 0; x < header.width; ++x, ++p) {
                    color_t c = *p;
                    *p = rgba(getr(c),
                        getg(c),
                        getb(c), 255);
                }
            }
        }
    }

  private:
    void readColormap(Header& header)
    {
        header.colormap = Colormap(header.colormapLength);

        for (int i = 0; i < header.colormapLength; ++i) {
            switch (header.colormapDepth) {

            case 15:
            case 16: {
                const uint16_t c = read16();
                header.colormap[i] =
                    rgba(scale_5bits_to_8bits((c >> 10) & 0x1F),
                        scale_5bits_to_8bits((c >> 5) & 0x1F),
                        scale_5bits_to_8bits(c & 0x1F));
                break;
            }

            case 24:
            case 32: {
                const uint8_t b = read8();
                const uint8_t g = read8();
                const uint8_t r = read8();
                uint8_t a;
                if (header.colormapDepth == 32)
                    a = read8();
                else
                    a = 255;
                header.colormap[i] = rgba(r, g, b, a);
                break;
            }
            }
        }
    }

    template<typename T>
    bool readUncompressedData(const int w, uint32_t (Decoder::*readPixel)())
    {
        for (int x = 0; x < w && m_file->ok(); ++x) {
            if (m_iterator.putPixel<T>(static_cast<T>((this->*readPixel)())))
                return true;
        }
        return false;
    }

    template<typename T>
    bool readRleData(const int w, uint32_t (Decoder::*readPixel)())
    {
        for (int x = 0; x < w && m_file->ok(); ) {
            int c = read8();
            if (c & 0x80) {
                c = (c & 0x7f) + 1;
                x += c;
                const T pixel = static_cast<T>((this->*readPixel)());
                while (c-- > 0)
                    if (m_iterator.putPixel<T>(pixel))
                        return true;
            }
            else {
                ++c;
                x += c;
                while (c-- > 0) {
                    if (m_iterator.putPixel<T>(static_cast<T>((this->*readPixel)())))
                        return true;
                }
            }
        }
        return false;
    }

    uint8_t read8()
    {
        return m_file->read8();
    }
    uint16_t read16()
    {
        uint8_t b1 = m_file->read8();
        uint8_t b2 = m_file->read8();

        if (m_file->ok()) {
            return ((b2 << 8) | b1); // Little endian
        }
        else
            return 0;
    }
    uint32_t read32()
    {
        const uint8_t b1 = m_file->read8();
        const uint8_t b2 = m_file->read8();
        const uint8_t b3 = m_file->read8();
        const uint8_t b4 = m_file->read8();

        if (m_file->ok()) {
            // Little endian
            return ((b4 << 24) | (b3 << 16) | (b2 << 8) | b1);
        }
        else
            return 0;
    }

    color_t read32AsRgb()
    {
        const uint8_t b = read8();
        const uint8_t g = read8();
        const uint8_t r = read8();
        uint8_t a = read8();
        if (!m_hasAlpha)
            a = 255;
        return rgba(r, g, b, a);
    }
    color_t read24AsRgb()
    {
        const uint8_t b = read8();
        const uint8_t g = read8();
        const uint8_t r = read8();
        return rgba(r, g, b, 255);
    }
    color_t read16AsRgb()
    {
        const uint16_t v = read16();
        uint8_t a = 255;
        if (m_hasAlpha) {
            if ((v & 0x8000) == 0)    // Transparent bit
                a = 0;
        }
        return rgba(scale_5bits_to_8bits((v >> 10) & 0x1F),
            scale_5bits_to_8bits((v >> 5) & 0x1F),
            scale_5bits_to_8bits(v & 0x1F),
            a);
    }
    color_t read8Color() { return (color_t)read8(); }

    FileInterface* m_file;
    bool m_hasAlpha = false;
    details::ImageIterator m_iterator;
  };

  template<typename T>
  inline color_t get_pixel(Image& image, int x, int y) {
      return *(T*)(image.pixels + y * image.rowstride + x * image.bytesPerPixel);
  }

  class Encoder {
  public:
    Encoder(FileInterface* file)
        : m_file(file)
    {
    }

    // Writes the header + colormap
    void writeHeader(const Header& header)
    {
        write8(header.idLength);
        write8(header.colormapType);
        write8(header.imageType);
        write16(header.colormapOrigin);
        write16(header.colormapLength);
        write8(header.colormapDepth);
        write16(header.xOrigin);
        write16(header.yOrigin);
        write16(header.width);
        write16(header.height);
        write8(header.bitsPerPixel);
        write8(header.imageDescriptor);

        m_hasAlpha = (header.bitsPerPixel == 16 ||
            header.bitsPerPixel == 32);

        assert(header.colormapLength == header.colormap.size());

        // Write colormap
        if (header.colormapType == 1) {
            for (int i = 0; i < header.colormap.size(); ++i) {
                color_t c = header.colormap[i];
                switch (header.colormapDepth) {
                case 15:
                case 16: write16Rgb(c); break;
                case 24: write24Rgb(c); break;
                case 32: write32Rgb(c); break;
                }
            }
        }
        else {
            assert(header.colormapLength == 0);
        }
    }
    void writeImage(const Header& header,
                    const Image& image,
                    Delegate* delegate = nullptr)
    {
        const int w = header.width;
        const int h = header.height;

        m_iterator = details::ImageIterator(header, const_cast<Image&>(image));

        switch (header.imageType) {

        case tga::UncompressedIndexed:
        case tga::UncompressedGray:
            for (int y = 0; y < h; ++y) {
                for (int x = 0; x < w; ++x)
                    write8(m_iterator.getPixel<uint8_t>());

                if (delegate && !delegate->notifyProgress(float(y) / float(h)))
                    return;
            }
            break;

        case tga::RleIndexed:
        case tga::RleGray:
            for (int y = 0; y < h; ++y) {
                writeRleScanline<uint8_t>(w, image, &Encoder::write8Color);

                if (delegate && !delegate->notifyProgress(float(y) / float(h)))
                    return;
            }
            break;

        case tga::UncompressedRgb: {
            switch (header.bitsPerPixel) {
            case 15:
            case 16:
                for (int y = 0; y < h; ++y) {
                    for (int x = 0; x < w; ++x)
                        write16Rgb(m_iterator.getPixel<uint32_t>());

                    if (delegate && !delegate->notifyProgress(float(y) / float(h)))
                        return;
                }
                break;
            case 24:
                for (int y = 0; y < h; ++y) {
                    for (int x = 0; x < w; ++x)
                        write24Rgb(m_iterator.getPixel<uint32_t>());

                    if (delegate && !delegate->notifyProgress(float(y) / float(h)))
                        return;
                }
                break;
            case 32:
                for (int y = 0; y < h; ++y) {
                    for (int x = 0; x < w; ++x)
                        write32Rgb(m_iterator.getPixel<uint32_t>());

                    if (delegate && !delegate->notifyProgress(float(y) / float(h)))
                        return;
                }
                break;
            }
        }
                                 break;

        case tga::RleRgb:
            for (int y = 0; y < h; ++y) {
                switch (header.bitsPerPixel) {
                case 15:
                case 16: writeRleScanline<uint32_t>(w, image, &Encoder::write16Rgb); break;
                case 24: writeRleScanline<uint32_t>(w, image, &Encoder::write24Rgb); break;
                case 32: writeRleScanline<uint32_t>(w, image, &Encoder::write32Rgb); break;
                default:
                    assert(false);
                    return;
                }
                if (delegate && !delegate->notifyProgress(float(y) / float(h)))
                    return;
            }
            break;

        }
    }
    void writeFooter()
    {
        const char* tga2_footer = "\0\0\0\0\0\0\0\0TRUEVISION-XFILE.\0";
        for (int i = 0; i < 26; ++i)
            write8(tga2_footer[i]);
    }

  private:
    template<typename T>
    void writeRleScanline(const int w, const Image& image,
                          void (Encoder::*writePixel)(color_t))
    {
        int x = 0;
        while (x < w) {
            int count, offset;
            countRepeatedPixels<T>(w, image, x, offset, count);

            // Save a sequence of pixels with different colors
            while (offset > 0) {
                const int n = std::min(offset, 128);

                assert(n >= 1 && n <= 128);
                write8(static_cast<uint8_t>(n - 1));
                for (int i = 0; i < n; ++i) {
                    const color_t c = m_iterator.getPixel<T>();
                    (this->*writePixel)(c);
                }
                offset -= n;
                x += n;
            }

            // Save a sequence of pixels with the same color
            while (count * image.bytesPerPixel > 1 + image.bytesPerPixel) {
                const int n = std::min(count, 128);
                const color_t c = m_iterator.getPixel<T>();

                for (int i = 1; i < n; ++i) {
#ifndef NDEBUG
                    const color_t c2 =
#endif
                        m_iterator.getPixel<T>();
                    assert(c == c2);
                }

                assert(n >= 1 && n <= 128);
                write8(0x80 | static_cast<uint8_t>(n - 1));
                (this->*writePixel)(c);
                count -= n;
                x += n;
            }
        }
        assert(x == w);
    }

    template<typename T>
    void countRepeatedPixels(const int w, const Image& image,
                             int x0, int& offset, int& count)
    {
        auto it = m_iterator;

        for (int x = x0; x < w; ) {
            const color_t p = it.getPixel<T>();

            int u = x + 1;
            auto next_it = it;

            for (; u < w; ++u) {
                const color_t q = it.getPixel<T>();
                if (p != q)
                    break;
            }

            if ((u - x) * image.bytesPerPixel > 1 + image.bytesPerPixel) {
                offset = x - x0;
                count = u - x;
                return;
            }

            ++x;
            it = next_it;
        }

        offset = w - x0;
        count = 0;
    }

    void write8(uint8_t value)
    {
        m_file->write8(value);
    }
    void write16(uint16_t value)
    {
        // Little endian
        m_file->write8(value & 0x00FF);
        m_file->write8((value & 0xFF00) >> 8);
    }
    void write32(uint32_t value)
    {
        // Little endian
        m_file->write8(value & 0xFF);
        m_file->write8((value >> 8) & 0xFF);
        m_file->write8((value >> 16) & 0xFF);
        m_file->write8((value >> 24) & 0xFF);
    }

    void write8Color(color_t c) { write8(uint8_t(c)); }
    void write16Rgb(color_t c)
    {
        const uint8_t r = getr(c);
        const uint8_t g = getg(c);
        const uint8_t b = getb(c);
        const uint8_t a = geta(c);
        const uint16_t v =
            ((r >> 3) << 10) |
            ((g >> 3) << 5) |
            ((b >> 3)) |
            (m_hasAlpha && a >= 128 ? 0x8000 : 0); // TODO configurable threshold
        write16(v);
    }
    void write24Rgb(color_t c)
    {
        write8(getb(c));
        write8(getg(c));
        write8(getr(c));
    }
    void write32Rgb(color_t c)
    {
        write8(getb(c));
        write8(getg(c));
        write8(getr(c));
        write8(geta(c));
    }

    FileInterface* m_file;
    bool m_hasAlpha = false;
    details::ImageIterator m_iterator;
  };

} // namespace tga

#endif
