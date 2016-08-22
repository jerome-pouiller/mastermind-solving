/* vim: set sw=4 expandtab: */
/*
 * Licence: GPL
 * Created: 2016-01-31 15:51:50+01:00
 * Main authors:
 *     - Jérôme Pouiller <jezz@sysmic.org>
 *
 * Given an history, return the best shot for player and for each possible master answer, return the best shot to play.
 * Exemple of output:
 *
 * 0012 (score: 222)
 *     00 (score: 81)
 *         3333, 3334, 3343, 3344, 3345, 3433, 3434, 3435, 3443, 3444, 3445, 3453,
 *     01 (score: 222)
 *         1131, 1133, 1134, 1331, 1333, 1334, 1341, 1343, 1344, 1345, 2223, 2233, [...]
 *     02 (score: 160)
 *         1121, 1123, 1221, 1223, 1231, 1233, 1234, 1321, 1323, 1324, 2121, 2123, [...]
 *     03 (score: 130)
 *         1100, 1101, 1103, 1130, 1300, 1301, 1303, 1304, 1330, 1340, 2200, 2203, [...]
 *     04 (score: 32)
 *         1120, 1200, 1201, 1203, 1220, 1230, 1320, 2100, 2101, 2103, 2120,
 *     10 (score: 182)
 *         0333, 0334, 0343, 0344, 0345, 1111, 1113, 1311, 1313, 1314, 2222, 2232, [...]
 *     11 (score: 198)
 *         0131, 0133, 0134, 0223, 0233, 0234, 0300, 0303, 0304, 0323, 0324, 0330, [...]
 *     12 (score: 98)
 *         0100, 0101, 0103, 0121, 0123, 0130, 0200, 0203, 0220, 0221, 0230, 0231, [...]
 *     13 (score: 22)
 *         0120, 0201, 1020, 1102, 1202, 1210, 1302, 2001, 2102, 2110,
 *     20 (score: 105)
 *         0000, 0003, 0030, 0033, 0034, 0111, 0113, 0222, 0232, 0311, 0313, 0314, [...]
 *     21 (score: 40)
 *         0001, 0020, 0023, 0031, 0110, 0122, 0132, 0202, 0211, 0213, 0302,
 *     22 (score: 5)
 *         0021, 0102, 0210, 1002, 2010,
 *     30 (score: 20)
 *         0002, 0010, 0011, 0013, 0022, 0032, 0112, 0212, 0312, 1012, 2012, 3012,
 *     40 (score: 1)
 *         0012,
 * 0102 (score: 222)
 *     00 (score: 81)
 *         3333, 3334, 3343, 3344, 3345, 3433, 3434, 3435, 3443, 3444, 3445,
 *     01 (score: 222)
 *         1311, 1313, 1314, 1331, 1333, 1334, 1341, 1343, 1344, 1345, 2223, 2233, [...]
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include "mastermind.h"

struct dbg_shot {
    shot_t s;
    struct dbg_shot *subs;
    int subs_len;
};

static int initialDepth = 0;
static int isMasterSearch = 0;

void *zalloc(size_t size) {
    void *buf;
    if (!size)
        return NULL;
    buf = malloc(size);
    memset(buf, 0, size);
    return buf;
}

void reccursive_free(struct dbg_shot *s) {
    if (!s)
        return;
    if (!s->subs && s->subs_len)
        assert(0);
    if (s->subs && !s->subs_len)
        assert(0);
    reccursive_free(s->subs);
    free(s);
};

void dbg_prShot(const shot_t shot) {
    int i;

    if (shot.d[NB_PLACES] == -1) {
        for (i = 0; i < NB_PLACES; i++)
            printf("%c", shot.d[i]);
    } else {
        for (i = NB_PLACES; i < NB_PLACES + NB_HINTS; i++)
            printf("%d", shot.d[i]);
    }
}

void reccursive_print(struct dbg_shot *in, int depth) {
    int i, j;
    int full_print = 0;

    if (!in || !in->subs_len)
        return;

    for (i = 0; i < in->subs_len; i++)
        if (in->subs[i].subs_len)
            full_print = 1;
    if (full_print) {
        for (i = 0; i < in->subs_len; i++) {
            if (in->subs[i].s.d[IDX_SCORE] != INT_MAX) {
                for (j = 0; j < depth; j++)
                    printf("    ");
                dbg_prShot(in->subs[i].s);
                printf(" (score: %d)\n", in->subs[i].s.d[IDX_SCORE]);
                reccursive_print(in->subs + i, depth + 1);
            } else {
                assert(!in->subs[i].subs_len);
            }
        }
    } else {
        for (j = 0; j < depth; j++)
            printf("    ");
        for (i = 0; i < in->subs_len; i++) {
            dbg_prShot(in->subs[i].s);
            printf(", ");
            assert(!in->subs[i].subs_len);
        }
        printf("\n");
    }
}

void *dbg_start(shot_t history[], colorlist_t *colors, int depth, int masterSearch, void *priv) {
    initialDepth = depth;
    isMasterSearch = masterSearch;
    struct dbg_shot *result = (struct dbg_shot *) zalloc(sizeof(struct dbg_shot));
    return result;
}

void dbg_end(shot_t history[], colorlist_t *colors, int depth, shot_t results[], int score, int isMasterSearch, void *priv, void *dbg_local) {
    struct dbg_shot *local = (struct dbg_shot *) dbg_local;

    reccursive_print(local, 0);
}

void *dbg_inMin(shot_t history[], colorlist_t *colors, int minMaxDepth, playerPossibleShots_t *results, void *priv, void *dbg_parent, int sibling) {
    struct dbg_shot *parent = ((struct dbg_shot *) dbg_parent) + sibling;
    parent->subs_len = getNumShots(results->d, 0, 0);
    parent->subs = (struct dbg_shot *) zalloc(sizeof(struct dbg_shot) * parent->subs_len);

    return parent->subs;
}

void dbg_outMin(shot_t history[], colorlist_t *colors, int minMaxDepth, playerPossibleShots_t *results, int min, void *priv, void *dbg_parent, void *dbg_local, int sibling) {
    struct dbg_shot *parent = ((struct dbg_shot *) dbg_parent) + sibling;
    int j = 0;
    int i;

    if (min == INT_MAX) {
        reccursive_free(parent->subs);
        parent->subs = NULL;
        parent->subs_len = 0;
    } else {
        for (i = 0; i < parent->subs_len; i++) {
            if (results->d[i].d[0] && results->d[i].d[IDX_SCORE] == min) {
                memcpy(parent->subs + j, parent->subs + i, sizeof(*parent->subs));
                memcpy(&parent->subs[j].s, &results->d[i], sizeof(parent->subs[0].s));
                j++;
            } else {
                reccursive_free(parent->subs[i].subs);
            }
        }
        assert(j);
        parent->subs = realloc(parent->subs, sizeof(*parent->subs) * j);
        parent->subs_len = j;
    }
}

void *dbg_inMax(shot_t history[], colorlist_t *colors, int minMaxDepth, masterPossibleShots_t *results, void *priv, void *dbg_parent, int sibling) {
    struct dbg_shot *parent = ((struct dbg_shot *) dbg_parent) + sibling;
    parent->subs_len = getNumShots(results->d, 0, 0);
    parent->subs = (struct dbg_shot *) zalloc(sizeof(struct dbg_shot) * parent->subs_len);

    return parent->subs;
}

void dbg_outMax(shot_t history[], colorlist_t *colors, int minMaxDepth, masterPossibleShots_t *results, int max, void *priv, void *dbg_parent, void *dbg_local, int sibling) {
    struct dbg_shot *parent = ((struct dbg_shot *) dbg_parent) + sibling;
    int i;

    for (i = 0; i < parent->subs_len; i++) {
        memcpy(&parent->subs[i].s, &results->d[i], sizeof(parent->subs[0].s));
    }
}

debug_t debugPrint = {
    .priv = NULL,
    .start = dbg_start,
    .end = dbg_end,
    .inMin = dbg_inMin,
    .outMin = dbg_outMin,
    .inMax = dbg_inMax,
    .outMax = dbg_outMax,
};

int main(int argc, char **argv) {
    static const int minMaxDepth = 1;
    playerPossibleShots_t results;
    shot_t history[] = {
        // S(A, A, B, C, -1, -1),
        S()
    };

    getBestShot(history, minMaxDepth, results.d, &debugPrint);
    return 0;
}

