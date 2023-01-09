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
  //
  template <typename sizeN_t>
  inline void __attribute__ ((noinline)) longest_common_prefixes_naive(const u8* s, const sizeN_t* ss, sizeN_t* lcp, sizeN_t n) {
    
    for (sizeN_t i = 0; i < n-1; i++) {
      sizeN_t i1 = ss[i];
      sizeN_t i2 = ss[i+1];

      lcp[i] = Util::longest_common_prefix(&s[i1], n-i1, &s[i2], n-i2);
    }

    lcp[n-1] = 0;
  }

  //
  // lcp[i] will contain the longest-common-prefix of ss[i] and ss[i+1].
  // lcp[n-1] will be zero.
  //
  // Kasai O(N) algo.
  //
  template <typename sizeN_t>
  inline void __attribute__ ((noinline)) longest_common_prefixes_kasai(const u8* s, const sizeN_t* ss, const sizeN_t* ssi, sizeN_t* lcp, sizeN_t n) {

    sizeN_t curr_lcp = 0;

    // Iterate over suffixes s[i..] of the string
    for (sizeN_t i = 0; i < n; i++) {
      // Suffix sort rank of the substring
      sizeN_t rank_i = ssi[i];

      // Highest rank suffix in suffi sort order
      if (rank_i == n-1) {
	// No successor in suffix sort order
	lcp[rank_i] = 0;
	curr_lcp = 0;
	continue;
      }

      // Next suffix in suffix order
      int j = ss[rank_i+1];

      // We know already that at least curr_lcp characters match.
      // Manually find the actual lcp by counting from there.
      const sizeN_t lcp_limit = std::min(n-i, n-j);
      for (; curr_lcp < lcp_limit; curr_lcp++) {
	if (s[i+curr_lcp] != s[j+curr_lcp]) {
	  break;
	}
      }

      lcp[rank_i] = curr_lcp;

      // We're moving to the next string character, so lcp no longer includes this character.
      if (curr_lcp != 0) {
	curr_lcp--;
      }
    }
  }

  template <typename sizeN_t>
  inline void longest_common_prefixes(const u8* s, const sizeN_t* ss, const sizeN_t* ssi, sizeN_t* lcp, sizeN_t n) {
    return longest_common_prefixes_kasai(s, ss, ssi, lcp, n);
  }
  
  template <typename sizeN_t>
  inline bool __attribute__ ((noinline)) check_longest_common_prefixes(const u8* s, const sizeN_t* ss, const sizeN_t* lcp, sizeN_t n) {
    
    for (sizeN_t i = 0; i < n-1; i++) {
      sizeN_t i1 = ss[i];
      sizeN_t i2 = ss[i+1];

      if (lcp[i] != Util::longest_common_prefix(&s[i1], n-i1, &s[i2], n-i2)) {
	printf("ss[%zu] = %zu starting 0x%02x, ss[%zu] = %zu starting 0x%02x, lcp[%zu] = %zu but actual lcp is %zu\n", i, i1, s[i1], i+1, i2, s[i2], i, lcp[i], Util::longest_common_prefix(&s[i1], n-i1, &s[i2], n-i2));
	return false;
      }
    }

    return true;
  }
  
} // namespace LongestPrefixMatch

#endif //def LONGEST_PREFIX_MATCH
