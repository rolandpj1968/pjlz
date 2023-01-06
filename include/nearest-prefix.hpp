#ifndef NEAREST_PREFIX_HPP
#define NEAREST_PREFIX_HPP

#include <stack>

#include "int-types.hpp"
#include "util.hpp"

namespace NearestPrefix {

  //
  // For i in [0, n) populate npf[i] with the index in ss of the closest prefix of ss[i]
  // that is at index j > i in ss.
  //
  // Value n in npf means no such prefix found.
  //
  template <typename sizeN_t>
  inline void nearest_prefix_forwards(const sizeN_t* ss, sizeN_t* npf, sizeN_t n) {

    // Contains indexes in ss of suffixes without a prefix (yet)
    std::stack<sizeN_t> unmatched;

    for (sizeN_t i = 0; i < n; i++) {
      sizeN_t suffix_index = ss[i];

      while (!unmatched.empty() && suffix_index < ss[unmatched.top()]) {
	sizeN_t matched_ss_index = unmatched.top();
	unmatched.pop();

	npf[matched_ss_index] = i;
      }

      unmatched.push(i);
      npf[i] = n; // will be overwritten with the real match if it is found
    }
  }
  
  //
  // For i in [0, n) populate npb[i] with the index in ss of the closest prefix of ss[i]
  // that is at index j < i in ss.
  //
  // Value n in npf means no prefix found.
  //
  template <typename sizeN_t>
  inline void nearest_prefix_backwards(const sizeN_t* ss, sizeN_t* npb, sizeN_t n) {

    // Contains indexes in ss of suffixes without a prefix (yet)
    std::stack<sizeN_t> unmatched;

    // Ovoiding underflow for unsigned sizeN_t
    for (sizeN_t i_plus_1 = n; i_plus_1 > 0; --i_plus_1) {
      sizeN_t i = i_plus_1 - 1;
      sizeN_t suffix_index = ss[i];

      while (!unmatched.empty() && suffix_index < ss[unmatched.top()]) {
	sizeN_t matched_ss_index = unmatched.top();
	unmatched.pop();

	npb[matched_ss_index] = i;
      }

      unmatched.push(i);
      npb[i] = n; // will be overwritten with the real match if it is found
    }
  }

  //
  // For i in [0, n) populate bpm[i] with the index j in s of the prefix of s[i...]
  // with the longest common prefix match with s[i...], i.e. j < i.
  //
  // Also populate lcp[i] with the longest common prefix between s[i...] and s[bpm[i]...], or with
  // 0 if bpm[i] == n.
  //
  // Value n in bpm means no such prefix was found.
  //
  // Note that s[i...] and s[j...] might actually have no common prefix, however if
  // they do, then it is the longest common prefix of s[i...] and any s[j...] where j < i.
  //
  template <typename sizeN_t>
  inline void best_prefix_match(const u8* s, const sizeN_t* ss, const sizeN_t* npf, const sizeN_t* npb, sizeN_t* bpm, sizeN_t* lcp, sizeN_t n) {
    
    for (sizeN_t i = 0; i < n; i++) {
      // Offset in s of suffix at index i in suffix order.
      sizeN_t s_i = ss[i];

      // These are both indexes in ss.
      sizeN_t npf_ss_j = npf[i];
      sizeN_t npb_ss_j = npb[i];

      if (npf_ss_j == n && npb_ss_j == n) {
	// No matching prefix
	bpm[s_i] = n;
	lcp[s_i] = 0;
      } else if (npf_ss_j == n || npb_ss_j == n) {
	// Only one ss index is valid, and it's the minimum ss index cos the other one is n
	sizeN_t np_ss_j = std::min(npf_ss_j, npb_ss_j);
	sizeN_t np_s_j = ss[np_ss_j];
	bpm[s_i] = np_s_j;
	lcp[s_i] = Util::longest_common_prefix(&s[s_i], n-s_i, &s[np_s_j], n-np_s_j);
      } else {
	// Both prefixes are valid, so the best is the one with the longest common prefix with s[i...]
	sizeN_t npf_s_j = ss[npf_ss_j];
	sizeN_t npb_s_j = ss[npb_ss_j];

	sizeN_t lcp_npf_s_j = Util::longest_common_prefix(&s[s_i], n-s_i, &s[npf_s_j], n-npf_s_j);
	sizeN_t lcp_npb_s_j = Util::longest_common_prefix(&s[s_i], n-s_i, &s[npb_s_j], n-npb_s_j);

	if (lcp_npf_s_j == lcp_npb_s_j) {
	  // Pick the prefix that's closer to s_i
	  bpm[s_i] = std::max(lcp_npf_s_j, lcp_npb_s_j);
	} else {
	  // Pick the prefix with the longer lcp
	  bpm[s_i] = lcp_npf_s_j > lcp_npb_s_j ? npf_s_j : npb_s_j;
	}
	lcp[s_i] = std::max(lcp_npf_s_j, lcp_npb_s_j);
      }
    }

    delete[] npf;
    delete[] npb;
  }
  
} // namespace NearestPrefix

#endif //def NEAREST_PREFIX_HPP
