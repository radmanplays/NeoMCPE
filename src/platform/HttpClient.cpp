#include "HttpClient.h"
#include "log.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <winhttp.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winhttp.lib")
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace {

bool startsWith(const std::string& s, const std::string& prefix) {
    return s.size() >= prefix.size() && std::equal(prefix.begin(), prefix.end(), s.begin());
}

bool parseUrl(const std::string& url, std::string& scheme, std::string& host, int& port, std::string& path);

#if defined(_WIN32)

bool downloadHttpsWinHttp(const std::string& url, std::vector<unsigned char>& outBody) {
    outBody.clear();

    std::string scheme, host, path;
    int port = 0;
    if (!parseUrl(url, scheme, host, port, path))
        return false;

    // WinHTTP expects the path to include the leading '/'.
    HINTERNET hSession = WinHttpOpen(L"MinecraftPE/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession)
        return false;

    HINTERNET hConnect = WinHttpConnect(hSession, std::wstring(host.begin(), host.end()).c_str(), port, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        return false;
    }

    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect,
        L"GET",
        std::wstring(path.begin(), path.end()).c_str(),
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE
    );

    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    DWORD redirectPolicy = WINHTTP_OPTION_REDIRECT_POLICY_ALWAYS;
    WinHttpSetOption(hRequest, WINHTTP_OPTION_REDIRECT_POLICY, &redirectPolicy, sizeof(redirectPolicy));

    BOOL result = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
    if (!result) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    result = WinHttpReceiveResponse(hRequest, NULL);
    if (!result) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    DWORD bytesAvailable = 0;
    while (WinHttpQueryDataAvailable(hRequest, &bytesAvailable) && bytesAvailable > 0) {
        std::vector<unsigned char> buffer(bytesAvailable);
        DWORD bytesRead = 0;
        if (!WinHttpReadData(hRequest, buffer.data(), bytesAvailable, &bytesRead) || bytesRead == 0)
            break;
        outBody.insert(outBody.end(), buffer.begin(), buffer.begin() + bytesRead);
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return !outBody.empty();
}

#endif

std::string toLower(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);
    return out;
}

bool parseUrl(const std::string& url, std::string& scheme, std::string& host, int& port, std::string& path) {
    scheme.clear();
    host.clear();
    path.clear();
    port = 0;

    // Very simple URL parser.
    // url format: scheme://host[:port]/path
    auto pos = url.find("://");
    if (pos == std::string::npos) return false;
    scheme = toLower(url.substr(0, pos));
    size_t start = pos + 3;

    size_t slash = url.find('/', start);
    std::string hostPort = (slash == std::string::npos) ? url.substr(start) : url.substr(start, slash - start);
    path = (slash == std::string::npos) ? "/" : url.substr(slash);

    size_t colon = hostPort.find(':');
    if (colon != std::string::npos) {
        host = hostPort.substr(0, colon);
        port = atoi(hostPort.c_str() + colon + 1);
    } else {
        host = hostPort;
    }

    if (scheme == "http") {
        if (port == 0) port = 80;
    } else if (scheme == "https") {
        if (port == 0) port = 443;
    }

    return !host.empty() && !scheme.empty();
}

bool readAll(int sockfd, std::vector<unsigned char>& out) {
    const int BUF_SIZE = 4096;
    unsigned char buffer[BUF_SIZE];

    while (true) {
        int received = recv(sockfd, (char*)buffer, BUF_SIZE, 0);
        if (received <= 0)
            break;
        out.insert(out.end(), buffer, buffer + received);
    }
    return true;
}

bool resolveAndConnect(const std::string& host, int port, int& outSock) {
#if defined(_WIN32)
    static bool initialized = false;
    if (!initialized) {
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
        initialized = true;
    }
#endif

    struct addrinfo hints;
    struct addrinfo* result = NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    std::ostringstream portStream;
    portStream << port;
    const std::string portStr = portStream.str();
    if (getaddrinfo(host.c_str(), portStr.c_str(), &hints, &result) != 0)
        return false;

    int sock = -1;
    for (struct addrinfo* rp = result; rp != NULL; rp = rp->ai_next) {
        sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock < 0) continue;
        if (connect(sock, rp->ai_addr, (int)rp->ai_addrlen) == 0)
            break; // success

#if defined(_WIN32)
        closesocket(sock);
#else
        close(sock);
#endif
        sock = -1;
    }

    freeaddrinfo(result);

    if (sock < 0)
        return false;

    outSock = sock;
    return true;
}

std::string getHeaderValue(const std::string& headers, const std::string& key) {
    std::string lower = toLower(headers);
    std::string lowerKey = toLower(key);
    size_t pos = lower.find(lowerKey);
    if (pos == std::string::npos) return "";
    size_t colon = lower.find(':', pos + lowerKey.size());
    if (colon == std::string::npos) return "";
    size_t start = colon + 1;
    while (start < lower.size() && (lower[start] == ' ' || lower[start] == '\t')) start++;
    size_t end = lower.find('\r', start);
    if (end == std::string::npos) end = lower.find('\n', start);
    if (end == std::string::npos) end = lower.size();
    return headers.substr(start, end - start);
}

bool extractStatusCode(const std::string& headers, int& outStatus) {
    size_t pos = headers.find(" ");
    if (pos == std::string::npos) return false;
    size_t pos2 = headers.find(" ", pos + 1);
    if (pos2 == std::string::npos) return false;
    std::string code = headers.substr(pos + 1, pos2 - pos - 1);
    outStatus = atoi(code.c_str());
    return true;
}

} // anonymous namespace

namespace HttpClient {

bool download(const std::string& url, std::vector<unsigned char>& outBody) {
    outBody.clear();

    std::string currentUrl = url;
    for (int redirect = 0; redirect < 3; ++redirect) {
        std::string scheme, host, path;
        int port = 0;
        if (!parseUrl(currentUrl, scheme, host, port, path)) {
            LOGW("[HttpClient] parseUrl failed for '%s'\n", currentUrl.c_str());
            return false;
        }

        if (scheme == "https") {
#if defined(_WIN32)
            LOGI("[HttpClient] using WinHTTP for HTTPS URL %s\n", currentUrl.c_str());
            return downloadHttpsWinHttp(currentUrl, outBody);
#else
            LOGW("[HttpClient] HTTPS not supported on this platform: %s\n", currentUrl.c_str());
            return false;
#endif
        }

        if (scheme != "http") {
            LOGW("[HttpClient] unsupported scheme '%s' for URL '%s'\n", scheme.c_str(), currentUrl.c_str());
            return false;
        }

        int sock = -1;
        if (!resolveAndConnect(host, port, sock)) {
            LOGW("[HttpClient] resolve/connect failed for %s:%d\n", host.c_str(), port);
            return false;
        }

        std::string request = "GET " + path + " HTTP/1.1\r\n";
        request += "Host: " + host + "\r\n";
        request += "User-Agent: MinecraftPE\r\n";
        request += "Connection: close\r\n";
        request += "\r\n";

        send(sock, request.c_str(), (int)request.size(), 0);

        std::vector<unsigned char> raw;
        readAll(sock, raw);

#if defined(_WIN32)
        closesocket(sock);
#else
        close(sock);
#endif

        if (raw.empty()) {
            LOGW("[HttpClient] no response data from %s\n", currentUrl.c_str());
            return false;
        }

        // split headers and body
        const std::string delim = "\r\n\r\n";
        auto it = std::search(raw.begin(), raw.end(), delim.begin(), delim.end());
        if (it == raw.end())
            return false;

        size_t headerLen = it - raw.begin();
        std::string headers(reinterpret_cast<const char*>(raw.data()), headerLen);
        size_t bodyStart = headerLen + delim.size();

        int status = 0;
        if (!extractStatusCode(headers, status))
            return false;

        if (status == 301 || status == 302 || status == 307 || status == 308) {
            std::string location = getHeaderValue(headers, "Location");
            if (location.empty()) {
                LOGW("[HttpClient] redirect without Location header for %s\n", currentUrl.c_str());
                return false;
            }
            LOGI("[HttpClient] redirect %s -> %s\n", currentUrl.c_str(), location.c_str());
            currentUrl = location;
            continue;
        }

        if (status != 200) {
            std::string bodySnippet;
            if (!outBody.empty()) {
                size_t len = outBody.size() < 1024 ? outBody.size() : 1024;
                bodySnippet.assign(outBody.begin(), outBody.begin() + len);
            }
            LOGW("[HttpClient] HTTP status %d for %s\n", status, currentUrl.c_str());
            LOGW("[HttpClient] Headers:\n%s\n", headers.c_str());
            LOGW("[HttpClient] Body (up to 1024 bytes):\n%s\n", bodySnippet.c_str());
            return false;
        }

        outBody.assign(raw.begin() + bodyStart, raw.end());
        return true;
    }

    return false;
}

} // namespace HttpClient
