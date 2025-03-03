MICO
====

Mico-controller based Internet Connectivity Operation System


### 特性
* 为嵌入式设备设计的物联网开发软件平台
* 基于实时操作系统设计
* 支持各种微控制器平台
* 无线网络接入方案：Wi-Fi，蓝牙
* 内置云服务接入中间件和各种示例代码
* 优秀的低功耗控制功能
* 为物联网产品量产设计的应用程序框架
* 为应用开发提供的大量编程工具

### 目录结构
* demos: 基于MiCO开发的各种示例程序
* include: MiCO核心功能接口
* MiCO: MiCO核心库,二进制文件和相关系统代码
* libraries: MiCO软件中间件，应用支持代码等
* platform: 基于不同开发环境，硬件平台的特性文件
* board: 板级定制代码
* Projects: 基于IAR开发环境的的工程文件
* bootloader：系统引导程序源代码
* makefiles：micoder编译工具链文件

### 使用前准备
1. 首先您需要拥有一个MiCOKit开发套件，具体信息请参考：[MiCOKit开发板简介](http://developer.mico.io/docs/34)；
2. 安装MiCoder IDE集成开发环境，下载地址：[MiCoder IDE](http://developer.mico.io/downloads/2)；
3. 准备一个Jlink下载调试工具(针对ST开发板，可使用Stlink)，并在PC上安装Jlink驱动软件；
4. 连接Jlink工具到PC端，并更新驱动程序，具体方法参考：[MiCO SDK 使用](http://developer.mico.io/docs/10)页面中步骤 1；
5. 使用USB线连接PC和MiCOKit，实现一个虚拟串口用于供电和输出调试信息, 驱动下载地址：[VCP driver](http://www.ftdichip.com/Drivers/VCP.htm)；
6. 打开MiCoder IDE，导入当前MiCO SDK，导入方法请参考：[MiCoder IDE使用](http://developer.mico.io/docs/13)页面中的“导入其它版本SDK”；
7. 在MiCoder IDE中make target命令框中，输入make命令，具体make命令格式请参考下节：wifi_uart串口透传实例使用。

### 使用示例：“wifi_uart” 串口透传
1. 打开MiCoder IDE,导入最新MiCO SDK，具体请参考上节第6条；
2. 打开make target命令窗口，输入make命令，编译与下载对应的应用程序；
3. make命令格式：make [total] [download] [run | debug] [JTAG=xxx] [VERBOSE=1] (Windows下为：make，Linux和Mac OS为：./make)
4. 以MiCOKit-3165为例：

    仅编译应用程序，make命令： application.wifi_uart@MK3165
    
    编译bootloader和应用程序，make命令：application.wifi_uart@MK3165 total
    
    编译与下载bootloader和应用程序，make命令：application.wifi_uart@MK3165 total download
    
    编译与下载bootloader和应用程序，并重启运行，make命令：application.wifi_uart@MK3165 total download run
    
5. 待编译下载完成，设备重启后，即可观察 MiCOKit 开发板状态是否发生变化，即配网指示灯闪烁;
6. 下载后，方可进入MiCoder IDE在线调试功能界面，点击界面图标 ![bug虫子](http://developer.mico.io/fileDownload/120) 下拉列表中 “MiCO” 进入在线调试参数设置界面
7. 更详细MiCoder调试信息，请参考：[MiCoder IDE使用](http://developer.mico.io/docs/13)页面中的第5条“使用仿真器进行调试”;
8. 更详细关于wifi_uart应用程序使用方法，请参考：[基于 wifi_uart 的 Easylink 使用说明](http://developer.mico.io/downloads/8).




Notice: Internal use only


MICO
====
Mico-controller based Internet Connectivity Operation System


### Features
* Software development platform designed for embedded devices
* Based on a real time operation system
* Support abundant MCUs
* Wi-Fi, bluetooth connectivity total solution
* Build-in protocols for cloud service
* State-of-the-art low power management
* Application framework for I.O.T product
* Rich tools and mobile APP to accelerate development

### Contents:
* Demos: Demos applications and application framework.
* include: MiCO core APIs.
* mico: MiCO core libraries, binary file and system codes.
* libraries: MiCO middleware, support functions.
* platform: codes based in different IDE and hardware.
* board: BSP file, Hardware resources and configurations on different boards.
* Projects: IAR workbench projects.
* libraries: Open source software libraries.
* bootloader：System boot code.
* makefiles：MiCoder compile toolchain files.

### Preparations:
1. You should have a development board [MiCOKit-xxx](http://developer.mico.io/docs/34)
2. Install MiCoder IDE and download address: [MiCoder IDE] (http://developer.mico.io/downloads/2);
3. Prepare a Jlink tool (for ST develpment board，use STlink)，and install its driver；
4. Connect Jlink to PC, and update its driver following step 1 in http://developer.mico.io/docs/10；
5. Connect USB cable to PC to have a virtual serial port, install [VCP driver](http://www.ftdichip.com/Drivers/VCP.htm)；
6. Open MiCoder IDE and import the current MiCO SDK, please refer to: [MiCoder IDE use] (http://developer.mico.io/docs/13) page"; 
7. Open the“make target” window in MiCoder IDE，and input make command，specific format please refer to the following section: wifi_uart example use.

### Example "wifi_uart" serial transmission  usage
1. Open MiCoder IDE and import the latest MiCO SDK; specific method refer to the 6 step of“Preparation;
2. Open the make target command window, and edit the make command, compile and download the corresponding application;
3. Format of command： make [total] [download] [run command format: | debug] [JTAG=xxx] [VERBOSE=1] (Windows: make, Linux and Mac OS:./make);
4. Take MiCOKit-3165 as example, in the MiCoder IDE target command window input:

    1) Compile applications only, make command:    application.wifi_uart@MK3165;
    
    2) Compile bootloader and applications, make command:    application.wifi_uart@MK3165 total
    
    3) Compile and download bootloader and applications, make command:   application.wifi_uart@MK3165 total download
    
    4) Compile and download bootloader and applications, and restart run, make command:   application.wifi_uart@MK3165 total download run;
    
5. After compile，download, and restart, you can observe the status of MiCOKit development board is changed, that is, the distribution network flashing lights;
6. After download, you can enter the MiCoder IDE online debugging interface, click the icon ![bug bugs] (http://developer.mico.io/fileDownload/120) and drop-down list 
"MiCO" to enter debugging interface;
7. More detail about MiCoder IDE online debugging information, please refer to: [MiCoder IDE use] (http://developer.mico.io/docs/13) page, the step 5 about using the 
debugger for debugging".
8. More detail about the wifi_uart application usage, please refer to: [Usage of Easylink based on wifi_uart ] (http://developer.mico.io/downloads/8).