# Multi-Sensor Fusion
### 初衷
- 希望学习组合导航和VIO相关内容.
- 了解到组合导航方面开源代码较少,正好自己在学习相关内容,希望可以和有兴趣的小伙伴们一起学习讨论.

### 程序依赖
- glog 
- Eigen
- OpenCV 3.4
- Ceres

### 使用说明
最新版本对应为dev分支
程序使用了submodules形式挂载了tools,因此clone完本程序需要更新tools

```shell
git checkout -b dev origin/dev
git submodule init
git submodule update
```
已经安装完依赖库后,可以直接编译程序
```shell
mkdir build && cd build 
cmake .. && make -j3
```

### 功能
- 松组合功能完善
    - 发布版本链接 [release 1.0.0](https://github.com/2013fangwentao/Multi-Sensor-Combined-Navigation/releases) 使用时注意同时下载对应的tools程序[链接在此](https://github.com/2013fangwentao/tools/releases)
    - 对应分支为: [release/loose_couple_gnssins](https://github.com/2013fangwentao/Multi-Sensor-Combined-Navigation/tree/release/loose_couple_gnssins)
- 视觉前端特征点提取，匹配，外点剔除功能完成（基于OpenCV和ORB-SLAM2）
- msckf功能流程完善，需要测试调bug
- 目前程序还在持续开发,后续希望能做好GPS+IMU+CAMERA的定位功能

### 讨论交流
- QQ: 1280269817
- e-mail: fangwentaowhu@outlook.com   wtfang@whu.edu.cn
