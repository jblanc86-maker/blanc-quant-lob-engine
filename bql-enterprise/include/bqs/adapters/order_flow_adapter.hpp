#pragma once

#include "bqs/adapters/adapter_types.hpp"

#include <cstdint>
#include <string>

namespace bqs::adapters
{

    // Placeholder interface for enterprise order-flow connectivity.
    // No venue-specific semantics are expressed here.
    class IOrderFlowAdapter
    {
    public:
        virtual ~IOrderFlowAdapter() = default;

        virtual void configure(const AdapterConfig &cfg) = 0;
        virtual void connect() = 0;
        virtual void disconnect() = 0;

        // Submit a pre-encoded message payload. In an enterprise build this would be
        // typed and validated per venue; here we keep it generic.
        virtual void send_raw(std::string_view payload) = 0;

        [[nodiscard]] virtual AdapterStatus status() const = 0;
    };

} // namespace bqs::adapters
