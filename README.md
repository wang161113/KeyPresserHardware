# KeyPresser Hardware

KeyPresser Hardware 是一款功能强大的自动化操作工具，通过 Arduino 硬件设备实现键盘模拟、鼠标操作等自动化功能。该项目由两部分组成：客户端应用程序和 Arduino 固件。

![KeyPresser Hardware](C:\Users\X\AppData\Roaming\Typora\typora-user-images\image-20260120155941809.png)

## 项目组成

### 1. 客户端应用程序
客户端是使用 Qt 框架开发的桌面应用程序，提供用户界面和控制逻辑，用于配置和控制 Arduino 设备执行自动化操作。

### 2. Arduino 固件
Arduino 固件需要烧录到 Arduino Leonardo 开发板中，负责接收来自客户端的命令并执行实际的键盘和鼠标操作。

## 功能特点

### 核心功能
- **Arduino 设备自动检测**：自动识别并连接 Arduino Leonardo 设备
- **键盘模拟**：支持单个按键、组合键、字符串输入
- **定时任务**：设置定时开始和结束的自动化任务
- **窗口选择**：可以指定目标窗口进行操作
- **顺序执行**：支持按顺序执行多个按键操作
- **随机间隔**：可设置按键间隔的最小和最大值，模拟更自然的操作

### 高级功能
- **独立/顺序模式**：支持独立按键操作或按顺序执行
- **置顶窗口**：保持应用窗口在最上层
- **设置保存**：保存和加载配置文件

## 系统要求

- **操作系统**：Windows 10/11
- **硬件需求**：Arduino Leonardo 开发板
- **软件依赖**：
  - Qt 5.15.2 或更高版本
  - Arduino CLI（用于固件上传）
  - Visual Studio 2019 或更高版本（用于编译）

## 安装方法

### 1. 编译项目
使用 Qt Creator 编译项目生成可执行文件。

### 2. 连接 Arduino 设备
将 Arduino Leonardo 开发板通过 USB 连接到电脑。

Leonardo开发板购买链接：

> 【淘宝】https://e.tb.cn/h.72kLyFd6p2Ci12Y?tk=9UMIfuG2wjP MF278 「Keypresser搭档 Arduino Leonardo R3单片机开发板ATMEGA32U4版本」
>
> 点击链接直接打开 或者 淘宝搜索直接打开

### 3. 安装驱动
如果是第一次使用 Arduino 设备，Windows 会自动安装驱动程序。

### 4. 启动应用程序
运行编译生成的 `KeyPresserHardware.exe` 启动应用程序。

## Arduino固件烧录(首次运行时，需要执行此步骤)

### 准备工作

1. **安装Arduino IDE**：从[Arduino官网](https://www.arduino.cc/en/software)下载并安装最新版本的Arduino IDE
2. **准备Arduino Leonardo开发板**：确保开发板处于良好状态

### 烧录步骤

1. **启动Arduino IDE**
2. **打开固件文件**：在Arduino IDE中打开[./arduino/keypresser.ino](./arduino/keypresser.ino)固件文件
3. **选择开发板类型**：
   - 点击「工具」→「开发板」→「Arduino AVR Boards」→「Arduino Leonardo」
4. **选择端口**：
   - 点击「工具」→「端口」→ 选择连接Arduino的COM端口
5. **编译固件**：
   - 点击左上角的「验证」按钮（✓图标）编译固件
   - 确保编译成功，没有错误提示
6. **烧录固件**：
   - 点击左上角的「上传」按钮（→图标）将固件烧录到Arduino板
   - 等待烧录完成，IDE底部状态栏会显示「上传成功」

## 使用教程

### 1. 设备连接

1. 启动应用程序后，系统会自动检测 Arduino 设备
2. 如未自动检测到，请检查设备连接和驱动安装

### 2. 窗口选择

1. 点击「选择窗口」按钮
2. 鼠标移动到目标窗口并点击
3. 应用程序将自动附着到选中的窗口

### 3. 设置按键操作

#### 单个按键设置
1. 勾选需要使用的按键复选框
2. 选择按键类型（普通键、组合键等）
3. 设置按键间隔时间
4. 可选：设置最大间隔时间以实现随机间隔

#### 组合键设置
1. 在快捷键下拉框中选择组合键类型（如 Ctrl、Shift、Alt）
2. 在按键下拉框中选择具体按键

### 4. 设置定时任务

1. 勾选「定时任务」复选框
2. 设置开始时间和结束时间
3. 应用程序将在指定时间范围内自动执行操作

### 5. 开始执行

1. 点击「开始」按钮启动自动化操作
2. 点击「停止」按钮停止操作

### 6. 保存和加载配置

1. 点击「保存配置」按钮保存当前设置
2. 点击「加载配置」按钮加载已保存的设置

## 开发说明

### 项目结构

```
KeyPresserHardware/
├── Arduino/                 # Arduino 固件文件夹
│   └── keypresser.ino      # Arduino 固件源代码
├── png/                     # 图片资源文件夹
├── ArduinoController.hpp    # Arduino 控制器类
├── KeyPresserHardware.pro   # Qt 项目文件
├── KeyPresser_resource.rc   # 资源文件
├── aboutmedlg.cpp           # 关于对话框实现
├── aboutmedlg.h             # 关于对话框头文件
├── aboutmedlg.ui            # 关于对话框界面
├── keypresser.ico           # 应用图标
├── keypresserHardware.cpp   # 主应用程序实现
├── keypresserHardware.h     # 主应用程序头文件
├── main.cpp                 # 程序入口
├── README.md                # 项目说明文档
├── res.qrc                  # Qt 资源文件
├── style.qss                # 样式表文件
└── vx.jpg                   # 图片资源
```

### 构建项目

1. 使用 Qt Creator 打开 KeyPresserHardware.pro
2. 选择合适的构建配置（Debug/Release）
3. 点击「构建」按钮编译项目

## 常见问题

### Q: 无法检测到 Arduino 设备
A: 请检查：
1. USB 连接是否正常
2. 驱动程序是否正确安装
3. 设备是否为 Arduino Leonardo 型号

### Q: 按键操作没有反应
A: 请检查：
1. Arduino 设备是否连接成功
2. 目标窗口是否正确选择
3. 按键设置是否正确

### Q: 定时任务不执行
A: 请检查：
1. 定时任务是否已启用
2. 开始时间和结束时间设置是否正确
3. 系统时间是否准确

## 许可证

本项目的许可证信息请参考项目根目录下的相关文件。

## 联系方式

如有问题或建议，请联系项目开发者。

## 赞赏作者

![image-20260120160450883](C:\Users\X\AppData\Roaming\Typora\typora-user-images\image-20260120160450883.png)