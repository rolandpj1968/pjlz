#ifndef MAXIMAL_SUBSTRING_MATCH
#define MAXIMAL_SUBSTRING_MATCH

#include <utility>
#include <stack>

#include "util.hpp" // TODO get rid

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

    // Search forwards for matches
    {
      // Contains indexes in ss of suffixes without a prefix (yet)
      std::stack<sizeN_t> unmatched;
      
      for (sizeN_t ss_i = 0; ss_i < n; ss_i++) {
	sizeN_t s_i = ss[ss_i];
      
	while (!unmatched.empty() && s_i < ss[unmatched.top()]) {
	  sizeN_t matched_ss_i = unmatched.top();
	  unmatched.pop();
	
	  sizeN_t matched_s_i = ss[matched_ss_i];
	  sizeN_t match_offset = matched_s_i - s_i;
	  sizeN_t match_len = Util::longest_common_prefix(&s[s_i], n-s_i, &s[matched_s_i], n-matched_s_i);
	  
	  if (matched_s_i == 166965) {
	    printf("\n\n");
	    printf("                                                    forwards: matching s[166965...] with s[%zu...] len %zu\n", s_i, match_len);
	    printf("\n\n");
	  }
	  
	  msm_offsets[matched_s_i] = match_offset;
	  msm_lens[matched_s_i] = match_len;
	}
      
	// TODO why not just use s_i here?
	unmatched.push(ss_i);

	// Offset/len will be overwritten if/when a match is found.
	msm_offsets[s_i] = 0;
	msm_lens[s_i] = 0;
      }
    }

    // Search backwards for matches
    {
      // Contains indexes in ss of suffixes without a prefix (yet)
      std::stack<sizeN_t> unmatched;

      // Ovoiding underflow for unsigned sizeN_t
      for (sizeN_t ss_i_plus_1 = n; ss_i_plus_1 > 0; --ss_i_plus_1) {
	sizeN_t ss_i = ss_i_plus_1 - 1;
	sizeN_t s_i = ss[ss_i];

	while (!unmatched.empty() && s_i < ss[unmatched.top()]) {
	  sizeN_t matched_ss_i = unmatched.top();
	  unmatched.pop();

	  sizeN_t matched_s_i = ss[matched_ss_i];
	  sizeN_t match_offset = matched_s_i - s_i;
	  sizeN_t match_len = Util::longest_common_prefix(&s[s_i], n-s_i, &s[matched_s_i], n-matched_s_i);

	  sizeN_t curr_match_offset = msm_offsets[matched_s_i];
	  sizeN_t curr_match_len = msm_lens[matched_s_i];

	  if (matched_s_i == 166965) {
	    printf("\n\n");
	    printf("                                                    backwards: matching s[166965...] with s[%zu...] len %zu curr-len %zu\n", s_i, match_len, curr_match_len);
	    printf("\n\n");
	  }
	  
	  // Only use this match if it is longer or closer than the forwards match.
	  if (match_len > curr_match_len || (match_len == curr_match_len && match_offset < curr_match_offset)) {
	    msm_offsets[matched_s_i] = match_offset;
	    msm_lens[matched_s_i] = match_len;
	  }
	}

	// TODO why not just use s_i here?
	unmatched.push(ss_i);
      }
    }
  }
  
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
  inline void __attribute__ ((noinline)) maximal_substring_matches_abandon(const u8* s, const sizeN_t* ss, const sizeN_t* lcp, sizeN_t* msm_offsets, sizeN_t* msm_lens, sizeN_t n, sizeN_t min_match_len) {
    // stack of as-yet unmatched string indexes
    // pair (rank_i, lcp_i)
    std::pair<sizeN_t, sizeN_t>* unmatched = new std::pair<sizeN_t, sizeN_t>[n];
    sizeN_t unmatched_top = 0;

    // TODO remove @#$%@#$%@#$%
    const sizeN_t WATCH_S_I = /*9843143;*/10192440;

    // Iterate forwards over suffix sort to find substring matches later in the suffix sort
    for (sizeN_t rank_i = 0; rank_i < n; rank_i++) {
      // Index of suffix in s
      sizeN_t s_i = ss[rank_i];

      if (s_i == WATCH_S_I) {
	printf("                                                                      processing s[%zu] forwards at rank %zu tos %zu\n", s_i, rank_i, unmatched_top);
      }

      // Process matches where this string index is lower than stack entries
      while (unmatched_top > 0) {
	
	std::pair<sizeN_t, sizeN_t> match_rank_and_lcp_i = unmatched[unmatched_top];
	sizeN_t match_rank_i = match_rank_and_lcp_i.first;
	
	sizeN_t match_s_i = ss[match_rank_i];

	if (match_s_i < s_i) {
	  break;
	}

	// Pop from stack
	--unmatched_top;

	// TODO - use lcp for this
	sizeN_t match_len = Util::longest_common_prefix(&s[s_i], n-s_i, &s[match_s_i], n-match_s_i);

	if (match_s_i == WATCH_S_I) {
	  printf("                                                                         matched s[%zu] forwards at rank %zu tos %zu with s[%zu] offset %zu match-len %zu\n", match_s_i, rank_i, unmatched_top, s_i, match_s_i-s_i, match_len);
	}
	if (match_len >= min_match_len) {
	  //printf("                             Got 1 substring match!!!!!!!!!!!!!!!!!!!!!\n");
	  //assert(false);
	  msm_offsets[match_s_i] = match_s_i - s_i;
	  msm_lens[match_s_i] = match_len;
	}
      }

      if (s_i == WATCH_S_I) {
	printf("                                                                           pushing s[%zu] forwards at rank %zu tos %zu\n", s_i, rank_i, unmatched_top);
      }
      unmatched[unmatched_top++] = std::pair<sizeN_t, sizeN_t>(rank_i, lcp[rank_i]);

      // Will be overwritten with the real match if one is found
      msm_offsets[s_i] = 0;
      msm_lens[s_i] = 0;
    }

    // Clear stack for re-use - remaining elements have no forwards matches
    unmatched_top = 0;

    printf("                                      starting backwards tos %zu\n", unmatched_top);

    // Iterate backwards over suffix sort to find matches earlier in the suffix sort
    // @$%@#$%@#$%#$% TODO....
    if (1) for (sizeN_t rank_i = n-1; /*break in loop*/; --rank_i) {
      // Index of suffix in s
      sizeN_t s_i = ss[rank_i];

      if (s_i == WATCH_S_I) {
	printf("                                                                      processing s[%zu] backwards at rank %zu tos %zu\n", s_i, rank_i, unmatched_top);
      }

      {
	static bool done = false;
	if (!done && unmatched_top > 0 && unmatched[unmatched_top].first == WATCH_S_I) {
	  done = true;
	  printf("                                                                               s[%zu] at tos at rank %zu tos %zu for s[%zu]\n", WATCH_S_I, rank_i, unmatched_top, s_i);
	}
      }

      // Process matches if this string index is lower than stack entries
      while (unmatched_top > 0) {
	
	std::pair<sizeN_t, sizeN_t> match_rank_and_lcp_i = unmatched[unmatched_top];
	sizeN_t match_rank_i = match_rank_and_lcp_i.first;
	
	sizeN_t match_s_i = ss[match_rank_i];

	if (match_s_i < s_i) {
	  break;
	}

	// Pop from stack
	--unmatched_top;

	// TODO - use lcp for this
	sizeN_t match_len = Util::longest_common_prefix(&s[s_i], n-s_i, &s[match_s_i], n-match_s_i);

	if (match_s_i == WATCH_S_I) {
	  printf("                                                                         matched s[%zu] backwards at rank %zu tos %zu with s[%zu] offset %zu match-len %zu\n", match_s_i, rank_i, unmatched_top, s_i, match_s_i-s_i, match_len);
	}
	
	// Only use if the match is longer or closer than the forwards match.
	if (match_len >= min_match_len) {
	  //sizeN_t curr_match_offset = msm_offsets[match_s_i];
	  sizeN_t curr_match_len = msm_lens[match_s_i];
	  // @#$%@#$%@#$% TODO prefer closer too
	  if (curr_match_len < match_len) {
	    msm_offsets[match_s_i] = match_s_i - s_i;
	    msm_lens[match_s_i] = match_len;
	  }
	}
      }

      if (s_i == WATCH_S_I) {
	printf("                                                                           pushing s[%zu] backwards at rank %zu tos %zu\n", s_i, rank_i, unmatched_top);
      }
      unmatched[unmatched_top++] = std::pair<sizeN_t, sizeN_t>(rank_i, lcp[rank_i]);

      // Will be overwritten with the real match if one is found
      //msm_offsets[s_i] = 0;
      //msm_lens[s_i] = 0;

      // avoid underflow
      if (rank_i == 0) {
	break;
      }
    }

    delete[] unmatched;
  }

  template <typename sizeN_t>
  inline bool __attribute__ ((noinline)) check_maximal_substring_matches(const u8* s, sizeN_t* msm_offsets, sizeN_t* msm_lens, sizeN_t n, sizeN_t min_match_len) {
    
    for (sizeN_t s_i = 0; s_i < n; s_i++) {

      sizeN_t msm_offset = msm_offsets[s_i];
      sizeN_t msm_len = msm_lens[s_i];

      // TODO #$%@#$%@#$
      if (false && (msm_offset == 0) != (msm_len == 0)) {
	printf("                        s_i %zu Boooooooo1 match offset %zu len %zu\n", s_i, msm_offset, msm_len);
	return false;
      }

      if (msm_offset == 0) {
	continue;
      }

      // TODO @#$%@#$%
      if (false && msm_len < min_match_len) {
	printf("                        s_i %zu Boooooooo2\n", s_i);
	return false;
      }

      if (msm_offset > s_i) {
	printf("                        s_i %zu Boooooooo3\n", s_i);
	return false;
      }

      sizeN_t match_s_i = s_i - msm_offset;

      sizeN_t match_len = Util::longest_common_prefix(&s[s_i], n-s_i, &s[match_s_i], n-match_s_i);

      if (match_len != msm_len) {
	printf("                        s_i %zu Boooooooo4\n", s_i);
	return false;
      }
    }

    return true;
  }  
  
} // namespace MaximalSubstringMatch

#endif //def MAXIMAL_SUBSTRING_MATCH
