#include "doctest.h"

#include <string>

#include "World.h"
#include "analysis/dataflow/fact/SetFact.h"
#include "analysis/dataflow/fact/MapFact.h"

namespace al = analyzer;
namespace fact = al::analysis::dataflow::fact;

TEST_SUITE_BEGIN("testDataflowFacts");

TEST_CASE("testSetFact"
    * doctest::description("testing set like facts for dataflow analysis")) {

    al::World::getLogger().Progress("Testing set like facts for dataflow analysis ...");

    std::shared_ptr<std::string> s1 = std::make_shared<std::string>("s1");
    std::shared_ptr<std::string> s2 = std::make_shared<std::string>("s2");
    std::shared_ptr<std::string> s3 = std::make_shared<std::string>("s3");
    std::shared_ptr<std::string> s4 = std::make_shared<std::string>("s4");
    std::shared_ptr<std::string> s5 = std::make_shared<std::string>("s5");
    std::shared_ptr<std::string> s6 = std::make_shared<std::string>("s6");

    std::shared_ptr<fact::SetFact<std::string>> test1 = std::make_shared<fact::SetFact<std::string>>();

    CHECK(test1->isEmpty());
    CHECK_EQ(test1->size(), 0);

    CHECK(test1->add(s1));
    CHECK(test1->add(s2));
    CHECK(test1->add(s3));
    CHECK(test1->add(s4));
    CHECK_FALSE(test1->add(s2));
    CHECK_FALSE(test1->add(s3));
    CHECK_FALSE(test1->add(s4));

    CHECK_FALSE(test1->isEmpty());
    CHECK_EQ(test1->size(), 4);

    CHECK(test1->contains(s1));
    CHECK(test1->contains(s2));
    CHECK(test1->contains(s3));
    CHECK(test1->contains(s4));
    CHECK_FALSE(test1->contains(s5));
    CHECK_FALSE(test1->contains(s6));

    std::shared_ptr<fact::SetFact<std::string>> test2 = test1->copy();
    CHECK(test2->equalsTo(test2));
    test2->clear();
    CHECK(test2->isEmpty());
    CHECK(test2->add(s3));
    CHECK(test2->add(s4));
    CHECK(test2->add(s5));
    CHECK(test2->add(s6));
    CHECK_EQ(test2->size(), 4);

    CHECK(test2->unionN(test1));
    CHECK_EQ(test2->size(), 6);

    CHECK(test2->remove(s1));
    CHECK(test2->remove(s2));
    CHECK_FALSE(test2->remove(s1));
    CHECK_FALSE(test2->remove(s2));

    CHECK(test2->intersect(test1));
    CHECK_EQ(test2->size(), 2);

    CHECK(test2->removeIf([&](const std::shared_ptr<std::string>& e) -> bool {
        return e == s3 || e == s4;
    }));
    CHECK(test2->isEmpty());

    test2->setSetFact(test1);
    CHECK_EQ(test2->size(), 4);
    CHECK(test1->equalsTo(test2));

    test2->removeAll(test1);
    CHECK(test2->isEmpty());

    al::World::getLogger().Success("Finish testing set like facts for dataflow analysis ...");

}

TEST_CASE("testMapFact"
    * doctest::description("testing map like facts for dataflow analysis")) {

    al::World::getLogger().Progress("Testing map like facts for dataflow analysis ...");

    std::shared_ptr<std::string> k1 = std::make_shared<std::string>("k1");
    std::shared_ptr<std::string> k2 = std::make_shared<std::string>("k2");
    std::shared_ptr<std::string> k3 = std::make_shared<std::string>("k3");
    std::shared_ptr<std::string> k4 = std::make_shared<std::string>("k4");
    std::shared_ptr<std::string> k5 = std::make_shared<std::string>("k5");
    std::shared_ptr<std::string> k6 = std::make_shared<std::string>("k6");

    std::shared_ptr<std::string> v1 = std::make_shared<std::string>("v1");
    std::shared_ptr<std::string> v2 = std::make_shared<std::string>("v2");
    std::shared_ptr<std::string> v3 = std::make_shared<std::string>("v3");
    std::shared_ptr<std::string> v4 = std::make_shared<std::string>("v4");
    std::shared_ptr<std::string> v5 = std::make_shared<std::string>("v5");
    std::shared_ptr<std::string> v6 = std::make_shared<std::string>("v6");

    std::shared_ptr<fact::MapFact<std::string, std::string>> test1 = std::make_shared<fact::MapFact<std::string, std::string>>();

    CHECK(test1->isEmpty());

    CHECK(test1->update(k1, v1));
    CHECK_EQ(test1->size(), 1);
    CHECK_FALSE(test1->update(k1, v1));
    CHECK_EQ(test1->size(), 1);
    CHECK(test1->update(k1, v2));
    CHECK(test1->update(k1, v1));
    CHECK(test1->update(k2, v2));
    CHECK(test1->update(k3, v3));
    CHECK(test1->update(k4, v4));
    CHECK(test1->update(k5, v5));

    CHECK_EQ(test1->size(), 5);

    CHECK_EQ(test1->get(k1), v1);
    CHECK_EQ(test1->get(k2), v2);
    CHECK_EQ(test1->get(k3), v3);
    CHECK_EQ(test1->get(k4), v4);
    CHECK_EQ(test1->get(k5), v5);

    CHECK_FALSE(test1->get(k6));

    CHECK_EQ(test1->keySet(), std::unordered_set<std::shared_ptr<std::string>>{k1, k2, k3, k4, k5});
    CHECK_EQ(test1->valueSet(), std::unordered_set<std::shared_ptr<std::string>>{v1, v2, v3, v4, v5});

    std::shared_ptr<fact::MapFact<std::string, std::string>> test2 = test1->copy();
    CHECK_EQ(test2->size(), 5);
    CHECK(test1->equalsTo(test2));
    CHECK(test2->equalsTo(test1));

    test2->clear();
    CHECK(test2->isEmpty());

    CHECK(test2->copyFrom(test1));
    CHECK_EQ(test2->keySet(), std::unordered_set<std::shared_ptr<std::string>>{k1, k2, k3, k4, k5});
    CHECK_EQ(test2->valueSet(), std::unordered_set<std::shared_ptr<std::string>>{v1, v2, v3, v4, v5});
    CHECK(test2->equalsTo(test1));

    CHECK_FALSE(test1->copyFrom(test2));
    CHECK(test1->equalsTo(test2));

    CHECK_EQ(test2->remove(k1), v1);
    CHECK_EQ(test2->remove(k2), v2);
    CHECK_EQ(test2->remove(k3), v3);
    CHECK_EQ(test2->size(), 2);
    CHECK_FALSE(test2->equalsTo(test1));

    al::World::getLogger().Success("Finish testing map like facts for dataflow analysis ...");

}

TEST_SUITE_END();

