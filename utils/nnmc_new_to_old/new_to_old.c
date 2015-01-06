#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv)
{
        FILE *in  = fopen(argv[1], "r");
        FILE *out = fopen(argv[2], "w");

        long y, u, v;

        char b;
        long offset;

        for (v = 0; v < 128; ++ v) {
                for (u = 0; u < 128; ++ u) {
                        for (y = 0; y < 128; ++ y) {
                                offset = (u * 0x2 + y * 0x200 + v * 0x20000 + 0x1000000);
                                fseek (in, offset, SEEK_SET);
                                fread (&b, 1, 1, in);
                                fwrite (&b, 1, 1, out);
                        }
                }
        }

        return 0;
}

