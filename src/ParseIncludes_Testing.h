#pragma once
#include "ParseIncludes.h"

struct Node
{
    std::string_view fileName;
    size_t depth;
};

std::optional<Node> isIncludeMSVC(std::string_view line);

std::optional<Node> isIncludeClang(std::string_view line);