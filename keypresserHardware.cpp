#include "KeyPresserHardware.h"
#include "aboutmedlg.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>
#include <QIntValidator>
#include <QKeyEvent>
#include <QApplication>
#include <QSettings>
#include <QFileInfo>
#include <QDateTime>
#include <QRandomGenerator>
#include <QDesktopServices>
#include <QProcess>
#include <QUrl>
#include <QToolButton>
#include <QDebug>
#include <qlogging.h>
#include <QInputDialog>


KeyPresserHardware *KeyPresserHardware::instance = nullptr;



KeyPresserHardware::KeyPresserHardware(QWidget *parent) : QWidget(parent)
{
    setWindowTitle(QStringLiteral("KeyPresser硬件版"));
    setWindowIcon(QIcon(":/png/hardware.png"));
    instance = this;
    resize(330, 400);

    // 设置窗口的大小策略：宽度可扩展，高度自适应最小值
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    QVBoxLayout *globalLayout = new QVBoxLayout(this);
    globalLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(9, 0, 9, 9);

    QToolButton *clearButton = new QToolButton(this);
    clearButton->setIcon(QIcon(":/png/reset.png"));
    clearButton->setText(QStringLiteral("重置"));
    clearButton->setToolTip(QStringLiteral("重置设置"));
    clearButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    // 添加导入按钮
    QToolButton *importButton = new QToolButton(this);
    importButton->setIcon(QIcon(":/png/import.png"));
    importButton->setText(QStringLiteral("导入"));
    importButton->setToolTip(QStringLiteral("从文件导入设置"));
    importButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    // 添加导出按钮
    QToolButton *exportButton = new QToolButton(this);
    exportButton->setIcon(QIcon(":/png/export.png"));
    exportButton->setText(QStringLiteral("导出"));
    exportButton->setToolTip(QStringLiteral("导出设置到文件"));
    exportButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    //定时任务
    /*
    QToolButton *timingButton = new QToolButton(this);
    timingButton->setIcon(QIcon(":/png/timing.png"));
    timingButton->setText(QStringLiteral("定时"));
    timingButton->setToolTip(QStringLiteral("添加定时任务"));
    timingButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    */

    QToolButton *aboutButton = new QToolButton(this);
    aboutButton->setIcon(QIcon(":/png/about.png"));
    aboutButton->setText(QStringLiteral("关于"));
    aboutButton->setToolTip(QStringLiteral("关于"));
    aboutButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    QToolButton *helpButton = new QToolButton(this);
    helpButton->setIcon(QIcon(":/png/help.png"));
    helpButton->setText(QStringLiteral("帮助"));
    helpButton->setToolTip(QStringLiteral("帮助"));
    helpButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(helpButton, &QPushButton::clicked, this, []() {
        QDesktopServices::openUrl(QUrl("https://ocn1lplwg6gi.feishu.cn/wiki/Ual4wc4SxiKIodkuSSUcpLBTnsc?from=from_copylink"));
    });

    QToolButton *openMouseButton = new QToolButton(this);
    openMouseButton->setIcon(QIcon(":/png/mouse.png"));
    openMouseButton->setText(QStringLiteral("鼠标"));
    openMouseButton->setToolTip(QStringLiteral("打开鼠标助手"));
    openMouseButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    openMouseButton->hide();

    QToolButton *recordingButton = new QToolButton(this);
    recordingButton->setIcon(QIcon(":/png/recording.png"));
    recordingButton->setText(QStringLiteral("录制"));
    recordingButton->setToolTip(QStringLiteral("类似按键精灵的鼠标键盘录制和自动化操作 模拟点击和键入 | automate mouse clicks and keyboard input"));
    recordingButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    recordingButton->hide();

    QFrame *toolButtonFrame = new QFrame();
    toolButtonFrame->setFixedHeight(30);
    toolButtonFrame->setStyleSheet("QFrame { border-bottom: 1px solid #cccccc; border-radius: 0px; padding-bottom: 0px; background-color: #dadcde; }");
    QHBoxLayout *toolButtonLayout = new QHBoxLayout(toolButtonFrame);
    toolButtonLayout->setSpacing(0);
    toolButtonLayout->setContentsMargins(2, 1, 0, 0);
    globalLayout->addWidget(toolButtonFrame);

    // 在布局部分修改
    toolButtonLayout->addWidget(clearButton);
    toolButtonLayout->addWidget(importButton);    // 添加导入按钮
    toolButtonLayout->addWidget(exportButton);    // 添加导出按钮
    //toolButtonLayout->addWidget(timingButton);
    toolButtonLayout->addWidget(aboutButton);
    toolButtonLayout->addWidget(helpButton);
    toolButtonLayout->addWidget(openMouseButton);
    toolButtonLayout->addWidget(recordingButton);
    toolButtonLayout->addStretch();
    layout->addLayout(toolButtonLayout);


    // 添加按钮事件处理
    connect(importButton, &QPushButton::clicked, this, [this]() {
        QString filename = QFileDialog::getOpenFileName(
            this,
            QStringLiteral("导入设置"),
            QDir::currentPath(),
            QStringLiteral("KeyPresserHardware设置文件 (*.kphset)"));
            
        if (!filename.isEmpty()) {
            loadSettingsFromFile(filename);
            QMessageBox::information(this, QStringLiteral("成功"), 
                QStringLiteral("设置已成功导入！"));
        }
    });

    connect(exportButton, &QPushButton::clicked, this, [this]() {
        QString defaultFileName = QString("KeyPresserHardware_%1.kpset")
                .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
            QString filename = QFileDialog::getSaveFileName(
                this,
                QStringLiteral("导出设置"),
                QDir::currentPath() + "/" + defaultFileName,
                QStringLiteral("KeyPresserHardware设置文件 (*.kphset)"));
            
        if (!filename.isEmpty()) {
            if(!filename.endsWith(".kpset", Qt::CaseInsensitive)) {
                filename += ".kphset";
            }
            saveSettingsToFile(filename);
            QMessageBox::information(this, QStringLiteral("成功"), 
                QStringLiteral("设置已成功导出！"));
        }
    });

    connect(clearButton, &QPushButton::clicked, this, &KeyPresserHardware::clearSettings);

    //QPushButton *uploadSketchBtn = new QPushButton(QStringLiteral("烧录程序（首次运行时先点击此按钮）"));
    //layout->addWidget(uploadSketchBtn);
    arduinoLabel = new QLabel(QStringLiteral("未连接"), this);
    layout->addWidget(arduinoLabel);
    QLabel *label = new QLabel(QStringLiteral("选择窗口:"), this);
    layout->addWidget(label);

    selectedWindowLabel = new QLabel(QStringLiteral("未选择窗口"), this);
    selectedWindowLabel->setStyleSheet("color: green;");
    selectedWindowLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout->addWidget(selectedWindowLabel, 0, Qt::AlignLeft);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *selectButton = new QPushButton(QStringLiteral("选择窗口"), this);
    buttonLayout->addWidget(selectButton);

    QPushButton *highlightButton = new QPushButton(QStringLiteral("凸显所选窗口"), this);
    buttonLayout->addWidget(highlightButton);

    connect(selectButton, &QPushButton::clicked, this, &KeyPresserHardware::selectWindow);
    connect(highlightButton, &QPushButton::clicked, this, &KeyPresserHardware::highlightWindow);



    layout->addLayout(buttonLayout);

    // 添加模式选择单选按钮
    QGroupBox *modeGroupBox = new QGroupBox(QStringLiteral("触发模式选择"), this);
    QHBoxLayout *modeLayout = new QHBoxLayout(modeGroupBox);
    QButtonGroup *modeGroup = new QButtonGroup(this);
    independentModeRadio = new QRadioButton(QStringLiteral("独立触发"), this);
    independentModeRadio->setToolTip(QStringLiteral("勾选的按键会独立计算时间间隔，独立触发！"));
    sequentialModeRadio = new QRadioButton(QStringLiteral("顺序触发"), this);
    sequentialModeRadio->setToolTip(QStringLiteral("勾选的按键会按设定的间隔，顺序触发！"));
    modeGroup->addButton(independentModeRadio);
    modeGroup->addButton(sequentialModeRadio);
    independentModeRadio->setChecked(true);

    modeLayout->addWidget(independentModeRadio);
    modeLayout->addWidget(sequentialModeRadio);
    modeLayout->addStretch();
    layout->addWidget(modeGroupBox);

    // 初始化置顶复选框
    topmostCheckBox = new QCheckBox(QStringLiteral("保持选择窗口激活置顶"), this);
    topmostCheckBox->setToolTip(QStringLiteral("部分应用不支持后台运行，需勾选此项确保选择的窗口保持激活置顶！"));
    layout->addWidget(topmostCheckBox);

    QHBoxLayout *spaceLayout = new QHBoxLayout();
    spaceCheckBox = new QCheckBox(this);
    spaceLayout->addWidget(spaceCheckBox);

    QLabel *spaceLabel = new QLabel(QStringLiteral("空格键时间间隔 (毫秒) [范围]:"), this);
    spaceLayout->addWidget(spaceLabel);

    spaceIntervalLineEdit = new QLineEdit(this);
    spaceIntervalLineEdit->setValidator(new QIntValidator(this));
    spaceIntervalLineEdit->setText("1000");
    spaceIntervalLineEdit->setPlaceholderText(QStringLiteral("最小值"));
    spaceLayout->addWidget(spaceIntervalLineEdit);

    QLabel *dashLabel1 = new QLabel("-", this);
    spaceLayout->addWidget(dashLabel1);

    spaceMaxIntervalLineEdit = new QLineEdit(this);
    spaceMaxIntervalLineEdit->setValidator(new QIntValidator(this));
    spaceMaxIntervalLineEdit->setText("1000");
    spaceMaxIntervalLineEdit->setPlaceholderText(QStringLiteral("最大值"));
    spaceLayout->addWidget(spaceMaxIntervalLineEdit);

    layout->addLayout(spaceLayout);

    QLabel *keysLabel = new QLabel(QStringLiteral("自定义组合键&按键和时间间隔 (毫秒) [范围]:"), this);
    layout->addWidget(keysLabel);

    QVBoxLayout *allKeysLayout = new QVBoxLayout();

    // 创建主要的10个按键的网格布局
    QGridLayout *mainKeysLayout = new QGridLayout();
    // 创建额外的5个按键的网格布局
    QWidget *extraKeysWidget = new QWidget(this);
    QGridLayout *extraKeysLayout = new QGridLayout(extraKeysWidget);
    extraKeysLayout->setContentsMargins(0, 0, 0, 0);
    extraKeysWidget->setVisible(false);

    // 创建展开/折叠按钮
    QPushButton *toggleExtraKeysButton = new QPushButton(QStringLiteral("▼ 更多按键"), this);
    toggleExtraKeysButton->setStyleSheet("text-align: left; padding-left: 5px;");
    connect(toggleExtraKeysButton, &QPushButton::clicked, [this, toggleExtraKeysButton, extraKeysWidget]() {
        bool isVisible = extraKeysWidget->isVisible();
        extraKeysWidget->setVisible(!isVisible);
        toggleExtraKeysButton->setText(isVisible ? QStringLiteral("▼ 更多按键") : QStringLiteral("▲ 收起按键"));

        // 只调整高度，保持当前宽度
        int currentWidth = width();
        adjustSize();
        resize(currentWidth, height());
    });

    for (int i = 0; i < 15; ++i) {
        keyCheckBoxes[i] = new QCheckBox(this);
        shortcutCombos[i] = new QComboBox(this);
        keyCombos[i] = new QComboBox(this);
        populateShortcutCombos(shortcutCombos[i]);
        populateKeyCombos(keyCombos[i]);
        // 设置默认值：前12个为F1-F12，后3个为ABC
        if (i < 12) {
            keyCombos[i]->setCurrentText(QString("F%1").arg(i + 1));
        } else {
            keyCombos[i]->setCurrentText(QChar('A' + (i - 12)));
        }

        intervalLineEdits[i] = new QLineEdit(this);
        intervalLineEdits[i]->setValidator(new QIntValidator(this));
        intervalLineEdits[i]->setText("1000");
        intervalLineEdits[i]->setPlaceholderText(QStringLiteral("最小值"));

        QLabel *dashLabel2 = new QLabel("-", this);

        maxIntervalLineEdits[i] = new QLineEdit(this);
        maxIntervalLineEdits[i]->setValidator(new QIntValidator(this));
        maxIntervalLineEdits[i]->setText("1000");
        maxIntervalLineEdits[i]->setPlaceholderText(QStringLiteral("最大值"));

        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [this, i]() {
            pressKeys(i);
            int minInterval = intervalLineEdits[i]->text().toInt();
            int maxInterval = maxIntervalLineEdits[i]->text().toInt();
            if (minInterval > maxInterval) std::swap(minInterval, maxInterval);
            int interval = getRandomInterval(minInterval, maxInterval);
            timers[i]->start(interval);
        });
        timers.push_back(timer);

        // 将前10个按键添加到主布局
        if (i < 10) {
            mainKeysLayout->addWidget(keyCheckBoxes[i], i, 0);
            mainKeysLayout->addWidget(shortcutCombos[i], i, 1);
            mainKeysLayout->addWidget(keyCombos[i], i, 2);
            mainKeysLayout->addWidget(intervalLineEdits[i], i, 3);
            mainKeysLayout->addWidget(dashLabel2, i, 4);
            mainKeysLayout->addWidget(maxIntervalLineEdits[i], i, 5);
        }
        // 将后5个按键添加到额外布局
        else {
            int row = i - 10;
            extraKeysLayout->addWidget(keyCheckBoxes[i], row, 0);
            extraKeysLayout->addWidget(shortcutCombos[i], row, 1);
            extraKeysLayout->addWidget(keyCombos[i], row, 2);
            extraKeysLayout->addWidget(intervalLineEdits[i], row, 3);
            extraKeysLayout->addWidget(dashLabel2, row, 4);
            extraKeysLayout->addWidget(maxIntervalLineEdits[i], row, 5);
        }
    }

    allKeysLayout->addLayout(mainKeysLayout);
    allKeysLayout->addWidget(toggleExtraKeysButton);
    allKeysLayout->addWidget(extraKeysWidget);
    layout->addLayout(allKeysLayout);

    toggleButton = new QPushButton(QStringLiteral("开始"), this);
    layout->addWidget(toggleButton);

    // 添加"快捷键设置"标签和组合框
    // 添加定时任务相关UI元素
    QGroupBox *timerTaskGroupBox = new QGroupBox(QStringLiteral("设置定时任务"), this);

    QPushButton *timerTaskButton = new QPushButton(QStringLiteral("▼ 定时任务"), this);
    timerTaskGroupBox->setVisible(false);
    layout->addWidget(timerTaskButton);
    timerTaskButton->setStyleSheet("text-align: left; padding-left: 5px;");
    connect(timerTaskButton, &QPushButton::clicked, [this, timerTaskButton, timerTaskGroupBox]() {
        bool isVisible = timerTaskGroupBox->isVisible();
        timerTaskGroupBox->setVisible(!isVisible);
        timerTaskButton->setText(isVisible ? QStringLiteral("▼ 定时任务") : QStringLiteral("▲ 定时任务"));
        // 只调整高度，保持当前宽度
        int currentWidth = width();
        adjustSize();
        resize(currentWidth, height());
    });

    QVBoxLayout *timerTaskLayout = new QVBoxLayout();

    QVBoxLayout *timeRangeLayout = new QVBoxLayout();
    QLabel *startTimeLabel = new QLabel(QStringLiteral("开始时间:"), this);
    startTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime(), this);
    startTimeEdit->setCalendarPopup(true);
    QLabel *endTimeLabel = new QLabel(QStringLiteral("停止时间:"), this);
    endTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime().addDays(1), this);
    endTimeEdit->setCalendarPopup(true);

    QHBoxLayout *startTimeLayout = new QHBoxLayout();
    QHBoxLayout *endTimeLayout = new QHBoxLayout();
    startTimeLayout->addWidget(startTimeLabel);
    startTimeLayout->addWidget(startTimeEdit);
    endTimeLayout->addWidget(endTimeLabel);
    endTimeLayout->addWidget(endTimeEdit);
    timeRangeLayout->addLayout(startTimeLayout);
    timeRangeLayout->addLayout(endTimeLayout);

    timerTaskCheckBox = new QCheckBox(QStringLiteral("启用定时任务"), this);
    connect(timerTaskCheckBox, &QCheckBox::stateChanged, this, &KeyPresserHardware::enableTimerTask);

    timerTaskLayout->addLayout(timeRangeLayout);
    timerTaskLayout->addWidget(timerTaskCheckBox);
    timerTaskGroupBox->setLayout(timerTaskLayout);

    layout->addWidget(timerTaskGroupBox);

    // 初始化定时任务检查器
    timerTaskChecker = new QTimer(this);
    connect(timerTaskChecker, &QTimer::timeout, this, &KeyPresserHardware::checkTimerTask);
    timerTaskChecker->start(1000); // 每秒检查一次
    bTimerTaskEnabled = false;

    QHBoxLayout *shortcutLayout = new QHBoxLayout();
    QLabel *shortcutLabel = new QLabel(QStringLiteral("开始/停止快捷键:"), this);
    shortcutLayout->addWidget(shortcutLabel);

    triggerKeyComboBox = new QComboBox(this);
    // 添加特殊按键（不易冲突的按键）
    triggerKeyComboBox->addItem("Home", VK_HOME);
    triggerKeyComboBox->addItem("*", 106);
    triggerKeyComboBox->addItem("Alt", 164);
    triggerKeyComboBox->addItem("Insert", VK_INSERT);
    triggerKeyComboBox->addItem("End", VK_END);
    triggerKeyComboBox->addItem("Page Up", VK_PRIOR);
    triggerKeyComboBox->addItem("Page Down", VK_NEXT);
    triggerKeyComboBox->addItem("Esc", VK_ESCAPE);
    // 添加功能键（可能会冲突，但仍可选择）
    triggerKeyComboBox->insertSeparator(triggerKeyComboBox->count());
    triggerKeyComboBox->addItem("F1", VK_F1);
    triggerKeyComboBox->addItem("F2", VK_F2);
    triggerKeyComboBox->addItem("F3", VK_F3);
    triggerKeyComboBox->addItem("F4", VK_F4);
    triggerKeyComboBox->addItem("F5", VK_F5);
    triggerKeyComboBox->addItem("F6", VK_F6);
    triggerKeyComboBox->addItem("F7", VK_F7);
    triggerKeyComboBox->addItem("F8", VK_F8);
    triggerKeyComboBox->addItem("F9", VK_F9);
    triggerKeyComboBox->addItem("F10", VK_F10);
    triggerKeyComboBox->addItem("F11", VK_F11);
    triggerKeyComboBox->addItem("F12", VK_F12);

    // 设置浮动提示
    triggerKeyComboBox->setToolTip(QStringLiteral("建议选择不会与自定义按键冲突的快捷键\n特殊按键(推荐): Home、*、Alt等\n功能键(谨慎使用): F1-F12"));
    shortcutLabel->setToolTip(QStringLiteral("鼠标悬停在下拉框上可查看按键选择建议"));

    shortcutLayout->addWidget(triggerKeyComboBox);
    shortcutLayout->addStretch();

    layout->addLayout(shortcutLayout);

    QLabel *labelPrompt = new QLabel(QStringLiteral("修改配置后需点击开始按钮以使更改生效。"), this);
    labelPrompt->setStyleSheet("color: red;");
    layout->addWidget(labelPrompt);

    instructionLabel = new QLabel(QStringLiteral("停止中"), this);
    instructionLabel->setStyleSheet("color: green;");
    layout->addWidget(instructionLabel);

    globalLayout->addLayout(layout);

    connect(selectButton, &QPushButton::clicked, this, &KeyPresserHardware::selectWindow);
    connect(toggleButton, &QPushButton::clicked, this, &KeyPresserHardware::togglePressing);
    connect(aboutButton, &QPushButton::clicked, this, &KeyPresserHardware::aboutMe);
    connect(topmostCheckBox, &QCheckBox::stateChanged, this, &KeyPresserHardware::onTopmostCheckBoxChanged);

    spaceTimer = new QTimer(this);
    connect(spaceTimer, &QTimer::timeout, [this]() {
        pressSpace();
        int minInterval = spaceIntervalLineEdit->text().toInt();
        int maxInterval = spaceMaxIntervalLineEdit->text().toInt();
        if (minInterval > maxInterval) std::swap(minInterval, maxInterval);
        int interval = getRandomInterval(minInterval, maxInterval);
        spaceTimer->start(interval);
    });

    loadSettings();


    //QString currentPath = QCoreApplication::applicationDirPath();
    //QString hexPath = QDir(currentPath).filePath("keypresser.ino.hex");
    //_controller.uploadSketch(hexPath.toLocal8Bit().toStdString());


}

bool KeyPresserHardware::checkArduino()
{
    static bool bFirst = true;
    if(!bFirst) return true;
    std::string portName = SerialPort::findArduinoLeonardoPort(true);
    if (portName.empty()) {
        portName = QInputDialog::getText(this, QStringLiteral("无法自动检测到Arduino Leonardo！"), QStringLiteral("请插入Arduino Leonardo后重启软件或手动输入端口名称 (如 COM3): ")).toStdString();
    }
    else {
        qInfo() << QStringLiteral("\n自动检测到Arduino Leonardo端口: ");
    }
    // 2. 连接到Arduino
    if (!_controller.connect(portName)) {

        switch( QMessageBox::critical(this,QStringLiteral("错误"),QStringLiteral("无法连接到Arduino Leonardo！请重新插拔Arduino Leonardo开发板的USB 或 点击Arduino Leonardo开发板上的Reset按钮（红色按钮）后重试！"),QStringLiteral("购买Leonardo开发板"), QStringLiteral("关闭"),0,1))
        {
        case 0:
            //https://h5.m.taobao.com/awp/core/detail.htm?ft=t&id=1004108236292
            QDesktopServices::openUrl(QUrl("https://h5.m.taobao.com/awp/core/detail.htm?ft=t&id=1004108236292"));
            break;
        case 1:
            this->close();
            break;
        default:
            this->close();
            break;
        }

        return false;
    }

    qInfo() << QStringLiteral("成功连接到Arduino！");

    //setWindowTitle(QStringLiteral("KeyPresser硬件版-成功连接到Arduino Leonardo端口:") + portName.c_str());
    arduinoLabel->setText(QStringLiteral("成功连接到Arduino Leonardo端口:") + portName.c_str());
    arduinoLabel->setStyleSheet("color: green;");
    bFirst = false;
    return true;
}

KeyPresserHardware::~KeyPresserHardware() {
    saveSettings();
}

void KeyPresserHardware::highlightWindow() {
    if (targetHwnd) {
        // 如果窗口最小化，先恢复窗口
        if (IsIconic(targetHwnd)) {
            ShowWindow(targetHwnd, SW_RESTORE);
        }

        // 将窗口置于前台并闪烁
        SetForegroundWindow(targetHwnd);
        FlashWindow(targetHwnd, TRUE);
        raise();
        activateWindow();

        // 获取窗口的矩形区域
        RECT rect;
        GetWindowRect(targetHwnd, &rect);

        // 获取窗口的设备上下文
        HDC hdc = GetWindowDC(NULL);  // 使用屏幕的设备上下文

        // 创建一个红色画笔
        HPEN hRedPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
        HPEN hWhitePen = CreatePen(PS_SOLID, 3, RGB(255, 255, 255));

        int borderWidth = 3;
        for(int i = 0; i < 10; i++){
            SelectObject(hdc, i%2 ? hRedPen : hWhitePen);
            // 绘制红色框，稍微扩大边界以避免遮盖
            Rectangle(hdc, rect.left - borderWidth, rect.top - borderWidth, rect.right + borderWidth, rect.bottom + borderWidth);
            Sleep(30);
        }

        // 释放资源
        //SelectObject(hdc, hOldPen);
        DeleteObject(hRedPen);
        DeleteObject(hWhitePen);
        ReleaseDC(NULL, hdc);

        // 等待3秒
        //Sleep(2000);

        // 重新绘制窗口以清除红色框
        InvalidateRect(targetHwnd, NULL, TRUE);
        UpdateWindow(targetHwnd);

    } else {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("请先选择一个窗口！"));
    }
}


// Arduino特殊键码定义
#define ARDUINO_KEY_LEFT_SHIFT 129
#define ARDUINO_KEY_LEFT_CTRL 128
#define ARDUINO_KEY_LEFT_ALT 130
#define ARDUINO_KEY_LEFT_GUI 131
#define ARDUINO_KEY_RIGHT_GUI 135
#define ARDUINO_KEY_F1 0x3A
#define ARDUINO_KEY_F2 0x3B
#define ARDUINO_KEY_F3 0x3C
#define ARDUINO_KEY_F4 0x3D
#define ARDUINO_KEY_F5 0x3E
#define ARDUINO_KEY_F6 0x3F
#define ARDUINO_KEY_F7 0x40
#define ARDUINO_KEY_F8 0x41
#define ARDUINO_KEY_F9 0x42
#define ARDUINO_KEY_F10 0x43
#define ARDUINO_KEY_F11 0x44
#define ARDUINO_KEY_F12 0x45
#define ARDUINO_KEY_SPACE 0x20
#define ARDUINO_KEY_RETURN 0x28
#define ARDUINO_KEY_TAB 0x2B
#define ARDUINO_KEY_ESCAPE 0x29
#define ARDUINO_KEY_BACK 0x2A
#define ARDUINO_KEY_INSERT 0x49
#define ARDUINO_KEY_DELETE 0x4C
#define ARDUINO_KEY_HOME 0x4A
#define ARDUINO_KEY_END 0x4D
#define ARDUINO_KEY_PAGE_UP 0x4B
#define ARDUINO_KEY_PAGE_DOWN 0x4E
#define ARDUINO_KEY_LEFT 0x50
#define ARDUINO_KEY_RIGHT 0x4F
#define ARDUINO_KEY_UP 0x52
#define ARDUINO_KEY_DOWN 0x51

void KeyPresserHardware::populateShortcutCombos(QComboBox *comboBox)
{
    // 添加单个修饰键
    comboBox->addItem("", QStringList());
    comboBox->addItem("Shift", QStringList() << QString::number(ARDUINO_KEY_LEFT_SHIFT));
    comboBox->addItem("Ctrl", QStringList() << QString::number(ARDUINO_KEY_LEFT_CTRL));
    comboBox->addItem("Alt", QStringList() << QString::number(ARDUINO_KEY_LEFT_ALT));
    comboBox->addItem("Win", QStringList() << QString::number(ARDUINO_KEY_LEFT_GUI));

    // 添加常见的两键组合
    comboBox->addItem("Shift+Ctrl", QStringList() << QString::number(ARDUINO_KEY_LEFT_SHIFT) << QString::number(ARDUINO_KEY_LEFT_CTRL));
    comboBox->addItem("Shift+Alt", QStringList() << QString::number(ARDUINO_KEY_LEFT_SHIFT) << QString::number(ARDUINO_KEY_LEFT_ALT));
    comboBox->addItem("Ctrl+Alt", QStringList() << QString::number(ARDUINO_KEY_LEFT_CTRL) << QString::number(ARDUINO_KEY_LEFT_ALT));
    comboBox->addItem("Ctrl+Win", QStringList() << QString::number(ARDUINO_KEY_LEFT_CTRL) << QString::number(ARDUINO_KEY_LEFT_GUI));
    comboBox->addItem("Alt+Win", QStringList() << QString::number(ARDUINO_KEY_LEFT_ALT) << QString::number(ARDUINO_KEY_LEFT_GUI));
    comboBox->addItem("Shift+Win", QStringList() << QString::number(ARDUINO_KEY_LEFT_SHIFT) << QString::number(ARDUINO_KEY_LEFT_GUI));
}

void KeyPresserHardware::populateKeyCombos(QComboBox *comboBox) {
    comboBox->addItem("F1", ARDUINO_KEY_F1);
    comboBox->addItem("F2", ARDUINO_KEY_F2);
    comboBox->addItem("F3", ARDUINO_KEY_F3);
    comboBox->addItem("F4", ARDUINO_KEY_F4);
    comboBox->addItem("F5", ARDUINO_KEY_F5);
    comboBox->addItem("F6", ARDUINO_KEY_F6);
    comboBox->addItem("F7", ARDUINO_KEY_F7);
    comboBox->addItem("F8", ARDUINO_KEY_F8);
    comboBox->addItem("F9", ARDUINO_KEY_F9);
    comboBox->addItem("F10", ARDUINO_KEY_F10);
    comboBox->addItem("F11", ARDUINO_KEY_F11);
    comboBox->addItem("F12", ARDUINO_KEY_F12);
    comboBox->addItem("A", 'A');
    comboBox->addItem("B", 'B');
    comboBox->addItem("C", 'C');
    comboBox->addItem("D", 'D');
    comboBox->addItem("E", 'E');
    comboBox->addItem("F", 'F');
    comboBox->addItem("G", 'G');
    comboBox->addItem("H", 'H');
    comboBox->addItem("I", 'I');
    comboBox->addItem("J", 'J');
    comboBox->addItem("K", 'K');
    comboBox->addItem("L", 'L');
    comboBox->addItem("M", 'M');
    comboBox->addItem("N", 'N');
    comboBox->addItem("O", 'O');
    comboBox->addItem("P", 'P');
    comboBox->addItem("Q", 'Q');
    comboBox->addItem("R", 'R');
    comboBox->addItem("S", 'S');
    comboBox->addItem("T", 'T');
    comboBox->addItem("U", 'U');
    comboBox->addItem("V", 'V');
    comboBox->addItem("W", 'W');
    comboBox->addItem("X", 'X');
    comboBox->addItem("Y", 'Y');
    comboBox->addItem("Z", 'Z');
    comboBox->addItem("0", '0');
    comboBox->addItem("1", '1');
    comboBox->addItem("2", '2');
    comboBox->addItem("3", '3');
    comboBox->addItem("4", '4');
    comboBox->addItem("5", '5');
    comboBox->addItem("6", '6');
    comboBox->addItem("7", '7');
    comboBox->addItem("8", '8');
    comboBox->addItem("9", '9');
    comboBox->addItem("Shift", ARDUINO_KEY_LEFT_SHIFT);
    comboBox->addItem("Space", ARDUINO_KEY_SPACE);
    comboBox->addItem("Enter", ARDUINO_KEY_RETURN);
    comboBox->addItem("Tab", ARDUINO_KEY_TAB);
    comboBox->addItem("Esc", ARDUINO_KEY_ESCAPE);
    comboBox->addItem("Backspace", ARDUINO_KEY_BACK);
    comboBox->addItem("Insert", ARDUINO_KEY_INSERT);
    comboBox->addItem("Delete", ARDUINO_KEY_DELETE);
    comboBox->addItem("Home", ARDUINO_KEY_HOME);
    comboBox->addItem("End", ARDUINO_KEY_END);
    comboBox->addItem("Page Up", ARDUINO_KEY_PAGE_UP);
    comboBox->addItem("Page Down", ARDUINO_KEY_PAGE_DOWN);
    comboBox->addItem("Left Arrow", ARDUINO_KEY_LEFT);
    comboBox->addItem("Right Arrow", ARDUINO_KEY_RIGHT);
    comboBox->addItem("Up Arrow", ARDUINO_KEY_UP);
    comboBox->addItem("Down Arrow", ARDUINO_KEY_DOWN);
}

void KeyPresserHardware::selectWindow() {
    SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    selectedWindowLabel->setText(QStringLiteral("请点击目标窗口..."));
}

void KeyPresserHardware::togglePressing()
{
    bIsRuning ? stopPressing() : startPressing();
    qApp->setStyleSheet(qApp->styleSheet());
}

void KeyPresserHardware::startPressing() {
    if(!targetHwnd)
    {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("请选择窗口后，再点击开始！"));
        return;
    }

    bIsRuning = !bIsRuning;
    toggleButton->setText(QStringLiteral("停止"));
    toggleButton->setProperty("state", "running");

    instructionLabel->setText(QStringLiteral("运行中"));
    stopAllTimers();

    // 根据模式选择不同处理逻辑
    if(sequentialModeRadio->isChecked()) {
        // 顺序触发模式
        QList<int> selectedKeys;
        for(int i = 0; i < 15; ++i) {
            if(keyCheckBoxes[i]->isChecked()) {
                selectedKeys.append(i);
            }
        }
        currentIndex = 0;
        sequenceTimer = new QTimer(this);
        connect(sequenceTimer, &QTimer::timeout, [this, selectedKeys]() {
            if(!selectedKeys.isEmpty()) {
                if(currentIndex < selectedKeys.size()) {
                    pressKeys(selectedKeys[currentIndex]);
                    currentIndex = (currentIndex + 1) % selectedKeys.size();
                }

                // 动态获取时间设置
                int currentKey = selectedKeys[currentIndex];
                int minInterval = intervalLineEdits[currentKey]->text().toInt();
                int maxInterval = maxIntervalLineEdits[currentKey]->text().toInt();
                if(minInterval > maxInterval) std::swap(minInterval, maxInterval);
                sequenceTimer->start(getRandomInterval(minInterval, maxInterval));
            }
        });

        // 初始时间间隔
        if(!selectedKeys.isEmpty()) {
            int minInterval = intervalLineEdits[selectedKeys[0]]->text().toInt();
            int maxInterval = maxIntervalLineEdits[selectedKeys[0]]->text().toInt();
            if(minInterval > maxInterval) std::swap(minInterval, maxInterval);
            sequenceTimer->start(getRandomInterval(minInterval, maxInterval));
        }
    } else {
        // 原有独立触发模式
        attachToTargetWindow();
        if (spaceCheckBox->isChecked()) {
            pressSpace();
            int minInterval = spaceIntervalLineEdit->text().toInt();
            int maxInterval = spaceMaxIntervalLineEdit->text().toInt();
            if (minInterval > maxInterval) std::swap(minInterval, maxInterval);
            int interval = getRandomInterval(minInterval, maxInterval);
            spaceTimer->start(interval);
        }
        for (int i = 0; i < 15; ++i) {
            if (keyCheckBoxes[i]->isChecked() && keyCombos[i]->currentIndex() != -1) {
                pressKeys(i);
                int minInterval = intervalLineEdits[i]->text().toInt();
                int maxInterval = maxIntervalLineEdits[i]->text().toInt();
                if (minInterval > maxInterval) std::swap(minInterval, maxInterval);
                int interval = getRandomInterval(minInterval, maxInterval);
                timers[i]->start(interval);
            }
        }
    }
}

void KeyPresserHardware::stopPressing() {
    if(instructionLabel->text() == QStringLiteral("停止中")) return;
    bIsRuning = !bIsRuning;
    toggleButton->setText(QStringLiteral("开始"));
    instructionLabel->setText(QStringLiteral("停止中"));
    toggleButton->setProperty("state", "stopped");
    stopAllTimers();
    if(sequenceTimer) {
        sequenceTimer->stop();
        delete sequenceTimer;
        sequenceTimer = nullptr;
    }
    // 清理消息队列中的按键和窗口消息
    if (targetHwnd) {
        MSG msg;
        // 清理按键消息
        while (PeekMessage(&msg, targetHwnd, WM_KEYDOWN, WM_KEYUP, PM_REMOVE)) {
            // 移除所有未处理的按键消息
        }
        // 清理窗口置顶消息
        while (PeekMessage(&msg, targetHwnd, WM_WINDOWPOSCHANGING, WM_WINDOWPOSCHANGED, PM_REMOVE)) {
            // 移除所有未处理的窗口位置消息
        }
    }
    detachFromTargetWindow();
}

void KeyPresserHardware::aboutMe()
{
    AboutMeDlg dlg(this);
    dlg.exec();
}

void KeyPresserHardware::stopAllTimers() {
    spaceTimer->stop();
    for (QTimer *timer : timers) {
        timer->stop();
    }
}

void KeyPresserHardware::pressSpace() {
    if(!bIsRuning) return;
    if (targetHwnd) {
        if(topmostCheckBox->isChecked()){
            // 如果窗口最小化则先恢复窗口
            if (IsIconic(targetHwnd)) {
                ShowWindow(targetHwnd, SW_RESTORE);
            }
            SetWindowPos(targetHwnd, topmostCheckBox->isChecked() ? HWND_TOPMOST : HWND_NOTOPMOST,
                         0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
        }

        _controller.sendKey(std::to_string(VK_SPACE));
        return;
    }
}

void KeyPresserHardware::onTopmostCheckBoxChanged(int state) {
    bool topmost = (state == Qt::Checked);
    if(!topmost) SetWindowPos(targetHwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
}

std::vector<std::string> qStringListToStdVector(const QStringList &qList)
{
    std::vector<std::string> stdVector;
    stdVector.reserve(qList.size()); // 预分配内存，提高性能

    for (const QString &str : qList) {
        // 使用toStdString()方法将QString转换为std::string
        stdVector.push_back(str.toStdString());
    }

    return stdVector;
}

void KeyPresserHardware::pressKeys(int index) {
    if(!bIsRuning) return;
    if (targetHwnd) {
        if(topmostCheckBox->isChecked()){
            // 如果窗口最小化则先恢复窗口
            if (IsIconic(targetHwnd)) {
                ShowWindow(targetHwnd, SW_RESTORE);
            }
            SetWindowPos(targetHwnd, topmostCheckBox->isChecked() ? HWND_TOPMOST : HWND_NOTOPMOST,
                         0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
        }

        int key = keyCombos[index]->currentData().toInt();

        QStringList shortCuts = shortcutCombos[index]->currentData().toStringList();
        std::vector<std::string> stdVector = qStringListToStdVector(shortCuts);
        if(shortCuts.size() == 0) _controller.sendKey(std::to_string(key));
        else {
            stdVector.emplace_back(std::to_string(key));
            _controller.pressKeyCombination(stdVector);
        }
        return;
    }
}

int KeyPresserHardware::getRandomInterval(int minInterval, int maxInterval) {
    return (minInterval == maxInterval) ? minInterval : QRandomGenerator::global()->bounded(minInterval, maxInterval + 1);
}

void KeyPresserHardware::loadSettings() {
    QSettings settings("FinnSoft", "KeyPresserHardware");
    loadSettingsFromObject(settings);
}

// 新增：从配置文件加载设置
void KeyPresserHardware::loadSettingsFromFile(const QString &filename) {
    QSettings fileSettings(filename, QSettings::IniFormat);
    loadSettingsFromObject(fileSettings);
}

// 新增：从QSettings对象加载设置的通用方法
void KeyPresserHardware::loadSettingsFromObject(QSettings &settings) {
    QString mode = settings.value("triggerMode", "independent").toString();
    independentModeRadio->setChecked(mode == "independent");
    sequentialModeRadio->setChecked(mode == "sequential");

    spaceCheckBox->setChecked(settings.value("spaceCheckBox", false).toBool());
    spaceIntervalLineEdit->setText(settings.value("spaceIntervalLineEdit", "1000").toString());
    spaceMaxIntervalLineEdit->setText(settings.value("spaceMaxIntervalLineEdit", "1000").toString());
    triggerKeyComboBox->setCurrentIndex(settings.value("triggerKeyComboBox", 0).toInt());
    topmostCheckBox->setChecked(settings.value("topmostCheckBox", false).toBool());
    
    for (int i = 0; i < 15; ++i) {
        QString keyName = QString("keyCombo%1").arg(i);
        if(settings.contains(keyName))
            keyCombos[i]->setCurrentIndex(settings.value(keyName, 0).toInt());
        else{
            if (i < 12) {
                keyCombos[i]->setCurrentText(QString("F%1").arg(i + 1));
            } else {
                keyCombos[i]->setCurrentText(QChar('A' + (i - 12)));
            }
        }

        QString shotcutKeyName = QString("shortcutCombo%1").arg(i);
        shortcutCombos[i]->setCurrentIndex(settings.value(shotcutKeyName, 0).toInt());

        keyCheckBoxes[i]->setChecked(settings.value(QString("keyCheckBox%1").arg(i), false).toBool());
        intervalLineEdits[i]->setText(settings.value(QString("intervalLineEdit%1").arg(i), "1000").toString());
        maxIntervalLineEdits[i]->setText(settings.value(QString("maxIntervalLineEdit%1").arg(i), "1000").toString());
    }
}

void KeyPresserHardware::saveSettings() {
    QSettings settings("FinnSoft", "KeyPresserHardware");
    saveSettingsToObject(settings);
}

// 新增：保存设置到配置文件
void KeyPresserHardware::saveSettingsToFile(const QString &filename) {
    QSettings fileSettings(filename, QSettings::IniFormat);
    fileSettings.clear(); // 清除可能存在的旧设置
    saveSettingsToObject(fileSettings);
}

// 新增：保存设置到QSettings对象的通用方法
void KeyPresserHardware::saveSettingsToObject(QSettings &settings) {
    settings.setValue("triggerMode", independentModeRadio->isChecked() ? "independent" : "sequential");

    settings.setValue("spaceCheckBox", spaceCheckBox->isChecked());
    settings.setValue("spaceIntervalLineEdit", spaceIntervalLineEdit->text());
    settings.setValue("spaceMaxIntervalLineEdit", spaceMaxIntervalLineEdit->text());
    settings.setValue("triggerKeyComboBox", triggerKeyComboBox->currentIndex());
    settings.setValue("topmostCheckBox", topmostCheckBox->isChecked());
    
    for (int i = 0; i < 15; ++i) {
        settings.setValue(QString("keyCheckBox%1").arg(i), keyCheckBoxes[i]->isChecked());
        settings.setValue(QString("shortcutCombo%1").arg(i), shortcutCombos[i]->currentIndex());
        settings.setValue(QString("keyCombo%1").arg(i), keyCombos[i]->currentIndex());
        settings.setValue(QString("intervalLineEdit%1").arg(i), intervalLineEdits[i]->text());
        settings.setValue(QString("maxIntervalLineEdit%1").arg(i), maxIntervalLineEdits[i]->text());
    }
}

void KeyPresserHardware::clearSettings() {
    QSettings settings("FinnSoft", "KeyPresserHardware");
    settings.clear();  // 清除所有设置

    // 重置界面上的控件为默认值
    spaceCheckBox->setChecked(false);
    spaceIntervalLineEdit->setText("1000");
    spaceMaxIntervalLineEdit->setText("1000");

    for (int i = 0; i < 15; ++i) {
        keyCheckBoxes[i]->setChecked(false);
        // 设置默认值：前12个为F1-F12，后3个为ABC
        if (i < 12) {
            keyCombos[i]->setCurrentText(QString("F%1").arg(i + 1));
        } else {
            keyCombos[i]->setCurrentText(QChar('A' + (i - 12)));
        }
        intervalLineEdits[i]->setText("1000");
        maxIntervalLineEdits[i]->setText("1000");
    }
}

void KeyPresserHardware::attachToTargetWindow() {
    if (targetHwnd) {
        DWORD targetThreadId = GetWindowThreadProcessId(targetHwnd, NULL);
        DWORD currentThreadId = GetCurrentThreadId();
        AttachThreadInput(currentThreadId, targetThreadId, TRUE);  // 关联当前线程与目标窗口的线程
    }
}

void KeyPresserHardware::detachFromTargetWindow() {
    if (targetHwnd) {
        DWORD targetThreadId = GetWindowThreadProcessId(targetHwnd, NULL);
        DWORD currentThreadId = GetCurrentThreadId();
        AttachThreadInput(currentThreadId, targetThreadId, FALSE);  // 解除当前线程与目标窗口的线程关联
    }
}

void CALLBACK KeyPresserHardware::WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    if (event == EVENT_SYSTEM_FOREGROUND) {
        wchar_t windowTitle[256];
        GetWindowText(hwnd, windowTitle, 256);
        if (KeyPresserHardware::instance) {
            KeyPresserHardware::instance->targetHwnd = hwnd;
            auto label = KeyPresserHardware::instance->selectedWindowLabel;
            QString newStr = QString::fromWCharArray(windowTitle);
            QFontMetrics fontWidth(label->font());
            QString elideNode = fontWidth.elidedText(newStr,Qt::ElideRight,KeyPresserHardware::instance->width()-20);
            KeyPresserHardware::instance->selectedWindowLabel->setText(elideNode);
            KeyPresserHardware::instance->selectedWindowLabel->setToolTip(newStr);
            Q_EMIT KeyPresserHardware::instance->windowStateChanged();
            UnhookWinEvent(hWinEventHook);

            QTimer::singleShot(300, []() {
                if (KeyPresserHardware::instance) {
                    KeyPresserHardware::instance->raise();
                    KeyPresserHardware::instance->activateWindow();
                }
            });
        }
    }
}


void KeyPresserHardware::enableTimerTask(bool enable) {
    bTimerTaskEnabled = enable;
    if (!enable) {
        // 如果禁用定时任务，清除可能的任务状态
        timerTaskChecker->stop();
        timerTaskChecker->start(1000); // 保持每秒检查一次，但不执行任务
    }
}

void KeyPresserHardware::checkTimerTask() {
    if (!bTimerTaskEnabled) return;

    QDateTime currentTime = QDateTime::currentDateTime();
    QDateTime startTime = startTimeEdit->dateTime();
    QDateTime endTime = endTimeEdit->dateTime();

    // 检查是否在定时任务时间范围内
    if (currentTime >= startTime && currentTime <= endTime) {
        // 如果不在运行中，则开始运行
        if (!bIsRuning && targetHwnd) {
            startPressing();
        }
    } else {
        // 如果超出时间范围且在运行中，则停止运行
        if (bIsRuning) {
            stopPressing();
        }
    }

    qApp->setStyleSheet(qApp->styleSheet());
}

void KeyPresserHardware::setTimerTask(QDateTime start, QDateTime end) {
    startTimeEdit->setDateTime(start);
    endTimeEdit->setDateTime(end);
    bTimerTaskEnabled = true;
    timerTaskCheckBox->setChecked(true);
}
