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
    * ![window](屏幕截图%202021-08-10%20152431.png)
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
    * ![window](屏幕截图%202021-08-31%20214230.png)
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
* 2021/09/02:
    * 今天自己抄一遍教程的代码，发现出来的结果，图形是黑色的，很奇怪，一直在检查代码哪里出了问题，一直在VScode自带的终端想看输出，因为教程中是写了很多错误输出信息的，然而是空的，后面我才想到，我设置了独立控制台输出，在launch.json文件里面。于是，终于找到了问题所在
    * ![window](屏幕截图%202021-09-02%20165857.png)
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