#include "ModelIndexIterator_Tests.h"
#include "ParseIncludes_Tests.h"

#include <QTest>

// todo:
// - parallel execution?
// - random execution order?

template <class TestClass> int test_me(int argc, char **argv)
{
    TestClass obj;
    return QTest::qExec(&obj, argc, argv);
}

#define TestExec(x) status |= test_me<x>(argc, argv);

int main(int argc, char **argv)
{
    int status = 0;

    TestExec(ParseIncludes_Tests);
    TestExec(ModelIndexIterator_Tests);

    return status;
}