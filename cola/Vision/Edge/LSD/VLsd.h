/**
 * @FilePath     : /cola/cola/Vision/Edge/LSD/VLsd.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-15 18:46:09
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-15 18:57:25
 **/
#ifndef NAONAO_VLSD_H
#define NAONAO_VLSD_H

/*--------------------------------------------------------------------------------------------------
参考链接：
https://blog.csdn.net/tianwaifeimao/article/details/17678669?utm_medium=distribute.pc_relevant.none-task-blog-2~default~BlogCommendFromMachineLearnPai2~default-3.vipsorttest&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2~default~BlogCommendFromMachineLearnPai2~default-3.vipsorttest
https://blog.csdn.net/zhou4411781/article/details/98537761
https://blog.csdn.net/chishuideyu/article/details/78081643
https://blog.csdn.net/ivandark/article/details/29562201       //理论参考部分
--------------------------------------------------------------------------------------------------*/

#include <cfloat>
#include <climits>
#include <cmath>
#include <cstdlib>


#include "../../VisionObject.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

class VLsd : public VisionObject
{

public:
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
    double* lsd_scale_region(int* n_out, double* img, int X, int Y, double scale, int** reg_img, int* reg_x, int* reg_y);

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
    double* lsd_scale(int* n_out, double* img, int X, int Y, double scale);

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
    //返回为7个数的double的数组指针，个数为n_out个。 - x1,y1,x2,y2,width,p,-log10(NFA)
    //'out[7*n+0]' to 'out[7*n+6]'.
    --------------------------------------------------------------------------------------------------*/
    double* lsd(int* n_out, double* img, int X, int Y);
};
NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END
#endif