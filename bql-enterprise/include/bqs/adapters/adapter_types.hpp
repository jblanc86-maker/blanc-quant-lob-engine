#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace bqs::adapters
{

    // Lightweight, stable types for enterprise adapters.
    // These are placeholders and intentionally avoid venue-specific protocol details.

    enum class FeedKind : std::uint8_t
    {
        synthetic = 0,
        itch = 1,
        fix = 2,
        other = 255,
    };

    enum class SessionState : std::uint8_t
    {
        disconnected = 0,
        connecting = 1,
        established = 2,
        recovering = 3,
        closed = 4,
    };

    struct SequenceRange
    {
        std::uint64_t begin_inclusive{0};
        std::uint64_t end_inclusive{0};

        [[nodiscard]] constexpr bool empty() const noexcept
        {
            return begin_inclusive == 0 && end_inclusive == 0;
        }
    };

    struct AdapterConfig
    {
        std::string name;
        FeedKind kind{FeedKind::other};

        // Endpoint is intentionally generic (host:port, file path, unix socket path, etc.).
        std::string endpoint;

        // Optional reset/start sequence.
        std::uint64_t start_sequence{0};
    };

    struct AdapterStatus
    {
        SessionState state{SessionState::disconnected};
        std::uint64_t last_sequence{0};
        std::string detail;
    };

} // namespace bqs::adapters
