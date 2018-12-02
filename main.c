#include <stdio.h>
#include "superstring.h"

int main(int argc, char **argv) {
  if (argc > 1)
    printf("%s\n", ukkonen_superstring(argc - 1, argv + 1));
  return 0;
}
