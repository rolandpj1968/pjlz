#include <chrono>
#include <cstddef>
#include <cstdio>

#include "slurp.hpp"
#include "suffix-sort.hpp"

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
}
