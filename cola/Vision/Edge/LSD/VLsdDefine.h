﻿/**
 * @FilePath     : /cola/cola/Vision/Edge/LSD/VLsdDefine.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-15 19:02:22
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-19 15:48:50
 **/
#ifndef NAONAO_VLSDDEFINE_H
#define NAONAO_VLSDDEFINE_H

#include "../../VisionObject.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

/** ln(10) */
#ifndef M_LN10
#    define M_LN10 2.30258509299404568402
#endif /* !M_LN10 */

/** PI */
#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif /* !M_PI */

#ifndef FALSE
#    define FALSE 0
#endif /* !FALSE */

#ifndef TRUE
#    define TRUE 1
#endif /* !TRUE */

/** Label for pixels with undefined gradient. */
#ifndef NOTDEF
#    define NOTDEF -1024.0
#endif

/** 3/2 pi */
#ifndef M_3_2_PI
#    define M_3_2_PI 4.71238898038
#endif

/** 2 pi */
#ifndef M_2__PI
#    define M_2__PI 6.28318530718
#endif

/** Label for pixels not used in yet. */
#ifndef NOTUSED
#    define NOTUSED 0
#endif

/** Label for pixels already used in detection. */
#ifndef USED
#    define USED 1
#endif

/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:     坐标链表，与  点坐标或像素值
* @Version:   1.0.0.1
* @Date:	  2021/5/11 15:15
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:
--------------------------------------------------------------------------------------------------*/
struct coorlist
{
    NInt               x, y;
    struct coorlist* next;
};
struct point
{
    NInt  x, y;
};

/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:    向stderr输出一条信息，然后退出.
* @Version:   1.0.0.1
* @Date:	  2021/5/11 15:15
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:
--------------------------------------------------------------------------------------------------*/
NVoid error(const char* msg);


/// Doubles relative error factor 双精度误差比较因子
#ifndef RELATIVE_ERROR_FACTOR
#    define RELATIVE_ERROR_FACTOR 100.0
#endif

/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:    Compare doubles by relative error.误差比较
* @Version:   1.0.0.1
* @Date:	  2021/5/11 15:11
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:    https://blog.csdn.net/x356982611/article/details/19922453
--------------------------------------------------------------------------------------------------*/
NInt  double_equal(NDouble  a, NDouble  b);

/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:   计算两点距离 point (x1,y1) and point (x2,y2).
* @Version:   1.0.0.1
* @Date:	  2021/5/11 15:14
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:
--------------------------------------------------------------------------------------------------*/
NDouble  dist(NDouble  x1, NDouble  y1, NDouble  x2, NDouble  y2);

/*----------------------- 'list of n-tuple' data type ------------------------*/
/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:    'list of n-tuple' data type
* @Version:   1.0.0.1
* @Date:	  2021/5/11 15:16
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:

The i-th component of the j-th n-tuple of an n-tuple list 'ntl'
is accessed with:

ntl->values[ i + j * ntl->dim ]  第j个数组的第i个元素

The dimension of the n-tuple (n) is: 数组的维度，指的是每个数组的大小

ntl->dim

The number of n-tuples in the list is:  容量

ntl->size

The maximum number of n-tuples that can be stored in the  最大容量
list with the allocated memory at a given time is given by:

ntl->max_size
--------------------------------------------------------------------------------------------------*/
typedef struct ntuple_list_s
{
    NUInt  size;       // 数组的个数
    NUInt  max_size;   // 数组的最大容量，扩容时需要
    NUInt  dim;        // 维度
    NDouble *      values;     // 数组
}* ntuple_list;
/*-------------------------- 释放链表与内部数组  Free memory used in n-tuple 'in'.----------------------------------------*/
NVoid free_ntuple_list(ntuple_list input);
/*------------------------------------------- 新建链表 -------------------------------------------*/
ntuple_list new_ntuple_list(NUInt  dim);
/*------------------------------------------- 扩充链表 -------------------------------------------*/
NVoid enlarge_ntuple_list(ntuple_list n_tuple);
/*------------------------------------------- 增加7个数字到链表中 -------------------------------------------*/
// 此时链表的维度是7维
NVoid add_7tuple(ntuple_list out, NDouble  v1, NDouble  v2, NDouble  v3, NDouble  v4, NDouble  v5, NDouble  v6, NDouble  v7);

/*----------------------------- Image Data Types -----------------------------*/
/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:    图像数据类型
* @Version:   1.0.0.1
* @Date:	  2021/5/11 15:30
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:
char image data type

The pixel value at (x,y) is accessed by:

image->data[ x + y * image->xsize ]

with x and y integer.
--------------------------------------------------------------------------------------------------*/
typedef struct image_char_s
{
    unsigned char* data;
    NUInt    xsize, ysize;   // 宽度，高度
}* image_char;

/*--------------------------------------------释放图像数据 --------------------------------------------*/
NVoid free_image_char(image_char i);

/*--------------------------------------------创造图像数据--------------------------------------------*/
image_char new_image_char(NUInt  xsize, NUInt  ysize);

/*--------------------------------------------初始化图像数据--------------------------------------------*/
// initialized to the value 'fill_value'. 用fill_value的值 初始化图像
image_char new_image_char_ini(NUInt  xsize, NUInt  ysize, unsigned char fill_value);

/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:    NInt  image data type    NInt  数据类型的图像数据
* @Version:   1.0.0.1
* @Date:	  2021/5/11 15:37
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:
The pixel value at (x,y) is accessed by:
image->data[ x + y * image->xsize ]
with x and y integer.
--------------------------------------------------------------------------------------------------*/
typedef struct image_int_s
{
    NInt *         data;
    NUInt  xsize, ysize;
}* image_int;


/*--------------------------------------------创造图像数据--------------------------------------------*/
image_int new_image_int(NUInt  xsize, NUInt  ysize);
/*--------------------------------------------初始化图像数据--------------------------------------------*/
// initialized to the value 'fill_value'. 用fill_value的值 初始化图像
image_int new_image_int_ini(NUInt  xsize, NUInt  ysize, NInt  fill_value);

/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:     NDouble  image data type   double类型图像数据
* @Version:   1.0.0.1
* @Date:	  2021/5/11 15:39
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:
The pixel value at (x,y) is accessed by:
image->data[ x + y * image->xsize ]
with x and y integer.
--------------------------------------------------------------------------------------------------*/
typedef struct image_double_s
{
    NDouble *      data;
    NUInt  xsize, ysize;
}* image_double;

/*--------------------------------------------释放图像数据 --------------------------------------------*/
NVoid free_image_double(image_double i);
/*--------------------------------------------创造图像数据--------------------------------------------*/
image_double new_image_double(NUInt  xsize, NUInt  ysize);
/*--------------------------------------------初始化图像数据--------------------------------------------*/
// initialized to the value 'data'. 用data的值 初始化图像
image_double new_image_double_ptr(NUInt  xsize, NUInt  ysize, NDouble * data);

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif   // NAONAO_VLSDDEFINE_H