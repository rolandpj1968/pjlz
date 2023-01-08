#ifndef LONGEST_PREFIX_MATCH
#define LONGEST_PREFIX_MATCH

#include "int-types.hpp"
#include "util.hpp"

namespace LongestCommonPrefix {

  //
  // lcp[i] will contain the longest-common-prefix of ss[i] and ss[i+1].
  // lcp[n-1] will be zero.
  //
  // Naive O(N^2) algo.
  // TODO use Kasai O(N) algo.
  //
  template <typename sizeN_t>
  inline void __attribute__ ((noinline)) longest_common_prefixes(const u8* s, const sizeN_t* ss, sizeN_t* lcp, sizeN_t n) {
    
    for (sizeN_t i = 0; i < n-1; i++) {
      sizeN_t i1 = ss[i];
      sizeN_t i2 = ss[i+1];

      lcp[i] = Util::longest_common_prefix(&s[i1], n-i1, &s[i2], n-i2);
    }

    lcp[n-1] = 0;
  }

  template <typename sizeN_t>
  inline bool __attribute__ ((noinline)) check_longest_common_prefixes(const u8* s, const sizeN_t* ss, const sizeN_t* lcp, sizeN_t n) {
    
    for (sizeN_t i = 0; i < n-1; i++) {
      sizeN_t i1 = ss[i];
      sizeN_t i2 = ss[i+1];

      if (!(lcp[i] == Util::longest_common_prefix(&s[i1], n-i1, &s[i2], n-i2))) {
	return false;
      }
    }

    return true;
  }
  
} // namespace LongestPrefixMatch

#endif //def LONGEST_PREFIX_MATCH
