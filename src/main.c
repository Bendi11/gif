#include "endian.h"

#include <stdio.h>
#include <assert.h>

int main(int argc, const char * const argv[]) {
    uint64_t le = 0x003E0000efbeadde;

    printf("%0lx -> %0lx\n", le, le_to_host64(le)); 
}
