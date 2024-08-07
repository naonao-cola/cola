/**
 * @FilePath     : /cola/test/02/test_fit.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-21 10:05:45
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-21 22:27:30
 **/
#include "../Common/common.h"

using namespace nao::vision;

void test_lineestimator()
{
    std::vector<double> dataX, dataY;
    int                 numForEstimate                  = 2;
    int                 numSamples                      = 200;   // 样本点
    int                 numOutliers                     = 60;    // 噪点
    double              desiredProbabilityForNoOutliers = 0.999;
    double              maximalOutlierPercentage        = 0.1 + static_cast<double>(numOutliers) / static_cast<double>(numSamples + numOutliers);
    double              noiseSpreadRadius               = 0.4;   // 噪声传播半径
    double              outlierSpreadRadius             = 10;    // 离群点传播半径
    int                 i;
    double              newX;
    double              newY;
    double              dx;
    double              dy;
    double              norm;

    // 1.与异常值一起创建数据
    // 随机选择一个方向[dx，dy]并为在该线上采样的每个点创建一条穿过原点的线，这会添加随机噪声，最后在线法线方向上添加偏外点。
    srand((unsigned)time(NULL));   // 种子随机数生成器
    // 得到随机方向
    dx   = rand();
    dy   = rand();
    norm = sqrt(dx * dx + dy * dy);
    dx /= norm;
    dy /= norm;
    dx *= (rand() > RAND_MAX / 2 ? 1 : -1);
    dy *= (rand() > RAND_MAX / 2 ? 1 : -1);
    // 添加'numSamples'点
    for (i = 0; i < numSamples; i++) {
        newX = i * dx + noiseSpreadRadius * (double)rand() / (double)RAND_MAX * (rand() > RAND_MAX / 2 ? 1 : -1);
        newY = 3 * newX * newX + 4 * newX + 5 + noiseSpreadRadius * static_cast<double>(rand()) / (double)RAND_MAX * (rand() > RAND_MAX / 2 ? 1 : -1);
        dataX.push_back(newX);
        dataY.push_back(newY);
    }

    std::cout << "总的点数(包含噪点): " << dataX.size() << std::endl;
    std::cout << "噪点数: " << numOutliers << std::endl;

    VLineParamEstimator<double> line(5);
    Eigen::VectorXd             ret = line.LeastSquareMethod(dataX, dataY, 2);
    for (size_t i = 0; i < ret.size(); ++i) {
        std::cout << "VLineParamEstimator_theta_" << i << ": " << ret[i] << std::endl;
    }

    // 非常耗时
    /*Eigen::VectorXd param;
    VRansac<double>::compute(param, dataX, dataY, 2, 10);
    for (size_t i = 0; i < param.size(); ++i) {
        std::cout << "VRansac_theta_" << i << ": " << param[i] << std::endl;
    }*/

    VFit<double> fit;
    fit.polyfit(dataX, dataY, 2);
    std::vector<double> factor;
    fit.getFactor(factor);
    for (int i = 0; i < factor.size(); i++) {
        std::cout << "VFit_theta_" << i << ": " << factor[i] << std::endl;
    }
}

void test_spline()
{
    std::vector<double> X, Y;
    X.push_back(1);
    X.push_back(5);
    X.push_back(15);
    X.push_back(20);
    X.push_back(30);
    Y.push_back(2);
    Y.push_back(10);
    Y.push_back(20);
    Y.push_back(22);
    Y.push_back(30);
    VSpline s;
    s.set_points(X, Y);

    for (int i = 0; i < 30; i++) {
        std::cout << i << " " << s(i) << std::endl;
    }
}
int main()
{
    printf("----------------- lineestimator -----------------\n");
    test_lineestimator();
    printf("----------------- spline -----------------\n");
    test_spline();
    return 0;
}