#include "test_utils.hpp"

struct Node
{
    int id{};
    std::vector<std::weak_ptr<Node>> neighbors;

    Node(int id): id(id) {}

    bool operator==(const Node& other) const
    {
        return id == other.id &&
               neighbors.size() == other.neighbors.size(); // simple shallow comparison
    }
};

template <>
struct reflexx::provider<Node>
{
    Node operator()() const
    {
        return Node(0);
    }
};


template <typename TSerializer>
using TSerializerWithGraphHandler = serializer
<
    TSerializer::settings,
    typename TSerializer::backend_type,
    typename type_handler_list<reflexx::ptr_graph_handler>::extend<typename TSerializer::handler_list_type>
>;

TEMPLATE_LIST_TEST_CASE("Cyclic graph with shared_ptr", "", serializers_list)
{
    using serializer = TSerializerWithGraphHandler<TestType>;

    // Create cyclic graph:
    //
    //    A --> B
    //    ^     |
    //    |_____|
    //

    auto A { std::make_shared<Node>(1) };
    auto B { std::make_shared<Node>(2) };

    A->neighbors.push_back(B);
    B->neighbors.push_back(A); // cycle

    std::shared_ptr<Node> original { A };

    auto result = serializer::serialize(original);
    auto deserialized_holder { serializer::template deserialize<std::shared_ptr<Node>>(result.get()) };
    auto& deserialized = *deserialized_holder;
    
    REQUIRE(deserialized->id == 1);
    REQUIRE(deserialized->neighbors.size() == 1);
    REQUIRE(deserialized->neighbors[0].lock()->id == 2);
    REQUIRE(deserialized->neighbors[0].lock()->neighbors.size() == 1);
    REQUIRE(deserialized->neighbors[0].lock()->neighbors[0].lock() == deserialized); // check cycle
}

TEMPLATE_LIST_TEST_CASE("DAG graph with shared_ptr", "", serializers_list)
{
    using serializer = TSerializerWithGraphHandler<TestType>;

    auto A { std::make_shared<Node>(1) };
    auto B { std::make_shared<Node>(2) };
    auto C { std::make_shared<Node>(3) };

    A->neighbors.push_back(C);
    B->neighbors.push_back(C);

    auto root { std::make_shared<Node>(0) };
    root->neighbors.push_back(A);
    root->neighbors.push_back(B);

    auto result = serializer::serialize(root);
    auto deserialized_holder { serializer::template deserialize<std::shared_ptr<Node>>(result.get()) };
    auto& deserialized = *deserialized_holder;

    REQUIRE(deserialized->id == 0);
    REQUIRE(deserialized->neighbors.size() == 2);

    auto A_des = deserialized->neighbors[0].lock();
    auto B_des = deserialized->neighbors[1].lock();

    REQUIRE(A_des->id == 1);
    REQUIRE(B_des->id == 2);
    REQUIRE(A_des->neighbors[0].lock() == B_des->neighbors[0].lock()); // shared C
    REQUIRE(A_des->neighbors[0].lock()->id == 3);
}

TEMPLATE_LIST_TEST_CASE("Deep graph with cycle at the end", "", serializers_list)
{
    using serializer = TSerializerWithGraphHandler<TestType>;

    auto A { std::make_shared<Node>(1) };
    auto B { std::make_shared<Node>(2) };
    auto C { std::make_shared<Node>(3) };
    auto D { std::make_shared<Node>(4) };

    A->neighbors.push_back(B);
    B->neighbors.push_back(C);
    C->neighbors.push_back(D);
    D->neighbors.push_back(A); // cycle

    auto result = serializer::serialize(A);
    auto deserialized_holder { serializer::template deserialize<std::shared_ptr<Node>>(result.get()) };
    auto& deserialized = *deserialized_holder;

    REQUIRE(deserialized->id == 1);
    REQUIRE(deserialized->neighbors[0].lock()->neighbors[0].lock()->neighbors[0].lock()->neighbors[0].lock() == deserialized);
}

TEMPLATE_LIST_TEST_CASE("Graph with nullptr in neighbors", "", serializers_list)
{
    using serializer = TSerializerWithGraphHandler<TestType>;

    auto A { std::make_shared<Node>(1) };
    auto B { std::make_shared<Node>(0) };
    A->neighbors.push_back(std::shared_ptr<Node>(nullptr));
    A->neighbors.push_back(B);

    auto result = serializer::serialize(A);
    auto deserialized_holder { serializer::template deserialize<std::shared_ptr<Node>>(result.get()) };
    auto& deserialized = *deserialized_holder;

    REQUIRE(deserialized->id == 1);
    REQUIRE(deserialized->neighbors.size() == 2);
    REQUIRE(deserialized->neighbors[0].lock() == nullptr);
    REQUIRE(deserialized->neighbors[1].lock() != nullptr);
}