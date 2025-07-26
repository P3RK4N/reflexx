#include <catch2/catch_all.hpp>

#include <reflexx/util/move_construct_if_possible.hpp>

struct TrackerBase {
    bool default_constructed = false;
    bool copy_constructed = false;
    bool move_constructed = false;

    TrackerBase() : default_constructed(true) {}
    TrackerBase(const TrackerBase&) : copy_constructed(true) {}
    TrackerBase(TrackerBase&&) : move_constructed(true) {}
};

struct FullyMovable : TrackerBase {
    using TrackerBase::TrackerBase;
};

struct NonMovable : TrackerBase {
    NonMovable() = default;
    NonMovable(const NonMovable& other) : TrackerBase(other) {}
    NonMovable(NonMovable&&) = delete;
};

TEST_CASE("Valid constructions", "[move_construct_if_possible]") {
    SECTION("Fully Movable") {
        FullyMovable a;
        FullyMovable b { reflexx::util::move_construct_if_possible(a) };

        REQUIRE(b.move_constructed);
    }

    SECTION("Non Movable") {
        NonMovable a;
        NonMovable b{ reflexx::util::move_construct_if_possible(a) };

        REQUIRE(b.copy_constructed);
    }
}