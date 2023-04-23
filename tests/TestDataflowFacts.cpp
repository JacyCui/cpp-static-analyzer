#include "doctest.h"

#include <string>

#include "World.h"
#include "analysis/dataflow/fact/SetFact.h"

namespace fact = analyzer::analysis::dataflow::fact;

TEST_SUITE_BEGIN("testDataflowFacts");

TEST_CASE("testSetFact"
    * doctest::description("testing set like facts for dataflow analysis")) {

    analyzer::World::getLogger().Progress("Testing set like facts for dataflow analysis ...");

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

    test2->setSet(test1);
    CHECK_EQ(test2->size(), 4);
    CHECK(test1->equalsTo(test2));

    test2->removeAll(test1);
    CHECK(test2->isEmpty());

    analyzer::World::getLogger().Progress("Finish testing set like facts for dataflow analysis ...");

}

TEST_SUITE_END();

