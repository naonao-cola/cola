/**
 * @FilePath     : /cola/cola/Vision/Filter/VFilter.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-23 17:57:45
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-09 21:45:51
 * @Copyright (c) 2024 by G, All Rights Reserved.
 **/
#include "VFilter.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

cv::Mat __image_make_border(cv::Mat& src)
{
    // 获取DFT变换的最佳宽度
    NInt w = cv::getOptimalDFTSize(src.cols);
    // 获取DFT变换的最佳高度
    NInt    h = cv::getOptimalDFTSize(src.rows);
    cv::Mat padded;
    // 常量法扩充图像边界，常量 = 0
    cv::copyMakeBorder(src, padded, 0, h - src.rows, 0, w - src.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
    padded.convertTo(padded, CV_32FC1);
    return padded;
}

void __fftshift(cv::Mat& plane0, cv::Mat& plane1)
{
    /*----------------------以下的操作是移动图像(零频移到中心)----------------------*/
    NInt cx = plane0.cols / 2;
    NInt cy = plane0.rows / 2;

    // 元素坐标表示为(cx, cy)
    cv::Mat part1_r(plane0, cv::Rect(0, 0, cx, cy));
    cv::Mat part2_r(plane0, cv::Rect(cx, 0, cx, cy));
    cv::Mat part3_r(plane0, cv::Rect(0, cy, cx, cy));
    cv::Mat part4_r(plane0, cv::Rect(cx, cy, cx, cy));

    /*------------------------------------------------------------*/
    cv::Mat temp;
    // 左上与右下交换位置(实部)
    part1_r.copyTo(temp);
    part4_r.copyTo(part1_r);
    temp.copyTo(part4_r);

    /*------------------------------------------------------------*/
    // 右上与左下交换位置(实部)
    part2_r.copyTo(temp);
    part3_r.copyTo(part2_r);
    temp.copyTo(part3_r);

    /*------------------------------------------------------------*/
    // 元素坐标(cx,cy)
    cv::Mat part1_i(plane1, cv::Rect(0, 0, cx, cy));
    cv::Mat part2_i(plane1, cv::Rect(cx, 0, cx, cy));
    cv::Mat part3_i(plane1, cv::Rect(0, cy, cx, cy));
    cv::Mat part4_i(plane1, cv::Rect(cx, cy, cx, cy));

    /*------------------------------------------------------------*/
    // 左上与右下交换位置(虚部)
    part1_i.copyTo(temp);
    part4_i.copyTo(part1_i);
    temp.copyTo(part4_i);

    /*------------------------------------------------------------*/
    // 右上与左下交换位置(虚部)
    part2_i.copyTo(temp);
    part3_i.copyTo(part2_i);
    temp.copyTo(part3_i);
}

cv::Mat __frequency_filter(cv::Mat& scr, cv::Mat& blur)
{
    /*----------------------------DFT---------------------------*/
    // 创建通道，存储dft后的实部与虚部（CV_32F，必须为单通道数）
    cv::Mat plane[] = {scr.clone(), cv::Mat::zeros(scr.size(), CV_32FC1)};
    cv::Mat complexIm;
    // 合并通道 （把两个矩阵合并为一个2通道的Mat类容器）
    cv::merge(plane, 2, complexIm);
    // 进行傅立叶变换，结果保存在自身
    cv::dft(complexIm, complexIm);

    /*----------------------------中心化-----------------------*/
    // 分离通道（数组分离）
    cv::split(complexIm, plane);
    /*-----------------以下的操作是频域迁移--------------------*/
    __fftshift(plane[0], plane[1]);
    /*-----------------滤波器函数与DFT结果的乘积---------------*/
    cv::Mat blur_r, blur_i, BLUR;
    // 滤波（实部与滤波器模板对应元素相乘）
    cv::multiply(plane[0], blur, blur_r);
    cv::multiply(plane[1], blur, blur_i);
    // 滤波（虚部与滤波器模板对应元素相乘）
    cv::Mat plane1[] = {blur_r, blur_i};
    // 实部与虚部合并
    cv::merge(plane1, 2, BLUR);

    /*------------------------得到原频谱图---------------------*/
    // 获取幅度图像，0通道为实部通道，1为虚部，因为二维傅立叶变换结果是复数
    cv::magnitude(plane[0], plane[1], plane[0]);
    // 傅立叶变换后的图片不好分析，进行对数处理，结果比较好看
    plane[0] += cv::Scalar::all(1);
    // float型的灰度空间为[0，1])
    cv::log(plane[0], plane[0]);
    // 归一化便于显示
    cv::normalize(plane[0], plane[0], 1, 0, cv::NORM_MINMAX);
    // cv::imshow("原图像频谱图", plane[0]);

    /*---------------------------------------------------------*/
    // idft结果也为复数
    cv::idft(BLUR, BLUR);
    // 分离通道，主要获取通道
    cv::split(BLUR, plane);
    // 求幅值(模)
    cv::magnitude(plane[0], plane[1], plane[0]);
    // 归一化便于显示
    cv::normalize(plane[0], plane[0], 1, 0, cv::NORM_MINMAX);
    return plane[0];
}

cv::Mat __ideal_low_kernel(cv::Mat& scr, NFloat sigma)
{
    // CV_32FC1
    cv::Mat ideal_low_pass(scr.size(), CV_32FC1);
    // 半径D0越小，模糊越大；半径D0越大，模糊越小
    NFloat d0 = sigma;
    for (NInt i = 0; i < scr.rows; i++) {
        for (NInt j = 0; j < scr.cols; j++) {
            // 分子,计算pow必须为float型
            NFloat d = sqrt(pow(static_cast<NFloat>(i - scr.rows / 2), 2) + pow(static_cast<NFloat>(j - scr.cols / 2), 2));
            if (d <= d0) {
                ideal_low_pass.at<NFloat>(i, j) = 1;
            }
            else {
                ideal_low_pass.at<NFloat>(i, j) = 0;
            }
        }
    }
    /*std::string name = "理想低通滤波器d0=" + std::to_string(sigma);
    cv::imshow(name, ideal_low_pass);*/
    return ideal_low_pass;
}

cv::Mat VFilter::ideal_low_pass_filter(cv::Mat& src, NFloat sigma)
{
    cv::Mat padded       = __image_make_border(src);
    cv::Mat ideal_kernel = __ideal_low_kernel(padded, sigma);
    cv::Mat result       = __frequency_filter(padded, ideal_kernel);
    return result;
}

cv::Mat __ideal_high_kernel(cv::Mat& scr, NFloat sigma)
{
    // CV_32FC1
    cv::Mat ideal_high_pass(scr.size(), CV_32FC1);
    // 半径D0越小，模糊越大；半径D0越大，模糊越小
    NFloat d0 = sigma;
    for (NInt i = 0; i < scr.rows; i++) {
        for (NInt j = 0; j < scr.cols; j++) {
            // 分子,计算pow必须为float型
            NFloat d = sqrt(pow(NFloat(i - scr.rows / 2), 2) + pow(static_cast<NFloat>(j - scr.cols / 2), 2));
            if (d <= d0) {
                ideal_high_pass.at<NFloat>(i, j) = 0;
            }
            else {
                ideal_high_pass.at<NFloat>(i, j) = 1;
            }
        }
    }
    // std::string name = "理想高通滤波器d0=" + std::to_string(sigma);
    // cv::imshow(name, ideal_high_pass);
    return ideal_high_pass;
}

cv::Mat VFilter::ideal_high_pass_filter(cv::Mat& src, NFloat sigma)
{
    cv::Mat padded       = __image_make_border(src);
    cv::Mat ideal_kernel = __ideal_high_kernel(padded, sigma);
    cv::Mat result       = __frequency_filter(padded, ideal_kernel);
    return result;
}

cv::Mat __butterworth_low_kernel(cv::Mat& scr, NFloat sigma, NInt n)
{
    // CV_32FC1
    cv::Mat butterworth_low_pass(scr.size(), CV_32FC1);
    // 半径D0越小，模糊越大；半径D0越大，模糊越小
    NDouble D0 = sigma;
    for (NInt i = 0; i < scr.rows; i++) {
        for (NInt j = 0; j < scr.cols; j++) {
            // 分子,计算pow必须为float型
            NDouble d                             = sqrt(pow(static_cast<NFloat>(i - scr.rows / 2), 2) + pow(static_cast<NFloat>(j - scr.cols / 2), 2));
            butterworth_low_pass.at<NFloat>(i, j) = 1.0 / (1 + pow(d / D0, 2 * n));
        }
    }

    /*std::string name = "巴特沃斯低通滤波器d0=" + std::to_string(sigma) + "n=" + std::to_string(n);
    cv::imshow(name, butterworth_low_pass);*/
    return butterworth_low_pass;
}

cv::Mat VFilter::butterworth_low_paass_filter(cv::Mat& src, NFloat d0, NInt n)
{
    /*
    H = 1 / (1+(D/D0)^2n)    n表示巴特沃斯滤波器的次数
    阶数n=1 无振铃和负值    阶数n=2 轻微振铃和负值  阶数n=5 明显振铃和负值   阶数n=20 与ILPF相似
    */
    cv::Mat padded             = __image_make_border(src);
    cv::Mat butterworth_kernel = __butterworth_low_kernel(padded, d0, n);
    cv::Mat result             = __frequency_filter(padded, butterworth_kernel);
    return result;
}

cv::Mat __butterworth_high_kernel(cv::Mat& scr, NFloat sigma, NInt n)
{
    // CV_32FC1
    cv::Mat butterworth_low_pass(scr.size(), CV_32FC1);
    // 半径D0越小，模糊越大；半径D0越大，模糊越小
    NDouble D0 = sigma;
    for (NInt i = 0; i < scr.rows; i++) {
        for (NInt j = 0; j < scr.cols; j++) {
            // 分子,计算pow必须为float型
            NDouble d                             = sqrt(pow(static_cast<NFloat>(i - scr.rows / 2), 2) + pow(static_cast<NFloat>(j - scr.cols / 2), 2));
            butterworth_low_pass.at<NFloat>(i, j) = 1.0 / (1 + pow(D0 / d, 2 * n));
        }
    }
    // std::string name = "巴特沃斯高通滤波器d0=" + std::to_string(sigma) + "n=" + std::to_string(n);
    // cv::imshow(name, butterworth_low_pass);
    return butterworth_low_pass;
}

cv::Mat VFilter::butterworth_high_paass_filter(cv::Mat& src, NFloat d0, NInt n)
{
    // H = 1 / (1+(D0/D)^2n)    n表示巴特沃斯滤波器的次数
    cv::Mat padded             = __image_make_border(src);
    cv::Mat butterworth_kernel = __butterworth_high_kernel(padded, d0, n);
    cv::Mat result             = __frequency_filter(padded, butterworth_kernel);
    return result;
}

cv::Mat __gaussian_low_pass_kernel(cv::Mat scr, NFloat sigma)
{
    // CV_32FC1
    cv::Mat gaussianBlur(scr.size(), CV_32FC1);
    // 高斯函数参数，越小，频率高斯滤波器越窄，滤除高频成分越多，图像就越平滑
    NFloat d0 = 2 * sigma * sigma;
    for (NInt i = 0; i < scr.rows; i++) {
        for (NInt j = 0; j < scr.cols; j++) {
            // 分子,计算pow必须为float型
            NFloat d = pow(NFloat(i - scr.rows / 2), 2) + pow(static_cast<NFloat>(j - scr.cols / 2), 2);
            // expf为以e为底求幂（必须为float型）
            gaussianBlur.at<NFloat>(i, j) = expf(-d / d0);
        }
    }
    //    Mat show = gaussianBlur.clone();
    //    //归一化到[0,255]供显示
    //    normalize(show, show, 0, 255, NORM_MINMAX);
    //    //转化成CV_8U型
    //    show.convertTo(show, CV_8U);
    //    std::string pic_name = "gaussi" + std::to_string((NInt)sigma) + ".jpg";
    //    imwrite( pic_name, show);

    // cv::imshow("高斯低通滤波器", gaussianBlur);
    return gaussianBlur;
}


// 高斯低通滤波器
cv::Mat VFilter::gaussian_low_pass_filter(cv::Mat& src, NFloat d0)
{
    cv::Mat padded          = __image_make_border(src);
    cv::Mat gaussian_kernel = __gaussian_low_pass_kernel(padded, d0);
    cv::Mat result          = __frequency_filter(padded, gaussian_kernel);
    return result;
}

cv::Mat __gaussian_high_pass_kernel(cv::Mat scr, NFloat sigma)
{
    // CV_32FC1
    cv::Mat gaussianBlur(scr.size(), CV_32FC1);
    NFloat  d0 = 2 * sigma * sigma;
    for (NInt i = 0; i < scr.rows; i++) {
        for (NInt j = 0; j < scr.cols; j++) {
            // 分子,计算pow必须为float型
            NFloat d                      = pow(static_cast<NFloat>(i - scr.rows / 2), 2) + pow(static_cast<NFloat>(j - scr.cols / 2), 2);
            gaussianBlur.at<NFloat>(i, j) = 1 - expf(-d / d0);
        }
    }
    // cv::imshow("高斯高通滤波器", gaussianBlur);
    return gaussianBlur;
}

// 高斯高通滤波器
cv::Mat VFilter::gaussian_high_pass_filter(cv::Mat& src, NFloat d0)
{
    cv::Mat padded          = __image_make_border(src);
    cv::Mat gaussian_kernel = __gaussian_high_pass_kernel(padded, d0);
    cv::Mat result          = __frequency_filter(padded, gaussian_kernel);
    return result;
}

cv::Mat VFilter::cascade_filter(const cv::Mat& src, NInt K, NInt ksize)
{
    NInt    img_size = src.cols * src.rows;
    cv::Mat median_dst;
    cv::Mat diff_dst;
    cv::Mat bilateral_dst;
    // 中值滤波
    cv::medianBlur(src, median_dst, ksize);
    // 图像作差，取绝对值，再除以图像的大小，表示中值滤波器的去噪能力
    cv::absdiff(src, median_dst, diff_dst);
    cv::Scalar diff_mean;
    diff_mean = cv::mean(diff_dst);
    // 二级滤波器的系数
    NInt coefficient = abs(K - diff_mean[0]);
    cv::bilateralFilter(median_dst, bilateral_dst, -1, coefficient, coefficient);
    return bilateral_dst;
}

void integralImgSqDiff(cv::Mat src, cv::Mat& dst, NInt Ds, NInt t1, NInt t2, NInt m1, NInt n1)
{
    // 计算图像A与图像B的差值图C
    cv::Mat Dist2 = src(cv::Range(Ds, src.rows - Ds), cv::Range(Ds, src.cols - Ds)) - src(cv::Range(Ds + t1, src.rows - Ds + t1), cv::Range(Ds + t2, src.cols - Ds + t2));
    NFloat* Dist2_data;
    for (NInt i = 0; i < m1; i++) {
        Dist2_data = Dist2.ptr<NFloat>(i);
        for (NInt j = 0; j < n1; j++) {
            Dist2_data[j] *= Dist2_data[j];   // 计算图像C的平方图D
        }
    }
    cv::integral(Dist2, dst, CV_32F);   // 计算图像D的积分图
}

void VFilter::nl_filter(cv::Mat src, cv::Mat& dst, NInt ds, NInt Ds, NFloat h)
{
    cv::Mat src_tmp;
    src.convertTo(src_tmp, CV_32F);
    NInt    m         = src_tmp.rows;
    NInt    n         = src_tmp.cols;
    NInt    boardSize = Ds + ds + 1;
    cv::Mat src_board;
    cv::copyMakeBorder(src_tmp, src_board, boardSize, boardSize, boardSize, boardSize, cv::BORDER_REFLECT);

    cv::Mat average(m, n, CV_32FC1, 0.0);
    cv::Mat sweight(m, n, CV_32FC1, 0.0);

    NFloat h2 = h * h;
    NInt   d2 = (2 * ds + 1) * (2 * ds + 1);

    NInt    m1 = src_board.rows - 2 * Ds;   // 行
    NInt    n1 = src_board.cols - 2 * Ds;   // 列
    cv::Mat St(m1, n1, CV_32FC1, 0.0);

    for (NInt t1 = -Ds; t1 <= Ds; t1++) {
        NInt Dst1 = Ds + t1;
        for (NInt t2 = -Ds; t2 <= Ds; t2++) {
            NInt Dst2 = Ds + t2;
            integralImgSqDiff(src_board, St, Ds, t1, t2, m1, n1);
#pragma omp for
            for (NInt i = 0; i < m; i++) {
                NFloat* sweight_p = sweight.ptr<NFloat>(i);
                NFloat* average_p = average.ptr<NFloat>(i);
                NFloat* v_p       = src_board.ptr<NFloat>(i + Ds + t1 + ds);
                NInt    i1        = i + ds + 1;   // row
                NFloat* St_p1     = St.ptr<NFloat>(i1 + ds);
                NFloat* St_p2     = St.ptr<NFloat>(i1 - ds - 1);

                for (NInt j = 0; j < n; j++) {

                    NInt   j1    = j + ds + 1;   // col
                    NFloat Dist2 = (St_p1[j1 + ds] + St_p2[j1 - ds - 1]) - (St_p1[j1 - ds - 1] + St_p2[j1 + ds]);

                    Dist2 /= (-d2 * h2);
                    NFloat w = std::exp(Dist2);
                    sweight_p[j] += w;
                    average_p[j] += w * v_p[j + Ds + t2 + ds];
                }
            }
        }
    }
    average = average / sweight;
    average.convertTo(dst, CV_8U);
}

cv::Mat VFilter::guided_filter(cv::Mat& I, cv::Mat& p, NInt r, NDouble eps)
{
    NInt wsize = 2 * r + 1;
    // 数据类型转换
    I.convertTo(I, CV_64F, 1.0 / 255.0);
    p.convertTo(p, CV_64F, 1.0 / 255.0);

    // meanI=fmean(I)
    cv::Mat mean_I;
    cv::boxFilter(I, mean_I, -1, cv::Size(wsize, wsize), cv::Point(-1, -1), true, cv::BORDER_REFLECT);   // 盒子滤波

    // meanP=fmean(P)
    cv::Mat mean_p;
    cv::boxFilter(p, mean_p, -1, cv::Size(wsize, wsize), cv::Point(-1, -1), true, cv::BORDER_REFLECT);   // 盒子滤波

    // corrI=fmean(I.*I)
    cv::Mat mean_II;
    mean_II = I.mul(I);
    cv::boxFilter(mean_II, mean_II, -1, cv::Size(wsize, wsize), cv::Point(-1, -1), true, cv::BORDER_REFLECT);   // 盒子滤波

    // corrIp=fmean(I.*p)
    cv::Mat mean_Ip;
    mean_Ip = I.mul(p);
    cv::boxFilter(mean_Ip, mean_Ip, -1, cv::Size(wsize, wsize), cv::Point(-1, -1), true, cv::BORDER_REFLECT);   // 盒子滤波

    // varI=corrI-meanI.*meanI
    cv::Mat var_I, mean_mul_I;
    mean_mul_I = mean_I.mul(mean_I);
    cv::subtract(mean_II, mean_mul_I, var_I);

    // covIp=corrIp-meanI.*meanp
    cv::Mat cov_Ip;
    cv::subtract(mean_Ip, mean_I.mul(mean_p), cov_Ip);

    // a=conIp./(varI+eps)
    // b=meanp-a.*meanI
    cv::Mat a, b;
    cv::divide(cov_Ip, (var_I + eps), a);
    cv::subtract(mean_p, a.mul(mean_I), b);

    // meana=fmean(a)
    // meanb=fmean(b)
    cv::Mat mean_a, mean_b;
    cv::boxFilter(a, mean_a, -1, cv::Size(wsize, wsize), cv::Point(-1, -1), true, cv::BORDER_REFLECT);   // 盒子滤波
    cv::boxFilter(b, mean_b, -1, cv::Size(wsize, wsize), cv::Point(-1, -1), true, cv::BORDER_REFLECT);   // 盒子滤波

    // q=meana.*I+meanb
    cv::Mat q;
    q = mean_a.mul(I) + mean_b;

    // 数据类型转换
    I.convertTo(I, CV_8U, 255);
    p.convertTo(p, CV_8U, 255);
    q.convertTo(q, CV_8U, 255);
    return q;
}
NDouble VFilter::compute_PSNR(const cv::Mat& Mat1, const cv::Mat& Mat2)
{
    cv::Mat M1 = Mat1.clone();
    cv::Mat M2 = Mat2.clone();
    cv::Mat diff_img;
    diff_img.convertTo(diff_img, CV_32F);
    cv::absdiff(M1, M2, diff_img);

    diff_img     = diff_img.mul(diff_img);
    cv::Scalar s = cv::sum(diff_img);
    NDouble    sse;
    if (M1.channels() == 3 && M2.channels() == 3) {
        sse = s.val[0] + s.val[1] + s.val[2];
    }
    else {
        sse = s.val[0];
    }
    std::size_t total_element = M1.channels() * M1.total();
    NDouble     mse           = (sse / static_cast<NDouble>(total_element));
    NDouble     psnr          = 10.0 * log10(static_cast<NDouble>(255) * 255 / (mse + 0.0000001));
    return psnr;
}

cv::Scalar VFilter::compute_MSSIM(const cv::Mat& mat1, const cv::Mat& mat2)
{
    cv::Mat       i1 = mat1;
    cv::Mat       i2 = mat2;
    const NDouble C1 = 6.5025;
    const NDouble C2 = 58.5225;
    NInt          d  = CV_32F;
    cv::Mat       I1;
    cv::Mat       I2;
    i1.convertTo(I1, d);
    i2.convertTo(I2, d);
    cv::Mat I2_2  = I2.mul(I2);
    cv::Mat I1_2  = I1.mul(I1);
    cv::Mat I1_I2 = I1.mul(I2);
    cv::Mat mu1;
    cv::Mat mu2;
    cv::GaussianBlur(I1, mu1, cv::Size(11, 11), 1.5);
    cv::GaussianBlur(I2, mu2, cv::Size(11, 11), 1.5);
    cv::Mat mu1_2   = mu1.mul(mu1);
    cv::Mat mu2_2   = mu2.mul(mu2);
    cv::Mat mu1_mu2 = mu1.mul(mu2);
    cv::Mat sigma1_2;
    cv::Mat sigma2_2;
    cv::Mat sigma12;
    cv::GaussianBlur(I1_2, sigma1_2, cv::Size(11, 11), 1.5);
    sigma1_2 -= mu1_2;
    cv::GaussianBlur(I2_2, sigma2_2, cv::Size(11, 11), 1.5);
    sigma2_2 -= mu2_2;
    cv::GaussianBlur(I1_I2, sigma12, cv::Size(11, 11), 1.5);
    sigma12 -= mu1_mu2;
    cv::Mat t1;
    cv::Mat t2;
    cv::Mat t3;
    t1 = 2 * mu1_mu2 + C1;
    t2 = 2 * sigma12 + C2;
    t3 = t1.mul(t2);
    t1 = mu1_2 + mu2_2 + C1;
    t2 = sigma1_2 + sigma2_2 + C2;
    t1 = t1.mul(t2);
    cv::Mat ssim_map;
    cv::divide(t3, t1, ssim_map);
    cv::Scalar mssim = mean(ssim_map);
    return mssim;
}

NDouble VFilter::variance_laplacian(const cv::Mat& src)
{
    cv::Mat laplacain;
    cv::Laplacian(src, laplacain, CV_64F, 3);
    cv::Mat means;
    cv::Mat stddev;
    cv::meanStdDev(laplacain, means, stddev);
    NDouble ret = (static_cast<NDouble>(*stddev.ptr<NFloat>(0, 0))) * (static_cast<NDouble>(*stddev.ptr<NFloat>(0, 0)));
    return ret;
}
NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END