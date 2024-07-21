/*
 * @FilePath     : /cola/cola/Vision/Math/Line/Ransac/VRansac.inl
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-21 10:26:15
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-21 10:33:39
 */
#ifndef NAONAO_VRANSAC_INL
#define NAONAO_VRANSAC_INL

#include "../LeastSquares/VLineEstimator.h"
#include "VRansac.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <set>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <vector>

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

template<typename T>
void VRansac<T>::compute(Eigen::VectorXd& param, std::vector<T> X, std::vector<T> Y, uint8_t orders, double error)
{
    if (X.size() < 2 || Y.size() < 2 || X.size() != Y.size() || orders < 1 || X.size() < orders + 1)
        exit(EXIT_FAILURE);

    int numDataObjects  = static_cast<int>(X.size());
    int numVotesForBest = -1;   // 最佳投票数为-1
    int numForEstimate  = orders + 1;
    if (numDataObjects < numForEstimate)
        return;

    int*   arr       = new int[numForEstimate];     // 拟合的阶数，二次曲线为2，所需点数为3
    short* curVotes  = new short[numDataObjects];   // 如果data [i]与当前模型一致，则为1，否则为0
    short* bestVotes = new short[numDataObjects];   // 如果data [i]符合最佳模型，则为1，否则为0

    computeAllChoices(param, X, Y, numForEstimate, bestVotes, curVotes, numVotesForBest, 0, static_cast<int>(X.size()), numForEstimate, 0, arr, error);

    std::vector<T> textX;
    std::vector<T> textY;
    for (int j = 0; j < numDataObjects; j++) {
        if (bestVotes[j]) {
            // TO DO
            textX.push_back(X[j]);
            textY.push_back(Y[j]);
        }
    }
    VLineParamEstimator<T> test(error);
    param = test.LeastSquareMethod(textX, textY, numForEstimate - 1);
    delete[] arr;
    delete[] bestVotes;
    delete[] curVotes;
};

template<typename T>
void VRansac<T>::computeAllChoices(Eigen::VectorXd& param, std::vector<T>& X, std::vector<T>& Y, uint8_t numForEstimate, short* bestVotes, short* curVotes, int& numVotesForBest, int startIndex, int n,
                                   int k, int arrIndex, int* arr, double error)
{
    if (k == 0) {   // 我们有新的索引选择
        estimate(param, X, Y, numForEstimate, bestVotes, curVotes, numVotesForBest, arr, error);
        return;
    }
    int endIndex = n - k;

    for (int i = startIndex; i <= endIndex; i++) {
        arr[arrIndex] = i;
        std::cout << "进度：" << i << std::endl;
        computeAllChoices(param, X, Y, numForEstimate, bestVotes, curVotes, numVotesForBest, i + 1, n, k - 1, arrIndex + 1, arr, error);
    }
};

template<typename T>
void VRansac<T>::estimate(Eigen::VectorXd& param, std::vector<T>& X, std::vector<T>& Y, uint8_t numForEstimate, short* bestVotes, short* curVotes, int& numVotesForBest, int* arr, double error)
{
    std::vector<T> textX;
    std::vector<T> textY;
    int            numDataObjects = static_cast<int>(X.size());
    int            numVotesForCur = 0;   // 以-1初始化，以便将第一个计算设置为最佳
    memset(curVotes, '\0', numDataObjects * sizeof(short));
    for (int j = 0; j < numForEstimate; j++) {
        textX.push_back(X[arr[j]]);
        textY.push_back(Y[arr[j]]);
    }
    VLineParamEstimator<T> test(error);
    test.LeastSquareMethod(textX, textY, numForEstimate - 1);
    for (int j = 0; j < numDataObjects; j++) {
        if (test.agree(X[j], Y[j])) {
            curVotes[j] = 1;
            numVotesForCur++;
        }
    }
    if (numVotesForCur > numVotesForBest) {
        numVotesForBest = numVotesForCur;
        memcpy(bestVotes, curVotes, numDataObjects * sizeof(short));
    }
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif   // NAONAO_VRANSAC_INL