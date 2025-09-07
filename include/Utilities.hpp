#pragma once

#include <algorithm>

namespace hx {

// 通用clamp函数
template<typename T>
T clamp(T v, T lo, T hi) {
    return std::max(lo, std::min(v, hi));
}

} // namespace hx
