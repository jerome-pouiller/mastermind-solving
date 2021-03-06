/* vim: set sw=4 expandtab: */
/*
 * Licence: GPL
 * Created: 2016-04-05 19:38:46+02:00
 * Main authors:
 *     - Jérôme Pouiller <jezz@sysmic.org>
 */
#include "mastermind.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

//#define DEBUG

static int getHistoryLen(shot_t shots[]) {
    int i;

    for (i = 0; shots[i].d[0]; i++)
        ;
    return i;
}

void prShot(const shot_t shot) {
    int i;

    for (i = 0; i < NB_PLACES; i++)
        printf("%c", shot.d[i]);
    printf(" ");
    for (; i < NB_PLACES + NB_HINTS; i++)
        if (shot.d[i] == -1)
            printf("-");
        else
            printf("%d", shot.d[i]);

#ifdef DEBUG
    printf(" sym: %3d", shot.d[IDX_NUM_SYM]);
    printf(" scor: %3d", shot.d[IDX_SCORE]);
#endif
}

int prShots(shot_t shots[], char op, int score) {
    int i;
    int num = 0;
    int len = getHistoryLen(shots);
    shot_t tmp[len + 1];

    filterShots(tmp, shots, op, score);
    for (i = 0; tmp[i].d[0]; i++) {
        num++;
        printf("  ");
        prShot(tmp[i]);
        printf("\n");
    }
    return num;
}

int getNumShots(shot_t shots[], char op, int score) {
    int i;
    int num = 0;
    int len = getHistoryLen(shots);
    shot_t tmp[len + 1];

    filterShots(tmp, shots, op, score);
    for (i = 0; tmp[i].d[0]; i++)
        num++;

    return num;
}

int getNumRealShots(shot_t shots[], char op, int score) {
    int i;
    int num = 0;
    int len = getHistoryLen(shots);
    shot_t tmp[len + 1];

    filterShots(tmp, shots, op, score);
    for (i = 0; tmp[i].d[0]; i++) {
        assert(tmp[i].d[IDX_NUM_SYM]); // Uninitialized value
        num += tmp[i].d[IDX_NUM_SYM];
    }
    return num;
}

// Given number of unknown colors and last joker used, compute symetries of a
// shot.
static int _computeSymetries(shot_t *shot, char lst_joker, int nb_unknown) {
    int i;
    int cnt = 1;
    for (i = 0; i < NB_PLACES; i++) {
        if (shot->d[i] > lst_joker && shot->d[i] < JOKER_OFFSET + NB_COLORS) {
            lst_joker = shot->d[i];
            cnt *= nb_unknown;
            nb_unknown--;
        }
    }
    assert(cnt > 0);
    return cnt;
}

int computeSymetries(shot_t shots[], colorlist_t *colors) {
    int total = 0;
    int i;
    int nb_unknown;
    char lst_joker = JOKER_OFFSET - 1;

    for (i = 0; colors->d[i]; i++)
        if (colors->d[i] > lst_joker && colors->d[i] < JOKER_OFFSET + NB_COLORS)
            lst_joker = colors->d[i];
    nb_unknown = NB_COLORS - i;
    for (i = 0; shots[i].d[0]; i++) {
        shots[i].d[IDX_NUM_SYM] = _computeSymetries(shots + i, lst_joker, nb_unknown);
        total += shots[i].d[IDX_NUM_SYM];
    }
    return total;
}

int getUsedColors(shot_t history[], colorlist_t *colors) {
    int i, j, k;
    int nb = 0;

    memset(colors, 0, sizeof(*colors));
    for (i = 0; history[i].d[0]; i++) {
        for (j = 0; j < NB_PLACES; j++) {
            for (k = 0; k < nb && colors->d[k] != history[i].d[j]; k++)
                ;
            if (k == nb)
                colors->d[nb++] = history[i].d[j];
        }
    }
    return nb;
}

int filterShots(shot_t out[], shot_t in[], char op, int score) {
    int i, j = 0;

    // Note: memmove is a no-op if out + j == in + i
    if (op == '=') {
        for (i = 0; in[i].d[0]; i++)
            if (in[i].d[IDX_SCORE] != score) {
                memmove(out + j, in + i, sizeof(shot_t));
                j++;
            }
    } else if (op == '>') {
        for (i = 0; in[i].d[0]; i++)
            if (in[i].d[IDX_SCORE] <= score) {
                memmove(out + j, in + i, sizeof(shot_t));
                j++;
            }
    } else if (op == '<') {
        for (i = 0; in[i].d[0]; i++)
            if (in[i].d[IDX_SCORE] >= score) {
                memmove(out + j, in + i, sizeof(shot_t));
                j++;
            }
    } else if (op == 0 || op == ' ') { // No-op
        if (in != out) {
            for (i = 0; in[i].d[0]; i++) {
                memmove(out + j, in + i, sizeof(shot_t));
                j++;
            }
        }
    } else {
        assert(0); // Bad operator
    }
    out[j].d[0] = 0;
    return j;
}

int checkOne(const shot_t *hint, const shot_t *prop) {
    int i, j;
    int count;
    int used[NB_PLACES] = { };

    count = 0;
    for (i = 0; i < NB_PLACES; i++)
        if (prop->d[i] == hint->d[i]) {
            used[i] = 3;
            count++;
        }
    if (count != hint->d[IDX_HINT_PLACE])
        return INT_MAX;
    count = 0;
    for (i = 0; i < NB_PLACES; i++)
        for (j = 0; j < NB_PLACES; j++)
            if (!(used[i] & 1) && !(used[j] & 2) && prop->d[i] == hint->d[j] && j != i) {
                used[i] |= 1;
                used[j] |= 2;
                count++;
            }
    if (count != hint->d[IDX_HINT_COLOR])
        return INT_MAX;

    return 0;
}

int check(shot_t hints[], const shot_t *prop) {
    int i;
    for (i = 0; hints[i].d[0]; i++)
        if (checkOne(hints + i, prop) == INT_MAX)
            return INT_MAX;
    return 0;
}

static int _getPossiblePlayerShots(colorlist_t *colors, shot_t *pattern, shot_t results[]) {
    int joker = JOKER_OFFSET;
    int pattern_len, colors_len;
    int total = 0;
    int i;

    for (i = 0; pattern->d[i] && i < NB_PLACES; i++)
        ;
    pattern_len = i;

    for (i = 0; colors->d[i]; i++) {
        if (colors->d[i] >= joker && colors->d[i] < JOKER_OFFSET + NB_COLORS)
            joker = colors->d[i] + 1;
    }
    colors_len = i;
    colors->d[colors_len] = 0;

    for (i = 0; i < colors_len + 1; i++) {
        if (i < NB_COLORS) {
            if (i == colors_len)
                colors->d[colors_len] = joker;
            pattern->d[pattern_len] = colors->d[i];
            if (pattern_len < NB_PLACES - 1) {
                total += _getPossiblePlayerShots(colors, pattern, results + total);
            } else {
                memcpy(results + total, pattern, sizeof(shot_t));
                total++;
            }
            if (i == colors_len)
                colors->d[colors_len] = 0;
        }
    }
    pattern->d[pattern_len] = 0;
    if (!pattern_len)
        results[total].d[0] = 0;
    return total;
}

int getPossiblePlayerShots(colorlist_t *colors, playerPossibleShots_t *results) {
    shot_t pattern = S();
    pattern.d[IDX_HINT_PLACE] = -1;
    pattern.d[IDX_HINT_COLOR] = -1;
    return _getPossiblePlayerShots(colors, &pattern, results->d);
}

int getPossibleMasterShots(shot_t *currentShot, masterPossibleShots_t *results) {
    int i = 0;
    int placed, colors;

    assert(currentShot->d[IDX_HINT_PLACE] == -1); // Should not be initialized
    assert(currentShot->d[IDX_HINT_COLOR] == -1); // Should not be initialized
    for (placed = 0; placed <= NB_PLACES; placed++) {
        for (colors = 0; colors + placed <= NB_PLACES; colors++) {
            if (! (placed == NB_PLACES - 1 && colors == 1)) {
                memcpy(results->d + i, currentShot, sizeof(*currentShot));
                results->d[i].d[IDX_HINT_PLACE] = placed;
                results->d[i].d[IDX_HINT_COLOR] = colors;
                //results->d[i].d[IDX_NUM_SYM] = pattern[IDX_NUM_SYM];
                //results->d[i].d[IDX_SCORE] = -1;
                i++;
            }
        }
    }
    results->d[i].d[0] = 0;
    return i;
}

static int getMax(shot_t history[], colorlist_t *colors, int minMaxDepth, masterPossibleShots_t *results, const debug_t *dbg, void *dbg_local, int sibling, int best);
static int getMin(shot_t history[], colorlist_t *colors, int minMaxDepth, playerPossibleShots_t *results, const debug_t *dbg, void *dbg_local, int sibling, int best);

static int getMin(shot_t history[], colorlist_t *colors, int minMaxDepth, playerPossibleShots_t *results, const debug_t *dbg, void *dbg_parent, int sibling, int best) {
    int i, j, k;
    int num_poss;
    int history_len;
    int min;
    void *dbg_local = NULL;

    history_len = getHistoryLen(history);
    getPossiblePlayerShots(colors, results);
    for (i = 0; results->d[i].d[0]; i++)
        results->d[i].d[IDX_SCORE] = check(history, results->d + i);
    computeSymetries(results->d, colors);
    num_poss = getNumRealShots(results->d, '>', INT_MAX - 1);
    assert(!history_len || history[history_len - 1].d[IDX_SCORE] <= 0 || num_poss <= history[history_len - 1].d[IDX_SCORE]);

    if (dbg && dbg->inMin)
        dbg_local = dbg->inMin(history, colors, minMaxDepth, results, dbg->priv, dbg_parent, sibling);
    if (num_poss == 0) { // Master provide us an impossible game
        min = INT_MAX;
    } else if (num_poss == 1) { // We found solution
        for (i = 0; results->d[i].d[0]; i++)
            if (results->d[i].d[IDX_SCORE])
                results->d[i].d[IDX_SCORE] = -minMaxDepth - 1;
            else
                results->d[i].d[IDX_SCORE] = -minMaxDepth - 2;
        min = -minMaxDepth - 2;
    } else if (history_len && num_poss == history[history_len - 1].d[IDX_SCORE]) {
#ifdef DEBUG
        // Should be same than parent score but it is difficult to do. However,
        // we know this information will never been used in normal cases. Only
        // dbg->outMin or hacker may use it.
        for (i = 0; results->d[i].d[0]; i++)
            results->d[i].d[IDX_SCORE] = num_poss;
#endif
        min = num_poss;
    } else if (!minMaxDepth) { // Do not go deeper
        for (i = 0; results->d[i].d[0]; i++)
            results->d[i].d[IDX_SCORE] = num_poss;
        min = num_poss;
    } else if (best < 0 && best <= -minMaxDepth - 2) { // Early prune
#ifdef DEBUG
        // Should be same than parent score but it is difficult to do. However,
        // we know this information will never been used in normal cases. Only
        // dbg->outMin or hacker may use it.
        for (i = 0; results->d[i].d[0]; i++)
            results->d[i].d[IDX_SCORE] = num_poss;
#endif
        min = num_poss;
    } else {
        min = INT_MAX;
        history[history_len + 1].d[0] = 0;
        for (i = 0; results->d[i].d[0]; i++) {
            if (results->d[i].d[IDX_SCORE] != INT_MAX) {
                colorlist_t colors_local;
                masterPossibleShots_t local = { };
                results->d[i].d[IDX_SCORE] = num_poss; // This is a temporary score
                memcpy(history + history_len, results->d + i, sizeof(shot_t));
                memcpy(&colors_local, colors, sizeof(colors_local));
                for (j = 0; results->d[i].d[j] && j < NB_PLACES; j++) {
                    for (k = 0; colors_local.d[k] && results->d[i].d[j] != colors_local.d[k]; k++)
                        ;
                    if (!colors_local.d[k]) {
                        colors_local.d[k] = results->d[i].d[j];
                        colors_local.d[k + 1] = 0;
                    }
                }
                results->d[i].d[IDX_SCORE] = getMax(history, &colors_local, minMaxDepth - 1, &local, dbg, dbg_local, i, min);
                assert(results->d[i].d[IDX_SCORE] != INT_MIN);
                assert(results->d[i].d[IDX_SCORE] != 0);
                assert(results->d[i].d[IDX_SCORE] != 1);
                assert(results->d[i].d[IDX_SCORE] != INT_MAX);
                if (results->d[i].d[IDX_SCORE] < min)
                    min = results->d[i].d[IDX_SCORE];
            }
        }
        for (i = 0; results->d[i].d[0]; i++) {
            if (results->d[i].d[IDX_SCORE] == INT_MAX) {
                colorlist_t colors_local;
                masterPossibleShots_t local = { };
                results->d[i].d[IDX_SCORE] = num_poss; // This is a temporary score
                memcpy(history + history_len, results->d + i, sizeof(shot_t));
                memcpy(&colors_local, colors, sizeof(colors_local));
                for (j = 0; results->d[i].d[j] && j < NB_PLACES; j++) {
                    for (k = 0; colors_local.d[k] && results->d[i].d[j] != colors_local.d[k]; k++)
                        ;
                    if (!colors_local.d[k]) {
                        colors_local.d[k] = results->d[i].d[j];
                        colors_local.d[k + 1] = 0;
                    }
                }
                results->d[i].d[IDX_SCORE] = getMax(history, &colors_local, minMaxDepth - 1, &local, dbg, dbg_local, i, min);
                assert(results->d[i].d[IDX_SCORE] != INT_MIN);
                assert(results->d[i].d[IDX_SCORE] != 0);
                assert(results->d[i].d[IDX_SCORE] != 1);
                assert(results->d[i].d[IDX_SCORE] != INT_MAX);
                if (results->d[i].d[IDX_SCORE] < min)
                    min = results->d[i].d[IDX_SCORE];
            }
        }
        history[history_len].d[0] = 0;
    }
    if (dbg && dbg->outMin)
        dbg->outMin(history, colors, minMaxDepth, results, min, dbg->priv, dbg_parent, dbg_local, sibling);
    return min;
}

static int getMax(shot_t history[], colorlist_t *colors, int minMaxDepth, masterPossibleShots_t *results, const debug_t *dbg, void *dbg_parent, int sibling, int best) {
    int max = INT_MIN;
    int history_len;
    int i;
    void *dbg_local = NULL;

    history_len = getHistoryLen(history) - 1;
    assert(history_len >= 0); // It make no mean for master to play first.

    // Note: it is difficult to filter impossible shots here nor compute score
    // here. It is easier to ask to getMin to do the job. Consequently, we
    // can't just return results in getMax().
    getPossibleMasterShots(history + history_len, results);

    history[history_len + 1].d[0] = 0;
    if (dbg && dbg->inMax)
        dbg_local = dbg->inMax(history, colors, minMaxDepth, results, dbg->priv, dbg_parent, sibling);
    for (i = 0; results->d[i].d[0]; i++) {
        playerPossibleShots_t local = { };
        memcpy(history + history_len, results->d + i, sizeof(shot_t));
        results->d[i].d[IDX_SCORE] = getMin(history, colors, minMaxDepth, &local, dbg, dbg_local, i, best);
        assert(results->d[i].d[IDX_SCORE] != 0);
        assert(results->d[i].d[IDX_SCORE] != 1);
        if (results->d[i].d[IDX_SCORE] > max && results->d[i].d[IDX_SCORE] < INT_MAX)
            max = results->d[i].d[IDX_SCORE];
    }
    if (dbg && dbg->outMax)
        dbg->outMax(history, colors, minMaxDepth, results, max, dbg->priv, dbg_parent, dbg_local, sibling);
    assert(max != INT_MIN);
    return max;
}

int getBestShot(shot_t history[], int minMaxDepth, shot_t results[], const debug_t *dbg) {
    colorlist_t colors;
    int ret;
    int i;
    int history_len;
    void *dbg_local = NULL;

    getUsedColors(history, &colors);
    for (i = 0; history[i].d[0]; i++) {
        history[i].d[IDX_NUM_SYM] = 1;
        history[i].d[IDX_SCORE] = -1;
    }
    history_len = getHistoryLen(history);
    if (history_len == 0 || history[history_len - 1].d[IDX_HINT_PLACE] != -1 || history[history_len - 1].d[IDX_HINT_COLOR] != -1) {
        playerPossibleShots_t tmp = S();
        if (dbg && dbg->start)
            dbg_local = dbg->start(history, &colors, minMaxDepth, 0, dbg->priv);
        ret = getMin(history, &colors, minMaxDepth, &tmp, dbg, dbg_local, 0, INT_MAX);
        for (i = 0; tmp.d[i].d[0]; i++)
            memcpy(results + i, tmp.d + i, sizeof(shot_t));
        results[i].d[0] = 0;
        if (dbg && dbg->end)
            dbg->end(history, &colors, minMaxDepth, results, ret, 0, dbg->priv, dbg_local);
        return ret;
    } else {
        masterPossibleShots_t tmp = S();
        if (dbg && dbg->start)
            dbg_local = dbg->start(history, &colors, minMaxDepth, 1, dbg->priv);
        ret = getMax(history, &colors, minMaxDepth, &tmp, dbg, dbg_local, 0, INT_MAX);
        for (i = 0; tmp.d[i].d[0]; i++)
            memcpy(results + i, tmp.d + i, sizeof(shot_t));
        results[i].d[0] = 0;
        if (dbg && dbg->end)
            dbg->end(history, &colors, minMaxDepth, results, ret, 1, dbg->priv, dbg_local);
        return ret;
    }
}

int getPossibleGameShots(shot_t history[], shot_t results[]) {
    getBestShot(history, 0, results, NULL);
    return getHistoryLen(results);
}


