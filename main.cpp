#include <chrono>
#include <cstddef>
#include <cstdio>

#include "nearest-prefix.hpp"
#include "slurp.hpp"
#include "suffix-sort.hpp"
#include "util.hpp"

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

  if (1) {
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
}
