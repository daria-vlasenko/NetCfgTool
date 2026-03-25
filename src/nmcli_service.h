#pragma once
#include <string>
#include <vector>

struct Iface {
    std::string device;
    std::string type;
    std::string state;
};

class NmcliService {
public:
    std::vector<Iface> list() const;
    std::string deviceShow(const std::string& dev) const;
};
