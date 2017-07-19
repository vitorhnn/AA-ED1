/*
 * Copyright (c) 2017 Victor Hermann "vitorhnn" Chiletto
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
**/

#include "sparse_matrix.h"

#include <stdlib.h>
#include <assert.h>

static unsigned long long __g_linked_list_jumps = 0;

static struct sparse_mtx_node *
aux_insert_after(
    sparse_matrix *matrix,
    struct sparse_mtx_node *node,
    size_t row,
    size_t column,
    double value
)
{
    struct sparse_mtx_node *new_node = malloc(sizeof(struct sparse_mtx_node));

    new_node->value = value;
    new_node->offset = (row * matrix->columns) + column;

    new_node->prev = node;
    if (node) {
        new_node->next = node->next;

        if (!node->next) {
            matrix->tail = new_node;
        } else {
            node->next->prev = new_node;
        }

        node->next = new_node;

    } else {
        new_node->next = NULL;

        matrix->head = matrix->tail = new_node;
    }

    return new_node;
}

static struct sparse_mtx_node *
aux_insert_before(
    sparse_matrix *matrix,
    struct sparse_mtx_node *node,
    size_t row,
    size_t column,
    double value
)
{
    struct sparse_mtx_node *new_node = malloc(sizeof(struct sparse_mtx_node));

    new_node->value = value;
    new_node->offset = (row * matrix->columns) + column;

    new_node->next = node;

    if (node) {
        new_node->prev = node->prev;

        if (!node->prev) {
            matrix->head = new_node;
        } else {
            node->prev->next = new_node;
        }

        node->prev = new_node;
    } else {
        new_node->prev = NULL;

        matrix->head = matrix->tail = new_node;
    }

    return new_node;
}

static struct sparse_mtx_node *
aux_create_iterator(
    sparse_matrix *matrix,
    size_t bias
)
{
    size_t mtx_sz = matrix->rows * matrix->columns;

    return bias > mtx_sz / 2 ? matrix->tail : matrix->head;
}

static struct sparse_mtx_node *
reverse_iterate(
    struct sparse_mtx_node *iterator,
    size_t target,
    bool *match
)
{
    while (iterator) {
        if (iterator->offset == target) {
            *match = true;

            return iterator;
        }

        if (iterator->offset < target || !iterator->prev) {
            *match = false;

            return iterator;
        }

        iterator = iterator->prev;
        __g_linked_list_jumps++;
    }

    *match = false;
    return NULL;
}

static struct sparse_mtx_node *
forwards_iterate(
    struct sparse_mtx_node *iterator,
    size_t target,
    bool *match
)
{
    while (iterator) {
        if (iterator->offset == target) {
            *match = true;

            return iterator;
        }

        if (iterator->offset > target || !iterator->next) {
            *match = false;

            return iterator;
        }

        iterator = iterator->next;
        __g_linked_list_jumps++;
    }

    *match = false;
    return NULL;
}

struct sparse_mtx_node *
sparse_matrix_set_at(
    struct sparse_mtx_node *iterator,
    sparse_matrix *matrix,
    const size_t row,
    const size_t column,
    const double value
)
{
    // discard null values
    if (value == 0.0) {
        return iterator;
    }

    size_t element_pos = (row * matrix->columns) + column;

    if (!iterator) {
        iterator = aux_create_iterator(matrix, element_pos);
    }

    if (!iterator) {
        // empty matrix.
        return aux_insert_after(matrix, NULL, row, column, value);
    }

    size_t iterator_pos = iterator->offset;

    if (element_pos < iterator_pos) {
        bool match;
        iterator = reverse_iterate(iterator, element_pos, &match);

        if (match) {
            iterator->value = value;

            return iterator;
        } else {
            if (element_pos < iterator->offset) {
                return aux_insert_before(matrix, iterator, row, column, value);
            } else {
                return aux_insert_after(matrix, iterator, row, column, value);
            }
        }

    } else if (element_pos > iterator_pos) {
        bool match;
        iterator = forwards_iterate(iterator, element_pos, &match);

        if (match) {
            iterator->value = value;

            return iterator;
        } else {
            if (element_pos < iterator->offset) {
                return aux_insert_before(matrix, iterator, row, column, value);
            } else {
                return aux_insert_after(matrix, iterator, row, column, value);
            }
        }
    } else {
        iterator->value = value;

        return iterator;
    }
}

struct sparse_mtx_node *
sparse_matrix_get_at(
    struct sparse_mtx_node *iterator,
    sparse_matrix *matrix,
    const size_t row,
    const size_t column,
    double *out
)
{
    size_t element_pos = (row * matrix->columns) + column;

    if (!iterator) {
        iterator = aux_create_iterator(matrix, element_pos);
    }

    size_t iterator_pos = iterator ? iterator->offset : 0;

    if (element_pos < iterator_pos) {
        bool match;
        iterator = reverse_iterate(iterator, element_pos, &match);

        if (match) {
            *out = iterator->value;

            return iterator;
        } else {
            *out = 0.0;
            return iterator;
        }

    } else if (element_pos > iterator_pos) {
        bool match;
        iterator = forwards_iterate(iterator, element_pos, &match);

        if (match) {
            *out = iterator->value;

            return iterator;
        } else {
            *out = 0.0;
            return iterator;
        }
    } else {
        *out = iterator->value;

        return iterator;
    }
}

sparse_matrix *
sparse_matrix_new(
    size_t rows,
    size_t columns
)
{
    sparse_matrix *mtx = malloc(sizeof(sparse_matrix));

    mtx->rows = rows;
    mtx->columns = columns;

    mtx->head = mtx->tail = NULL;

    return mtx;
}

void
smarse_matrix_free(
    sparse_matrix *matrix
)
{
    struct sparse_mtx_node *node = matrix->head;

    while (node) {
        struct sparse_mtx_node *next = node->next;
        free(node);
        node = next;
    }

    free(matrix);
}

sparse_matrix *
sparse_matrix_add(
    sparse_matrix * restrict a,
    sparse_matrix * restrict b
)
{
    assert(a->rows == b->rows);
    assert(a->columns == b->columns);

    struct sparse_mtx_node *ait = NULL,
                           *bit = NULL;

    for (size_t i = 0; i < a->rows; ++i) {
        for (size_t j = 0; j < a->columns; ++j) {
            double vA, vB;
            ait = sparse_matrix_get_at(ait, a, i, j, &vA);
            bit = sparse_matrix_get_at(bit, b, i, j, &vB);
            ait = sparse_matrix_set_at(ait, a, i, j, vA + vB);
        }
    }

    return a;
}

sparse_matrix *
sparse_matrix_mul(
    sparse_matrix * restrict a,
    sparse_matrix * restrict b
)
{
    assert(a->columns == b->rows);

    sparse_matrix *result = sparse_matrix_new(a->rows, b->columns);

    struct sparse_mtx_node *ait = NULL,
                           *bit = NULL,
                           *rit = NULL;

    for (size_t i = 0; i < a->rows; ++i) {
        for (size_t j = 0; j < b->columns; ++j) {
            double sum = 0;

            for (size_t k = 0; k < a->columns; ++k) {
                double vA, vB;

                ait = sparse_matrix_get_at(ait, a, i, k, &vA);
                bit = sparse_matrix_get_at(bit, b, k, j, &vB);

                sum += vA * vB;
            }

            rit = sparse_matrix_set_at(rit, result, i, j, sum);
        }
    }

    return result;
}
