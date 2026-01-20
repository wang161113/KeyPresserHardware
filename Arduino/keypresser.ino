#include <Keyboard.h>
#include <Mouse.h>

// 定义通信常量
const int BAUD_RATE = 9600;       // 串口波特率
const char START_CHAR = '<';      // 指令起始字符
const char END_CHAR = '>';        // 指令结束字符
const char SEPARATOR_CHAR = ',';  // 参数分隔符

// 定义指令类型
enum CommandType {
  PRESS_KEY,      // 按下按键
  RELEASE_KEY,    // 释放按键
  TYPE_STRING,    // 输入字符串
  PRESS_COMBINATION, // 按键组合
  DELAY,          // 延迟
  MOUSE_MOVE,     // 鼠标移动
  MOUSE_PRESS,    // 鼠标按下
  MOUSE_RELEASE,  // 鼠标释放
  MOUSE_CLICK,    // 鼠标点击
  MOUSE_WHEEL     // 鼠标滚轮
};

void setup() {
  Serial.begin(BAUD_RATE);  // 初始化串口通信
  Keyboard.begin();         // 初始化键盘模拟
  Mouse.begin();            // 初始化鼠标模拟
  pinMode(LED_BUILTIN, OUTPUT); // 初始化内置LED
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil(END_CHAR);

    // 验证指令格式
    if (command.startsWith(String(START_CHAR))) {
      // 移除起始字符
      command = command.substring(1);

      // 解析指令
      parseAndExecuteCommand(command);

      // 闪烁LED表示接收到有效指令
      blinkLED();
    }
  }
}

void parseAndExecuteCommand(String command) {
  // 查找指令类型
  int separatorIndex = command.indexOf(SEPARATOR_CHAR);
  if (separatorIndex == -1) return;

  String typeStr = command.substring(0, separatorIndex);
  CommandType cmdType = (CommandType)typeStr.toInt();
  String params = command.substring(separatorIndex + 1);

  switch (cmdType) {
    case PRESS_KEY:
      pressKey(params);
      break;
    case RELEASE_KEY:
      releaseKey(params);
      break;
    case TYPE_STRING:
      typeString(params);
      break;
    case PRESS_COMBINATION:
      pressKeyCombination(params);
      break;
    case DELAY:
      delayCommand(params);
      break;
    case MOUSE_MOVE:
      mouseMove(params);
      break;
    case MOUSE_PRESS:
      mousePress(params);
      break;
    case MOUSE_RELEASE:
      mouseRelease(params);
      break;
    case MOUSE_CLICK:
      mouseClick(params);
      break;
    case MOUSE_WHEEL:
      mouseWheel(params);
      break;
  }
}

void pressKey(String keyParam) {
  if (keyParam.length() == 1) {
    // 普通字符
    char key = keyParam.charAt(0);
    Keyboard.press(key);
  } else {
    // 特殊按键（使用虚拟按键码）
    int vkCode = keyParam.toInt();
    Keyboard.press((byte)vkCode);
  }
}

void releaseKey(String keyParam) {
  if (keyParam.length() == 1) {
    // 普通字符
    char key = keyParam.charAt(0);
    Keyboard.release(key);
  } else {
    // 特殊按键（使用虚拟按键码）
    int vkCode = keyParam.toInt();
    Keyboard.release((byte)vkCode);
  }
}

void typeString(String str) {
  Keyboard.print(str);
}

void pressKeyCombination(String keysParam) {
  // 解析按键组合，格式："key1,key2,key3"
  int startIndex = 0;

  // 按下所有按键
  while (startIndex < keysParam.length()) {
    int commaIndex = keysParam.indexOf(SEPARATOR_CHAR, startIndex);
    if (commaIndex == -1) commaIndex = keysParam.length();

    String key = keysParam.substring(startIndex, commaIndex);
    pressKey(key);

    startIndex = commaIndex + 1;
  }

  // 短暂延迟
  delay(100);

  // 释放所有按键
  startIndex = 0;
  while (startIndex < keysParam.length()) {
    int commaIndex = keysParam.indexOf(SEPARATOR_CHAR, startIndex);
    if (commaIndex == -1) commaIndex = keysParam.length();

    String key = keysParam.substring(startIndex, commaIndex);
    releaseKey(key);

    startIndex = commaIndex + 1;
  }
}

void delayCommand(String delayMsParam) {
  int delayMs = delayMsParam.toInt();
  if (delayMs > 0) {
    delay(delayMs);
  }
}

void blinkLED() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
}

void mouseMove(String params) {
  // 参数格式："dx,dy" 表示相对移动距离
  // 注意：Arduino的Mouse.move()函数只支持相对移动，
  // 即从当前位置向x和y方向移动指定的像素数
  // 重要：Mouse.move()的参数为char类型，范围为-128到127
  int commaIndex = params.indexOf(SEPARATOR_CHAR);
  if (commaIndex == -1) return;

  int dx = params.substring(0, commaIndex).toInt();
  int dy = params.substring(commaIndex + 1).toInt();

  Mouse.move(dx, dy);
}

void mousePress(String params) {
  // 参数格式："button" 表示鼠标按键 (1=左键, 2=中键, 4=右键)
  int button = params.toInt();
  Mouse.press(button);
}

void mouseRelease(String params) {
  // 参数格式："button" 表示鼠标按键 (1=左键, 2=中键, 4=右键)
  int button = params.toInt();
  Mouse.release(button);
}

void mouseClick(String params) {
  // 参数格式："button,clickCount" 表示鼠标按键和点击次数
  int commaIndex = params.indexOf(SEPARATOR_CHAR);
  if (commaIndex == -1) {
    // 默认点击一次
    int button = params.toInt();
    Mouse.click(button);
  } else {
    int button = params.substring(0, commaIndex).toInt();
    int clickCount = params.substring(commaIndex + 1).toInt();

    for (int i = 0; i < clickCount; i++) {
      Mouse.click(button);
      delay(50); // 点击间隔
    }
  }
}

void mouseWheel(String params) {
  // 参数格式："delta" 表示滚轮滚动量
  // 重要：Mouse.move()的滚轮参数为char类型，范围为-128到127
  int delta = params.toInt();
  Mouse.move(0, 0, delta);
}

void mouseMoveAbsolute(String params) {
  // 参数格式："x,y" 表示屏幕绝对位置
  //
  // 重要说明：
  // Arduino的Mouse.move()函数**不支持直接移动到绝对位置**，
  // 它只支持相对移动（即从当前位置向某个方向移动指定像素数）。
  //
  // 此函数通过以下技巧模拟绝对位置移动：
  // 1. 先将鼠标移动到屏幕左上角（通过大距离负向移动）
  // 2. 然后从原点开始进行相对移动到目标位置
  //
  // 局限性：
  // - 精度有限，依赖于操作系统的鼠标边界处理
  // - 不同操作系统和屏幕分辨率下效果可能不同
  // - 不适合需要高精度定位的场景

  int commaIndex = params.indexOf(SEPARATOR_CHAR);
  if (commaIndex == -1) return;

  int targetX = params.substring(0, commaIndex).toInt();
  int targetY = params.substring(commaIndex + 1).toInt();

  // 1. 移动到屏幕左上角（原点）
  // 使用char类型的最小值确保鼠标到达屏幕边界
  // 注意：Arduino的Mouse.move()函数参数为char类型（范围-128到127）
  for(int i = 0;  i < 50; i++)
  {
    Mouse.move(-127, -127);
  }
  delay(100); // 等待移动完成

  // 2. 从原点移动到目标位置
  // 分多次小步移动以提高精度
  const int MAX_STEP = 100; // 每次移动的最大像素数

  // 移动X轴
  int remainingX = targetX;
  while (remainingX > 0) {
    int step = min(remainingX, MAX_STEP);
    Mouse.move(step, 0);
    remainingX -= step;
    delay(5); // 短暂延迟确保移动生效
  }

  // 移动Y轴
  int remainingY = targetY;
  while (remainingY > 0) {
    int step = min(remainingY, MAX_STEP);
    Mouse.move(0, step);
    remainingY -= step;
    delay(5); // 短暂延迟确保移动生效
  }
}
