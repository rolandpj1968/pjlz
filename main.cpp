#include <chrono>
#include <cstddef>
#include <cstdio>

#include "nearest-prefix.hpp"
#include "slurp.hpp"
#include "suffix-sort.hpp"
#include "util.hpp"

// Assume 7-bit encoding with hi-bit as continuation.
static size_t encoded_len(size_t val, size_t max_nibble_val) {
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

  SuffixSort::suffix_sort(s, n, ss);

  t1 = Time::now();
  ds = t1 - t0;
  secs = ds.count();
  
  printf("Suffix sorted %s length %zu bytes in %.3lf milliseconds - %.3lf MB/s\n", argv[1], n, secs*1000.0, n/secs/1024/1024);
  
  t0 = Time::now();

  size_t* npf = new size_t[n];
  NearestPrefix::nearest_prefix_forwards(ss, npf, n);

  size_t* npb = new size_t[n];
  NearestPrefix::nearest_prefix_backwards(ss, npb, n);
  
  t1 = Time::now();
  ds = t1 - t0;
  secs = ds.count();
  
  printf("Found nearest prefixes forwards and backwards in %.3lf milliseconds - %.3lf MB/s\n", secs*1000.0, n/secs/1024/1024);
  
  t0 = Time::now();

  size_t* bpm = new size_t[n];
  size_t* lcp = new size_t[n];
  NearestPrefix::best_prefix_match(s, ss, npf, npb, bpm, lcp, n);

  t1 = Time::now();
  ds = t1 - t0;
  secs = ds.count();
  
  printf("Found best prefix matches in %.3lf milliseconds - %.3lf MB/s\n", secs*1000.0, n/secs/1024/1024);

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

      size_t j = bpm[i];

      if (j == n) {
	printf("<no matching prefix>\n");
      } else {
	size_t s_j_len = n-j;

	printf("best match at %6zu lcp %6zu: %*.16s\n", j, lcp[i], (int)std::min(s_j_len, prefix_printf_len), &s[j]);
      }
    }
  }

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
      size_t match_len = lcp[i];
      if (match_len >= MIN_MATCH_LEN) {
	// Match!
	size_t offset = i-bpm[i];
	size_t offset_len = encoded_len(offset, 0);

	if (offset_len + 1 > match_len) {
	  lit_len++;
	  continue;
	}
	
	n_matches++;
	  
	n_match_offsets_of_len[offset_len-1]++;

	size_t lit_len_len = encoded_len(lit_len, 15);
	n_literal_lengths_of_len[lit_len_len]++;

	size_t match_len_len = encoded_len(match_len-MIN_MATCH_LEN, 15);
	n_match_lengths_of_len[match_len_len]++;

	total_lit_len += lit_len;
	lit_len = 0;

	// Skip the match
	i += match_len-1;
      } else {
	lit_len++;
      }
    }

    printf("\n");
    printf("%zu matches / total match-len %zu / total lit-len %zu\n", n_matches, n-total_lit_len, total_lit_len);
    printf("    offset  encodings:                    1-byte: %6zu / 2-byte: %6zu / 3-byte: %6zu / 4-byte: %6zu\n", n_match_lengths_of_len[0], n_match_lengths_of_len[1], n_match_lengths_of_len[2], n_match_lengths_of_len[3]);
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
}
