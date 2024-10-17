#pragma once

#include "include/ConfigTypes.hpp"
#include <memory>
#include <map>

class MCPWMManager {
public:
    static MCPWMManager& instance() {
        static MCPWMManager s_manager;
        return s_manager;
    }


private:
    MCPWMManager() = default;

};
