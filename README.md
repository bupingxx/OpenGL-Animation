### OpenGL-Animation

___

#### 一、介绍

​	利用blender处理模型，使用C++ 加载模型和读取纹理，使用OpenGL + QT 创建窗口并绘制对象。



#### 二、特点

##### 1. 搭建场景

​	在blender中搭建场景，以森林为背景，绘制树林、草丛、地面、动物等模型，所有模型都使用对应的纹理。



##### 2. 加载模型

​	统一使用格式为`obj`的模型，使用C++ 读入，存储为顶点坐标、UV坐标和顶点法向量。



##### 3. 使用纹理

​	统一使用格式为`bmp`的贴图，BMP格式是一种无压缩的标准图像文件格式，优点是可以方便的读取像素信息，缺点是由于没有经过压缩，占用的磁盘空间比较大。



##### 4. 光照模型

​	`Blinn-Phong`光照模型 , 是对 `Phong `光照模型中镜面反射光的一种改进，他们的区别是，把`dot(V,R)`换成了`dot(N,H)`，其中`H`为半角向量，位于法线`N`和光线`L`的角平分线方向。`Blinn-Phong`模型的镜面反射光可表示为：

![image-1](https://github.com/bupingxx/OpenGL-Animation/blob/main/img/image-1.png)

​	其中`H = (L + V) / | L+V |`，计算`H`比`Phong`模型中计算反射向量`R`更快速。

![image-2](https://github.com/bupingxx/OpenGL-Animation/blob/main/img/image-2.png)



##### 5. 实时阴影

​	阴影是光线被阻挡的结果，当一个光源的光线由于其他的物体的阻挡不能够达到一个物体的表面的时候，那么这个物体此时就处于阴影中。这里选择了 shadow mapping （阴影贴图）实现阴影的绘制。

​	shadow mapping 的原理是从光源的角度出发，渲染获得此时场景对应的深度贴图， 这里储存的是各个像素到光源的距离。然后再从摄像机的视角出发，渲染每个像素，将每个像素的坐标从摄像机的view空间转成灯光的标准设置坐标。其x,y分量除以w就可以对shadow map进行采样了。如果此点到灯光的距离大于采样出来的值说明处于阴影当中。否则应该照亮，被照亮的部分此时就根据光照模型，显示对应的颜色。

![image-3](https://github.com/bupingxx/OpenGL-Animation/blob/main/img/image-3.png)



##### 6. 动画效果

​	基于矩阵的变换实现动画效果，主要使用平移和旋转两种变换方式。



##### 7. 优化渲染

​	使用GLSL语法，创建VBO（顶点缓冲区对象）来传递数据，在程序初始化时就加载所有的模型，然后创建并绑定VBO，一次性传给GPU。从性能上分析，传统的OpenGL每次绘制都是从CPU内存中获取顶点数据，然后传输给GPU，这样会频繁的调用CPU和GPU间的通信，浪费很多的CPU内存，降低效率；而使用VBO的方法可以一次性把缓冲区传给GPU，然后就可以清空存放在CPU上的内存，大大减少了开销。



#### 三、注意

##### 1. 环境

​	编程语言C++，使用OpenGL + QT绘制图像，VS中有相应的接口支持。



##### 2. 素材来源

​	所有模型+纹理均来自互联网，若有侵权请联系删除。



#### 四、运行效果

​	阴影效果：

![image-4](https://github.com/bupingxx/OpenGL-Animation/blob/main/img/image-4.png)

​	动画：

![image-5](https://github.com/bupingxx/OpenGL-Animation/blob/main/img/image-5.png)



