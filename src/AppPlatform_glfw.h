#ifndef APPPLATFORM_GLFW_H__
#define APPPLATFORM_GLFW_H__

#include "AppPlatform.h"
#include "platform/log.h"
#include "platform/HttpClient.h"
#include "platform/PngLoader.h"
#include "client/renderer/gles.h"
#include "world/level/storage/FolderMethods.h"
#include <png.h>
#include <cmath>
#include <fstream>
#include <sstream>
#include <GLFW/glfw3.h>
#include <ctime>
#include "util/StringUtils.h"

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#endif

static void png_funcReadFile(png_structp pngPtr, png_bytep data, png_size_t length) {
	((std::istream*)png_get_io_ptr(pngPtr))->read((char*)data, length);
}

class AppPlatform_glfw: public AppPlatform
{
public:
    AppPlatform_glfw()
    {
    }

	BinaryBlob readAssetFile(const std::string& filename) override {
		FILE* fp = fopen(("data/" + filename).c_str(), "r");
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

    void saveScreenshot(const std::string& filename, int glWidth, int glHeight) override {
        //@todo
    }

    __inline unsigned int rgbToBgr(unsigned int p) {
        return (p & 0xff00ff00) | ((p >> 16) & 0xff) | ((p << 16) & 0xff0000);
    }

    TextureData loadTexture(const std::string& filename_, bool textureFolder) override
	{
		// Support fetching PNG textures via HTTP/HTTPS (for skins, etc)
		if (Util::startsWith(filename_, "http://") || Util::startsWith(filename_, "https://")) {
			std::vector<unsigned char> body;
			if (HttpClient::download(filename_, body) && !body.empty()) {
				return loadTextureFromMemory(body.data(), body.size());
			}
			return TextureData();
		}

		TextureData out;

		std::string filename = textureFolder? "data/images/" + filename_
								: filename_;
		std::ifstream source(filename.c_str(), std::ios::binary);

		if (!source) {
			LOGI("Couldn't find file: %s\n", filename.c_str());
			return out;
		}

		std::vector<unsigned char> fileData((std::istreambuf_iterator<char>(source)), std::istreambuf_iterator<char>());
		source.close();

		if (fileData.empty()) {
			LOGI("Couldn't read file: %s\n", filename.c_str());
			return out;
		}

		return loadTextureFromMemory(fileData.data(), fileData.size());
    }

	TextureData loadTextureFromMemory(const unsigned char* data, size_t size) override {
		return loadPngFromMemory(data, size);
	}

	virtual std::string getDateString(int s) override {
		time_t tm = s;

		char mbstr[100];
		std::strftime(mbstr, sizeof(mbstr), "%F %T", std::localtime(&tm));

		return std::string(mbstr);
	}

	virtual int getScreenWidth() override { return 854; };
	virtual int getScreenHeight() override { return 480; };

	virtual float getPixelsPerMillimeter() override;

	virtual bool supportsTouchscreen() override { return true; }

	virtual void openURL(const std::string& url) override {
#ifdef _WIN32
		ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
#elif __linux__
		std::string command = "xdg-open " + url;
		system(command.c_str());
#endif
	}

	GLFWwindow* window;

private:
};
#endif /*APPPLATFORM_GLFW_H__*/
