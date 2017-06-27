/*
 * Copyright (c) 2016 Victor Hermann "vitorhnn" Chiletto
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
    new_node->next = node->next;

    if (!node->next) {
        matrix->tail = new_node;
    } else {
        node->next->prev = new_node;
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
    new_node->prev = node->prev;

    if (!node->prev) {
        matrix->head = new_node;
    } else {
        node->prev->next = new_node;
    }

    node->prev = new_node;
    
    return new_node;
} 

struct sparse_mtx_node *
sparse_matrix_set_at(
    struct sparse_mtx_node *iterator,
    sparse_matrix *matrix,
    size_t row,
    size_t column,
    double value
)
{
    size_t element_pos = (row * matrix->columns) + column;
    // if we got no iterator, this *might* be awful
    if (!iterator) {
        size_t mtx_sz = matrix->rows * matrix->columns;

        // start at the head or tail of the matrix depending on the distance from the element.
        iterator = element_pos > mtx_sz / 2 ? matrix->head : matrix->tail;
    }

    size_t iterator_pos = iterator->offset;

    if (element_pos < iterator_pos) {
        while (true) {
            if (iterator_pos == element_pos) {
                iterator->value = value;

                return iterator;
            }

            if (iterator_pos < element_pos) {
                return aux_insert_after(matrix, iterator, row, column, value);
            }

            iterator = iterator->prev;
        }
    } else if (element_pos > iterator_pos) {
        while (true) {
            if (iterator_pos == element_pos) {
                iterator->value = value;

                return iterator;
            }

            if (iterator_pos > element_pos) {
                return aux_insert_before(matrix, iterator, row, column, value);
            }

            iterator = iterator->next;
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
    size_t row,
    size_t column,
    double *out
);
{
    size_t element_pos = (row * matrix->columns) + column
}
