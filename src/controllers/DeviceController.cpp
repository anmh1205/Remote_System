#include "DeviceController.h"
#include <QDebug>

DeviceController::DeviceController(QObject *parent)
    : QObject(parent)
{
}

void DeviceController::processJoystickCommand(const QByteArray &data)
{
    // Parse joystick command and convert to pan/tilt command
    PanTiltCommand cmd = parseJoystickCommand(data);
    
    if (cmd != PanTiltCommand::Stop) {
        QByteArray panTiltCmd = createPanTiltCommand(cmd);
        emit commandToPanTilt(panTiltCmd);
    }
}

QByteArray DeviceController::createPanTiltCommand(PanTiltCommand cmd)
{
    // Placeholder: Create protocol message for pan/tilt unit
    // User will implement actual protocol based on ESP32 code
    
    QByteArray command;
    
    switch (cmd) {
    case PanTiltCommand::Up:
        command = "UP\n";  // Placeholder format
        break;
    case PanTiltCommand::Down:
        command = "DOWN\n";
        break;
    case PanTiltCommand::Left:
        command = "LEFT\n";
        break;
    case PanTiltCommand::Right:
        command = "RIGHT\n";
        break;
    case PanTiltCommand::ZoomIn:
        command = "ZOOM_IN\n";
        break;
    case PanTiltCommand::ZoomOut:
        command = "ZOOM_OUT\n";
        break;
    case PanTiltCommand::Stop:
        command = "STOP\n";
        break;
    }
    
    return command;
}

PanTiltCommand DeviceController::parseJoystickCommand(const QByteArray &data)
{
    // Placeholder: Parse joystick command from ESP32
    // User will implement actual parsing based on ESP32 protocol
    
    QString command = QString::fromUtf8(data).trimmed().toUpper();
    
    if (command == "UP" || command == "U") {
        return PanTiltCommand::Up;
    } else if (command == "DOWN" || command == "D") {
        return PanTiltCommand::Down;
    } else if (command == "LEFT" || command == "L") {
        return PanTiltCommand::Left;
    } else if (command == "RIGHT" || command == "R") {
        return PanTiltCommand::Right;
    } else if (command == "ZOOM_IN" || command == "ZI") {
        return PanTiltCommand::ZoomIn;
    } else if (command == "ZOOM_OUT" || command == "ZO") {
        return PanTiltCommand::ZoomOut;
    }
    
    return PanTiltCommand::Stop;
}

