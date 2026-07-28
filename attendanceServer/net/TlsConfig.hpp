#pragma once

#include <string>

namespace net {

struct TlsConfig {
    bool enabled = false;
    std::string cert_file = "certs/server.crt";
    std::string key_file = "certs/server.key";
    std::string ca_file = "certs/ca.crt";
    bool verify_client = false;
    std::string min_version = "TLSv1.2";
};

} // namespace net
