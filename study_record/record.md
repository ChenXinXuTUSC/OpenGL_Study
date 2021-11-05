## This is a Repository that may records my OpenGL_study
* 2021/08/10:
	* 今天上网查各种资料搞了好久，终于把GLFW3库编译好能使用了，当然，前提是计算机里安装的有libopengl32.a，这个库我也不知道什么时候有的，居然还是在MinGW的安装目录下翻到的，难道这个不是VS2019负责的吗？
	* 记录一下这期间遇到的坑：
	* 本地下载了好多MinGW版本，有32，有64，还有一些其他版本，VScode不知道为什么，直接忽略了我的32位版本MinGW，但是网上OpenGL基于GLFW的教程，特别是CSDN的都是直接下载源码的，然而GLFW3预编译源码，64位问题大大滴，迫不得已只好下载32位，然而VScode的CMake插件死活就是检测不到mingw32-make，佛了。
	* 多次折腾系统环境变量无果，尝试编译源码方案，自己编译64位的库用吧
	* CMake-GUI只负责生成makefile啊我淦，我还找了半天教程的make按钮在GUI界面哪个位置，无语子，基础不牢，地动山摇
	* 生成静态库动态库，好，一股脑全部扔到VScode工作区间自己键的测试文件夹吧！
	* VScode启动！mingw32-make！undefined reference.....
	* ？？？？为什么啊？不过好在今天没有像昨天那样直接说库不匹配跳过了，那说明库应该是正确找到并链接了，但是为什么还是未定义？
	* StackOverflow上找了一圈，有网友有相同问题，他回答说，自己工作区间的lib文件夹下的库，要么就是静态库，要么是动态库，不要dll和lib混放，不然CMake直接无法识别。。。还有这问题？他们文件名都不一样的嘛？！
	* 好的，现在应该没问题了，经过一番细致的调整，把task.json和launch.json也设置了相应的64位gdb。
	* 大功告成！

    * 另外的一些记录：
    * github创建新仓库的时候选择不创建README文件会自动显示远程库链接教程，贴心了
    * VScode总是检测不到自己在工作区间建的include文件夹中的头文件，即使设置了cpp.properties也不行，后来发现在CMakeLists中显式地指明自己的include目录，居然就没有红色波浪线了，代码自动补全也可以识别了？是因为我这个工作区间和git关联的原因嘛？其他没有git管理的工作目录，没有这个问题。
    * CMake中如果有自己的lib目录，其中有自己的库需要连接，那么在target_link_libraries()之前，需要先使用link_directory告知CMake你的lib目录
    * learnOpenGL-CN 你好，窗口：
    * ![window](./images/屏幕截图%202021-08-10%20152431.png)
___
* 2021/08/31:
    * 今天了解了OpenGL大致的绘图流程，整个绘图之前的准备感觉很像软件工程设计模式中的工厂模式和单例模式。
    * VAO，VBO，EBO是指当前生成的对象的ID，通过对象ID来使用这些对象。
    * 记住，一个OpenGL程序中可以有多个VAO，VBO，EBO，每次要使用哪个对象，记得先将对应的内存绑定到对应的对象先。
    * 一个OpenGL程序绘图的时候使用单一的一个VAO对象，使用其中保存的VBO和EBO。
    * 三个关键概念，VAO，VBO，EBO
        * VAO：管理着VBO和EBO，VBO代指所有从OpenGL那里申请得来的内存。OpenGL的对象都是通过一个ID来映射的，通常是unsigned int，有了对象ID以后，可以使用这个ID对这个对象进行一些操作。当每次要绘图前，必须绑定一个指定的VAO，因为里面有渲染所必须的顶点坐标，绘制顺序，贴图坐标，法线坐标。VBO和EBO是如何绑定到VAO中的呢？当使用
        ```
        glBindVertexArray(VAO);
        ```
        之后，直到调用
        ```
        glBindVertexArray(0); //绑定到0就是解绑
        ```
        之前，所申请的VBO和EBO自动绑定到当前VAO中。
        * VBO：VBO是指顶点缓存对象，只要是vec3，或者vec4都可以算顶点，顶点不一定就是指顶点的坐标，还有很多其他信息。因为VBO一般绑定的缓存类型是通用数组缓存。
        ```
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        ```
        在申请了通用数组缓存之后，我们必须创建一个对象来解释通用数组缓存中的信息，我们需要知道哪几个字节是顶点坐标，哪几个字节是贴图数据？在创建了解释器指针并启用后，VBO是可以解绑的，因为解释器指针同时注册了VBO中的内容，现在他们都在VAO中了！
        * EBO：顶点索引对象，也是需要申请缓存，存入顶点坐标的顺序，通过在绘图模式指定是填充还是线段模式指定绘图模式。要知道，那么多顶点，当然不可以乱连线，否则你将什么都看不懂，必须有顺序的连线才能看出来那些顶点所组成的3D对象！
    * 看来，OpenGL有一个分配显卡缓存的工厂模式，OpenGL绘图的时候需要一套缓存的单实例，绘制的时候需要指定一个VAO！
    * 附一张图：
    * ![window](./images/屏幕截图%202021-08-31%20214230.png)
    * 另外记录一下VScode下使用CMake配置当前工作目录来解决cpp文件中检测不到包含目录而没有intellisense代码补全的问题，众所周知，打代码的时候如果没有代码补全，在现在这个时代那将会是非常痛苦的事情。然而！如果你使用了CMake插件配置当前工作目录，那么你必须要在根目录的CMakeLists.txt指定一下你的include和lib目录，并且要链接好你的main所在的cpp，只有被ADD_EXECUTABLE所链接生成的文件，#include才不会检测错误。。。佛了，不清楚这个是什么操作，应该是插件的锅
    ```
    CMAKE_MINIMUM_REQUIRED(VERSION 3.19)
    PROJECT(OpenGL_test)

    SET(CMAKE_BUILD_TYPE "Debug")
    SET(CMAKE_CXX_STANDARD 14)
    SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

    INCLUDE_DIRECTORIES(./include)
    INCLUDE_DIRECTORIES(./include/GLFW)
    INCLUDE_DIRECTORIES(./include/glad)
    INCLUDE_DIRECTORIES(./include/KHR)


    LINK_DIRECTORIES(./lib)

    ADD_SUBDIRECTORY(./source)

    ADD_EXECUTABLE(OpenGL_test myHello_Triangle.cpp)
    TARGET_LINK_LIBRARIES(OpenGL_test SUB_LIB glfw3 opengl32)
    ```
___
* 2021/09/02:
    * 今天自己抄一遍教程的代码，发现出来的结果，图形是黑色的，很奇怪，一直在检查代码哪里出了问题，一直在VScode自带的终端想看输出，因为教程中是写了很多错误输出信息的，然而是空的，后面我才想到，我设置了独立控制台输出，在launch.json文件里面。于是，终于找到了问题所在
    * ![window](./images/屏幕截图%202021-09-02%20165857.png)
    * 发现是片段着色器中，输出变量没有定义！奇怪，我都是照抄代码的怎么会没有定义？后来仔细一看
    ```
    #version 330 core\n
    "out vec4 FragColor;
    void main()
    {
        Fragcolor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    };
    ```
    * 天哪，FragColor和Fragcolor，有一个c居然有大小写不一样。。。。
___
* 2021/09/07:
    * 今天碰到的一些注意事项(坑)有：
        * 顶点着色器和片段着色器之间的输入输出通信一定记得看清楚格式是vec3还是vec4，需不需要转换
        * 渲染循环中，每次渲染前需要指定使用哪一个着色器glUseProgram(shaderProgramID)
    * 为什么指定三个顶点的颜色就可以在其他地方进行线性插值？(如图)
    * ![window](./images/屏幕截图%202021-09-07%20112516.png)
___
* 2021/09/09:
    * 今天学习纹理贴图遇到的一些坑：
        * 首先，以后复制粘贴必须看清楚函数的调用参数
        ```
        data = stbi_load(std::string("../images/Flower.jpg").c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        ```
        看清楚了！加载jpg的时候glTexImage2D绑定纹理对象时，使用RGB，就是说加载jpg与png时，绑定纹理这个函数的参数不同的地方在第三个参数，对于jpg这种没有透明通道的图像使用GL_RGB来解释字节，加载png时，使用GL_RGBA来解释
        * 今天将代码复制过来后，由于需要更改使用自己的图像，于是将源代码中对jpg和png的纹理绑定函数使用剪切粘贴进行了位置对调，然而对调之后没有发现，原来图像文件字节的解释也需要对应改变。
        * 如果不对glTexImage2D的第三个参数和第七个参数作对应变更，那么OpenGL会绘制白屏。
        ```
        data = stbi_load(std::string("../images/awesomeface.png").c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        ```
        * ![windows](./images/屏幕截图%202021-09-09%20002342.png)
___
* 2021/09/09:
    * 这可真是不得了，计算机是如何计算出这些深度缓存，片面贴图最后显示在屏幕上的呢，要我说，OpenGL能显示3D场景下的效果最大功劳应该是他的插值采样，这样，就算原本是正方形平面的贴图纹理，在插值采样之后映射到三角形片面上可以成为各种形状(类似于单应矩阵)，而不同比例形状的贴图正是我们日常生活中不同视角看物体的效果。
    * ![windows](./images/屏幕截图%202021-09-09%20154202.png)
    * 还有一个非常重要的地方就是三个变换矩阵，目前弄明白了模型变换矩阵，就是将模型自身的坐标变换到世界坐标系下，这样你就可以在你的世界摆放它们了。
    * 本章教程的视口变换不是很明晰，是一个非常简单的摄像机向后移动的效果，由于我们摄像机看向的是-z方向，因此，摄像机向后移动，看到的东西实际上就是向-z方向移动，因此viewing transform也很简单，这章教程中，我们假定摄像机无法被操控，只能朝着-z方向看，因此视口变换也就没有多复杂了。
    * 最复杂的是投影变换，因为我们要实现透视效果，必须使用体型梯形视口区域viewing box(超过这个视口范围的坐标都会被裁剪掉)，具体的数学原理还在研究，但原文是英文文章，不太好读，只能先硬啃了。另外记录一下(视口区域也叫平截头体Frustum)
    * http://www.songho.ca/opengl/gl_projectionmatrix.html
___
* 2021/09/10:
    * 今天终于干完了两个图形学作业，记录一下第一个和第二个作业中遇到的一些问题和坑
    * 首先是第一个作业的效果图：
        * ![windows](./images/屏幕截图%202021-09-10%20222511.png)
        * 画一个球体，第一个难点在于如何计算出它的顶点，本程序通过球的极坐标方程以及指定的分段数求出顶点的三个坐标(从极坐标转化过来)
        * 第二个难点在于，画出球体之后，由于现在还没有学习光照，看起来就是一个园，如何体现它的立体结构？
        * 请勿启用面剔除功能glEnable(GL_CULL_FACE)，这会使得默认情况下，你看到的面被剔除而只显示背面
        * 为了体现出立体结构，我们需要线框模式再画一遍线框，画线框时，在指定绘制模式时，请指定GL_FRONT_AND_BACK，这样画出来的线框才能在原基础上进行RGB覆盖，否则的看起来还是一块纯色
    * 其次是第二个作业的效果图：
        * ![windows](./images/屏幕截图%202021-09-10%20223102.png)
        * 第二个作业要求画分形图形，我在迭代函数中遇到了一个隐藏的逻辑问题，花了一个晚上才发现
        * 那就是变换矩阵的左乘顺序非常重要
        ```
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, p1 - glm::vec3(0.0f, 0.5f, 0.0f));
        model = glm::scale(model,
                           glm::vec3(1.0f / float(1 << (MAX_LEVEL - 1)),
                                     1.0f / float(1 << (MAX_LEVEL - 1)),
                                     1.0f / float(1 << (MAX_LEVEL - 1))));
        shader->setMat4("model", model);
        ```
        * 最坑的地方在于这个model模型变换到底是先产生缩放还是先产生位移。
        * 首先如果我们先产生位移的话，在用这个位移矩阵去附加一个缩放效果，那么这个位移也会被缩放，导致最后分形的三角形位移不够就重叠了，所以正确的做法是先缩放，然后把缩放的效果位移一个距离，这个距离就不会受缩放的影响。
        * 在变换矩阵的应用中，最晚生成的矩阵效果总是最先被应用，因为我们是矩阵左乘。
___
* 2021/09/12:
    * 今天发现CMake-gui在对源码文件进行makefile生成的时候的一些问题
    * 原本打算利用原来的压缩包再生成一遍，解压到D盘压缩包所在目录，然后使用CMake-gui进行makefile生成，竟然生成错误。
    * 后来CMake-gui提示src目录中的路径和我指定的路径不匹配，我又把这个文件夹剪切到桌面，点击configure还是报错，而且没有询问我是否生成静态库还是动态库的选项。configure一直报错说cmakelists中的源文件路径和我选择的不一样，这个路径一直指向我压缩包第一次解压缩的路径。
    * 最后选择将压缩包直接解压到桌面，开CMake-gui，这次CMake空空如也，什么缓存信息也没有显示，并且目标生成目录自动选择到了我用PowerShell打开的地方，就是我解压到桌面的glfw源文件文件夹路径。这次点击configure就弹出正常选项了，选择好生成套件是MinGW-Make，选择好不生成share动态库，最后点击generate。去到makefile所在的build目录，在命令行中执行mingw32-make，进行目标生成。最后在build的src文件夹中找到glfw3.a。
___
* 2021/09/14:
    * 今天学习的第一个示例是漫反射光照，漫反射光照的原理教程讲的很清楚，就是利用每个片段的法向量点乘该法向量到光源的方向向量，这两个向量均被单位化，这样得到的就是sin值，也反映了角度的大小，按理来说，如果角度越大，则漫反射反射的光越少。
    * 很坑的地方在于教程说由于示例中光源的位置是不变的，大家可以在主渲染循环之外提前设置好光源位置，主循环中就不用设置了，但实践证明，每一次使用着色器程序之前，里面的uniform都是未初始化的，必须手动找到unifrom全局变量的地址并赋值着色器才能正常工作。

___
* 2021/10/24:
    * 隔了好久没有更新这个学习日记了，一方面是其他科目比较繁忙，实验室也比较忙，另一方面是懒^_^
    * 今天学习到高级OpenGL的帧缓冲了，在这一小节中，终于了解到了各种图像后期处理的渲染器写法，包括机器学习中常用的卷积核也可以在着色器中实现，比如高斯模糊和锐化，色相反相这些。
    * 最重要的是理解，什么是帧缓冲对象，什么是帧缓冲的附件？
        * 帧缓冲，OpenGL的片段着色器进行写入修改的就是这块内存，显示器读取显示像素信息的也是这块内存，那么如果我们先把所有图像渲染到一个非默认的帧缓冲(一般来说framebuffer0是显示器默认读取的缓冲)，在那里做一些有趣的修改，然后将那个离屏的帧缓冲中的颜色，生成为一个纹理，再将这个纹理贴到屏幕窗口上，也是可以达到一样的显示效果的。
        * 自定义的帧缓冲，必须要满足4个条件：
            * 附加至少一个缓冲（颜色、深度或模板缓冲）。
            * 至少有一个颜色附件(Attachment)。
            * 所有的附件都必须是完整的（保留了内存）。
            * 每个缓冲都应该有相同的样本数。
    * 另外，通过单独设置这一块画布，对VAO，VBO，EBO的理解更深刻了，只要先申请VAO缓存并绑定到当前VAO，那么之后所有绑定VBO，EBO的语句，都会被这个VAO记录下来，在渲染的时候如果绑定这个VAO套件，那么相应的VBO和EBO也会被调用，这为我们单独开一个画布非常有用，而且与之前其他代码不会冲突。
    * 看看卷积核在着色器中如何实现：
        ```
        const float offset = 1.0 / 300.0;  

        void main()
        {
            vec2 offsets[9] = vec2[](
                vec2(-offset,  offset), // 左上
                vec2( 0.0f,    offset), // 正上
                vec2( offset,  offset), // 右上
                vec2(-offset,  0.0f),   // 左
                vec2( 0.0f,    0.0f),   // 中
                vec2( offset,  0.0f),   // 右
                vec2(-offset, -offset), // 左下
                vec2( 0.0f,   -offset), // 正下
                vec2( offset, -offset)  // 右下
            );

            float kernel[9] = float[](
                -1, -1, -1,
                -1,  9, -1,
                -1, -1, -1
            );

            vec3 sampleTex[9];
            for(int i = 0; i < 9; i++)
            {
                sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
            }
            vec3 col = vec3(0.0);
            for(int i = 0; i < 9; i++)
                col += sampleTex[i] * kernel[i];

            FragColor = vec4(col, 1.0);
        }
        ```
        * 这是锐化效果：
        ![windows](./images/屏幕截图%202021-10-24%20170012.png)
        * 这是模糊效果：
        ![windows](./images/屏幕截图%202021-10-24%20170410.png)
    * 是不是很炫酷？非常巧妙的设计！(当然，如果你没有一定的基础知识，这个也许比较难理解，不过日记本来就是写给自己加强印象的，233)
___
* 2021/10/25:
    * 今天补上了之前光照教程中没有实现的多光源问题，然后顺便看了冯氏着色和改进后的blinn-Phong着色，后者更加真实，因为不是直接用反射光与视线夹角点积计算镜面光强度的，这样，镜面光镜面因子比较小的时候，不会出现断层太明显的效果。
    * 遇到的坑：
        * 由于官方教程将环境光，漫反射，镜面光三个分量的计算重写到了函数中，所以重构了自己的着色器，检查了很多遍全局变量和局部变量的命名，毕竟glsl文件在VScode中的高亮支持不是很好。
        * 一开始写的'#define NR_POINT_LIGHTS = 4'; 这是个巨大的错误，正确写法应该是'#define NR_POINT_LIGHTS 4'，基本功不扎实了属于是，因为平时C++编程也很少用到宏定义。
        * 可以对官方教程的光源设置阶段进行优化，渲染循环中只需要设置点光源的位置即可，点光源的其他分量性质可以在循环外设置好，这样节省了很多指令，当然我也不知道编译器会不会优化这种逻辑性代码。
        * model/view/projection三个变换矩阵设置完之后，一定记得要在shader上调用设置函数！让OpenGL去着色器程序中更新变换！！！
    * ![window](./images/屏幕截图%202021-10-25%20222846.png)
___
* 2021/11/05:
    * 目前正在尝试加载完整的obj文件，包括obj文件指定的mtl材质文件，然后修改了一下着色器中的结构体的成分，使得其更符合直觉。另外发现了CMake为什么对于新加入的头文件无法识别，只要在当前的main.cpp文件中include该头文件让CMake在生成makefile文件时扫描到起始文件main.cpp中的include文件，即可识别，但是显式的在CMakeLists.txt中指定include路径确没什么效果？