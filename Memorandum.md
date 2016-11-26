* 内存还有泄漏
* 判断是否需要重新计算MCS的条件没有改
* 需要修改matlab画图文件的顺序
* 将各个模块基类的成员设为私有，添加其他模块的友元
* RRM_RDM_DRA中groupSizeBasedTMD(bool)函数中，有/ATTENTION注释的部分，其中s_NTTI在Linux下链接错误，不知道为啥
