﻿/**
 * @FilePath     : /cola/cola/Vision/Edge/LSD/VLsd.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-15 18:46:19
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-19 15:47:40
 **/
#include "VLsd.h"
#include "VLsdDefine.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

/*----------------------------------------------------------------------------*/
/*----------------------------- Gaussian filter ------------------------------*/
/*----------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:   	计算高斯核， 高斯函数的方差为sigma ，均值为mean，
* @Version:   1.0.0.1
* @Date:	  2021/5/11 15:42
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:    标准正太分布
--------------------------------------------------------------------------------------------------*/
static NVoid gaussian_kernel(ntuple_list kernel, NDouble sigma, NDouble mean)
{
    NDouble sum = 0.0;
    NDouble val;
    NUInt   i;
    if (kernel == NULL || kernel->values == NULL)
        error("gaussian_kernel: invalid n-tuple 'kernel'.");
    if (sigma <= 0.0)
        error("gaussian_kernel: 'sigma' must be positive.");
    if (kernel->max_size < 1)
        enlarge_ntuple_list(kernel);
    kernel->size = 1;
    for (i = 0; i < kernel->dim; i++) {
        val               = ((NDouble)i - mean) / sigma;
        kernel->values[i] = exp(-0.5 * val * val);
        sum += kernel->values[i];
    }
    if (sum >= 0.0)
        for (i = 0; i < kernel->dim; i++)
            kernel->values[i] /= sum;   // 归一化
}

/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:     通过高斯卷积核来缩放图像，
* @Version:   1.0.0.1
* @Date:	  2021/5/11 16:10
* @InputParameter: intput,输入图像。scale,比例因子
* @OutputParameter:
* @Returns:
* @Others:
--------------------------------------------------------------------------------------------------*/
static image_double gaussian_sampler(image_double input, NDouble scale, NDouble sigma_scale)
{
    image_double aux, out;
    ntuple_list  kernel;
    NUInt        N, M, h, n, x, y, i;
    NInt         xc, yc, j, double_x_size, double_y_size;
    NDouble      sigma, xx, yy, sum, prec;
    if (input == NULL || input->data == NULL || input->xsize == 0 || input->ysize == 0) {
        error("gaussian_sampler: invalid image.");   // 检查参数
    }
    if (scale <= 0.0) {
        error("gaussian_sampler: 'scale' must be positive.");
    }
    if (sigma_scale <= 0.0) {
        error("gaussian_sampler: 'sigma_scale' must be positive.");
    }
    if (input->xsize * scale > (NDouble)UINT_MAX || input->ysize * scale > (NDouble)UINT_MAX) {
        error("gaussian_sampler: the output image size exceeds the handled size.");
    }

    N     = (NUInt)ceil(input->xsize * scale);
    M     = (NUInt)ceil(input->ysize * scale);
    aux   = new_image_double(N, input->ysize);                 // 第一次采样x轴方向
    out   = new_image_double(N, M);                            // 计算新图像大小，并得到内存
    sigma = scale < 1.0 ? sigma_scale / scale : sigma_scale;   // 高斯核的sigma大小
    /*选择内核的大小是为了保证第一个丢弃的项至少比中心值小10^prec倍。为此h 应该大于x,
    其中 e^(-x^2/2sigma^2) = 1/10^prec.
    所以
    x = sigma * sqrt( 2 * prec * ln(10) ). 根据对数公式计算得到
    */
    prec          = 3.0;
    h             = (NUInt)ceil(sigma * sqrt(2.0 * prec * log(10.0)));   // h计算出来为3
    n             = 1 + 2 * h;
    kernel        = new_ntuple_list(n);         // 核的大小为7
    double_x_size = (NInt)(2 * input->xsize);   // 辅助图像大小变量
    double_y_size = (NInt)(2 * input->ysize);

    for (x = 0; x < aux->xsize; x++) {   // 第一次采样x轴方向，x是新图像中的坐标。新图aux高度与原图一致，宽度缩放scale
        xx = (NDouble)x / scale;         // xx是原始尺寸图像中的相应x值。
        xc = (NInt)floor(xx + 0.5);      // xc是整数值，即xx的像素坐标。坐标（0.0,0.0）在像素（0,0）的中心，因此xc = 0的像素的xx值从 - 0.5到0.5
        gaussian_kernel(kernel, sigma, (NDouble)h + xx - (NDouble)xc);   // 必须为每个x计算内核，因为精细偏移xx - xc在每种情况下都不同
        for (y = 0; y < aux->ysize; y++) {
            sum = 0.0;
            for (i = 0; i < kernel->dim; i++) {
                j = xc - h + i;
                while (j < 0)
                    j += double_x_size;   // 对称边界条件
                while (j >= double_x_size)
                    j -= double_x_size;
                if (j >= (NInt)input->xsize)
                    j = double_x_size - 1 - j;
                sum += input->data[j + y * input->xsize] * kernel->values[i];
            }
            aux->data[x + y * aux->xsize] = sum;
        }
    }
    for (y = 0; y < out->ysize; y++) {   // y轴降采样，y是新图的y坐标
        yy = (NDouble)y / scale;         // yy是原始图像中对应的y值
        yc = (NInt)floor(yy + 0.5);      // yc是整数值，与xc同理
        gaussian_kernel(kernel, sigma, (NDouble)h + yy - (NDouble)yc);
        for (x = 0; x < out->xsize; x++) {
            sum = 0.0;
            for (i = 0; i < kernel->dim; i++) {
                j = yc - h + i;
                while (j < 0)
                    j += double_y_size;
                while (j >= double_y_size)
                    j -= double_y_size;
                if (j >= (NInt)input->ysize)
                    j = double_y_size - 1 - j;

                sum += aux->data[x + j * aux->xsize] * kernel->values[i];
            }
            out->data[x + y * out->xsize] = sum;
        }
    }
    free_ntuple_list(kernel);
    free_image_double(aux);
    return out;
}

/*--------------------------------- Gradient  梯度---------------------------------*/
/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:    在每个像素点处，计算基线方向，查找图像的角度和梯度。生成伪有序点的列表。降序排列
* @Version:   1.0.0.1
* @Date:	  2021/5/11 16:20
* @InputParameter:
* @OutputParameter: 'modgrad'：每个像素点的梯度大小。'list_p'：梯度按照降序排列的列表， 'mem_p'：列表所使用的指针，不用时请删除
* @Returns:    带有每个像素点梯度角度的图像，未定义则为-1024
* @Others:
/*
使用2x2像素窗口的范数2计算：
A B
C D
and
com1 = D-A,  com2 = B-C.
Then
gx = B+D - (A+C)   垂直方向的差异
gy = C+D - (A+B)   水平方向的差异
com1 and com2 are just to avoid 2 additions.  只是为了避免加法算数
--------------------------------------------------------------------------------------------------*/
static image_double ll_angle(image_double input, NDouble threshold, struct coorlist** list_p, NVoid** mem_p, image_double* modgrad, NUInt n_bins)
{
    image_double      g;
    NUInt             n, p, x, y, adr, i;
    NDouble           com1, com2, gx, gy, norm, norm2;
    NInt              list_count = 0;   // 其余变量用于伪排序梯度幅度值
    struct coorlist*  list;
    struct coorlist** range_l_s;   // 指向bin列表开始的指针数组
    struct coorlist** range_l_e;   // 指向bin列表开始的指针数组
    struct coorlist*  start;
    struct coorlist*  end;
    NDouble           max_grad = 0.0;
    if (input == NULL || input->data == NULL || input->xsize == 0 || input->ysize == 0)
        error("ll_angle: invalid image.");
    if (threshold < 0.0)
        error("ll_angle: 'threshold' must be positive.");
    if (list_p == NULL)
        error("ll_angle: NULL pointer 'list_p'.");
    if (mem_p == NULL)
        error("ll_angle: NULL pointer 'mem_p'.");
    if (modgrad == NULL)
        error("ll_angle: NULL pointer 'modgrad'.");
    if (n_bins == 0)
        error("ll_angle: 'n_bins' must be positive.");
    n        = input->ysize;   // 图像尺寸
    p        = input->xsize;
    g        = new_image_double(input->xsize, input->ysize);   // 申请输出图像
    *modgrad = new_image_double(input->xsize, input->ysize);   // 获取内存以存储梯度范数的图像

    list      = (struct coorlist*)calloc((size_t)(n * p), sizeof(struct coorlist));   // 获取“有序”像素列表的内存
    *mem_p    = (NVoid*)list;
    range_l_s = (struct coorlist**)calloc((size_t)n_bins, sizeof(struct coorlist*));
    range_l_e = (struct coorlist**)calloc((size_t)n_bins, sizeof(struct coorlist*));
    if (list == NULL || range_l_s == NULL || range_l_e == NULL)
        error("not enough memory.");
    for (i = 0; i < n_bins; i++)
        range_l_s[i] = range_l_e[i] = NULL;
    for (x = 0; x < p; x++)
        g->data[(n - 1) * p + x] = NOTDEF;   // 在下边界 右边界上“未定义”
    for (y = 0; y < n; y++)
        g->data[p * y + p - 1] = NOTDEF;

    for (x = 0; x < p - 1; x++) {   // 计算剩余像素的梯度
        for (y = 0; y < n - 1; y++) {
            adr                   = y * p + x;
            com1                  = input->data[adr + p + 1] - input->data[adr];
            com2                  = input->data[adr + 1] - input->data[adr + p];
            gx                    = com1 + com2;   // 梯度x分量
            gy                    = com1 - com2;   // 梯度y分量
            norm2                 = gx * gx + gy * gy;
            norm                  = sqrt(norm2 / 4.0);   // 梯度范数
            (*modgrad)->data[adr] = norm;                // 存储梯度

            if (norm <= threshold)       // 范数太小，设置为未定义 */
                g->data[adr] = NOTDEF;   // 梯度角度未定义
            else {
                g->data[adr] = atan2(gx, -gy);   // 梯度角度计算
                if (norm > max_grad)
                    max_grad = norm;   // 寻找梯度范数最大值
            }
        }
    }
    for (x = 0; x < p - 1; x++)   // 计算梯度值的直方图
        for (y = 0; y < n - 1; y++) {
            norm = (*modgrad)->data[y * p + x];
            i    = (NUInt)(norm * (NDouble)n_bins / max_grad);   // 根据范数，在正确的bins中存储点
            if (i >= n_bins)
                i = n_bins - 1;
            if (range_l_e[i] == NULL)
                range_l_s[i] = range_l_e[i] = list + list_count++;
            else {
                range_l_e[i]->next = list + list_count;
                range_l_e[i]       = list + list_count++;
            }
            range_l_e[i]->x    = (NInt)x;
            range_l_e[i]->y    = (NInt)y;
            range_l_e[i]->next = NULL;
        }
    for (i = n_bins - 1; i > 0 && range_l_s[i] == NULL; i--)
        ;   // 降序排列
    start = range_l_s[i];
    end   = range_l_e[i];
    if (start != NULL)
        while (i > 0) {
            --i;
            if (range_l_s[i] != NULL) {
                end->next = range_l_s[i];
                end       = range_l_e[i];
            }
        }
    *list_p = start;
    free((NVoid*)range_l_s);
    free((NVoid*)range_l_e);
    return g;
}

/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:   点是否与角度对齐，精度达到prec
* @Version:   1.0.0.1
* @Date:	  2021/5/11 16:29
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:
Is point (x,y) aligned to angle theta, up to precision 'prec'?
--------------------------------------------------------------------------------------------------*/
static NInt isaligned(NInt x, NInt y, image_double angles, NDouble theta, NDouble prec)
{
    NDouble a;
    /* check parameters */
    if (angles == NULL || angles->data == NULL)
        error("isaligned: invalid image 'angles'.");
    if (x < 0 || y < 0 || x >= (NInt)angles->xsize || y >= (NInt)angles->ysize)
        error("isaligned: (x,y) out of the image.");
    if (prec < 0.0)
        error("isaligned: 'prec' must be positive.");

    /* angle at pixel (x,y) */
    a = angles->data[x + y * angles->xsize];

    /* pixels whose level-line angle is not defined
    are considered as NON-aligned */
    if (a == NOTDEF)
        return FALSE; /* there is no need to call the function
                      'double_equal' here because there is
                      no risk of problems related to the
                      comparison doubles, we are only
                      interested in the exact NOTDEF value */

    /* it is assumed that 'theta' and 'a' are in the range [-pi,pi] */
    theta -= a;
    if (theta < 0.0)
        theta = -theta;
    if (theta > M_3_2_PI) {
        theta -= M_2__PI;
        if (theta < 0.0)
            theta = -theta;
    }

    return theta <= prec;
}

/*----------------------------------------------------------------------------*/
/** Absolute value angle difference. 绝对值角度差。
 */
static NDouble angle_diff(NDouble a, NDouble b)
{
    a -= b;
    while (a <= -M_PI)
        a += M_2__PI;
    while (a > M_PI)
        a -= M_2__PI;
    if (a < 0.0)
        a = -a;
    return a;
}

/*----------------------------------------------------------------------------*/
/** Signed angle difference.符号角度差。
 */
static NDouble angle_diff_signed(NDouble a, NDouble b)
{
    a -= b;
    while (a <= -M_PI)
        a += M_2__PI;
    while (a > M_PI)
        a -= M_2__PI;
    return a;
}

/*----------------------------- NFA computation ------------------------------*/
/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:    用Lanczos近似计算x的伽马函数。 当x 小于15时，效果好
* @Version:   1.0.0.1
* @Date:	  2021/5/11 15:58
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:
Computes the natural logarithm of the absolute value of
the gamma function of x using the Lanczos approximation.
See http://www.rskey.org/gamma.htm
The formula used is
@f[
\Gamma(x) = \frac{ \sum_{n=0}^{N} q_n x^n }{ \Pi_{n=0}^{N} (x+n) }
(x+5.5)^{x+0.5} e^{-(x+5.5)}
@f]
so
@f[
\log\Gamma(x) = \log\left( \sum_{n=0}^{N} q_n x^n \right)
+ (x+0.5) \log(x+5.5) - (x+5.5) - \sum_{n=0}^{N} \log(x+n)
@f]
and
q0 = 75122.6331530,
q1 = 80916.6278952,
q2 = 36308.2951477,
q3 = 8687.24529705,
q4 = 1168.92649479,
q5 = 83.8676043424,
q6 = 2.50662827511.
--------------------------------------------------------------------------------------------------*/
static NDouble log_gamma_lanczos(NDouble x)
{
    static NDouble q[7] = {75122.6331530, 80916.6278952, 36308.2951477, 8687.24529705, 1168.92649479, 83.8676043424, 2.50662827511};
    NDouble        a    = (x + 0.5) * log(x + 5.5) - (x + 5.5);
    NDouble        b    = 0.0;
    for (NInt n = 0; n < 7; n++) {
        a -= log(x + (NDouble)n);
        b += q[n] * pow(x, (NDouble)n);
    }
    return a + log(b);
}

/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:     使用Windschitl 方法近似计算 x 的伽马函数   , 当x大于15时，效果好
* @Version:   1.0.0.1
* @Date:	  2021/5/11 15:55
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:
Computes the natural logarithm of the absolute value of
the gamma function of x using Windschitl method.
See http://www.rskey.org/gamma.htm

The formula used is
@f[
\Gamma(x) = \sqrt{\frac{2\pi}{x}} \left( \frac{x}{e}
\sqrt{ x\sinh(1/x) + \frac{1}{810x^6} } \right)^x
@f]
so
@f[
\log\Gamma(x) = 0.5\log(2\pi) + (x-0.5)\log(x) - x
+ 0.5x\log\left( x\sinh(1/x) + \frac{1}{810x^6} \right).
@f]
This formula is a good approximation when x > 15.
--------------------------------------------------------------------------------------------------*/
static NDouble log_gamma_windschitl(NDouble x)
{
    return 0.918938533204673 + (x - 0.5) * log(x) - x + 0.5 * x * log(x * sinh(1 / x) + 1 / (810.0 * pow(x, 6.0)));
}

/*----------------------------------------------------------------------------*/
/** Computes the natural logarithm of the absolute value of
the gamma function of x. When x>15 use log_gamma_windschitl(),
otherwise use log_gamma_lanczos().
*/
#define log_gamma(x) ((x) > 15.0 ? log_gamma_windschitl(x) : log_gamma_lanczos(x))

/*----------------------------------------------------------------------------*/
/** Size of the table to store already computed inverse values.
 */
#ifndef TABSIZE
#    define TABSIZE 100000
#endif

/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:    基于总点数计算NFA值，即一致的点数。
* @Version:   1.0.0.1
* @Date:	  2021/5/11 16:33
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:
Computes -log10(NFA).

NFA stands for Number of False Alarms:
@f[
\mathrm{NFA} = NT \cdot B(n,k,p)
@f]

- NT       - number of tests
- B(n,k,p) - tail of binomial distribution with parameters n,k and p:
@f[
B(n,k,p) = \sum_{j=k}^n
\left(\begin{array}{c}n\\j\end{array}\right)
p^{j} (1-p)^{n-j}
@f]

The value -log10(NFA) is equivalent but more intuitive than NFA:
- -1 corresponds to 10 mean false alarms
-  0 corresponds to 1 mean false alarm
-  1 corresponds to 0.1 mean false alarms
-  2 corresponds to 0.01 mean false alarms
-  ...

Used this way, the bigger the value, better the detection,
and a logarithmic scale is used.

@param n,k,p binomial parameters.
@param logNT logarithm of Number of Tests

The computation is based in the gamma function by the following
relation:
@f[
\left(\begin{array}{c}n\\k\end{array}\right)
= \frac{ \Gamma(n+1) }{ \Gamma(k+1) \cdot \Gamma(n-k+1) }.
@f]
We use efficient algorithms to compute the logarithm of
the gamma function.

To make the computation faster, not all the sum is computed, part
of the terms are neglected based on a bound to the error obtained
(an error of 10% in the result is accepted).
--------------------------------------------------------------------------------------------------*/
static NDouble nfa(NInt n, NInt k, NDouble p, NDouble logNT)
{
    static NDouble inv[TABSIZE];    /* table to keep computed inverse values */
    NDouble        tolerance = 0.1; /* an error of 10% in the result is accepted */
    NDouble        log1term, term, bin_term, mult_term, bin_tail, err, p_term;
    NInt           i;

    /* check parameters */
    if (n < 0 || k < 0 || k > n || p <= 0.0 || p >= 1.0)
        error("nfa: wrong n, k or p values.");

    /* trivial cases */
    if (n == 0 || k == 0)
        return -logNT;
    if (n == k)
        return -logNT - (NDouble)n * log10(p);

    /* probability term */
    p_term = p / (1.0 - p);

    /* compute the first term of the series */
    /*
    binomial_tail(n,k,p) = sum_{i=k}^n bincoef(n,i) * p^i * (1-p)^{n-i}
    where bincoef(n,i) are the binomial coefficients.
    But
    bincoef(n,k) = gamma(n+1) / ( gamma(k+1) * gamma(n-k+1) ).
    We use this to compute the first term. Actually the log of it.
    */
    log1term = log_gamma((NDouble)n + 1.0) - log_gamma((NDouble)k + 1.0) - log_gamma((NDouble)(n - k) + 1.0) + (NDouble)k * log(p) + (NDouble)(n - k) * log(1.0 - p);
    term     = exp(log1term);

    /* in some cases no more computations are needed */
    if (double_equal(term, 0.0)) /* the first term is almost zero */
    {
        if ((NDouble)k > (NDouble)n * p)       /* at begin or end of the tail?  */
            return -log1term / M_LN10 - logNT; /* end: use just the first term  */
        else
            return -logNT; /* begin: the tail is roughly 1  */
    }

    /* compute more terms if needed */
    bin_tail = term;
    for (i = k + 1; i <= n; i++) {
        /*
        As
        term_i = bincoef(n,i) * p^i * (1-p)^(n-i)
        and
        bincoef(n,i)/bincoef(n,i-1) = n-1+1 / i,
        then,
        term_i / term_i-1 = (n-i+1)/i * p/(1-p)
        and
        term_i = term_i-1 * (n-i+1)/i * p/(1-p).
        1/i is stored in a table as they are computed,
        because divisions are expensive.
        p/(1-p) is computed only once and stored in 'p_term'.
        */
        bin_term = (NDouble)(n - i + 1) * (i < TABSIZE ? (inv[i] != 0.0 ? inv[i] : (inv[i] = 1.0 / (NDouble)i)) : 1.0 / (NDouble)i);

        mult_term = bin_term * p_term;
        term *= mult_term;
        bin_tail += term;
        if (bin_term < 1.0) {
            /* When bin_term<1 then mult_term_j<mult_term_i for j>i.
            Then, the error on the binomial tail when truncated at
            the i term can be bounded by a geometric series of form
            term_i * sum mult_term_i^j.                            */
            err = term * ((1.0 - pow(mult_term, (NDouble)(n - i + 1))) / (1.0 - mult_term) - 1.0);

            /* One wants an error at most of tolerance*final_result, or:
            tolerance * abs(-log10(bin_tail)-logNT).
            Now, the error that can be accepted on bin_tail is
            given by tolerance*final_result divided by the derivative
            of -log10(x) when x=bin_tail. that is:
            tolerance * abs(-log10(bin_tail)-logNT) / (1/bin_tail)
            Finally, we truncate the tail if the error is less than:
            tolerance * abs(-log10(bin_tail)-logNT) * bin_tail        */
            if (err < tolerance * fabs(-log10(bin_tail) - logNT) * bin_tail)
                break;
        }
    }
    return -log10(bin_tail) - logNT;
}

/*--------------------------- Rectangle structure ----------------------------*/
/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:    Rectangle structure: line segment with width.   具有宽度的线段
* @Version:   1.0.0.1
* @Date:	  2021/5/11 16:40
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:
--------------------------------------------------------------------------------------------------*/
struct rect
{
    NDouble x1, y1, x2, y2; /* first and second point of the line segment */
    NDouble width;          /* rectangle width */
    NDouble x, y;           /* center of the rectangle */
    NDouble theta;          /* angle */
    NDouble dx, dy;         /* (dx,dy) is vector oriented as the line segment */
    NDouble prec;           /* tolerance angle  公差角*/
    NDouble p;              // 角度在“精确”范围内的点的概率
};

/*---------------------------- 复制线段 ------------------------------------------------*/
static NVoid rect_copy(struct rect* input, struct rect* out)
{
    /* check parameters */
    if (input == NULL || out == NULL)
        error("rect_copy: invalid 'in' or 'out'.");

    /* copy values */
    out->x1    = input->x1;
    out->y1    = input->y1;
    out->x2    = input->x2;
    out->y2    = input->y2;
    out->width = input->width;
    out->x     = input->x;
    out->y     = input->y;
    out->theta = input->theta;
    out->dx    = input->dx;
    out->dy    = input->dy;
    out->prec  = input->prec;
    out->p     = input->p;
}

/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:     Rectangle points iterator. 结构体的迭代器
* @Version:   1.0.0.1
* @Date:	  2021/5/11 16:47
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:
The integer coordinates of pixels inside a rectangle are
iteratively explored. This structure keep track of the process and
functions ri_ini(), ri_inc(), ri_end(), and ri_del() are used in
the process. An example of how to use the iterator is as follows:
\code
使用方法：
struct rect * rec = XXX; // some rectangle
rect_iter * i;
for( i=ri_ini(rec); !ri_end(i); ri_inc(i) )
{
// your code, using 'i->x' and 'i->y' as coordinates
}
ri_del(i); // delete iterator

\endcode
The pixels are explored 'column' by 'column', where we call
'column' a set of pixels with the same x value that are inside the
rectangle. The following is an schematic representation of a
rectangle, the 'column' being explored is marked by colons, and
the current pixel being explored is 'x,y'.
\verbatim

vx[1],vy[1]
*   *
*       *
*           *
*               ye
*                :  *
vx[0],vy[0]           :     *
*              :        *
*          x,y          *
*        :              *
*     :            vx[2],vy[2]
*  :                *
y                     ys              *
^                        *           *
|                           *       *
|                              *   *
+---> x                      vx[3],vy[3]

\endverbatim
The first 'column' to be explored is the one with the smaller x
value. Each 'column' is explored starting from the pixel of the
'column' (inside the rectangle) with the smallest y value.

The four corners of the rectangle are stored in order that rotates
around the corners at the arrays 'vx[]' and 'vy[]'. The first
point is always the one with smaller x value.

'x' and 'y' are the coordinates of the pixel being explored. 'ys'
and 'ye' are the start and end values of the current column being
explored. So, 'ys' < 'ye'.
--------------------------------------------------------------------------------------------------*/
typedef struct
{
    NDouble vx[4];  /* rectangle's corner X coordinates in circular order */
    NDouble vy[4];  /* rectangle's corner Y coordinates in circular order */
    NDouble ys, ye; /* start and end Y values of current 'column' */
    NInt    x, y;   /* coordinates of currently explored pixel */
} rect_iter;


/*----------------------------------------------------------------------------*/
/** Interpolate y value corresponding to 'x' value given, in
the line 'x1,y1' to 'x2,y2'; if 'x1=x2' return the smaller
of 'y1' and 'y2'.

The following restrictions are required:
- x1 <= x2
- x1 <= x
- x  <= x2
*/
static NDouble inter_low(NDouble x, NDouble x1, NDouble y1, NDouble x2, NDouble y2)
{
    /* check parameters */
    if (x1 > x2 || x < x1 || x > x2) {
        error("inter_low: unsuitable input, 'x1>x2' or 'x<x1' or 'x>x2'.");
    }

    /* interpolation */
    if (double_equal(x1, x2) && y1 < y2) {
        return y1;
    }
    if (double_equal(x1, x2) && y1 > y2) {
        return y2;
    }
    return y1 + (x - x1) * (y2 - y1) / (x2 - x1);
}

/*----------------------------------------------------------------------------*/
/** Interpolate y value corresponding to 'x' value given, in
the line 'x1,y1' to 'x2,y2'; if 'x1=x2' return the larger
of 'y1' and 'y2'.

The following restrictions are required:
- x1 <= x2
- x1 <= x
- x  <= x2
*/
static NDouble inter_hi(NDouble x, NDouble x1, NDouble y1, NDouble x2, NDouble y2)
{
    /* check parameters */
    if (x1 > x2 || x < x1 || x > x2)
        error("inter_hi: unsuitable input, 'x1>x2' or 'x<x1' or 'x>x2'.");

    /* interpolation */
    if (double_equal(x1, x2) && y1 < y2)
        return y2;
    if (double_equal(x1, x2) && y1 > y2)
        return y1;
    return y1 + (x - x1) * (y2 - y1) / (x2 - x1);
}

/*----------------------------------------------------------------------------*/
/** Free memory used by a rectangle iterator.
 */
static NVoid ri_del(rect_iter* iter)
{
    if (iter == NULL)
        error("ri_del: NULL iterator.");
    free((NVoid*)iter);
}

/*----------------------------------------------------------------------------*/
/** Check if the iterator finished the full iteration.
See details in \ref rect_iter
*/
static NInt ri_end(rect_iter* i)
{
    /* check input */
    if (i == NULL)
        error("ri_end: NULL iterator.");

    /* if the current x value is larger than the largest
    x value in the rectangle (vx[2]), we know the full
    exploration of the rectangle is finished. */
    return (NDouble)(i->x) > i->vx[2];
}

/*----------------------------------------------------------------------------*/
/** Increment a rectangle iterator.
See details in \ref rect_iter
*/
static NVoid ri_inc(rect_iter* i)
{
    /* check input */
    if (i == NULL)
        error("ri_inc: NULL iterator.");

    /* if not at end of exploration,
    increase y value for next pixel in the 'column' */
    if (!ri_end(i))
        i->y++;

    /* if the end of the current 'column' is reached,
    and it is not the end of exploration,
    advance to the next 'column' */
    while ((NDouble)(i->y) > i->ye && !ri_end(i)) {
        /* increase x, next 'column' */
        i->x++;

        /* if end of exploration, return */
        if (ri_end(i))
            return;

        /* update lower y limit (start) for the new 'column'.

        We need to interpolate the y value that corresponds to the
        lower side of the rectangle. The first thing is to decide if
        the corresponding side is

        vx[0],vy[0] to vx[3],vy[3] or
        vx[3],vy[3] to vx[2],vy[2]

        Then, the side is interpolated for the x value of the
        'column'. But, if the side is vertical (as it could happen if
        the rectangle is vertical and we are dealing with the first
        or last 'columns') then we pick the lower value of the side
        by using 'inter_low'.
        */
        if ((NDouble)i->x < i->vx[3])
            i->ys = inter_low((NDouble)i->x, i->vx[0], i->vy[0], i->vx[3], i->vy[3]);
        else
            i->ys = inter_low((NDouble)i->x, i->vx[3], i->vy[3], i->vx[2], i->vy[2]);

        /* update upper y limit (end) for the new 'column'.

        We need to interpolate the y value that corresponds to the
        upper side of the rectangle. The first thing is to decide if
        the corresponding side is

        vx[0],vy[0] to vx[1],vy[1] or
        vx[1],vy[1] to vx[2],vy[2]

        Then, the side is interpolated for the x value of the
        'column'. But, if the side is vertical (as it could happen if
        the rectangle is vertical and we are dealing with the first
        or last 'columns') then we pick the lower value of the side
        by using 'inter_low'.
        */
        if ((NDouble)i->x < i->vx[1])
            i->ye = inter_hi((NDouble)i->x, i->vx[0], i->vy[0], i->vx[1], i->vy[1]);
        else
            i->ye = inter_hi((NDouble)i->x, i->vx[1], i->vy[1], i->vx[2], i->vy[2]);

        /* new y */
        i->y = (NInt)ceil(i->ys);
    }
}

/*----------------------------------------------------------------------------*/
/** Create and initialize a rectangle iterator.
See details in \ref rect_iter
*/
static rect_iter* ri_ini(struct rect* r)
{
    NDouble    vx[4], vy[4];
    NInt       n, offset;
    rect_iter* i;

    /* check parameters */
    if (r == NULL)
        error("ri_ini: invalid rectangle.");

    /* get memory */
    i = (rect_iter*)malloc(sizeof(rect_iter));
    if (i == NULL)
        error("ri_ini: Not enough memory.");

    /* build list of rectangle corners ordered
    in a circular way around the rectangle */
    vx[0] = r->x1 - r->dy * r->width / 2.0;
    vy[0] = r->y1 + r->dx * r->width / 2.0;
    vx[1] = r->x2 - r->dy * r->width / 2.0;
    vy[1] = r->y2 + r->dx * r->width / 2.0;
    vx[2] = r->x2 + r->dy * r->width / 2.0;
    vy[2] = r->y2 - r->dx * r->width / 2.0;
    vx[3] = r->x1 + r->dy * r->width / 2.0;
    vy[3] = r->y1 - r->dx * r->width / 2.0;

    /* compute rotation of index of corners needed so that the first
    point has the smaller x.

    if one side is vertical, thus two corners have the same smaller x
    value, the one with the largest y value is selected as the first.
    */
    if (r->x1 < r->x2 && r->y1 <= r->y2)
        offset = 0;
    else if (r->x1 >= r->x2 && r->y1 < r->y2)
        offset = 1;
    else if (r->x1 > r->x2 && r->y1 >= r->y2)
        offset = 2;
    else
        offset = 3;

    /* apply rotation of index. */
    for (n = 0; n < 4; n++) {
        i->vx[n] = vx[(offset + n) % 4];
        i->vy[n] = vy[(offset + n) % 4];
    }

    /* Set an initial condition.

    The values are set to values that will cause 'ri_inc' (that will
    be called immediately) to initialize correctly the first 'column'
    and compute the limits 'ys' and 'ye'.

    'y' is set to the integer value of vy[0], the starting corner.

    'ys' and 'ye' are set to very small values, so 'ri_inc' will
    notice that it needs to start a new 'column'.

    The smallest integer coordinate inside of the rectangle is
    'ceil(vx[0])'. The current 'x' value is set to that value minus
    one, so 'ri_inc' (that will increase x by one) will advance to
    the first 'column'.
    */
    i->x  = (NInt)ceil(i->vx[0]) - 1;
    i->y  = (NInt)ceil(i->vy[0]);
    i->ys = i->ye = -DBL_MAX;

    /* advance to the first pixel */
    ri_inc(i);

    return i;
}

/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:     Compute a rectangle's NFA value.
* @Version:   1.0.0.1
* @Date:	  2021/5/11 17:01
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:
--------------------------------------------------------------------------------------------------*/
static NDouble rect_nfa(struct rect* rec, image_double angles, NDouble logNT)
{
    rect_iter* i;
    NInt       pts = 0;
    NInt       alg = 0;

    /* check parameters */
    if (rec == NULL)
        error("rect_nfa: invalid rectangle.");
    if (angles == NULL)
        error("rect_nfa: invalid 'angles'.");

    /* compute the total number of pixels and of aligned points in 'rec' */
    for (i = ri_ini(rec); !ri_end(i); ri_inc(i)) /* rectangle iterator */
        if (i->x >= 0 && i->y >= 0 && i->x < (NInt)angles->xsize && i->y < (NInt)angles->ysize) {
            ++pts; /* total number of pixels counter */
            if (isaligned(i->x, i->y, angles, rec->theta, rec->prec))
                ++alg; /* aligned points counter */
        }
    ri_del(i); /* delete iterator */

    return nfa(pts, alg, rec->p, logNT); /* compute NFA value */
}

/*---------------------------------- Regions ---------------------------------*/
/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:  计算区域的角度作为区域的主惯性轴。
* @Version:   1.0.0.1
* @Date:	  2021/5/11 17:02
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:
Compute region's angle as the principal inertia axis of the region.

The following is the region inertia matrix A:
@f[

A = \left(\begin{array}{cc}
Ixx & Ixy \\
Ixy & Iyy \\
\end{array}\right)

@f]
where

Ixx =   sum_i G(i).(y_i - cx)^2

Iyy =   sum_i G(i).(x_i - cy)^2

Ixy = - sum_i G(i).(x_i - cx).(y_i - cy)

and
- G(i) is the gradient norm at pixel i, used as pixel's weight.
- x_i and y_i are the coordinates of pixel i.
- cx and cy are the coordinates of the center of th region.

lambda1 and lambda2 are the eigenvalues of matrix A,
with lambda1 >= lambda2. They are found by solving the
characteristic polynomial:

det( lambda I - A) = 0

that gives:

lambda1 = ( Ixx + Iyy + sqrt( (Ixx-Iyy)^2 + 4.0*Ixy*Ixy) ) / 2

lambda2 = ( Ixx + Iyy - sqrt( (Ixx-Iyy)^2 + 4.0*Ixy*Ixy) ) / 2

To get the line segment direction we want to get the angle the
eigenvector associated to the smallest eigenvalue. We have
to solve for a,b in:

a.Ixx + b.Ixy = a.lambda2

a.Ixy + b.Iyy = b.lambda2

We want the angle theta = atan(b/a). It can be computed with
any of the two equations:

theta = atan( (lambda2-Ixx) / Ixy )

or

theta = atan( Ixy / (lambda2-Iyy) )

When |Ixx| > |Iyy| we use the first, otherwise the second (just to
get better numeric precision).
--------------------------------------------------------------------------------------------------*/
static NDouble get_theta(struct point* reg, NInt reg_size, NDouble x, NDouble y, image_double modgrad, NDouble reg_angle, NDouble prec)
{
    NDouble lambda, theta, weight;
    NDouble Ixx = 0.0;
    NDouble Iyy = 0.0;
    NDouble Ixy = 0.0;
    NInt    i;

    /* check parameters */
    if (reg == NULL)
        error("get_theta: invalid region.");
    if (reg_size <= 1)
        error("get_theta: region size <= 1.");
    if (modgrad == NULL || modgrad->data == NULL)
        error("get_theta: invalid 'modgrad'.");
    if (prec < 0.0)
        error("get_theta: 'prec' must be positive.");

    /* compute inertia matrix */
    for (i = 0; i < reg_size; i++) {
        weight = modgrad->data[reg[i].x + reg[i].y * modgrad->xsize];
        Ixx += ((NDouble)reg[i].y - y) * ((NDouble)reg[i].y - y) * weight;
        Iyy += ((NDouble)reg[i].x - x) * ((NDouble)reg[i].x - x) * weight;
        Ixy -= ((NDouble)reg[i].x - x) * ((NDouble)reg[i].y - y) * weight;
    }
    if (double_equal(Ixx, 0.0) && double_equal(Iyy, 0.0) && double_equal(Ixy, 0.0))
        error("get_theta: null inertia matrix.");

    /* compute smallest eigenvalue */
    lambda = 0.5 * (Ixx + Iyy - sqrt((Ixx - Iyy) * (Ixx - Iyy) + 4.0 * Ixy * Ixy));

    /* compute angle */
    theta = fabs(Ixx) > fabs(Iyy) ? atan2(lambda - Ixx, Ixy) : atan2(Ixy, lambda - Iyy);

    /* The previous procedure doesn't cares about orientation,
    so it could be wrong by 180 degrees. Here is corrected if necessary. */
    if (angle_diff(theta, reg_angle) > prec)
        theta += M_PI;

    return theta;
}

/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:     计算覆盖点区域的矩形。查找区域的边界旋转矩形。
* @Version:   1.0.0.1
* @Date:	  2021/5/11 17:05
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:
Computes a rectangle that covers a region of points.
--------------------------------------------------------------------------------------------------*/
static NVoid region2rect(struct point* reg, NInt reg_size, image_double modgrad, NDouble reg_angle, NDouble prec, NDouble p, struct rect* rec)
{
    NDouble x, y, dx, dy, l, w, theta, weight, sum, l_min, l_max, w_min, w_max;
    NInt    i;

    /* check parameters */
    if (reg == NULL)
        error("region2rect: invalid region.");
    if (reg_size <= 1)
        error("region2rect: region size <= 1.");
    if (modgrad == NULL || modgrad->data == NULL)
        error("region2rect: invalid image 'modgrad'.");
    if (rec == NULL)
        error("region2rect: invalid 'rec'.");

    /* center of the region:

    It is computed as the weighted sum of the coordinates
    of all the pixels in the region. The norm of the gradient
    is used as the weight of a pixel. The sum is as follows:
    cx = \sum_i G(i).x_i
    cy = \sum_i G(i).y_i
    where G(i) is the norm of the gradient of pixel i
    and x_i,y_i are its coordinates.
    */
    x = y = sum = 0.0;
    for (i = 0; i < reg_size; i++) {
        weight = modgrad->data[reg[i].x + reg[i].y * modgrad->xsize];
        x += (NDouble)reg[i].x * weight;
        y += (NDouble)reg[i].y * weight;
        sum += weight;
    }
    if (sum <= 0.0)
        error("region2rect: weights sum equal to zero.");
    x /= sum;
    y /= sum;

    /* theta */
    theta = get_theta(reg, reg_size, x, y, modgrad, reg_angle, prec);

    /* length and width:

    'l' and 'w' are computed as the distance from the center of the
    region to pixel i, projected along the rectangle axis (dx,dy) and
    to the orthogonal axis (-dy,dx), respectively.

    The length of the rectangle goes from l_min to l_max, where l_min
    and l_max are the minimum and maximum values of l in the region.
    Analogously, the width is selected from w_min to w_max, where
    w_min and w_max are the minimum and maximum of w for the pixels
    in the region.
    */
    dx    = cos(theta);
    dy    = sin(theta);
    l_min = l_max = w_min = w_max = 0.0;
    for (i = 0; i < reg_size; i++) {
        l = ((NDouble)reg[i].x - x) * dx + ((NDouble)reg[i].y - y) * dy;
        w = -((NDouble)reg[i].x - x) * dy + ((NDouble)reg[i].y - y) * dx;

        if (l > l_max)
            l_max = l;
        if (l < l_min)
            l_min = l;
        if (w > w_max)
            w_max = w;
        if (w < w_min)
            w_min = w;
    }

    /* store values */
    rec->x1    = x + l_min * dx;
    rec->y1    = y + l_min * dy;
    rec->x2    = x + l_max * dx;
    rec->y2    = y + l_max * dy;
    rec->width = w_max - w_min;
    rec->x     = x;
    rec->y     = y;
    rec->theta = theta;
    rec->dx    = dx;
    rec->dy    = dy;
    rec->prec  = prec;
    rec->p     = p;

    /* we impose a minimal width of one pixel

    A sharp horizontal or vertical step would produce a perfectly
    horizontal or vertical region. The width computed would be
    zero. But that corresponds to a one pixels width transition in
    the image.
    */
    if (rec->width < 1.0)
        rec->width = 1.0;
}

/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:    建立一个共享相同角度的像素区域，最大从点（x，y）开始的公差'prec'。
* @Version:   1.0.0.1
* @Date:	  2021/5/11 17:06
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:
Build a region of pixels that share the same angle, up to a
tolerance 'prec', starting at point (x,y).
--------------------------------------------------------------------------------------------------*/
static NVoid region_grow(NInt x, NInt y, image_double angles, struct point* reg, NInt* reg_size, NDouble* reg_angle, image_char used, NDouble prec)
{
    NDouble sumdx, sumdy;
    NInt    xx, yy, i;

    /* check parameters */
    if (x < 0 || y < 0 || x >= (NInt)angles->xsize || y >= (NInt)angles->ysize)
        error("region_grow: (x,y) out of the image.");
    if (angles == NULL || angles->data == NULL)
        error("region_grow: invalid image 'angles'.");
    if (reg == NULL)
        error("region_grow: invalid 'reg'.");
    if (reg_size == NULL)
        error("region_grow: invalid pointer 'reg_size'.");
    if (reg_angle == NULL)
        error("region_grow: invalid pointer 'reg_angle'.");
    if (used == NULL || used->data == NULL)
        error("region_grow: invalid image 'used'.");

    /* first point of the region */
    *reg_size                       = 1;
    reg[0].x                        = x;
    reg[0].y                        = y;
    *reg_angle                      = angles->data[x + y * angles->xsize]; /* region's angle */
    sumdx                           = cos(*reg_angle);
    sumdy                           = sin(*reg_angle);
    used->data[x + y * used->xsize] = USED;

    /* try neighbors as new region points */
    for (i = 0; i < *reg_size; i++)
        for (xx = reg[i].x - 1; xx <= reg[i].x + 1; xx++)
            for (yy = reg[i].y - 1; yy <= reg[i].y + 1; yy++)
                if (xx >= 0 && yy >= 0 && xx < (NInt)used->xsize && yy < (NInt)used->ysize && used->data[xx + yy * used->xsize] != USED && isaligned(xx, yy, angles, *reg_angle, prec)) {
                    /* add point */
                    used->data[xx + yy * used->xsize] = USED;
                    reg[*reg_size].x                  = xx;
                    reg[*reg_size].y                  = yy;
                    ++(*reg_size);

                    /* update region's angle */
                    sumdx += cos(angles->data[xx + yy * angles->xsize]);
                    sumdy += sin(angles->data[xx + yy * angles->xsize]);
                    *reg_angle = atan2(sumdy, sumdx);
                }
}

/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:    尝试一些矩形变化以提高NFA值。 只有在矩形没有意义（即log_nfa <= log_eps）。
* @Version:   1.0.0.1
* @Date:	  2021/5/11 17:07
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:
Try some rectangles variations to improve NFA value. Only if the
rectangle is not meaningful (i.e., log_nfa <= log_eps).
--------------------------------------------------------------------------------------------------*/
static NDouble rect_improve(struct rect* rec, image_double angles, NDouble logNT, NDouble log_eps)
{
    struct rect r;
    NDouble     log_nfa, log_nfa_new;
    NDouble     delta   = 0.5;
    NDouble     delta_2 = delta / 2.0;
    NInt        n;

    log_nfa = rect_nfa(rec, angles, logNT);

    if (log_nfa > log_eps)
        return log_nfa;

    /* try finer precisions */
    rect_copy(rec, &r);
    for (n = 0; n < 5; n++) {
        r.p /= 2.0;
        r.prec      = r.p * M_PI;
        log_nfa_new = rect_nfa(&r, angles, logNT);
        if (log_nfa_new > log_nfa) {
            log_nfa = log_nfa_new;
            rect_copy(&r, rec);
        }
    }

    if (log_nfa > log_eps)
        return log_nfa;

    /* try to reduce width */
    rect_copy(rec, &r);
    for (n = 0; n < 5; n++) {
        if ((r.width - delta) >= 0.5) {
            r.width -= delta;
            log_nfa_new = rect_nfa(&r, angles, logNT);
            if (log_nfa_new > log_nfa) {
                rect_copy(&r, rec);
                log_nfa = log_nfa_new;
            }
        }
    }

    if (log_nfa > log_eps)
        return log_nfa;

    /* try to reduce one side of the rectangle */
    rect_copy(rec, &r);
    for (n = 0; n < 5; n++) {
        if ((r.width - delta) >= 0.5) {
            r.x1 += -r.dy * delta_2;
            r.y1 += r.dx * delta_2;
            r.x2 += -r.dy * delta_2;
            r.y2 += r.dx * delta_2;
            r.width -= delta;
            log_nfa_new = rect_nfa(&r, angles, logNT);
            if (log_nfa_new > log_nfa) {
                rect_copy(&r, rec);
                log_nfa = log_nfa_new;
            }
        }
    }

    if (log_nfa > log_eps)
        return log_nfa;

    /* try to reduce the other side of the rectangle */
    rect_copy(rec, &r);
    for (n = 0; n < 5; n++) {
        if ((r.width - delta) >= 0.5) {
            r.x1 -= -r.dy * delta_2;
            r.y1 -= r.dx * delta_2;
            r.x2 -= -r.dy * delta_2;
            r.y2 -= r.dx * delta_2;
            r.width -= delta;
            log_nfa_new = rect_nfa(&r, angles, logNT);
            if (log_nfa_new > log_nfa) {
                rect_copy(&r, rec);
                log_nfa = log_nfa_new;
            }
        }
    }

    if (log_nfa > log_eps)
        return log_nfa;

    /* try even finer precisions */
    rect_copy(rec, &r);
    for (n = 0; n < 5; n++) {
        r.p /= 2.0;
        r.prec      = r.p * M_PI;
        log_nfa_new = rect_nfa(&r, angles, logNT);
        if (log_nfa_new > log_nfa) {
            log_nfa = log_nfa_new;
            rect_copy(&r, rec);
        }
    }

    return log_nfa;
}

/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:    通过消除远离点的点来减小区域大小起点，直到导致右边的矩形区域满足点的密度，如果太小则丢弃该区域。
* @Version:   1.0.0.1
* @Date:	  2021/5/11 17:08
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:
Reduce the region size, by elimination the points far from the
starting point, until that leads to rectangle with the right
density of region points or to discard the region if too small.
--------------------------------------------------------------------------------------------------*/
static NInt reduce_region_radius(struct point* reg, NInt* reg_size, image_double modgrad, NDouble reg_angle, NDouble prec, NDouble p, struct rect* rec, image_char used, image_double angles,
                                 NDouble density_th)
{
    // NDouble density,rad1,rad2,rad,xc,yc;
    NDouble density;
    NDouble radistance1;
    NDouble radistance2;
    NDouble rad;
    NDouble xc;
    NDouble yc;
    NInt    i;

    /* check parameters */
    if (reg == NULL)
        error("reduce_region_radius: invalid pointer 'reg'.");
    if (reg_size == NULL)
        error("reduce_region_radius: invalid pointer 'reg_size'.");
    if (prec < 0.0)
        error("reduce_region_radius: 'prec' must be positive.");
    if (rec == NULL)
        error("reduce_region_radius: invalid pointer 'rec'.");
    if (used == NULL || used->data == NULL)
        error("reduce_region_radius: invalid image 'used'.");
    if (angles == NULL || angles->data == NULL)
        error("reduce_region_radius: invalid image 'angles'.");

    /* compute region points density */
    density = (NDouble)*reg_size / (dist(rec->x1, rec->y1, rec->x2, rec->y2) * rec->width);

    /* if the density criterion is satisfied there is nothing to do */
    if (density >= density_th)
        return TRUE;

    /* compute region's radius */
    xc          = (NDouble)reg[0].x;
    yc          = (NDouble)reg[0].y;
    radistance1 = dist(xc, yc, rec->x1, rec->y1);
    radistance2 = dist(xc, yc, rec->x2, rec->y2);
    rad         = radistance1 > radistance2 ? radistance1 : radistance2;

    /* while the density criterion is not satisfied, remove farther pixels */
    while (density < density_th) {
        rad *= 0.75; /* reduce region's radius to 75% of its value */

        /* remove points from the region and update 'used' map */
        for (i = 0; i < *reg_size; i++)
            if (dist(xc, yc, (NDouble)reg[i].x, (NDouble)reg[i].y) > rad) {
                /* point not kept, mark it as NOTUSED */
                used->data[reg[i].x + reg[i].y * used->xsize] = NOTUSED;
                /* remove point from the region */
                reg[i].x = reg[*reg_size - 1].x; /* if i==*reg_size-1 copy itself */
                reg[i].y = reg[*reg_size - 1].y;
                --(*reg_size);
                --i; /* to avoid skipping one point */
            }

        /* reject if the region is too small.
        2 is the minimal region size for 'region2rect' to work. */
        if (*reg_size < 2)
            return FALSE;

        /* re-compute rectangle */
        region2rect(reg, *reg_size, modgrad, reg_angle, prec, p, rec);

        /* re-compute region points density */
        density = (NDouble)*reg_size / (dist(rec->x1, rec->y1, rec->x2, rec->y2) * rec->width);
    }

    /* if this point is reached, the density criterion is satisfied */
    return TRUE;
}

/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:    角度公差的估计是通过点的角度的标准偏差进行的在该地区的起点附近。 然后，从同一点开始生长一个新区域，但是使用 估计的角度公差。 如果这样无法产生具有正确密度的区域点的矩形，调用“
reduce_region_radius”以尝试满足此条件。
* @Version:   1.0.0.1
* @Date:	  2021/5/11 17:09
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:
Refine a rectangle.

For that, an estimation of the angle tolerance is performed by the
standard deviation of the angle at points near the region's
starting point. Then, a new region is grown starting from the same
point, but using the estimated angle tolerance. If this fails to
produce a rectangle with the right density of region points,
'reduce_region_radius' is called to try to satisfy this condition.
--------------------------------------------------------------------------------------------------*/
static NInt refine(struct point* reg, NInt* reg_size, image_double modgrad, NDouble reg_angle, NDouble prec, NDouble p, struct rect* rec, image_char used, image_double angles, NDouble density_th)
{
    NDouble angle, ang_d, mean_angle, tau, density, xc, yc, ang_c, sum, s_sum;
    NInt    i, n;

    /* check parameters */
    if (reg == NULL)
        error("refine: invalid pointer 'reg'.");
    if (reg_size == NULL)
        error("refine: invalid pointer 'reg_size'.");
    if (prec < 0.0)
        error("refine: 'prec' must be positive.");
    if (rec == NULL)
        error("refine: invalid pointer 'rec'.");
    if (used == NULL || used->data == NULL)
        error("refine: invalid image 'used'.");
    if (angles == NULL || angles->data == NULL)
        error("refine: invalid image 'angles'.");

    /* compute region points density */
    density = (NDouble)*reg_size / (dist(rec->x1, rec->y1, rec->x2, rec->y2) * rec->width);

    /* if the density criterion is satisfied there is nothing to do */
    if (density >= density_th)
        return TRUE;

    /*------ First try: reduce angle tolerance ------*/

    /* compute the new mean angle and tolerance */
    xc    = (NDouble)reg[0].x;
    yc    = (NDouble)reg[0].y;
    ang_c = angles->data[reg[0].x + reg[0].y * angles->xsize];
    sum = s_sum = 0.0;
    n           = 0;
    for (i = 0; i < *reg_size; i++) {
        used->data[reg[i].x + reg[i].y * used->xsize] = NOTUSED;
        if (dist(xc, yc, (NDouble)reg[i].x, (NDouble)reg[i].y) < rec->width) {
            angle = angles->data[reg[i].x + reg[i].y * angles->xsize];
            ang_d = angle_diff_signed(angle, ang_c);
            sum += ang_d;
            s_sum += ang_d * ang_d;
            ++n;
        }
    }
    mean_angle = sum / (NDouble)n;
    tau        = 2.0 * sqrt((s_sum - 2.0 * mean_angle * sum) / (NDouble)n + mean_angle * mean_angle); /* 2 * standard deviation */

    /* find a new region from the same starting point and new angle tolerance */
    region_grow(reg[0].x, reg[0].y, angles, reg, reg_size, &reg_angle, used, tau);

    /* if the region is too small, reject */
    if (*reg_size < 2)
        return FALSE;

    /* re-compute rectangle */
    region2rect(reg, *reg_size, modgrad, reg_angle, prec, p, rec);

    /* re-compute region points density */
    density = (NDouble)*reg_size / (dist(rec->x1, rec->y1, rec->x2, rec->y2) * rec->width);

    /*------ Second try: reduce region radius ------*/
    if (density < density_th)
        return reduce_region_radius(reg, reg_size, modgrad, reg_angle, prec, p, rec, used, angles, density_th);

    /* if this point is reached, the density criterion is satisfied */
    return TRUE;
}

/*-------------------------- Line Segment Detector ---------------------------*/
/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:    LSD full interface.
* @Version:   1.0.0.1
* @Date:	  2021/5/11 17:11
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:
--------------------------------------------------------------------------------------------------*/
NDouble* LineSegmentDetection(NInt* n_out, NDouble* img, NInt X, NInt Y, NDouble scale, NDouble sigma_scale, NDouble quant, NDouble ang_th, NDouble log_eps, NDouble density_th, NInt n_bins,
                              NInt** reg_img, NInt* reg_x, NInt* reg_y)
{
    image_double     image;
    ntuple_list      out = new_ntuple_list(7);
    NDouble*         return_value;
    image_double     scaled_image, angles, modgrad;
    image_char       used;
    image_int        region = NULL;
    struct coorlist* list_p;
    NVoid*           mem_p;
    struct rect      rec;
    struct point*    reg;
    NInt             reg_size, min_reg_size, i;
    NUInt            xsize, ysize;
    NDouble          rho, reg_angle, prec, p, log_nfa, logNT;
    NInt             ls_count = 0;   // 线段个数

    if (img == NULL || X <= 0 || Y <= 0)
        error("invalid image input.");   // 检查参数
    if (scale <= 0.0)
        error("'scale' value must be positive.");
    if (sigma_scale <= 0.0)
        error("'sigma_scale' value must be positive.");
    if (quant < 0.0)
        error("'quant' value must be positive.");
    if (ang_th <= 0.0 || ang_th >= 180.0)
        error("'ang_th' value must be in the range (0,180).");
    if (density_th < 0.0 || density_th > 1.0)
        error("'density_th' value must be in the range [0,1].");
    if (n_bins <= 0)
        error("'n_bins' value must be positive.");


    prec  = M_PI * ang_th / 180.0;                           // 角度阈值
    p     = ang_th / 180.0;                                  // contrario model中角度符合基线角度阈值的概率
    rho   = quant / sin(prec);                               // 梯度阈值
    image = new_image_double_ptr((NUInt)X, (NUInt)Y, img);   // 创造图像
    if (scale != 1.0) {
        scaled_image = gaussian_sampler(image, scale, sigma_scale);                             // 对图像进行尺度变换
        angles       = ll_angle(scaled_image, rho, &list_p, &mem_p, &modgrad, (NUInt)n_bins);   // 计算每个点的方向
        free_image_double(scaled_image);
    }
    else
        angles = ll_angle(image, rho, &list_p, &mem_p, &modgrad, (NUInt)n_bins);   // 计算每个点的方向
    xsize = angles->xsize;
    ysize = angles->ysize;

    /* Number of Tests - NT

    The theoretical number of tests is Np.(XY)^(5/2)
    where X and Y are number of columns and rows of the image.
    Np corresponds to the number of angle precisions considered.
    As the procedure 'rect_improve' tests 5 times to halve the
    angle precision, and 5 more times after improving other factors,
    11 different precision values are potentially tested. Thus,
    the number of tests is
    11 * (X*Y)^(5/2)
    whose logarithm value is
    log10(11) + 5/2 * (log10(X) + log10(Y)).
    */
    logNT        = 5.0 * (log10((NDouble)xsize) + log10((NDouble)ysize)) / 2.0 + log10(11.0);
    min_reg_size = (NInt)(-logNT / log10(p)); /* minimal number of points in region
                                            that can give a meaningful event */


    /* initialize some structures */
    if (reg_img != NULL && reg_x != NULL && reg_y != NULL) /* save region data */
        region = new_image_int_ini(angles->xsize, angles->ysize, 0);
    used = new_image_char_ini(xsize, ysize, NOTUSED);
    reg  = (struct point*)calloc((size_t)(xsize * ysize), sizeof(struct point));
    if (reg == NULL)
        error("not enough memory!");


    /* search for line segments */
    for (; list_p != NULL; list_p = list_p->next)
        if (used->data[list_p->x + list_p->y * used->xsize] == NOTUSED && angles->data[list_p->x + list_p->y * angles->xsize] != NOTDEF)
        /* there is no risk of NDouble comparison problems here
        because we are only interested in the exact NOTDEF value */
        {
            /* find the region of connected point and ~equal angle 找出位置相邻接且方向相近的点构成的区域*/
            region_grow(list_p->x, list_p->y, angles, reg, &reg_size, &reg_angle, used, prec);

            /* reject small regions */
            if (reg_size < min_reg_size)
                continue;   // 构造矩形，近似该区域

            /* construct rectangular approximation for the region */
            region2rect(reg, reg_size, modgrad, reg_angle, prec, p, &rec);   // 检查矩形中点的密度，若小于阈值，则改进矩形表示。若仍旧不满足阈值条件，则清除该区域

            /* Check if the rectangle exceeds the minimal density of
            region points. If not, try to improve the region.
            The rectangle will be rejected if the final one does
            not fulfill the minimal density condition.
            This is an addition to the original LSD algorithm published in
            "LSD: A Fast Line Segment Detector with a False Detection Control"
            by R. Grompone von Gioi, J. Jakubowicz, J.M. Morel, and G. Randall.
            The original algorithm is obtained with density_th = 0.0.
            */
            if (!refine(reg, &reg_size, modgrad, reg_angle, prec, p, &rec, used, angles, density_th))
                continue;

            /* compute NFA value */
            log_nfa = rect_improve(&rec, angles, logNT, log_eps);
            if (log_nfa <= log_eps)
                continue;

            /* A New Line Segment was found! */
            ++ls_count; /* increase line segment counter */

            /*
            The gradient was computed with a 2x2 mask, its value corresponds to
            points with an offset of (0.5,0.5), that should be added to output.
            The coordinates origin is at the center of pixel (0,0).
            */
            rec.x1 += 0.5;
            rec.y1 += 0.5;
            rec.x2 += 0.5;
            rec.y2 += 0.5;

            /* scale the result values if a subsampling was performed */
            if (scale != 1.0) {
                rec.x1 /= scale;
                rec.y1 /= scale;
                rec.x2 /= scale;
                rec.y2 /= scale;
                rec.width /= scale;
            }

            /* add line segment found to output */
            add_7tuple(out, rec.x1, rec.y1, rec.x2, rec.y2, rec.width, rec.p, log_nfa);

            /* add region number to 'region' image if needed */
            if (region != NULL)
                for (i = 0; i < reg_size; i++)
                    region->data[reg[i].x + reg[i].y * region->xsize] = ls_count;
        }


    /* free memory */
    free((NVoid*)image); /* only the double_image structure should be freed,
                        the data pointer was provided to this functions
                        and should not be destroyed.                 */
    free_image_double(angles);
    free_image_double(modgrad);
    free_image_char(used);
    free((NVoid*)reg);
    free((NVoid*)mem_p);

    /* return the result */
    if (reg_img != NULL && reg_x != NULL && reg_y != NULL) {
        if (region == NULL)
            error("'region' should be a valid image.");
        *reg_img = region->data;
        if (region->xsize > (NUInt)INT_MAX || region->xsize > (NUInt)INT_MAX)
            error("region image to big to fit in NInt  sizes.");
        *reg_x = (NInt)(region->xsize);
        *reg_y = (NInt)(region->ysize);

        /* free the 'region' structure.
        we cannot use the function 'free_image_int' because we need to keep
        the memory with the image data to be returned by this function. */
        free((NVoid*)region);
    }
    if (out->size > (NUInt)INT_MAX)
        error("too many detections to fit in an NInt .");
    *n_out = (NInt)(out->size);

    return_value = out->values;
    free((NVoid*)out); /* only the 'ntuple_list' structure must be freed,
                      but the 'values' pointer must be keep to return
                      as a result. */

    return return_value;
}

/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:     LSD外部接口程序，自定义高斯核缩放因子，与区域参数。
* @Version:   1.0.0.1
* @Date:	  2021/5/11 17:12
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:
--------------------------------------------------------------------------------------------------*/
NDouble* VLsd::lsd_scale_region(NInt* n_out, NDouble* img, NInt X, NInt Y, NDouble scale, NInt** reg_img, NInt* reg_x, NInt* reg_y)
{
    NDouble sigma_scale = 0.6;    // 高斯核的计算公式  sigma = sigma_scale/scale.
    NDouble quant       = 2.0;    // 与梯度范数上的量化误差有关。
    NDouble ang_th      = 22.5;   // 梯度角公差，用角度表示
    NDouble log_eps     = 0.0;    // 检测阈值 -log10(NFA) > log_eps
    NDouble density_th  = 0.7;    // 矩形中区域点的最小密度
    NInt    n_bins      = 1024;   // 梯度模伪排序中的bins的个数
    return LineSegmentDetection(n_out, img, X, Y, scale, sigma_scale, quant, ang_th, log_eps, density_th, n_bins, reg_img, reg_x, reg_y);
}

/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:	  LSD外部接口程序，自己定义高斯核缩放因子
* @Version:   1.0.0.1
* @Date:	  2021/5/11 17:14
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:
--------------------------------------------------------------------------------------------------*/
NDouble* VLsd::lsd_scale(NInt* n_out, NDouble* img, NInt X, NInt Y, NDouble scale)
{
    return lsd_scale_region(n_out, img, X, Y, scale, NULL, NULL, NULL);
}

/*--------------------------------------------------------------------------------------------------
* @FuncName:
* @Author:    闹闹
* @Brief:    LSD外部接口程序
* @Version:   1.0.0.1
* @Date:	  2021/5/11 17:14
* @InputParameter:
* @OutputParameter:
* @Returns:
* @Others:
--------------------------------------------------------------------------------------------------*/
NDouble* VLsd::lsd(NInt* n_out, NDouble* img, NInt X, NInt Y)
{
    NDouble scale = 0.8;   // 高斯核缩放因子
    return lsd_scale(n_out, img, X, Y, scale);
}

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END
