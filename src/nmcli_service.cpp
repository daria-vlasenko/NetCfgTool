#include "nmcli_service.h"
#include <cstdio>
#include <array>
#include <sstream>
#include <string>

static std::string run(const char* cmd) {
    std::array<char, 512> buf{};
    std::string out;
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return out;
    while (fgets(buf.data(), buf.size(), pipe)) out += buf.data();
    pclose(pipe);
    return out;
}

std::vector<Iface> NmcliService::list() const {
    std::vector<Iface> v;
    auto raw = run("nmcli -t -f DEVICE,TYPE,STATE device status");
    std::istringstream ss(raw);
    std::string line;
    while (std::getline(ss, line)) {
        if (line.empty()) continue;
        size_t p1 = line.find(':');
        size_t p2 = line.find(':', p1 + 1);
        if (p1 == std::string::npos || p2 == std::string::npos) continue;
        v.push_back({
            line.substr(0, p1),
            line.substr(p1 + 1, p2 - p1 - 1),
            line.substr(p2 + 1)
        });
    }
    return v;
}

std::string NmcliService::deviceShow(const std::string& dev) const {
    std::string cmd = "nmcli device show " + dev;
    return run(cmd.c_str());
}
