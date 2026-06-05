#include "bqs/adapters/adapter_types.hpp"

#include <cassert>
#include <type_traits>

using namespace bqs::adapters;

int main()
{
    static_assert(std::is_trivially_copyable_v<SequenceRange>);

    assert(SequenceRange{}.empty());

    AdapterConfig cfg;
    cfg.name = "demo";
    cfg.kind = FeedKind::itch;
    cfg.endpoint = "localhost:1234";
    cfg.start_sequence = 42;

    assert(cfg.name == "demo");
    assert(cfg.kind == FeedKind::itch);
    assert(cfg.start_sequence == 42);

    AdapterStatus st;
    assert(st.state == SessionState::disconnected);

    return 0;
}
