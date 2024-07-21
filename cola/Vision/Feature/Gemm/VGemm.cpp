/**
 * @FilePath     : /cola/cola/Vision/Feature/Gemm/VGemm.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-18 16:45:41
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-20 12:16:24
 **/

#include "VGemm.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

struct GGCMFeatures
{
    GGCMFeatures()
        : small_grads_dominance(0.0)
        , big_grads_dominance(0.0)
        , gray_asymmetry(0.0)
        , grads_asymmetry(0.0)
        , energy(0.0)
        , gray_mean(0.0)
        , grads_mean(0.0)
        , gray_variance(0.0)
        , grads_variance(0.0)
        , corelation(0.0)
        , gray_entropy(0.0)
        , grads_entropy(0.0)
        , entropy(0.0)
        , inertia(0.0)
        , differ_moment(0.0)
    {
    }
    float small_grads_dominance;   // 小梯度优势
    float big_grads_dominance;     // 大梯度优势
    float gray_asymmetry;          // 灰度分布不均匀性
    float grads_asymmetry;         // 梯度分布不均匀性
    float energy;                  // 能量
    float gray_mean;               // 灰度均值
    float grads_mean;              // 梯度均值
    float gray_variance;           // 灰度均方差
    float grads_variance;          // 梯度均方差
    float corelation;              // 相关性
    float gray_entropy;            // 灰度熵
    float grads_entropy;           // 梯度熵
    float entropy;                 // 混合熵
    float inertia;                 // 惯性
    float differ_moment;           // 逆差距
};


VGemm::VGemm()
    : grayLevel_(16)
{
}
VGemm::~VGemm() = default;
void VGemm::initGGCM(VecGGCM& vecGGCM, int size)
{
    assert(size == grayLevel_);
    vecGGCM.resize(size);
    for (int i = 0; i < size; ++i) {
        vecGGCM[i].resize(size);
    }
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            vecGGCM[i][j] = 0;
        }
    }
}

void VGemm::calGGCM(cv::Mat& inputImg, VecGGCM& vecGGCM)
{
    assert(inputImg.channels() == 1);
    cv::Mat src;
    src        = inputImg.clone();
    int height = src.rows;
    int width  = src.cols;

    // 寻找最大像素灰度最大值
    double    min;
    double    maxGrayLevel;
    cv::Point minloc;
    cv::Point maxloc;
    cv::minMaxLoc(src, &min, &maxGrayLevel, &minloc, &maxloc);
    ++maxGrayLevel;

    // 生成一个和原图一般大小的二维vector,初始化动态数组
    VecGGCM tempVec_Gray;
    tempVec_Gray.resize(height);
    for (int i = 0; i < height; ++i) {
        tempVec_Gray[i].resize(width);
    }

    //  灰度归一化
    //  若灰度级数大于16，则将图像的灰度级缩小至16级。
    if (maxGrayLevel > grayLevel_) {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                int tmpVal         = src.at<uchar>(i, j);
                tempVec_Gray[i][j] = static_cast<int>(tmpVal * grayLevel_ / maxGrayLevel);
            }
        }
    }
    else {   // 若灰度级数小于16，则生成相应的灰度矩阵
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                int tmpVal         = src.at<uchar>(i, j);
                tempVec_Gray[i][j] = tmpVal;
            }
        }
    }

    VecGGCM tempVec_Gradient;
    tempVec_Gradient.resize(height);
    for (int i = 0; i < height; ++i) {
        tempVec_Gradient[i].resize(width);
    }
    int maxGradientLevel = 0;
    //  求图像的梯度
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
                tempVec_Gradient[i][j] = 0;
            }
            else {
                int g_x =
                    src.at<uchar>(i + 1, j - 1) + 2 * src.at<uchar>(i + 1, j) + src.at<uchar>(i + 1, j + 1) - src.at<uchar>(i - 1, j - 1) - 2 * src.at<uchar>(i - 1, j) - src.at<uchar>(i - 1, j + 1);
                int g_y =
                    src.at<uchar>(i - 1, j + 1) + 2 * src.at<uchar>(i, j + 1) + src.at<uchar>(i + 1, j + 1) - src.at<uchar>(i - 1, j + 1) - 2 * src.at<uchar>(i, j - 1) - src.at<uchar>(i + 1, j - 1);
                int g                  = sqrt(g_x * g_x + g_y * g_y);
                tempVec_Gradient[i][j] = g;
                if (g > maxGradientLevel) {
                    maxGradientLevel = g;
                }
            }
        }
    }
    ++maxGradientLevel;
    //  梯度归一化
    if (maxGradientLevel > grayLevel_) {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                int tmpVal             = tempVec_Gradient[i][j];
                tempVec_Gradient[i][j] = int(tmpVal * grayLevel_ / maxGradientLevel);
            }
        }
    }
    // 得到梯度-灰度共生矩阵
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int row = tempVec_Gray[i][j];
            int col = tempVec_Gradient[i][j];
            vecGGCM[row][col]++;
        }
    }
}

// 二维数组求和
template<typename T>
float sumVVector(std::vector<std::vector<T>> v)
{
    float ans = 0;
    for (int i = 0; i < v.size(); ++i) {
        for (int j = 0; j < v[i].size(); ++j) {
            ans += v[i][j];
        }
    }
    return ans;
}

// 二维数组按行求和
template<typename T>
float sumRowVVector(std::vector<std::vector<T>> v, int num)
{
    float ans = 0;
    for (int i = 0; i < v.size(); ++i) {
        ans += v[num][i];
    }
    return ans;
}

// 二维数组按列求和
template<typename T>
float sumColVVector(std::vector<std::vector<T>> v, int num)
{
    float ans = 0;
    for (int i = 0; i < v.size(); ++i) {
        ans += v[i][num];
    }
    return ans;
}

void VGemm::getGGCMFeatures(VecGGCM& vecGGCM, GGCMFeatures& features)
{
    float total = sumVVector(vecGGCM);
    for (int i = 0; i < grayLevel_; ++i) {
        float sumRowGray = 0;
        sumRowGray       = sumRowVVector(vecGGCM, i);
        float sumColGrad = 0;
        sumColGrad       = sumColVVector(vecGGCM, i);
        for (int j = 0; j < grayLevel_; ++j) {
            features.small_grads_dominance += vecGGCM[i][j] / pow(j + 1, 2);
            features.big_grads_dominance += vecGGCM[i][j] * pow(j + 1, 2);
        }
        features.gray_asymmetry += pow(sumRowGray, 2);
        features.grads_asymmetry += pow(sumColGrad, 2);
    }

    features.small_grads_dominance /= total;
    features.big_grads_dominance /= total;
    features.gray_asymmetry /= total;
    features.grads_asymmetry /= total;

    std::vector<std::vector<float>> vecPGGCM;
    vecPGGCM.resize(grayLevel_);
    for (int i = 0; i < grayLevel_; ++i) {
        vecPGGCM[i].resize(grayLevel_);
    }

    for (int i = 0; i < vecGGCM.size(); i++) {
        for (int j = 0; j < vecGGCM[i].size(); j++) {
            int tmp        = vecGGCM[i][j];
            vecPGGCM[i][j] = tmp / total;
        }
    }

    for (int i = 0; i < grayLevel_; i++) {
        float sumRowGray = 0;
        sumRowGray       = sumRowVVector(vecPGGCM, i);
        float sumColGrad = 0;
        sumColGrad       = sumColVVector(vecPGGCM, i);
        for (int j = 0; j < grayLevel_; j++) {
            features.energy += pow(vecPGGCM[i][j], 2);
            if (vecGGCM[i][j] != 0) {
                features.entropy -= vecPGGCM[i][j] * log(vecPGGCM[i][j]);
                features.inertia += pow((i - j), 2) * vecPGGCM[i][j];
            }
            features.differ_moment += vecPGGCM[i][j] / (1 + pow((i - j), 2));
        }
        features.gray_mean += (i + 1) * sumRowGray;
        features.grads_mean += (i + 1) * sumColGrad;
        if (sumRowGray != 0) {
            features.gray_entropy -= sumRowGray * log(sumRowGray);
        }
        if (sumColGrad != 0) {
            features.grads_entropy -= sumColGrad * log(sumColGrad);
        }
    }

    for (int i = 0; i < grayLevel_; i++) {
        float sumRowGray = 0;
        sumRowGray       = sumRowVVector(vecPGGCM, i);
        features.gray_variance += pow(i + 1 - features.gray_mean, 2) * sumRowGray;
        float sumColGrad = 0;
        sumColGrad       = sumColVVector(vecPGGCM, i);
        features.grads_variance += pow(i + 1 - features.grads_mean, 2) * sumColGrad;
    }
    features.gray_variance  = pow(features.gray_variance, 0.5);
    features.grads_variance = pow(features.grads_variance, 0.5);

    for (int i = 0; i < grayLevel_; ++i) {
        for (int j = 0; j < grayLevel_; ++j) {
            features.corelation += (i + 1 - features.gray_mean) * (j + 1 - features.grads_mean) * vecPGGCM[i][j];
        }
    }
    features.corelation = features.corelation / (features.gray_variance * features.grads_variance);
}

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END