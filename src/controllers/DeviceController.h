#ifndef DEVICECONTROLLER_H
#define DEVICECONTROLLER_H

#include <QObject>
#include <QByteArray>

enum class PanTiltCommand {
    Up,
    Down,
    Left,
    Right,
    ZoomIn,
    ZoomOut,
    Stop
};

class DeviceController : public QObject
{
    Q_OBJECT

public:
    explicit DeviceController(QObject *parent = nullptr);
    
    void processJoystickCommand(const QByteArray &data);
    QByteArray createPanTiltCommand(PanTiltCommand cmd);

signals:
    void commandToPanTilt(const QByteArray &command);

private:
    PanTiltCommand parseJoystickCommand(const QByteArray &data);
};

Q_DECLARE_METATYPE(PanTiltCommand)

#endif // DEVICECONTROLLER_H

