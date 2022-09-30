#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define BUF_SIZE 16

const int WORD_BITS = sizeof(long) * 8;

struct CacheLine {
  unsigned long tag;
  unsigned long last_access;
  int is_valid;
};

void die(char *msg) {
  fprintf(stderr, "error: %s\n", msg);
  exit(1);
}

int main(int argc, char *argv[]) {
  char *t = NULL;
  int s = 4;  // default 2^4 = 16 sets
  int E = 1;  // default 1 cache line
  int b = 6;  // default b^6 = 64B blocks
  // 16 * 1 * 64B = 1024B default cache size

  // get args
  int opt = 0;
  while ((opt = getopt(argc, argv, "s:E:b:t:")) != -1) {
    switch (opt) {
      case 's':
        s = atoi(optarg);
        if (s < 0) die("s < 0");
        break;
      case 'E':
        E = atoi(optarg);
        if (E < 1) die("E < 1");
        break;
      case 'b':
        b = atoi(optarg);
        if (b < 0) die("b < 0");
        break;
      case 't':
        t = optarg;
        break;
      default:
        die("invalid argument");
        break;
    }
  }

  // open file and check it
  FILE *file = fopen(t, "r");
  if (file == NULL) {
    die("bad file argument");
  }

  // initialize cache lines & useful values
  int S = pow(2.0, (double)s);
  struct CacheLine *cache = calloc(S * E, sizeof(struct CacheLine));
  int tag_size = WORD_BITS - (s + b);

  int hit_count = 0;
  int miss_count = 0;
  int eviction_count = 0;
  unsigned long cycle = 0;

  // read lines
  char buf[BUF_SIZE];
  while (fgets(buf, BUF_SIZE, file)) {
    if (buf[0] == 'I') continue;  // skip instruction accesses

    // read line
    char access_type = 0;
    unsigned long address = 0;
    sscanf(buf, " %c %lx", &access_type, &address);

    // tag & set bits
    unsigned long tag = address >> (WORD_BITS - tag_size);
    unsigned long set = address << tag_size >> (WORD_BITS - s);

    struct CacheLine *set_ptr = cache + set * E;
    struct CacheLine *lru_line = NULL;
    struct CacheLine *invalid_line = NULL;

    // loop the set & check for hit
    for (int i = 0; i < E; ++i) {
      struct CacheLine *curr_line = set_ptr + i;

      if (!curr_line->is_valid) {
        invalid_line = curr_line;
        continue;
      }

      if (curr_line->tag == tag) {
        ++hit_count;
        curr_line->last_access = cycle;
        goto hit_found;
      }

      if (!lru_line) {
        lru_line = curr_line;
      } else {
        lru_line = curr_line->last_access < lru_line->last_access ? curr_line
                                                                  : lru_line;
      }
    }

    // hit not found in set, handle cache miss
    ++miss_count;

    struct CacheLine new_block = (struct CacheLine){tag, cycle, 1};

    if (invalid_line) {
      *invalid_line = new_block;
    } else {
      ++eviction_count;
      *lru_line = new_block;
    }

  hit_found:
    // M is basically L followed by S, but where S is guaranteed to hit
    if (access_type == 'M') ++hit_count;

    ++cycle;
    if (!cycle) die("cycle overflow - LRU compromised");
  }

  printf("hits:%d, misses:%d, evictions:%d\n", hit_count, miss_count,
         eviction_count);

  fclose(file);
  free(cache);

  return 0;
}
