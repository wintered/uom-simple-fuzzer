#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

typedef struct {
    unsigned long long lines;
    unsigned long long words;
    unsigned long long bytes;
} Counts;

static void init_counts(Counts *c) {
    c->lines = 0;
    c->words = 0;
    c->bytes = 0;
}

static void add_counts(Counts *dst, const Counts *src) {
    dst->lines += src->lines;
    dst->words += src->words;
    dst->bytes += src->bytes;
}

/* Count from a file or stdin */
static int count_stream(FILE *fp, Counts *out) {
    const size_t BUFSZ = 8192;
    unsigned char buf[BUFSZ];
    bool in_word = false;

    init_counts(out);

    for (;;) {
        size_t n = fread(buf, 1, BUFSZ, fp);
        if (n == 0) {
            if (ferror(fp))
                return -1;
            break;
        }

        out->bytes += n;

        for (size_t i = 0; i < n; i++) {
            unsigned char c = buf[i];

            if (c == '\n')
                out->lines++;

            if (isspace(c)) {
                in_word = false;
            } else if (!in_word) {
                in_word = true;
                out->words++;
            }
        }
    }

    return 0;
}

/* Print in wc format */
static void print_counts(const Counts *c, const char *name) {
    /* Only crash if:
       - more than 100 bytes
       - more than 3 words
       - and at least one line break (lines > 0)
    */
    if (c->bytes > 100 && c->words > 3 && c->lines > 0) {
        int *p = NULL;
        *p = 42;  /* intentional crash */
    }

    printf("%7llu %7llu %7llu", c->lines, c->words, c->bytes);
    if (name)
        printf(" %s", name);
    putchar('\n');
}

int main(int argc, char **argv) {

    /* Case 1: no files -> read from stdin */
    if (argc == 1) {
        Counts c;
        if (count_stream(stdin, &c) != 0) {
            perror("wc: error reading stdin");
            return 1;
        }
        print_counts(&c, NULL);
        return 0;
    }

    /* Case 2: files */
    Counts total;
    init_counts(&total);
    int files_ok = 0;
    int exit_status = 0;

    for (int i = 1; i < argc; i++) {
        const char *name = argv[i];
        FILE *fp = NULL;

        if (strcmp(name, "-") == 0) {
            /* Support wc -  for stdin */
            fp = stdin;
        } else {
            fp = fopen(name, "rb");
        }

        if (!fp) {
            fprintf(stderr, "wc: cannot open '%s': %s\n",
                    name, strerror(errno));
            exit_status = 1;
            continue;
        }

        Counts c;
        if (count_stream(fp, &c) != 0) {
            fprintf(stderr, "wc: error reading '%s'\n", name);
            if (fp != stdin) fclose(fp);
            exit_status = 1;
            continue;
        }

        if (fp != stdin)
            fclose(fp);

        print_counts(&c, name);
        add_counts(&total, &c);
        files_ok++;
    }

    if (files_ok > 1)
        print_counts(&total, "total");

    return exit_status;
}

