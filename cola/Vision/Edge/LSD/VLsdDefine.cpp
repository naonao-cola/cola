﻿/**
 * @FilePath     : /cola/cola/Vision/Edge/LSD/VLsdDefine.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-15 19:15:00
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-15 21:57:53
 **/

#include "VLsdDefine.h"
#include <cstdio>
NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

void error(const char* msg)
{
    fprintf(stderr, "LSD Error: %s\n", msg);
    exit(EXIT_FAILURE);
}

int double_equal(double a, double b)
{
    double abs_diff;
    double aa;
    double bb;
    double abs_max;
    /* trivial case */
    if (a == b) {
        return TRUE;
    }
    abs_diff = fabs(a - b);
    aa       = fabs(a);
    bb       = fabs(b);
    abs_max  = aa > bb ? aa : bb;

    /* DBL_MIN 是最小的浮点数
       DBL_EPSILON ， FLT_EPSILON 主要用于单精度和双精度的比较当中
     */
    if (abs_max < DBL_MIN) {
        abs_max = DBL_MIN;
    }
    /* equal if relative error <= factor x eps */
    return static_cast<int>((abs_diff / abs_max) <= (RELATIVE_ERROR_FACTOR * DBL_EPSILON));
}

double dist(double x1, double y1, double x2, double y2)
{
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void free_ntuple_list(ntuple_list input)
{
    if (input == nullptr || input->values == nullptr) {
        error("free_ntuple_list: invalid n-tuple input.");
    }
    free((void*)input->values);
    free((void*)input);
}

ntuple_list new_ntuple_list(unsigned int dim)
{
    ntuple_list n_tuple = nullptr;
    /* check parameters */
    if (dim == 0) {
        error("new_ntuple_list: 'dim' must be positive.");
    }

    /* get memory for list structure */
    n_tuple = static_cast<ntuple_list>(malloc(sizeof(struct ntuple_list_s)));
    if (n_tuple == nullptr) {
        error("not enough memory.");
    }

    /* initialize list */
    n_tuple->size     = 0;
    n_tuple->max_size = 1;
    n_tuple->dim      = dim;

    /* get memory for tuples */
    n_tuple->values = static_cast<double*>(malloc(dim * n_tuple->max_size * sizeof(double)));
    if (n_tuple->values == nullptr) {
        error("not enough memory.");
    }
    return n_tuple;
}

void enlarge_ntuple_list(ntuple_list n_tuple)
{
    /* check parameters */
    if (n_tuple == nullptr || n_tuple->values == nullptr || n_tuple->max_size == 0) {
        error("enlarge_ntuple_list: invalid n-tuple.");
    }

    /* duplicate number of tuples */
    n_tuple->max_size *= 2;

    /* realloc memory */
    n_tuple->values = (double*)realloc((void*)n_tuple->values, n_tuple->dim * n_tuple->max_size * sizeof(double));
    if (n_tuple->values == nullptr) {
        error("not enough memory.");
    }
}

void add_7tuple(ntuple_list out, double v1, double v2, double v3, double v4, double v5, double v6, double v7)
{
    /* check parameters */
    if (out == nullptr) {
        error("add_7tuple: invalid n-tuple input.");
    }
    if (out->dim != 7) {
        error("add_7tuple: the n-tuple must be a 7-tuple.");
    }

    /* if needed, alloc more tuples to 'out' */
    if (out->size == out->max_size)
        enlarge_ntuple_list(out);
    if (out->values == nullptr) {
        error("add_7tuple: invalid n-tuple input.");
    }

    /* add new 7-tuple */
    out->values[out->size * out->dim + 0] = v1;
    out->values[out->size * out->dim + 1] = v2;
    out->values[out->size * out->dim + 2] = v3;
    out->values[out->size * out->dim + 3] = v4;
    out->values[out->size * out->dim + 4] = v5;
    out->values[out->size * out->dim + 5] = v6;
    out->values[out->size * out->dim + 6] = v7;
    /* update number of tuples counter */
    out->size++;
}

void free_image_char(image_char i)
{
    if (i == nullptr || i->data == nullptr) {
        error("free_image_char: invalid input image.");
    }
    free((void*)i->data);
    free((void*)i);
}

image_char new_image_char(unsigned int xsize, unsigned int ysize)
{
    image_char image = nullptr;
    /* check parameters */
    if (xsize == 0 || ysize == 0) {
        error("new_image_char: invalid image size.");
    }

    /* get memory */
    image = (image_char)malloc(sizeof(struct image_char_s));
    if (image == nullptr) {
        error("not enough memory.");
    }
    image->data = (unsigned char*)calloc((size_t)(xsize * ysize), sizeof(unsigned char));
    if (image->data == nullptr) {
        error("not enough memory.");
    }
    /* set image size */
    image->xsize = xsize;
    image->ysize = ysize;

    return image;
}

image_char new_image_char_ini(unsigned int xsize, unsigned int ysize, unsigned char fill_value)
{
    image_char   image = new_image_char(xsize, ysize);
    unsigned int N     = xsize * ysize;
    unsigned int i;
    /* check parameters */
    if (image == nullptr || image->data == nullptr) {
        error("new_image_char_ini: invalid image.");
    }

    /* initialize */
    for (i = 0; i < N; i++)
        image->data[i] = fill_value;

    return image;
}

image_int new_image_int(unsigned int xsize, unsigned int ysize)
{
    image_int image = nullptr;
    /* check parameters */
    if (xsize == 0 || ysize == 0) {
        error("new_image_int: invalid image size.");
    }
    /* get memory */
    image = (image_int)malloc(sizeof(struct image_int_s));
    if (image == nullptr) {
        error("not enough memory.");
    }
    image->data = (int*)calloc((size_t)(xsize * ysize), sizeof(int));
    if (image->data == nullptr) {
        error("not enough memory.");
    }
    /* set image size */
    image->xsize = xsize;
    image->ysize = ysize;
    return image;
}

image_int new_image_int_ini(unsigned int xsize, unsigned int ysize, int fill_value)
{
    image_int    image = new_image_int(xsize, ysize); /* create image */
    unsigned int N     = xsize * ysize;
    unsigned int i;
    /* initialize */
    for (i = 0; i < N; i++) {
        image->data[i] = fill_value;
    }
    return image;
}

void free_image_double(image_double i)
{
    if (i == nullptr || i->data == nullptr) {
        error("free_image_double: invalid input image.");
    }
    free((void*)i->data);
    free((void*)i);
}

image_double new_image_double(unsigned int xsize, unsigned int ysize)
{
    image_double image = nullptr;
    if (xsize == 0 || ysize == 0) {
        error("new_image_double: invalid image size.");
    }
    image = static_cast<image_double>(malloc(sizeof(struct image_double_s)));
    if (image == nullptr) {
        error("not enough memory.");
    }
    image->data = (double*)calloc((size_t)(xsize * ysize), sizeof(double));
    if (image->data == nullptr) {
        error("not enough memory.");
    }
    image->xsize = xsize;
    image->ysize = ysize;
    return image;
}

image_double new_image_double_ptr(unsigned int xsize, unsigned int ysize, double* data)
{
    image_double image = nullptr;
    if (xsize == 0 || ysize == 0) {
        error("new_image_double_ptr: invalid image size.");
    }
    if (data == nullptr) {
        error("new_image_double_ptr: NULL data pointer.");
    }
    image = static_cast<image_double>(malloc(sizeof(struct image_double_s)));
    if (image == nullptr) {
        error("not enough memory.");
    }
    image->xsize = xsize;
    image->ysize = ysize;
    image->data  = data;
    return image;
}

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END