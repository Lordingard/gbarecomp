#pragma once

#include "save_file.h"
#include <toml.hpp>
#include <chrono>
#include <fstream>
#include <string>

namespace gbarecomp {

struct RuntimeControlPreferences {
    bool assist_tools_enabled = true;
    bool rewind_enabled = true;
    int fast_forward_multiplier = 4;
    int state_slot = 1;
};

inline bool load_runtime_controls(const std::filesystem::path& path,
                                  RuntimeControlPreferences& controls,
                                  int slot_count, std::string& error) {
    try {
        std::ifstream file(path);
        if (!file) {
            std::error_code ec;
            error = std::filesystem::exists(path, ec) || ec
                ? "cannot read runtime-controls file; using defaults" : "";
            return false;
        }
        const auto table = toml::parse(file);
        auto read_int = [&](const char* key, int minimum, int maximum, int& out) {
            if (auto value = table[key].value<int64_t>(); value &&
                *value >= minimum && *value <= maximum)
                out = static_cast<int>(*value);
        };
        if (auto value = table["assist_tools_enabled"].value<bool>())
            controls.assist_tools_enabled = *value;
        if (auto value = table["rewind_enabled"].value<bool>())
            controls.rewind_enabled = *value;
        read_int("fast_forward_multiplier", 2, 10, controls.fast_forward_multiplier);
        read_int("state_slot", 1, slot_count, controls.state_slot);
        error.clear();
        return true;
    } catch (const toml::parse_error&) {
        error = "invalid runtime-controls TOML; using defaults";
        return false;
    }
}

inline bool save_runtime_controls(const std::filesystem::path& path,
                                  const RuntimeControlPreferences& controls,
                                  std::string& error) {
    try {
        toml::table table;
        std::ifstream previous(path);
        std::error_code read_error;
        if (!previous && (std::filesystem::exists(path, read_error) || read_error)) {
            error = "cannot read existing runtime-controls file; file left unchanged";
            return false;
        }
        if (previous) table = toml::parse(previous);
        previous.close();
        table.insert_or_assign("assist_tools_enabled", controls.assist_tools_enabled);
        table.insert_or_assign("rewind_enabled", controls.rewind_enabled);
        table.insert_or_assign("fast_forward_multiplier", controls.fast_forward_multiplier);
        table.insert_or_assign("state_slot", controls.state_slot);
        auto temporary = path;
        temporary += ".tmp." + std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count());
        {
            std::ofstream file(temporary, std::ios::binary | std::ios::trunc);
            if (!file) { error = "cannot create runtime-controls temporary file"; return false; }
            file << toml::toml_formatter(table);
            file.flush();
            if (!file) { error = "cannot write runtime-controls temporary file"; return false; }
            file.close();
            if (!file) { error = "cannot close runtime-controls temporary file"; return false; }
        }
        std::error_code ec;
        if (!replace_save_file(temporary, path, ec)) {
            error = "cannot replace runtime-controls file; temporary copy retained: " +
                    temporary.string();
            return false;
        }
        error.clear();
        return true;
    } catch (const toml::parse_error&) {
        error = "invalid existing runtime-controls TOML; file left unchanged";
        return false;
    }
}

} // namespace gbarecomp
