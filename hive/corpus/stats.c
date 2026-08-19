#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define ALPHABET 29

int main(void)
{
    uint64_t one[ALPHABET] = {0};
    uint64_t two[ALPHABET][ALPHABET] = {0};
    uint64_t three[ALPHABET][ALPHABET][ALPHABET] = {0};

    int prev1 = -1;
    int prev2 = -1;
    uint64_t n = 0;

    int c;

    while ((c = getchar()) != EOF) {
        if (c < 'a' || c > 's') { continue; }
        int x = c - 'a';
        one[x]++;
        if (prev1 >= 0)
            two[prev1][x]++;
        if (prev2 >= 0 && prev1 >= 0)
            three[prev2][prev1][x]++;
        prev2 = prev1;
        prev1 = x;
        n++;
    }

    if (n < 3) {
        fprintf(stderr, "Input must contain at least 3 characters\n");
        return EXIT_FAILURE;
    }

    double loss0 = 0.0;
    double loss1 = 0.0;
    double loss2 = 0.0;

    for (int z = 0; z < ALPHABET; z++) {
        if (one[z] != 0) {
            double p = (double)one[z] / (double)n;
            loss0 -= (double)one[z] * log2(p);
        }
    }

    for (int y = 0; y < ALPHABET; y++) {
        uint64_t total = 0;
        for (int z = 0; z < ALPHABET; z++)
            total += two[y][z];
        for (int z = 0; z < ALPHABET; z++) {
            if (two[y][z] != 0) {
                double p = (double)two[y][z] / (double)total;
                loss1 -= (double)two[y][z] * log2(p);
            }
        }
    }

    for (int x = 0; x < ALPHABET; x++) {
        for (int y = 0; y < ALPHABET; y++) {
            uint64_t total = 0;
            for (int z = 0; z < ALPHABET; z++)
                total += three[x][y][z];
            for (int z = 0; z < ALPHABET; z++) {
                if (three[x][y][z] != 0) {
                    double p = (double)three[x][y][z] /
                               (double)total;
                    loss2 -= (double)three[x][y][z] * log2(p);
                }
            }
        }
    }

    double L0 = loss0 / (double)n;
    double L1 = loss1 / (double)(n - 1);
    double L2 = loss2 / (double)(n - 2);

    double improvement1 = L0 - L1;
    double improvement2 = L1 - L2;
    double improvement_total = L0 - L2;

    printf("Prediction performance\n");
    printf("======================\n\n");
    printf("No context:          %.6f bits/letter\n", L0);
    printf("Previous letter:     %.6f bits/letter\n", L1);
    printf("Previous two letters: %.6f bits/letter\n\n", L2);
    printf("Improvement from 1 previous letter:  %.6f bits/letter\n", improvement1);
    printf("Additional improvement from 2nd letter: %.6f bits/letter\n", improvement2);
    printf("Total improvement from 2 letters:    %.6f bits/letter\n\n", improvement_total);
    printf("Perplexity with no context:           %.6f\n", pow(2.0, L0));
    printf("Perplexity with previous letter:      %.6f\n", pow(2.0, L1));
    printf("Perplexity with previous two letters: %.6f\n", pow(2.0, L2));

    FILE *f;
    f = fopen("1.probs", "w");
    if (!f) {
        perror("1.probs");
        return EXIT_FAILURE;
    }
    for (int z = 0; z < ALPHABET; z++) {
        if (one[z] != 0)
            fprintf(f, "%c %.17g\n",
                    'a' + z,
                    (double)one[z] / (double)n);
    }
    fclose(f);

    f = fopen("2.probs", "w");
    if (!f) {
        perror("2.probs");
        return EXIT_FAILURE;
    }
    for (int y = 0; y < ALPHABET; y++) {
        uint64_t total = 0;
        for (int z = 0; z < ALPHABET; z++)
            total += two[y][z];
        if (total == 0)
            continue;
        for (int z = 0; z < ALPHABET; z++) {
            if (two[y][z] != 0)
                fprintf(f, "%c%c %.17g\n",
                        'a' + y,
                        'a' + z,
                        (double)two[y][z] / (double)total);
        }
    }
    fclose(f);

    f = fopen("3.probs", "w");
    if (!f) {
        perror("3.probs");
        return EXIT_FAILURE;
    }
    for (int x = 0; x < ALPHABET; x++) {
        for (int y = 0; y < ALPHABET; y++) {
            uint64_t total = 0;
            for (int z = 0; z < ALPHABET; z++)
                total += three[x][y][z];
            if (total == 0)
                continue;
            for (int z = 0; z < ALPHABET; z++) {
                if (three[x][y][z] != 0)
                    fprintf(f, "%c%c%c %.17g\n",
                            'a' + x,
                            'a' + y,
                            'a' + z,
                            (double)three[x][y][z] / (double)total);
            }
        }
    }
    fclose(f);

    return EXIT_SUCCESS;
}

