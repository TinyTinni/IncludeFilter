
#include "ParseIncludes_Testing.h"

#include <optional>
#include <ranges>
#include <string_view>

std::optional<Node> isIncludeMSVC(std::string_view line)
{
    // remove leading whitespaces
    ;
    if (auto ltrimpos = line.find_first_not_of(" \t", 0); ltrimpos != line.npos)
        line.remove_prefix(ltrimpos);

    constexpr const char *const startString = "Note: including file:";
    if (!line.starts_with(startString))
        return std::nullopt;

    const auto file = line.substr(std::char_traits<char>::length(startString));
    if (file.empty())
        return std::nullopt;

    const auto depth = file.find_first_not_of(" ");
    if (depth == file.npos || depth == 0)
        return std::nullopt;
    const auto substrsize =
        (file[file.size() - 1] == '\r') ? file.size() - 1 : file.size();
    return std::optional{
        Node{.fileName = file.substr(depth, substrsize), .depth = depth}};
}

std::optional<Node> isIncludeClang(std::string_view line)
{
    if (!line.starts_with('.'))
        return std::nullopt;
    const auto depth = line.find_first_not_of(".");
    if (depth == std::string_view::npos || depth == 0 || line[depth] != ' ')
        return std::nullopt;
    return std::optional{
        Node{.fileName = line.substr(depth + 1, line.size() - depth),
             .depth = depth}};
}

QStandardItemModel *createModel(std::string_view corpus, Parsing parsing)
{

    QStandardItemModel *model = new QStandardItemModel();
    QStandardItem *const root_note = model->invisibleRootItem();

    std::string_view previous_line = "";
    QStandardItem *previous_item = root_note;
    int previous_depth = 0;

    const auto isInclude = [parsing]()
    {
        switch (parsing)
        {
        case Parsing::MSVC:
            return &isIncludeMSVC;
        case Parsing::CLANG:
            return &isIncludeClang;
        }
        throw std::runtime_error("No parser defined for the specified mode");
    }();

    for (auto &&current_line :
         corpus | std::views::split('\n') |
             std::views::transform(
                 [](auto &&rng) {
                     return std::string_view{rng.data(), rng.size()};
                 }))
    {
        if (auto include = isInclude(current_line))
        {
            // it is a new node. Add the previous line as root.
            // This root is the base file the include hierarchy starts from
            if (previous_item == root_note)
            {

                auto title_size =
                    (previous_line[previous_line.size() - 1] == '\r')
                        ? previous_line.size() - 1
                        : previous_line.size();
                previous_item = new QStandardItem(
                    QString::fromUtf8(previous_line.data(), title_size));
                root_note->appendRow(previous_item);
            }

            auto current_depth = include->depth;
            auto new_item = new QStandardItem(QString::fromUtf8(
                include->fileName.data(), include->fileName.size()));
            if (current_depth > previous_depth)
            {
                previous_item->appendRow(new_item);
            }
            else if (current_depth <= previous_depth)
            {
                auto parent = previous_item->parent();
                for (auto d = previous_depth; d != include->depth; --d)
                    parent = parent->parent();
                parent->appendRow(new_item);
            }
            previous_depth = current_depth;
            previous_item = new_item;
        }
        else
        {
            previous_depth = 0;
            previous_item = root_note;
            previous_line = current_line;
        }
    }
    return model;
}
