#pragma once

#include <ModelIndexIterator.h>
#include <QStandardItemModel>

#include <QObject>
#include <QTest>
#include <ranges>

class ModelIndexIterator_Tests : public QObject
{
    Q_OBJECT
    QStandardItemModel mEmptyModel;
    QStandardItemModel mSingleRowModel;
    QStandardItemModel mSingleColModel;
    QStandardItemModel mThreeLevelModel;
  private slots:
    void initTestCase()
    {

        mSingleRowModel.invisibleRootItem()->appendRow(new QStandardItem("r0"));
        mSingleRowModel.invisibleRootItem()->appendColumn(
            {new QStandardItem("c0")});

        auto r0 = new QStandardItem("r0");
        auto r1 = new QStandardItem("r1");
        auto r2 = new QStandardItem("r2");

        r0->appendRows({new QStandardItem("c0")});
        r1->appendRows({new QStandardItem("c1"), new QStandardItem("c2")});

        auto root = mThreeLevelModel.invisibleRootItem();
        root->appendRows({r0, r1, r2});
    }

    void cleanupTestCase() {}

    void iterating_data()
    {
        QTest::addColumn<QStandardItemModel *>("model");
        QTest::addColumn<QVector<QString>>("expected");

        QTest::addRow("Empty") << &mEmptyModel << QVector<QString>{};
        QTest::addRow("SingleRow")
            << &mSingleRowModel << QVector<QString>{QStringLiteral("r0")};
        QTest::addRow("SingleCol")
            << &mSingleColModel << QVector<QString>{QStringLiteral("c0")};
        QTest::addRow("ThreeLevelModel")
            << &mThreeLevelModel
            << QVector<QString>{"r0", "c0", "r1", "c1", "c2", "r2"};
    }

    void iterating()
    {
        QFETCH(QStandardItemModel *, model);
        QFETCH(QVector<QString>, expected);
        // c++23
        // for (auto &&[index, expected] :
        //      std::ranges::views::zip(*model, expected))
        for (int expectedIdx = 0; auto &&index : *model)
        {
            QVERIFY(index.isValid());
            QCOMPARE(model->itemFromIndex(index)->text(),
                     expected[expectedIdx]);
            ++expectedIdx;
        }
    }
};