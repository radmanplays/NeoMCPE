#ifndef APPPLATFORM_WIN32_H__
#define APPPLATFORM_WIN32_H__

#include "AppPlatform.h"
#include "platform/log.h"
#include "platform/HttpClient.h"
#include "platform/PngLoader.h"
#include "client/renderer/gles.h"
#include "world/level/storage/FolderMethods.h"
#include "util/StringUtils.h"
#include <png.h>
#include <cmath>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <shellapi.h>

static void png_funcReadFile(png_structp pngPtr, png_bytep data, png_size_t length) {
	((std::istream*)png_get_io_ptr(pngPtr))->read((char*)data, length);
}

class AppPlatform_win32: public AppPlatform
{
public:
    AppPlatform_win32()
    {
		char modulePath[MAX_PATH] = { 0 };
		const DWORD len = GetModuleFileNameA(NULL, modulePath, MAX_PATH);
		if (len > 0 && len < MAX_PATH) {
			std::string exePath(modulePath, len);
			const size_t slashPos = exePath.find_last_of("\\/");
			if (slashPos != std::string::npos) {
				dataRoot = exePath.substr(0, slashPos) + "/data";
			}
		}

		if (dataRoot.empty()) {
			dataRoot = "../../data";
		}
    }

	BinaryBlob readAssetFile(const std::string& filename) {
		FILE* fp = openAssetFile(filename, "rb");
		if (!fp)
			return BinaryBlob();

		int size = getRemainingFileSize(fp);

		BinaryBlob blob;
		blob.size = size;
		blob.data = new unsigned char[size];

		fread(blob.data, 1, size, fp);
		fclose(fp);

		return blob;
	}

    void saveScreenshot(const std::string& filename, int glWidth, int glHeight) {
        //@todo
    }

    __inline unsigned int rgbToBgr(unsigned int p) {
        return (p & 0xff00ff00) | ((p >> 16) & 0xff) | ((p << 16) & 0xff0000);
    }

    TextureData loadTexture(const std::string& filename_, bool textureFolder)
	{
		// Support fetching PNG textures via HTTP/HTTPS (for skins, etc).
		if (Util::startsWith(filename_, "http://") || Util::startsWith(filename_, "https://")) {
			std::vector<unsigned char> body;
			if (HttpClient::download(filename_, body) && !body.empty()) {
				return loadTextureFromMemory(body.data(), body.size());
			}
			return TextureData();
		}

		TextureData out;

		std::string filename = textureFolder ? resolveAssetPath("images/" + filename_)
												 : filename_;
		std::ifstream source(filename.c_str(), std::ios::binary);

		if (source) {
			png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

			if (!pngPtr)
				return out;

			png_infop infoPtr = png_create_info_struct(pngPtr);

			if (!infoPtr) {
				png_destroy_read_struct(&pngPtr, NULL, NULL);
				return out;
			}

			// Hack to get around the broken libpng for windows
			png_set_read_fn(pngPtr,(void*)&source, png_funcReadFile);

			png_read_info(pngPtr, infoPtr);

			// png to 8-bit rgba
			// https://github.com/oldminecraftcommunity/MCPE-PEWIKU/blob/main/handheld/src/AppPlatform_win32.h
			png_byte colorType = png_get_color_type(pngPtr, infoPtr);
			png_byte bitDepth  = png_get_bit_depth(pngPtr, infoPtr);

			if (bitDepth == 16)
				png_set_strip_16(pngPtr);

			if (colorType == PNG_COLOR_TYPE_PALETTE)
				png_set_palette_to_rgb(pngPtr);

			if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
				png_set_expand_gray_1_2_4_to_8(pngPtr);

			if (png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS))
				png_set_tRNS_to_alpha(pngPtr);

			if (colorType == PNG_COLOR_TYPE_GRAY ||
				colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
			{
				png_set_gray_to_rgb(pngPtr);
			}

			// if image with no alpha -> add opaque alpha
			if (colorType == PNG_COLOR_TYPE_RGB ||
				colorType == PNG_COLOR_TYPE_GRAY ||
				colorType == PNG_COLOR_TYPE_PALETTE)
			{
				png_set_filler(pngPtr, 0xFF, PNG_FILLER_AFTER);
			}

			png_read_update_info(pngPtr, infoPtr);

			// Set up the texdata properties
			out.w = png_get_image_width(pngPtr, infoPtr);
			out.h = png_get_image_height(pngPtr, infoPtr);

			png_size_t rowBytes = png_get_rowbytes(pngPtr, infoPtr);

			out.data = new unsigned char[rowBytes * out.h];
			out.memoryHandledExternally = false;

			png_bytep* rowPtrs = new png_bytep[out.h];
			for (int i = 0; i < out.h; ++i)
				rowPtrs[i] = out.data + i * rowBytes;

			png_read_image(pngPtr, rowPtrs);

			// Teardown and return
			png_destroy_read_struct(&pngPtr, &infoPtr,(png_infopp)0);
			delete[] (png_bytep)rowPtrs;
			source.close();

			return out;
		}
		else
		{
			LOGI("Couldn't find file: %s\n", filename.c_str());
			return out;
		}
    }

	TextureData loadTextureFromMemory(const unsigned char* data, size_t size) override {
		return loadPngFromMemory(data, size);
	}

	virtual int getScreenWidth();
	virtual int getScreenHeight();

	virtual float getPixelsPerMillimeter();

	virtual bool supportsTouchscreen();

	virtual void openURL(const std::string& url) {
		ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}

private:
	std::string dataRoot;

	std::string resolveAssetPath(const std::string& relativePath) const {
		const std::string primary = dataRoot + "/" + relativePath;
		std::ifstream primaryFile(primary.c_str(), std::ios::binary);
		if (primaryFile.good()) {
			return primary;
		}

		return "../../data/" + relativePath;
	}

	FILE* openAssetFile(const std::string& relativePath, const char* mode) const {
		std::string primary = dataRoot + "/" + relativePath;
		FILE* fp = fopen(primary.c_str(), mode);
		if (fp) {
			return fp;
		}

		return fopen(("../../data/" + relativePath).c_str(), mode);
	}
};

#endif /*APPPLATFORM_WIN32_H__*/
