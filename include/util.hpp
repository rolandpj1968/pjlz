#ifndef UTIL_HPP
#define UTIL_HPP

#include "int-types.hpp"

namespace Util {
  
  template <typename sizeN_t>
  inline sizeN_t longest_common_prefix(const u8* s1, sizeN_t len1, const u8* s2, sizeN_t len2) {
    sizeN_t len = std::min(len1, len2);

    for (sizeN_t i = 0; i < len; i++) {
      u8 c1 = s1[i];
      u8 c2 = s2[i];

      if (c1 == c2) {
	continue;
      }

      return i;
    }

    return len;
  }

} // namespace Util

#endif //def UTIL_HPP
