/**
 * @FilePath     : /cola/cola/Vision/Feature/LBP/VLbp.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-18 16:29:57
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-18 16:33:24
 **/
#include "VLbp.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

namespace xx
{
/**
 * @brief 旋转不变圆形LBP特征计算，
 * @param img 输入图像
 * @param radius 半径，默认为3
 * @param neighbors 邻域，默认为8
 * @return 返回特征图
 * 特征值的维度是 8邻域，即为2的8次方，256维
 * https://blog.csdn.net/cv_pyer/article/details/90646549
 */
cv::Mat getELBPfeature(const cv::Mat& img, const int& radius = 3, const int& neighbors = 8)
{
    cv::Mat gray_img;
    if (img.channels() == 3)
        cv::cvtColor(img, gray_img, cv::COLOR_RGB2GRAY);
    else
        gray_img = img;
    int     width   = gray_img.cols;
    int     height  = gray_img.rows;
    cv::Mat RLBPImg = cv::Mat::zeros(height - 2 * radius, width - 2 * radius, CV_8UC1);
    for (int n = 0; n < neighbors; n++) {
        // 计算采样点对于中心点坐标的偏移量rx，ry,计算方式结合圆形编码理解
        float rx = static_cast<float>(radius * cos(2.0 * CV_PI * n / static_cast<float>(neighbors)));
        float ry = -static_cast<float>(radius * sin(2.0 * CV_PI * n / static_cast<float>(neighbors)));
        // 为双线性插值做准备
        // 对采样点偏移量分别进行上下取整
        int x1 = static_cast<int>(floor(rx));
        int x2 = static_cast<int>(ceil(rx));
        int y1 = static_cast<int>(floor(ry));
        int y2 = static_cast<int>(ceil(ry));
        // 将坐标偏移量映射到0-1之间
        float tx = rx - x1;
        float ty = ry - y1;
        // 根据0-1之间的x，y的权重计算公式计算权重，权重与坐标具体位置无关，与坐标间的差值有关
        float w1 = (1 - tx) * (1 - ty);
        float w2 = tx * (1 - ty);
        float w3 = (1 - tx) * ty;
        float w4 = tx * ty;
        // 循环处理每个像素
        for (int i = radius; i < height - radius; i++) {
            for (int j = radius; j < width - radius; j++) {
                // 获得中心像素点的灰度值
                uchar centerPix = gray_img.at<uchar>(i, j);
                // 根据双线性插值公式计算第k个采样点的灰度值
                float neighbor = gray_img.at<uchar>(i + x1, j + y1) * w1 + gray_img.at<uchar>(i + x1, j + y2) * w2 + gray_img.at<uchar>(i + x2, j + y1) * w3 + gray_img.at<uchar>(i + x2, j + y2) * w4;
                // LBP特征图像的每个邻居的LBP值累加，累加通过或操作完成，对应的LBP值通过移位取得
                RLBPImg.at<uchar>(i - radius, j - radius) |= (neighbor > centerPix) << (neighbors - n - 1);
            }
        }
    }
    // 进行旋转不变处理
    for (int i = 0; i < RLBPImg.rows; i++) {
        for (int j = 0; j < RLBPImg.cols; j++) {
            unsigned char currentValue = RLBPImg.at<uchar>(i, j);
            unsigned char minValue     = currentValue;
            for (int k = 1; k < neighbors; k++) {
                // 循环左移
                unsigned char temp = (currentValue >> (neighbors - k)) | (currentValue << k);
                if (temp < minValue) {
                    minValue = temp;
                }
            }
            RLBPImg.at<uchar>(i, j) = minValue;
        }
    }
#ifdef NDEBUG
    ((void)0);
#else
    cv::imshow("RLBPImg", RLBPImg);
    cv::waitKey(0);
#endif   // _DEBUG
    return RLBPImg;
}

/**
 * @brief 计算每一个LBP特征图，分块小图像的直方图
 * @param src      LBP特征图的分块小图像
 * @param minValue 最小值
 * @param maxValue 最大值
 * @param normed   归一化标志
 * @return         小块图像的直方图，行向量
 * https://blog.csdn.net/zfjBIT/article/details/90644380
 */
cv::Mat getRegionLBPH(const cv::Mat& src, const int& minValue, const int& maxValue, const bool& normed = true)
{
    // 定义存储直方图的矩阵
    cv::Mat result;
    // 计算得到直方图bin的数目，直方图数组的大小
    int histSize = maxValue - minValue + 1;
    // 定义直方图每一维的bin的变化范围
    float range[] = {static_cast<float>(minValue), static_cast<float>(maxValue + 1)};
    // 定义直方图所有bin的变化范围
    const float* ranges = {range};
    // 计算直方图，src是要计算直方图的图像，1是要计算直方图的图像数目，0是计算直方图所用的图像的通道序号，从0索引
    // Mat()是要用的掩模，result为输出的直方图，1为输出的直方图的维度，histSize直方图在每一维的变化范围
    // ranges，所有直方图的变化范围（起点和终点）
    cv::calcHist(&src, 1, 0, cv::Mat(), result, 1, &histSize, &ranges, true, false);
    // 归一化
    if (normed) {
        result /= (int)src.total();
    }
    // 结果表示成只有1行的矩阵
    return result.reshape(1, 1);
}

/**
 * @brief 计算LBPH
 * @param src LBP特征图
 * @param numPatterns LBP值的模式种类
 * @param grid_x 分块的x方向的个数
 * @param grid_y 分块的y方向的个数
 * @param normed 归一化标志
 * @return LBPH
 * https://blog.csdn.net/zfjBIT/article/details/90644380
 * https://blog.csdn.net/quincuntial/article/details/50541815
 */
cv::Mat getLBPH(cv::Mat src, const int& numPatterns = 256, const int& grid_x = 8, const int& grid_y = 8, const bool& normed = true)
{
    int width  = src.cols / grid_x;
    int height = src.rows / grid_y;
    // 定义LBPH的行和列，grid_x*grid_y表示将图像分割成这么些块，numPatterns表示LBP值的模式种类
    cv::Mat result = cv::Mat::zeros(grid_x * grid_y, numPatterns, CV_32FC1);
    if (src.empty()) {
        return result.reshape(1, 1);
    }
    int resultRowIndex = 0;
    // 对图像进行分割，分割成grid_x*grid_y块，grid_x，grid_y默认为8
    for (int i = 0; i < grid_x; i++) {
        for (int j = 0; j < grid_y; j++) {
            // 图像分块
            cv::Mat src_cell = cv::Mat(src, cv::Range(i * height, (i + 1) * height), cv::Range(j * width, (j + 1) * width));
            // 计算直方图
            cv::Mat hist_cell = getRegionLBPH(src_cell, 0, (numPatterns - 1), true);
            // 将直方图放到result中
            cv::Mat rowResult = result.row(resultRowIndex);
            hist_cell.reshape(1, 1).convertTo(rowResult, CV_32FC1);
            resultRowIndex++;
        }
    }
    // 行向量
    return result.reshape(1, 1);
}
}   // namespace xx

cv::Mat VLbp::operator()()
{
    cv::Mat gray_img;
    if (train_size_.width < 20 || train_size_.height < 20) {
        throw std::runtime_error("设置的图像宽度或者高度太小");
        exit(0);
    }
    for (int training_index = 0; training_index < images_.size(); training_index++) {
        if (images_[training_index].channels() == 3) {
            cv::cvtColor(images_[training_index], gray_img, cv::COLOR_RGB2GRAY);
        }
        else {
            gray_img = images_[training_index];
        }
        // 统一大小
        cv::resize(gray_img, gray_img, train_size_);
        cv::Mat LBPImg        = xx::getELBPfeature(gray_img);
        cv::Mat lbpDescriptor = xx::getLBPH(LBPImg);
        lbp_descriptors_.push_back(lbpDescriptor);
    }
    return lbp_descriptors_.clone();
}

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END