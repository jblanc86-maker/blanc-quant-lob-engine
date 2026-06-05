#pragma once

#include "bqs/adapters/adapter_types.hpp"

#include <cstddef>
#include <cstdint>
#include <span>

namespace bqs::adapters
{

    // Contract: enterprise adapters provide an input byte stream (or frames)
    // and surface session/gap state. The OSS core can be wired to this later.
    class IMarketDataAdapter
    {
    public:
        virtual ~IMarketDataAdapter() = default;

        virtual void configure(const AdapterConfig &cfg) = 0;
        virtual void connect() = 0;
        virtual void disconnect() = 0;

        // Read up to out.size_bytes() into out. Returns byte count read.
        // Implementations may return 0 when no data is available.
        virtual std::size_t read(std::span<std::byte> out) = 0;

        // Request recovery for the missing range (if known).
        virtual void request_gap_fill(const SequenceRange &missing) = 0;

        [[nodiscard]] virtual AdapterStatus status() const = 0;
    };

} // namespace bqs::adapters
