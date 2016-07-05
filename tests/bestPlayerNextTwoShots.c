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
 * 0012 -- (score: 222)
 *   0012 00 (score: 81)
 *     3333 -- (score: 81)
 *     3334 -- (score: 81)
 *     3343 -- (score: 81)
 *     3344 -- (score: 81)
 *     3345 -- (score: 81)
 *     3433 -- (score: 81)
 *     3434 -- (score: 81)
 *     3435 -- (score: 81)
 *     3443 -- (score: 81)
 *     3444 -- (score: 81)
 *     3445 -- (score: 81)
 *     3453 -- (score: 81)
 *     3454 -- (score: 81)
 *     3455 -- (score: 81)
 *   0012 01 (score: 222)
 *     1131 -- (score: 222)
 *     1133 -- (score: 222)
 *     1134 -- (score: 222)
 *     1331 -- (score: 222)
 *     1333 -- (score: 222)
 *     1334 -- (score: 222)
 *     1341 -- (score: 222)
 *     1343 -- (score: 222)
 *     1344 -- (score: 222)
 *     1345 -- (score: 222)
 *     2223 -- (score: 222)
 *     2233 -- (score: 222)
 *     2234 -- (score: 222)
 *     2323 -- (score: 222)
 *     2324 -- (score: 222)
 *     2333 -- (score: 222)
 *     2334 -- (score: 222)
 *     2343 -- (score: 222)
 *     2344 -- (score: 222)
 *     2345 -- (score: 222)
 *     3131 -- (score: 222)
 * [...]
 *
 */
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

static const int minMaxDepth = 1;
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

void reccursive_print(struct dbg_shot *in, int depth) {
    int i;

    if (!in)
        return;
    for (i = 0; i < depth; i++)
        printf("  ");
    prShot(in->s);
    printf(" (score: %d)\n", in->s.d[IDX_SCORE]);
    for (i = 0; i < in->subs_len; i++)
        reccursive_print(in->subs + i, depth + 1);
}

void *dbg_start(shot_t history[], colorlist_t *colors, int depth, int masterSearch, void *priv) {
    initialDepth = depth;
    isMasterSearch = masterSearch;
    struct dbg_shot *result = (struct dbg_shot *) zalloc(sizeof(struct dbg_shot));
    return result;
}

void dbg_end(shot_t history[], colorlist_t *colors, int depth, shot_t results[], int score, int isMasterSearch, void *priv, void *dbg_local) {
    struct dbg_shot *local = (struct dbg_shot *) dbg_local;
    int i;

    for (i = 0; i < local->subs_len; i++)
        reccursive_print(local->subs + i, 0);
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
    playerPossibleShots_t results;
    shot_t history[] = {
        // S(A, A, B, C, -1, -1),
        S()
    };

    getBestShot(history, minMaxDepth, results.d, &debugPrint);
    return 0;
}

