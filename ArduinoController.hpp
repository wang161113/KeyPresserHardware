#ifndef ARDUINOCONTROLLER_HPP
#define ARDUINOCONTROLLER_HPP

#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
#include <setupapi.h>
#include <devguid.h>
#include <regstr.h>
#include <QDebug>
#include <QCoreApplication>

// Link SetupAPI library
#pragma comment(lib, "setupapi.lib")

// Serial port communication class
class SerialPort {
private:
    HANDLE hSerial;
    DCB dcbSerialParams;
    COMMTIMEOUTS timeouts;

public:
    SerialPort() : hSerial(INVALID_HANDLE_VALUE), portName("") {}

    ~SerialPort() {
        if (isOpen()) {
            CloseHandle(hSerial);
        }
    }

    // Alternative COM port detection method by direct scanning
    // Working principle: Directly try to open COM1 to COM20, successfully opened ones might be Arduino
    static std::string findArduinoByDirectPortScan(bool enableDebug = false) {
        if (enableDebug) {
            std::cout << "\n=== Starting direct COM port scan ===" << std::endl;
        }

        // Try COM1 to COM20 (usually enough to cover all possible ports)
        for (int i = 1; i <= 20; i++) {
            char portName[20];
            sprintf_s(portName, sizeof(portName), "\\\\.\\COM%d", i);

            if (enableDebug) {
                std::cout << "  Trying to open port: " << portName << std::endl;
            }

            // Try to open COM port
            HANDLE hPort = CreateFileA(
                portName,
                GENERIC_READ | GENERIC_WRITE,
                0,  // No sharing
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL
                );

            if (hPort != INVALID_HANDLE_VALUE) {
                // Port opened successfully!
                // Simple communication test can be added here to further verify if it's Arduino
                if (enableDebug) {
                    std::cout << "  ✅ Port " << portName << " opened successfully! May be Arduino device" << std::endl;
                }

                CloseHandle(hPort);

                // Extract port name (remove \\.\ prefix)
                char comOnly[10];
                sscanf_s(portName, "\\\\.\\%[COM0-9]", comOnly, sizeof(comOnly));

                if (enableDebug) {
                    std::cout << "=== Direct COM scanning completed ===" << std::endl;
                    std::cout << "Found possible Arduino port: " << comOnly << std::endl;
                }

                return std::string(comOnly);
            }
            else {
                // Failed to open port, get error code
                DWORD error = GetLastError();
                if (enableDebug) {
                    if (error == ERROR_FILE_NOT_FOUND) {
                        std::cout << "  ❌ Port does not exist" << std::endl;
                    }
                    else if (error == ERROR_ACCESS_DENIED) {
                        std::cout << "  ⚠️  Port is occupied" << std::endl;
                    }
                    else {
                        std::cout << "  ❌ Failed to open, error code: " << error << std::endl;
                    }
                }
            }
        }

        if (enableDebug) {
            std::cout << "=== Direct COM scanning completed ===" << std::endl;
            std::cout << "No accessible COM ports found!" << std::endl;
        }

        return "";  // No available ports found
    }

    // Automatically detect Arduino Leonardo port
    // Working principle:
    // 1. Priority use SetupAPI method to enumerate devices and verify VID/PID
    // 2. If SetupAPI method fails, use direct COM port scanning as backup
    static std::string findArduinoLeonardoPort(bool enableDebug = false) {
        // Arduino Leonardo's VID (Vendor ID) and PID (Product ID)
        // These values are officially defined by Arduino and can be verified in Device Manager
        const int ARDUINO_VID = 0x2341;  // Arduino's Vendor ID
        const int ARDUINO_LEONARDO_PID = 0x8036;  // Leonardo's Product ID

        // Build hardware ID string for matching
        char expectedHardwareId[64];
        sprintf_s(expectedHardwareId, sizeof(expectedHardwareId), "VID_%04X&PID_%04X", ARDUINO_VID, ARDUINO_LEONARDO_PID);

        if (enableDebug) {
            std::cout << "=== Arduino Leonardo port detection started ===" << std::endl;
            std::cout << "Expected hardware ID: " << expectedHardwareId << std::endl;
        }

        // Step 1: Enumerate all serial port devices
        // First try using GUID_DEVINTERFACE_COMPORT (more precise serial port GUID)
        GUID deviceGuid = GUID_DEVINTERFACE_COMPORT;
        HDEVINFO deviceInfoSet = SetupDiGetClassDevs(&deviceGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

        if (deviceInfoSet == INVALID_HANDLE_VALUE) {
            // If failed, try using GUID_DEVCLASS_PORTS (more general port class)
            if (enableDebug) {
                std::cout << "Failed to use GUID_DEVINTERFACE_COMPORT, trying GUID_DEVCLASS_PORTS" << std::endl;
            }

            deviceGuid = GUID_DEVCLASS_PORTS;
            deviceInfoSet = SetupDiGetClassDevs(&deviceGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

            if (deviceInfoSet == INVALID_HANDLE_VALUE) {
                if (enableDebug) {
                    std::cout << "Failed to enumerate port devices, error code: " << GetLastError() << std::endl;
                }
                return "";
            }
        }

        SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
        deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

        // Step 2: Iterate through all found port devices
        for (DWORD deviceIndex = 0; SetupDiEnumDeviceInterfaces(deviceInfoSet, NULL, &deviceGuid, deviceIndex, &deviceInterfaceData); ++deviceIndex) {
            if (enableDebug) {
                std::cout << "\nDevice detected #" << deviceIndex + 1 << std::endl;
            }

            // Get device interface details
            DWORD requiredSize = 0;
            SetupDiGetDeviceInterfaceDetailA(deviceInfoSet, &deviceInterfaceData, NULL, 0, &requiredSize, NULL);

            PSP_DEVICE_INTERFACE_DETAIL_DATA_A deviceDetailData =
                (PSP_DEVICE_INTERFACE_DETAIL_DATA_A)malloc(requiredSize);
            if (!deviceDetailData) {
                if (enableDebug) {
                    std::cout << "  Memory allocation failed" << std::endl;
                }
                continue;
            }

            deviceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);

            SP_DEVINFO_DATA devInfoData;
            devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

            if (SetupDiGetDeviceInterfaceDetailA(deviceInfoSet, &deviceInterfaceData, deviceDetailData, requiredSize, NULL, &devInfoData)) {
                // Extract COM port name
                std::string devicePath = deviceDetailData->DevicePath;
                char comPortName[10] = { 0 };
                unsigned int comPortSize = sizeof(comPortName);

                if (sscanf_s(devicePath.c_str(), "\\\\.\\%[COM0-9]", comPortName, comPortSize) == 1) {
                    if (enableDebug) {
                        std::cout << "  Port name: " << comPortName << std::endl;
                        std::cout << "  Device path: " << devicePath << std::endl;
                    }

                    // Step 3: Check device friendly name
                    char friendlyName[256] = { 0 };
                    DWORD dataSize = sizeof(friendlyName);
                    bool isArduinoByName = false;

                    if (SetupDiGetDeviceRegistryPropertyA(deviceInfoSet, &devInfoData, SPDRP_FRIENDLYNAME, NULL, (LPBYTE)friendlyName, dataSize, NULL)) {
                        if (enableDebug) {
                            std::cout << "  Friendly name: " << friendlyName << std::endl;
                        }

                        // Check if friendly name contains "Arduino" or "Leonardo"
                        if (strstr(friendlyName, "Arduino") != NULL || strstr(friendlyName, "Leonardo") != NULL) {
                            isArduinoByName = true;
                            if (enableDebug) {
                                std::cout << "  ✅ Matched Arduino device by friendly name!" << std::endl;
                            }
                        }
                    }

                    // Step 4: If friendly name doesn't match, check hardware ID
                    bool isArduinoByHardwareId = false;

                    if (!isArduinoByName) {
                        char hardwareIds[1024] = { 0 };
                        dataSize = sizeof(hardwareIds);

                        if (SetupDiGetDeviceRegistryPropertyA(deviceInfoSet, &devInfoData, SPDRP_HARDWAREID, NULL, (LPBYTE)hardwareIds, dataSize, NULL)) {
                            if (enableDebug) {
                                std::cout << "  Hardware ID: " << hardwareIds << std::endl;
                            }

                            // Check if hardware ID contains expected VID/PID
                            if (strstr(hardwareIds, expectedHardwareId) != NULL) {
                                isArduinoByHardwareId = true;
                                if (enableDebug) {
                                    std::cout << "  ✅ Matched Arduino Leonardo by hardware ID!" << std::endl;
                                }
                            }
                        }
                    }

                    // If any check passes, return COM port name
                    if (isArduinoByName || isArduinoByHardwareId) {
                        if (enableDebug) {
                            std::cout << "\n=== Detection completed ===" << std::endl;
                            std::cout << "Found Arduino Leonardo port: " << comPortName << std::endl;
                        }

                        free(deviceDetailData);
                        SetupDiDestroyDeviceInfoList(deviceInfoSet);
                        return std::string(comPortName);
                    }
                }
            }

            free(deviceDetailData);
        }

        if (enableDebug) {
            std::cout << "\n=== Detection completed ===" << std::endl;
            std::cout << "SetupAPI method didn't find Arduino Leonardo device!" << std::endl;
        }

        SetupDiDestroyDeviceInfoList(deviceInfoSet);

        // Method 2: If SetupAPI fails, use direct COM port scanning
        if (enableDebug) {
            std::cout << "Trying direct COM port scanning..." << std::endl;
        }
        return findArduinoByDirectPortScan(enableDebug);
    }

    bool open(const std::string& portName, DWORD baudRate) {
        // Open serial port
        hSerial = CreateFileA(
            portName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
            );

        // Store the port name
        this->portName = portName;

        if (hSerial == INVALID_HANDLE_VALUE) {
            return false;
        }

        // Configure serial port parameters
        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
        if (!GetCommState(hSerial, &dcbSerialParams)) {
            CloseHandle(hSerial);
            hSerial = INVALID_HANDLE_VALUE;
            return false;
        }

        dcbSerialParams.BaudRate = baudRate;
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.StopBits = ONESTOPBIT;
        dcbSerialParams.Parity = NOPARITY;

        if (!SetCommState(hSerial, &dcbSerialParams)) {
            CloseHandle(hSerial);
            hSerial = INVALID_HANDLE_VALUE;
            return false;
        }

        // Set timeouts
        timeouts.ReadIntervalTimeout = 50;
        timeouts.ReadTotalTimeoutConstant = 50;
        timeouts.ReadTotalTimeoutMultiplier = 10;
        timeouts.WriteTotalTimeoutConstant = 50;
        timeouts.WriteTotalTimeoutMultiplier = 10;

        if (!SetCommTimeouts(hSerial, &timeouts)) {
            CloseHandle(hSerial);
            hSerial = INVALID_HANDLE_VALUE;
            return false;
        }

        return true;
    }

    bool isOpen() const {
        return hSerial != INVALID_HANDLE_VALUE;
    }

    bool write(const std::string& data) {
        if (!isOpen()) {
            return false;
        }

        DWORD bytesWritten;
        if (!WriteFile(hSerial, data.c_str(), data.length(), &bytesWritten, NULL)) {
            return false;
        }

        return bytesWritten == data.length();
    }

    bool read(std::string& data, DWORD maxBytes) {
        if (!isOpen()) {
            return false;
        }

        char buffer[1024];
        DWORD bytesRead;
        DWORD bufferSize = static_cast<DWORD>(sizeof(buffer));

        #define min(a,b)            (((a) < (b)) ? (a) : (b))
        if (!ReadFile(hSerial, buffer, min(maxBytes, bufferSize), &bytesRead, NULL)) {
            return false;
        }

        data.assign(buffer, bytesRead);
        return true;
    }

    // Get the name of the currently open port
    std::string getPortName() const {
        if(portName.empty()){
            return SerialPort::findArduinoLeonardoPort(true);
        }
        return portName;
    }

private:
    std::string portName; // Store the name of the opened port
};

// Command type enumeration
enum class CommandType {
    PRESS_KEY = 0,
    RELEASE_KEY,
    TYPE_STRING,
    PRESS_COMBINATION,
    DELAY,
    MOUSE_MOVE,
    MOUSE_PRESS,
    MOUSE_RELEASE,
    MOUSE_CLICK,
    MOUSE_WHEEL
};

#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
#define MOUSE_MIDDLE 4
#define MOUSE_ALL (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE)

// Arduino controller class
class ArduinoController {
private:
    SerialPort serialPort;

public:
    bool connect(const std::string& portName, DWORD baudRate = 9600) {
        return serialPort.open(portName, baudRate);
    }

    bool isConnected() const {
        return serialPort.isOpen();
    }

    // Send key press command
    bool pressKey(const std::string& key) {
        std::string command = "<0," + key + ">";
        return serialPort.write(command);
    }

    // Send key release command
    bool releaseKey(const std::string& key) {
        std::string command = "<1," + key + ">";
        return serialPort.write(command);
    }

    // Send string input command
    bool typeString(const std::string& str) {
        std::string command = "<2," + str + ">";
        return serialPort.write(command);
    }

    // Send key combination command
    bool pressKeyCombination(const std::vector<std::string>& keys) {
        std::string keysStr;
        for (size_t i = 0; i < keys.size(); ++i) {
            keysStr += keys[i];
            if (i < keys.size() - 1) {
                keysStr += ",";
            }
        }
        std::string command = "<3," + keysStr + ">";
        return serialPort.write(command);
    }

    // Send delay command
    bool delay(unsigned int milliseconds) {
        std::string command = "<4," + std::to_string(milliseconds) + ">";
        return serialPort.write(command);
    }

    // Send complete key operation (press then release)
    bool sendKey(const std::string& key, unsigned int pressDuration = 100) {
        if (!pressKey(key)) {
            return false;
        }

        // Wait for key press duration
        ::Sleep(pressDuration);

        return releaseKey(key);
    }

    // Send mouse move command
    bool mouseMove(int dx, int dy) {
        std::string command = "<" + std::to_string(static_cast<int>(CommandType::MOUSE_MOVE)) + "," +
                              std::to_string(dx) + "," + std::to_string(dy) + ">";
        return serialPort.write(command);
    }

    // Send mouse press command
    bool mousePress(int button) {
        std::string command = "<" + std::to_string(static_cast<int>(CommandType::MOUSE_PRESS)) + "," +
                              std::to_string(button) + ">";
        return serialPort.write(command);
    }

    // Send mouse release command
    bool mouseRelease(int button) {
        std::string command = "<" + std::to_string(static_cast<int>(CommandType::MOUSE_RELEASE)) + "," +
                              std::to_string(button) + ">";
        return serialPort.write(command);
    }

    // Send mouse click command
    bool mouseClick(int button, int clickCount = 1) {
        std::string command = "<" + std::to_string(static_cast<int>(CommandType::MOUSE_CLICK)) + "," +
                              std::to_string(button) + "," + std::to_string(clickCount) + ">";
        return serialPort.write(command);
    }

    // Send mouse wheel command
    bool mouseWheel(int delta) {
        std::string command = "<" + std::to_string(static_cast<int>(CommandType::MOUSE_WHEEL)) + "," +
                              std::to_string(delta) + ">";
        return serialPort.write(command);
    }

    // Upload sketch (HEX file) to Arduino board
    // Parameters:
    // - hexFilePath: Path to the HEX file
    // - avrdudePath: Path to avrdude executable (default: common Arduino IDE path)
    // - boardType: Arduino board type (default: "leonardo" for Arduino Leonardo)
    // - baudRate: Baud rate for uploading (default: 57600 for Arduino Leonardo)
    // - enableDebug: Print debug information
    bool uploadSketch(const std::string& hexFilePath,
                      bool enableDebug = true) {

        std::string arduino_cli_path = (QCoreApplication::applicationDirPath() + "/arduino-cli.exe").toStdString();
        //  command
        // 使用 --input-file 参数直接指定 HEX 文件
        std::string command = "cmd.exe /S /C \"" + arduino_cli_path + "\" upload --fqbn arduino:avr:leonardo --port " + serialPort.getPortName() + " --input-file \"" + hexFilePath + "\"";
        if (enableDebug) {
            qInfo() << "=== Starting sketch upload ===";
            qInfo() << "AVRDUDE command: " << command.c_str();
        }

        // Execute avrdude command
        int result = system(command.c_str());

        if (enableDebug) {
            if (result == 0) {
                qInfo() << "✅ Sketch upload completed successfully!";
            } else {
                qDebug() << "❌ Sketch upload failed with error code: " << result;
            }
            qInfo() << "=== Upload process finished ===";
        }

        return (result == 0);
    }
};

#endif // ARDUINOCONTROLLER_HPP
