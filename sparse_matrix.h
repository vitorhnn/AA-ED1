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

#pragma once

#include <stdbool.h>
#include <stddef.h>

struct sparse_mtx_node {
    double value;
    size_t offset;

    struct sparse_mtx_node *prev, *next;
};

typedef struct {
    size_t rows;
    size_t columns;

    struct sparse_mtx_node *head, *tail;
} sparse_matrix;

struct sparse_mtx_node *
sparse_matrix_set_at(
    struct sparse_mtx_node *iterator,
    sparse_matrix *matrix,
    const size_t row,
    const size_t column,
    const double value;
);

struct sparse_mtx_node *
sparse_matrix_get_at(
    struct sparse_mtx_node *iterator,
    sparse_matrix *matrix,
    const size_t row,
    const size_t column,
    double *out
);

sparse_matrix *
sparse_matrix_new(
    size_t rows,
    size_t columns
);

void
sparse_matrix_free(
    sparse_matrix *matrix
);

sparse_matrix *
sparse_matrix_add(
    sparse_matrix * restrict a,
    sparse_matrix * restrict b
);

sparse_matrix *
sparse_matrix_mul(
    sparse_matrix * restrict a,
    sparse_matrix * restrict b
);

