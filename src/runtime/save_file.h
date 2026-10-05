#pragma once

#include <filesystem>
#include <system_error>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace gbarecomp {

// Never delete either copy if replacement fails; the temporary file is recoverable.
inline bool replace_save_file(const std::filesystem::path& temporary,
                              const std::filesystem::path& destination,
                              std::error_code& error) {
#if defined(_WIN32)
    if (MoveFileExW(temporary.c_str(), destination.c_str(),
                    MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        error.clear();
    } else {
        error = std::error_code(static_cast<int>(GetLastError()), std::system_category());
    }
#else
    std::filesystem::rename(temporary, destination, error);
#endif
    return !error;
}

} // namespace gbarecomp
