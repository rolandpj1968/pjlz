#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdio>

#include "longest-common-prefix.hpp"
#include "maximal-substring-match.hpp"
#include "slurp.hpp"
#include "suffix-sort.hpp"
#include "util.hpp"

// Assume 7-bit encoding with hi-bit as continuation.
static size_t encoded_len_pjlz(size_t val, size_t max_nibble_val) {
  if (val < max_nibble_val) {
    // It's all in the 4-bit nibble.
    return 0;
  }
  val -= max_nibble_val;
  size_t n_bytes = 0;

  do {
    n_bytes++;
    val = val >> 7;
  } while(val);

  return n_bytes;
}

// Assume 7-bit encoding with hi-bit as continuation.
static size_t encoded_len_lz4(size_t val, size_t max_nibble_val) {
  if (val < max_nibble_val) {
    // It's all in the 4-bit nibble.
    return 0;
  }
  val -= max_nibble_val;
  size_t n_bytes = 0;

  while (val > 255) {
    n_bytes++;
    val -= 255;
  };

  n_bytes++;

  return n_bytes;
}

int main(int argc, char* argv[]) {
  
  if(argc < 2) {
    fprintf(stderr, "%s <in-file>\n", argv[0]);
    exit(1);
  }

  typedef std::chrono::high_resolution_clock Time;
  typedef std::chrono::duration<double> dsec;
  
  auto t0 = Time::now();
  
  std::string s_str = Slurp::slurp(argv[1]);
  const u8* s = (const u8*) s_str.c_str();
  size_t n = s_str.length();
  
  auto t1 = Time::now();
  dsec ds = t1 - t0;
  double secs = ds.count();
  
  printf("Slurped %s length %zu bytes in %.3lf milliseconds\n", argv[1], n, secs*1000.0);

  t0 = Time::now();

  size_t* ss = new size_t[n];

  SuffixSort::suffix_sort(s, ss, n);

  t1 = Time::now();
  ds = t1 - t0;
  secs = ds.count();
  
  printf("Suffix sorted %s length %zu bytes in %.3lf milliseconds - %.3lf MB/s\n", argv[1], n, secs*1000.0, n/secs/1024/1024);
  
  t0 = Time::now();

  assert(SuffixSort::check_suffix_sort(s, ss, n) && "suffix sort is correct");

  t1 = Time::now();
  ds = t1 - t0;
  secs = ds.count();
  
  printf("Checked suffix sort for %s length %zu bytes in %.3lf milliseconds - %.3lf MB/s\n", argv[1], n, secs*1000.0, n/secs/1024/1024);
  
  t0 = Time::now();

  size_t* ssi = new size_t[n];

  SuffixSort::inverse_suffix_sort(ss, ssi, n);

  t1 = Time::now();
  ds = t1 - t0;
  secs = ds.count();
  
  printf("Inverse suffix sort of %s length %zu bytes in %.3lf milliseconds - %.3lf MB/s\n", argv[1], n, secs*1000.0, n/secs/1024/1024);
  
  t0 = Time::now();

  assert(SuffixSort::check_inverse_suffix_sort(ss, ssi, n) && "suffix sort is correct");

  t1 = Time::now();
  ds = t1 - t0;
  secs = ds.count();
  
  printf("Checked inverse suffix sort for %s length %zu bytes in %.3lf milliseconds - %.3lf MB/s\n", argv[1], n, secs*1000.0, n/secs/1024/1024);
  
  t0 = Time::now();

  size_t* lcp = new size_t[n];

  LongestCommonPrefix::longest_common_prefixes(s, ss, ssi, lcp, n);

  t1 = Time::now();
  ds = t1 - t0;
  secs = ds.count();
  
  printf("Generated ss lcp for %s length %zu bytes in %.3lf milliseconds - %.3lf MB/s\n", argv[1], n, secs*1000.0, n/secs/1024/1024);
  
  t0 = Time::now();

  assert(LongestCommonPrefix::check_longest_common_prefixes(s, ss, lcp, n) && "longest common prefixes are correct");

  t1 = Time::now();
  ds = t1 - t0;
  secs = ds.count();
  
  printf("Checked ss lcp sort for %s length %zu bytes in %.3lf milliseconds - %.3lf MB/s\n", argv[1], n, secs*1000.0, n/secs/1024/1024);
  
  t0 = Time::now();

  size_t* msm_offsets = new size_t[n];
  size_t* msm_lens = new size_t[n];
  const size_t MIN_MATCH_LEN = 4;
  
  MaximalSubstringMatch::maximal_substring_matches(s, ss, lcp, msm_offsets, msm_lens, n, MIN_MATCH_LEN);

  t1 = Time::now();
  ds = t1 - t0;
  secs = ds.count();
  
  printf("Found maximal substring matches in %.3lf milliseconds - %.3lf MB/s\n", secs*1000.0, n/secs/1024/1024);

  t0 = Time::now();

  assert(MaximalSubstringMatch::check_maximal_substring_matches(s, msm_offsets, msm_lens, n, MIN_MATCH_LEN) && "maximal substring matches are correct");

  t1 = Time::now();
  ds = t1 - t0;
  secs = ds.count();
  
  printf("Checked maximal substring matches in %.3lf milliseconds - %.3lf MB/s\n", secs*1000.0, n/secs/1024/1024);
  
  if (0) {
    size_t prefix_printf_len = 16;

    printf("\nSuffixes in sorted order:\n");
    for (size_t i = 0; i < n; i++) {
      size_t ss_i = ss[i];
      
      size_t ss_i_len = n-ss_i;

      printf("suffix %6zu at offset %6zu: %*.16s\n", i, ss_i, (int)std::min(ss_i_len, prefix_printf_len), &s[ss_i]);
    }

    printf("\n");
    
    for (size_t i = 0; i < n; i++) {
      size_t s_i_len = n-i;

      printf("suffix %6zu: %*.16s - ", i, (int)std::min(s_i_len, prefix_printf_len), &s[i]);

      size_t offset = msm_offsets[i];

      if (/*j == n*/offset == 0) {
	printf("<no matching prefix>\n");
      } else {
	size_t j = i-offset;
	size_t s_j_len = n-j;

	printf("best match at %6zu lsm %6zu: %*.16s\n", j, msm_lens[i], (int)std::min(s_j_len, prefix_printf_len), &s[j]);
      }
    }
  }

  // Greedy substring matches - pjlz.
  if (1) {
    const size_t MIN_MATCH_LEN = 4;
    
    size_t n_matches = 0;
    size_t n_match_offsets_of_len[4] = {};
    size_t n_literal_lengths_of_len[5] = {};
    size_t n_match_lengths_of_len[5] = {};
    size_t total_lit_len = 0;

    size_t lit_len = 0;

    // Let's just do a greedy walk...
    for (size_t i = 0; i < n; i++) {
      //size_t match_len = lsm[i];
      size_t match_len = msm_lens[i];

      if (match_len >= MIN_MATCH_LEN) {
	// Match!
	size_t offset = msm_offsets[i];

	size_t offset_len = encoded_len_pjlz(offset, 0);

	size_t match_len_len = encoded_len_pjlz(match_len-MIN_MATCH_LEN, 15);
	
	// > rather than >= cos it's actually beneficial to emit matches that themselves have
	//  zero benefit because they break up the literal string which then more often fits in
	//  a nibble.
	if (offset_len + match_len_len + 1 > match_len) {
	  lit_len++;
	  continue;
	}
	
	n_matches++;
	  
	n_match_offsets_of_len[offset_len-1]++;

	size_t lit_len_len = encoded_len_pjlz(lit_len, 15);
	n_literal_lengths_of_len[lit_len_len]++;

	n_match_lengths_of_len[match_len_len]++;

	total_lit_len += lit_len;
	lit_len = 0;

	// Skip the match
	i += match_len-1;
      } else {
	lit_len++;
      }
    }

    total_lit_len += lit_len;

    printf("\n");
    printf("pjlz encoding\n");
    printf("%zu matches / total match-len %zu / total lit-len %zu\n", n_matches, n-total_lit_len, total_lit_len);
    printf("    offset  encodings:                    1-byte: %6zu / 2-byte: %6zu / 3-byte: %6zu / 4-byte: %6zu\n", n_match_offsets_of_len[0], n_match_offsets_of_len[1], n_match_offsets_of_len[2], n_match_offsets_of_len[3]);
    printf("    lit-len encodings:   0-byte: %6zu / 1-byte: %6zu / 2-byte: %6zu / 3-byte: %6zu / 4-byte: %6zu\n", n_literal_lengths_of_len[0], n_literal_lengths_of_len[1], n_literal_lengths_of_len[2], n_literal_lengths_of_len[3], n_literal_lengths_of_len[4]);
    printf("    match-len encodings: 0-byte: %6zu / 1-byte: %6zu / 2-byte: %6zu / 3-byte: %6zu / 4-byte: %6zu\n", n_match_lengths_of_len[0], n_match_lengths_of_len[1], n_match_lengths_of_len[2], n_match_lengths_of_len[3], n_match_lengths_of_len[4]);
    printf("\n");

    size_t total_encoded_len = total_lit_len + n_matches;
    for (size_t i = 0; i < 4; i++) {
      total_encoded_len += n_match_lengths_of_len[i] * (i+1);
    }
    for (size_t i = 0; i < 5; i++) {
      total_encoded_len += n_literal_lengths_of_len[i] * i;
      total_encoded_len += n_match_lengths_of_len[i] * i;
    }

    printf("Raw %6zu bytes / compressed %6zu bytes / compression ratio %.3lf%%\n\n", n, total_encoded_len, (double)total_encoded_len/(double)n*100.0);
  }

  // Greedy substring matches - lz4.
  if (1) {
    const size_t MIN_MATCH_LEN = 4;
    
    size_t n_matches = 0;
    size_t total_lit_len = 0;

    size_t lit_len = 0;

    size_t lz4_len = 0;

    // Let's just do a greedy walk...
    for (size_t s_i = 0; s_i < n; s_i++) {
      size_t match_len = msm_lens[s_i];
      // lz4 offset is fixed 2-byte value - we need to prepare pbm with this but for now just ignore more distant matches
      if ((match_len >= MIN_MATCH_LEN) && (msm_offsets[s_i] < (1<<16))) {
	// Match!
	n_matches++;
	  
	size_t lit_len_len = encoded_len_lz4(lit_len, 15);
	size_t match_len_len = encoded_len_lz4(match_len-MIN_MATCH_LEN, 15);

	lz4_len += 1/*nibbles*/ + lit_len_len + lit_len + match_len_len + 2/*offset*/;

	total_lit_len += lit_len;
	lit_len = 0;

	// Skip the match
	s_i += match_len-1;
      } else {
	lit_len++;
      }
    }

    total_lit_len += lit_len;
    lz4_len += lit_len;
    
    // Last bit - inaccurate but pah
    lz4_len += 1 + 8 + 2;

    printf("\n");
    printf("lz4 encoding:\n");
    printf("%zu matches / total match-len %zu / total lit-len %zu\n", n_matches, n-total_lit_len, total_lit_len);

    printf("Raw %6zu bytes / compressed %6zu bytes / compression ratio %.3lf%%\n\n", n, lz4_len, (double)lz4_len/(double)n*100.0);
  }
}
