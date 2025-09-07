#pragma once
#include <string>
#include <vector>

namespace hx {
struct DialogueLine { std::string speaker; std::string text; };
struct Dialogue { std::vector<DialogueLine> lines; };
} // namespace hx
