#ifndef MAXIMAL_SUBSTRING_MATCH
#define MAXIMAL_SUBSTRING_MATCH

#include "util.hpp"

namespace MaximalSubstringMatch {

  //
  // msm_offsets[i] will contain the string s offset from i of a maximal substring match preceding s[i...], or 0 if there is no match
  // msm_lens[i] will contain the length of the corresponding substring match, or 0 if none is found
  //
  // TODO - we want the clostest such match ideally - pop from stack only when lcp drops
  //
  // Matches shorter than min_match_len will be ignored.
  //
  // O(N) algo.
  //
  template <typename sizeN_t>
  inline void __attribute__ ((noinline)) maximal_substring_matches(const u8* s, const sizeN_t* ss, const sizeN_t* lcp, sizeN_t* msm_offsets, sizeN_t* msm_lens, sizeN_t n, sizeN_t min_match_len) {

    // Contains indexes in ss of suffixes without a prefix (yet)
    sizeN_t* unmatched_s_is = new sizeN_t[n];
    // LCP's of unmatched substrings
    sizeN_t* unmatched_lcps = new sizeN_t[n];
    sizeN_t unmatched_top = 0;
    
    // Search forwards for matches
    {
      unmatched_top = 0;
      
      for (sizeN_t rank_i = 0; rank_i < n; rank_i++) {
	sizeN_t s_i = ss[rank_i];

	// Update the match lcp of the top-of-stack item
	if (unmatched_top != 0) {
	  unmatched_lcps[unmatched_top-1] = std::min(unmatched_lcps[unmatched_top-1], lcp[rank_i-1]);
	}
      
	while (unmatched_top != 0 && s_i < unmatched_s_is[unmatched_top-1]) {
	  
	  sizeN_t match_s_i = unmatched_s_is[unmatched_top-1];
	  sizeN_t match_lcp = unmatched_lcps[unmatched_top-1];
	  --unmatched_top;
	
	  // Update the match lcp of the new top-of-stack item.
	  if (unmatched_top != 0) {
	    unmatched_lcps[unmatched_top-1] = std::min(unmatched_lcps[unmatched_top-1], match_lcp);
	  }
	  
	  sizeN_t match_len = match_lcp;

	  // Only bother with matches of min_match_len or longer
	  if (match_len >= min_match_len) {
	    sizeN_t match_offset = match_s_i - s_i;
	  
	    msm_offsets[match_s_i] = match_offset;
	    msm_lens[match_s_i] = match_len;
	  }
	}
      
	unmatched_top++;
	unmatched_s_is[unmatched_top-1] = s_i;
	unmatched_lcps[unmatched_top-1] = n;

	// Offset/len will be overwritten if/when a match is found.
	msm_offsets[s_i] = 0;
	msm_lens[s_i] = 0;
      }
    }

    // Search backwards for matches
    {
      // Re-use - remaining items on the forwards stack have no matches.
      unmatched_top = 0;
      
      // Ovoiding underflow for unsigned sizeN_t
      for (sizeN_t rank_i_plus_1 = n; rank_i_plus_1 > 0; --rank_i_plus_1) {
	sizeN_t rank_i = rank_i_plus_1 - 1;
	sizeN_t s_i = ss[rank_i];

	// Update the match lcp of the top-of-stack item
	if (unmatched_top != 0) {
	  unmatched_lcps[unmatched_top-1] = std::min(unmatched_lcps[unmatched_top-1], lcp[rank_i]);
	}
      
	while (unmatched_top != 0 && s_i < unmatched_s_is[unmatched_top-1]) {

	  sizeN_t match_s_i = unmatched_s_is[unmatched_top-1];
	  sizeN_t match_lcp = unmatched_lcps[unmatched_top-1];
	  --unmatched_top;
	
	  // Update the match lcp of the new top-of-stack item.
	  if (unmatched_top != 0) {
	    unmatched_lcps[unmatched_top-1] = std::min(unmatched_lcps[unmatched_top-1], match_lcp);
	  }
	  
	  sizeN_t match_len = match_lcp;

	  // Only bother with matches of min_match_len or longer
	  if (match_len >= min_match_len) {
	    sizeN_t match_offset = match_s_i - s_i;
	  
	    sizeN_t curr_match_offset = msm_offsets[match_s_i];
	    sizeN_t curr_match_len = msm_lens[match_s_i];

	    // Only use this match if it is longer or closer than the forwards match.
	    if (match_len > curr_match_len || (match_len == curr_match_len && match_offset < curr_match_offset)) {
	      msm_offsets[match_s_i] = match_offset;
	      msm_lens[match_s_i] = match_len;
	    }
	  }
	}
      
	unmatched_top++;
	unmatched_s_is[unmatched_top-1] = s_i;
	unmatched_lcps[unmatched_top-1] = n;
      }
    }

    delete[] unmatched_lcps;
    delete[] unmatched_s_is;
  }

  template <typename sizeN_t>
  inline bool __attribute__ ((noinline)) check_maximal_substring_matches(const u8* s, sizeN_t* msm_offsets, sizeN_t* msm_lens, sizeN_t n, sizeN_t min_match_len) {
    
    for (sizeN_t s_i = 0; s_i < n; s_i++) {

      sizeN_t msm_offset = msm_offsets[s_i];
      sizeN_t msm_len = msm_lens[s_i];

      if ((msm_offset == 0) != (msm_len == 0)) {
	return false;
      }

      if (msm_offset == 0) {
	continue;
      }

      if (msm_len < min_match_len) {
	return false;
      }

      if (msm_offset > s_i) {
	return false;
      }

      sizeN_t match_s_i = s_i - msm_offset;

      sizeN_t match_len = Util::longest_common_prefix(&s[s_i], n-s_i, &s[match_s_i], n-match_s_i);

      if (match_len != msm_len) {
	return false;
      }
    }

    return true;
  }  
  
} // namespace MaximalSubstringMatch

#endif //def MAXIMAL_SUBSTRING_MATCH
