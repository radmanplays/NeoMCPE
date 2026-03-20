#include "OptionsFile.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <platform/log.h>

#if !defined(_WIN32)
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

OptionsFile::OptionsFile() {
#ifdef __APPLE__
	settingsPath = "./Documents/options.txt";
#elif defined(ANDROID)
	settingsPath = "options.txt";
#else
	settingsPath = "options.txt";
#endif
}

void OptionsFile::setOptionsPath(const std::string& path) {
	settingsPath = path;
}

std::string OptionsFile::getOptionsPath() const {
	return settingsPath;
}

void OptionsFile::save(const StringVector& settings) {
	FILE* pFile = fopen(settingsPath.c_str(), "w");
	if(pFile != NULL) {
		for(StringVector::const_iterator it = settings.begin(); it != settings.end(); ++it) {
			fprintf(pFile, "%s\n", it->c_str());
		}
		fclose(pFile);
	} else {
		if (errno != ENOENT)
			LOGI("OptionsFile::save failed to open '%s' for writing: %s", settingsPath.c_str(), strerror(errno));

		// Ensure parent directory exists for safekeeping if path contains directories
		std::string dir = settingsPath;
		size_t fpos = dir.find_last_of("/\\");
		if (fpos != std::string::npos) {
			dir.resize(fpos);
			struct stat st;
			if (stat(dir.c_str(), &st) != 0) {
				// attempt recursive mkdir
				std::string toCreate;
				for (size_t i = 0; i <= dir.size(); ++i) {
					if (i == dir.size() || dir[i] == '/' || dir[i] == '\\') {
						if (!toCreate.empty()) {
							mkdir(toCreate.c_str(), 0755);
						}
					}
					if (i < dir.size())
						toCreate.push_back(dir[i]);
				}
			}
		}
	}
}


StringVector OptionsFile::getOptionStrings() {
	StringVector returnVector;
	FILE* pFile = fopen(settingsPath.c_str(), "r");
	if(pFile != NULL) {
		char lineBuff[128];
		while(fgets(lineBuff, sizeof lineBuff, pFile)) {
			// Strip trailing newline
			size_t len = strlen(lineBuff);
			while(len > 0 && (lineBuff[len-1] == '\n' || lineBuff[len-1] == '\r'))
				lineBuff[--len] = '\0';
			if(len < 3) continue;
			// Split "key:value" into two separate entries to match update() pairing
			char* colon = strchr(lineBuff, ':');
			if(colon) {
				returnVector.push_back(std::string(lineBuff, colon - lineBuff));
				returnVector.push_back(std::string(colon + 1));
			}
		}
		fclose(pFile);
	} else {
		if (errno != ENOENT)
			LOGI("OptionsFile::getOptionStrings failed to open '%s' for reading: %s", settingsPath.c_str(), strerror(errno));
	}
	return returnVector;
}
