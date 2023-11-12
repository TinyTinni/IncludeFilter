#include <cstdint>
#include <string_view>

#include <ParseIncludes_Testing.h>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{

    std::string_view test_corpus{reinterpret_cast<const char *>(data), size};
    auto blub = createModel(test_corpus, Parsing::CLANG);
    delete blub;
    return 0;
}