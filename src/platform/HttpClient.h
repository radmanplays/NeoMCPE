#ifndef HTTPCLIENT_H__
#define HTTPCLIENT_H__

#include <string>
#include <vector>

namespace HttpClient {

/// Download the given URL into "outBody".
/// Returns true if the download completed successfully (HTTP 200) and the body is in outBody.
/// This function supports plain HTTP only (no TLS). It will follow up to 3 redirects.
bool download(const std::string& url, std::vector<unsigned char>& outBody);

} // namespace HttpClient

#endif /* HTTPCLIENT_H__ */
