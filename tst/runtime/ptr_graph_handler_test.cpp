#include <catch2/catch_all.hpp>

#include <reflexx/builtin/ptr_graph_handler.hpp>

#include <reflexx/backend.hpp>
#include <reflexx/serializer.hpp>
#include <reflexx/backends/yyjson_backend.hpp>

using handler_list_t = reflexx::type_handler_list<reflexx::ptr_graph_handler, reflexx::std_handler, reflexx::default_handler>;
using serializer = reflexx::serializer<reflexx::serializer_settings::Strict(), reflexx::backends::yyjson_backend, handler_list_t>;

struct Node
{
    int id{};
    std::vector<std::shared_ptr<Node>> neighbors;

    bool operator==(const Node& other) const
    {
        return id == other.id &&
               neighbors.size() == other.neighbors.size(); // simple shallow comparison
    }
};


TEST_CASE("Cyclic graph with shared_ptr") {

    // Create cyclic graph:
    //
    //    A --> B
    //    ^     |
    //    |_____|
    //

    auto A = std::make_shared<Node>();
    auto B = std::make_shared<Node>();

    A->id = 1;
    B->id = 2;

    A->neighbors.push_back(B);
    B->neighbors.push_back(A); // cycle

    std::shared_ptr<Node> original = A;

    auto result = serializer::serialize(original);
    auto deserialized = serializer::deserialize<std::shared_ptr<Node>>(result.get());

    REQUIRE(deserialized->id == 1);
    REQUIRE(deserialized->neighbors.size() == 1);
    REQUIRE(deserialized->neighbors[0]->id == 2);
    REQUIRE(deserialized->neighbors[0]->neighbors.size() == 1);
    REQUIRE(deserialized->neighbors[0]->neighbors[0] == deserialized); // check cycle
}

TEST_CASE("DAG graph with shared_ptr") {
    auto A = std::make_shared<Node>(); A->id = 1;
    auto B = std::make_shared<Node>(); B->id = 2;
    auto C = std::make_shared<Node>(); C->id = 3;

    A->neighbors.push_back(C);
    B->neighbors.push_back(C);

    auto root = std::make_shared<Node>(); root->id = 0;
    root->neighbors.push_back(A);
    root->neighbors.push_back(B);

    auto result = serializer::serialize(root);
    auto deserialized = serializer::deserialize<std::shared_ptr<Node>>(result.get());

    REQUIRE(deserialized->id == 0);
    REQUIRE(deserialized->neighbors.size() == 2);

    auto& A_des = deserialized->neighbors[0];
    auto& B_des = deserialized->neighbors[1];

    REQUIRE(A_des->id == 1);
    REQUIRE(B_des->id == 2);
    REQUIRE(A_des->neighbors[0] == B_des->neighbors[0]); // shared C
    REQUIRE(A_des->neighbors[0]->id == 3);
}

TEST_CASE("Deep graph with cycle at the end") {
    auto A = std::make_shared<Node>(); A->id = 1;
    auto B = std::make_shared<Node>(); B->id = 2;
    auto C = std::make_shared<Node>(); C->id = 3;
    auto D = std::make_shared<Node>(); D->id = 4;

    A->neighbors.push_back(B);
    B->neighbors.push_back(C);
    C->neighbors.push_back(D);
    D->neighbors.push_back(A); // cycle

    auto result = serializer::serialize(A);
    auto deserialized = serializer::deserialize<std::shared_ptr<Node>>(result.get());

    REQUIRE(deserialized->id == 1);
    REQUIRE(deserialized->neighbors[0]->neighbors[0]->neighbors[0]->neighbors[0] == deserialized);
}

TEST_CASE("Graph with nullptr in neighbors") {
    auto A = std::make_shared<Node>(); A->id = 1;
    A->neighbors.push_back(nullptr);
    A->neighbors.push_back(std::make_shared<Node>());

    auto result = serializer::serialize(A);
    auto deserialized = serializer::deserialize<std::shared_ptr<Node>>(result.get());

    REQUIRE(deserialized->id == 1);
    REQUIRE(deserialized->neighbors.size() == 2);
    REQUIRE(deserialized->neighbors[0] == nullptr);
    REQUIRE(deserialized->neighbors[1] != nullptr);
}