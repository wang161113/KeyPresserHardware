#ifndef KeyPresserHardware_H
#define KeyPresserHardware_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QSettings>
#include <windows.h>
#include <vector>
#include <QGroupBox>
#include <QPushButton>
#include <QFileDialog>
#include <QDir>
#include <QDateTimeEdit>
#include <ArduinoController.hpp>

class KeyPresserHardware : public QWidget {
    Q_OBJECT

public:
    KeyPresserHardware(QWidget *parent = nullptr);
    ~KeyPresserHardware();

    static KeyPresserHardware *instance;
    QLabel *instructionLabel;
    QComboBox *triggerKeyComboBox;  // 新增的控件声明
    HWND targetHwnd = nullptr;
    bool isProcessingHotkey = false;

    QLabel *selectedWindowLabel;
    QLabel *arduinoLabel;
    //Dm::Idmsoft* _dm = nullptr;
    ArduinoController _controller;
    bool checkArduino();
public slots:
    void selectWindow();
    void togglePressing();
    void startPressing();
    void stopPressing();
    void aboutMe();
    void pressSpace();
    void pressKeys(int index);
    void clearSettings();
    void loadSettingsFromFile(const QString &filename);
    void saveSettingsToFile(const QString &filename);
    
public slots:
    void setTimerTask(QDateTime start, QDateTime end);
    void checkTimerTask();
    void enableTimerTask(bool enable);

Q_SIGNALS:
    void windowStateChanged();

private:
    void loadSettingsFromObject(QSettings &settings);
    void saveSettingsToObject(QSettings &settings);
    bool bIsRuning = false;
    bool bTimerTaskEnabled = false;
    QPushButton *toggleButton;
    QTimer *sequenceTimer = nullptr;
    QTimer *timerTaskChecker = nullptr;
    QDateTimeEdit *startTimeEdit = nullptr;
    QDateTimeEdit *endTimeEdit = nullptr;
    QCheckBox *timerTaskCheckBox = nullptr;
    int currentIndex = 0;
    static void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime);

    QCheckBox *spaceCheckBox;
    QLineEdit *spaceIntervalLineEdit;
    QLineEdit *spaceMaxIntervalLineEdit;
    QCheckBox *keyCheckBoxes[15];
    QComboBox *shortcutCombos[15];
    QComboBox *keyCombos[15];
    QLineEdit *intervalLineEdits[15];
    QLineEdit *maxIntervalLineEdits[15];
    QCheckBox *topmostCheckBox;
    QRadioButton *independentModeRadio;
    QRadioButton *sequentialModeRadio;

    //QRadioButton *normalKeyModeRadio;
    //QRadioButton *widnowsKeyModelRadio;
   // QRadioButton *dxKeyModedlRadio;


    QButtonGroup *modeGroup;
    QTimer *spaceTimer;
    std::vector<QTimer*> timers;


    void populateShortcutCombos(QComboBox *comboBox);
    void populateKeyCombos(QComboBox *comboBox);
    void stopAllTimers();
    void loadSettings();
    void saveSettings();
    void attachToTargetWindow();
    void detachFromTargetWindow();
    int getRandomInterval(int minInterval, int maxInterval);
    void highlightWindow();
    void onTopmostCheckBoxChanged(int state);
};

#endif // KeyPresserHardware_H
