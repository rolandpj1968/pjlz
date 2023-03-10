#ifndef SUFFIX_SORT_HPP
#define SUFFIX_SORT_HPP

#include <algorithm>

#include "int-types.hpp"

//
// Utterly naive O(N^2 logN) suffix sort
//
namespace SuffixSort {

  template <typename sizeN_t>
  inline bool suffix_less(const u8* s, sizeN_t n, sizeN_t i1, sizeN_t i2) {
  
    sizeN_t min_len = n - std::max(i1, i2);

    for (sizeN_t i = 0; i < min_len; i++) {
    
      u8 c1 = s[i1+i];
      u8 c2 = s[i2+i];

      if (c1 == c2) {
	continue;
      }

      return c1 < c2;
    }

    // If suffixes are identical up to the end of the shorter suffix then the shorter suffix is considered smaller.
    return i1 > i2;
  }

  template <typename sizeN_t>
  struct suffix_less_comparator {
    const u8 *const s;
    const sizeN_t n;
    
    suffix_less_comparator(const u8 *const s, const sizeN_t n) :
      s(s),
      n(n)
    {}

    bool operator()(const sizeN_t i1, const sizeN_t i2) {
      return suffix_less(s, n, i1, i2);
    }
  };

  template <typename sizeN_t>
  inline void suffix_sort_range(const u8* s, sizeN_t n, sizeN_t* ss, sizeN_t ss_len) {
    suffix_less_comparator<sizeN_t> suffix_less(s, n);

    // stable_sort seems consistently faster than sort
    std::stable_sort(ss, ss + ss_len, suffix_less);
  }

  //
  // ss[i] is filled with the suffix sort of s
  //
  // Naive O(N^2logN) algo.
  // TODO implement more efficient algorithm.
  //
  template <typename sizeN_t>
  inline void __attribute__ ((noinline)) suffix_sort(const u8* s, sizeN_t* ss, sizeN_t n) {

    // unordered suffix indexes
    for (sizeN_t i = 0; i < n; i++) {
      ss[i] = i;
    }

    suffix_sort_range(s, n, ss, n);
  }

  template <typename sizeN_t>
  inline bool __attribute__ ((noinline)) check_suffix_sort(const u8* s, const sizeN_t* ss, sizeN_t n) {
    for (sizeN_t i = 0; i < n-1; i++) {
      if (!suffix_less(s, n, ss[i], ss[i+1])) {
	return false;
      }
    }

    return true;
  }

  //
  // Populate ssi with the inverse map of ss.
  //
  template <typename sizeN_t>
  inline void __attribute__ ((noinline)) inverse_suffix_sort(const sizeN_t* ss, sizeN_t* ssi, sizeN_t n) {
    for (sizeN_t i = 0; i < n; i++) {
      ssi[ss[i]] = i;
    }
  }

  template <typename sizeN_t>
  inline bool __attribute__ ((noinline)) check_inverse_suffix_sort(const sizeN_t* ss, const sizeN_t* ssi, sizeN_t n) {
    for (sizeN_t i = 0; i < n-1; i++) {
      if (ssi[ss[i]] != i) {
	return false;
      }
    }

    return true;
  }

} // namespace SuffixSort

#endif //def SUFFIX_SORT_HPP
