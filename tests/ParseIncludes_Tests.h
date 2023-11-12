#pragma once
#include <ParseIncludes_Testing.h>

#include <QObject>
#include <QTest>

class ParseIncludes_Tests : public QObject
{
    Q_OBJECT
  private slots:
    void initTestCase() {}

    void cleanupTestCase() {}

    void includeParsing_data()
    {
        QTest::addColumn<QString>("line");
        QTest::addColumn<Parsing>("method");
        QTest::addColumn<bool>("can_parse");
        QTest::addColumn<int>("depth");
        QTest::addColumn<QString>("fileName");

        QString clangHeader = "/usr/include/wx-3.2/wx/wxprec.h";
        QString clangLine = QString(". ") + clangHeader;
        QString clangDepthLine = QString("..... ") + clangHeader;
        QString clangCppLine =
            "[1/2] Building CXX object CMakeFiles/df_viewer.dir/main.cpp.o";

        QString msvcHeader =
            "C:\\libs\\Qt\\6.5.0\\msvc2019_64\\include\\QtWidgets\\QMainWindow";
        QString msvcLine = "    Note: including file: " + msvcHeader;
        QString msvcDepthLine = "    Note: including file:     " + msvcHeader;
        QString msvcCppLine = " mocs_compilation_Debug.cpp";
        QString msvcEmptyLine = "Note: including file:�";

        QTest::addRow("clangHeader-clang")
            << clangHeader << Parsing::CLANG << false;
        QTest::addRow("clangHeader-msvc")
            << clangHeader << Parsing::MSVC << false;
        QTest::addRow("clangLine-clang")
            << clangLine << Parsing::CLANG << true << 1 << clangHeader;
        QTest::addRow("clangLine-msvc") << clangLine << Parsing::MSVC << false;
        QTest::addRow("clangDepthLine-clang")
            << clangDepthLine << Parsing::CLANG << true << 5 << clangHeader;
        QTest::addRow("clangDepthLine-msvc")
            << clangDepthLine << Parsing::MSVC << false;
        QTest::addRow("clangCppLine-clang")
            << clangCppLine << Parsing::CLANG << false;
        QTest::addRow("clangCppLine-msvc")
            << clangCppLine << Parsing::MSVC << false;

        QTest::addRow("msvcHeader-clang")
            << msvcHeader << Parsing::CLANG << false;
        QTest::addRow("msvcHeader-msvc")
            << msvcHeader << Parsing::MSVC << false;
        QTest::addRow("msvcLine-clang") << msvcLine << Parsing::CLANG << false;
        QTest::addRow("msvcLine-msvc")
            << msvcLine << Parsing::MSVC << true << 1 << msvcHeader;
        QTest::addRow("msvcDepthLine-clang")
            << msvcDepthLine << Parsing::CLANG << false;
        QTest::addRow("msvcDepthLine-msvc")
            << msvcDepthLine << Parsing::MSVC << true << 5 << msvcHeader;
        QTest::addRow("msvcCppLine-clang")
            << msvcCppLine << Parsing::CLANG << false;
        QTest::addRow("msvcCppLine-msvc")
            << msvcCppLine << Parsing::MSVC << false;

        QTest::addRow("msvcEmptyLine-msvc")
            << msvcEmptyLine << Parsing::MSVC << false;
    }

    std::optional<Node> methodIsInclude(Parsing p, const std::string &f)
    {
        if (p == Parsing::MSVC)
            return isIncludeMSVC(f);
        else if (p == Parsing::CLANG)
            return isIncludeClang(f);
        throw std::runtime_error("No Parser defined");
    }

    void includeParsing()
    {
        QFETCH(Parsing, method);
        QFETCH(QString, line);
        std::string std_line = line.toStdString();
        auto out{methodIsInclude(method, std_line)};
        QFETCH(bool, can_parse);
        QVERIFY(out.has_value() == can_parse);
        if (can_parse)
        {
            QFETCH(int, depth);
            QFETCH(QString, fileName);
            QCOMPARE(out->depth, depth);
            QCOMPARE(out->fileName, fileName.toStdString());
        }
    }

    void buildModel_data()
    {
        QTest::addColumn<Parsing>("method");
        QTest::addColumn<QString>("filename");
        QTest::addColumn<int>("root_count");
        QTest::addColumn<int>("first_level_count");

        QString msvc_partial_file = TEST_DATA "partial.txt";
        QString msvc_file = TEST_DATA "show_includes_msvc.txt";
        QString ninja_file = TEST_DATA "includes_ninja_clang.txt";

        QTest::newRow("partial-msvc")
            << Parsing::MSVC << msvc_partial_file << 1 << 1;
        QTest::newRow("partial-clang")
            << Parsing::CLANG << msvc_partial_file << 0;
        QTest::newRow("ninja-msvc") << Parsing::MSVC << ninja_file << 0;
        QTest::newRow("ninja-clang") << Parsing::CLANG << ninja_file << 1 << 7;
        QTest::newRow("msvc-msvc") << Parsing::MSVC << msvc_file << 4 << 1;
        QTest::newRow("msvc-clang") << Parsing::CLANG << msvc_file << 0;
    }
    void buildModel()
    {
        QFETCH(Parsing, method);
        QFETCH(QString, filename);

        QFile file(filename);
        if (!file.open(QIODeviceBase::ReadOnly))
        {
            throw std::runtime_error(file.errorString().toStdString());
        }

        auto file_content = [&file]()
        {
            auto file_ptr = (char *)file.map(0, file.size());
            if (file_ptr == nullptr)
            {
                throw std::runtime_error(file.errorString().toStdString());
            }
            return std::string_view{file_ptr, static_cast<size_t>(file.size())};
        };

        std::unique_ptr<QStandardItemModel> model{
            createModel(file_content(), method)};
        QFETCH(int, root_count);
        QCOMPARE(model->rowCount(), root_count);
        if (root_count > 0)
        {
            QFETCH(int, first_level_count);
            auto lvl_count = model->item(0, 0)->rowCount();
            QCOMPARE(lvl_count, first_level_count);
        }
    }
};