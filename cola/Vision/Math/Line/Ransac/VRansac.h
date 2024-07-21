/**
 * @FilePath     : /cola/cola/Vision/Math/Line/Ransac/VRansac.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-21 10:25:58
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-21 10:25:58
 **/
#ifndef NAONAO_VRANSAC_H
#define NAONAO_VRANSAC_H

/*
https://blog.csdn.net/xrwang/article/details/6232327
https://blog.csdn.net/u010551600/article/details/79013453?utm_medium=distribute.pc_relevant.none-task-blog-baidujs_title-0&spm=1001.2101.3001.4242
https://blog.csdn.net/robinhjwy/article/details/79174914?utm_medium=distribute.pc_relevant.none-task-blog-2~default~BlogCommendFromMachineLearnPai2~default-2.control&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2~default~BlogCommendFromMachineLearnPai2~default-2.control
https://blog.csdn.net/luoshixian099/article/details/50217655
https://blog.csdn.net/pamxy/article/details/9206485
*/

#include "../../../VisionObject.h"
#include <Eigen\Core>
#include <Eigen\Dense>
#include <Eigen\Eigenvalues>
#include <Eigen\Geometry>

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

template<typename T>
class VRansac : public VisionObject
{
public:
    /*--------------------------------------------------------------------------------------------------
    * @FuncName:
    * @Author:    闹闹
    * @Brief:     brief
    * @Version:   1.0.0.1
    * @Date:	  2021/5/19 10:31
    * @InputParameter: param:返回的参数,,X数组，Y数组。orders,拟合曲线所用的阶数(二次曲线是2，直线是1)。error,错误阈值
    * @OutputParameter:
    * @Returns:
    * @Others:
    --------------------------------------------------------------------------------------------------*/
    static void compute(Eigen::VectorXd& param, std::vector<T> X, std::vector<T> Y, uint8_t orders, double error);

private:
    /*--------------------------------------------------------------------------------------------------
    * @FuncName:
    * @Author:    闹闹
    * @Brief:     计算所有的选项
    * @Version:   1.0.0.1
    * @Date:	  2021/6/18 15:30
    * @InputParameter:
    * @OutputParameter:
    * @Returns:
    * @Others:
    --------------------------------------------------------------------------------------------------*/
    static void computeAllChoices(Eigen::VectorXd& param, std::vector<T>& X, std::vector<T>& Y, uint8_t orders, short* bestVotes, short* curVotes, int& numVotesForBest, int startIndex, int n, int k,
                                  int arrIndex, int* arr, double error);
    /*--------------------------------------------------------------------------------------------------
    * @FuncName:
    * @Author:    闹闹
    * @Brief:     brief
    * @Version:   1.0.0.1
    * @Date:	  2021/6/18 15:32
    * @InputParameter:
    * @OutputParameter:
    * @Returns:
    * @Others:
    --------------------------------------------------------------------------------------------------*/
    static void estimate(Eigen::VectorXd& param, std::vector<T>& X, std::vector<T>& Y, uint8_t orders, short* bestVotes, short* curVotes, int& numVotesForBest, int* arr, double error);

    /*--------------------------------------------------------------------------------------------------
        --------------------------------------------------------------------------------------------------*/
    class SubSetIndexComparator
    {   // 子集序列比较器
    private:
        int length_;
    public:
        SubSetIndexComparator(int arrayLength)
            : length_(arrayLength)
        {
        }
        bool operator()(const int* arr1, const int* arr2) const
        {
            for (int i = 0; i < length_; i++) {
                if (arr1[i] < arr2[i]) {
                    return true;
                }
            }
            return false;
        }
    };
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#include "VRansac.inl"

#endif   // NAONAO_VRANSAC_H