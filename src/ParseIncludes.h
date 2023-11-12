#pragma once

#include <QStandardItemModel>
#include <string_view>

enum class Parsing
{
    MSVC,
    CLANG
};

QStandardItemModel *createModel(std::string_view corpus, Parsing parsing);
