#include <httplib.h>
#include <curl/curl.h>

#include <string>

extern "C" const char* probe_net_versions()
{
    static const std::string versions = [] {
        httplib::Client client("https://example.com");
        const curl_version_info_data* info = curl_version_info(CURLVERSION_NOW);
        return std::string(client.is_valid() ? "httplib-ok" : "httplib-bad") + " | " +
               (info ? info->version : "curl-unknown");
    }();
    return versions.c_str();
}
