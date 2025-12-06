#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>

class MainWindow;
class CameraController;
class NetworkController;
class DeviceController;
class NetworkConfigModel;
class DeviceConfigModel;
class ConnectionStatusModel;
class WifiInfoModel;

class MainController : public QObject
{
    Q_OBJECT

public:
    explicit MainController(QObject *parent = nullptr);
    ~MainController();
    
    void setMainWindow(MainWindow *window);
    
    CameraController* cameraController() const { return m_cameraController; }
    NetworkController* networkController() const { return m_networkController; }
    DeviceController* deviceController() const { return m_deviceController; }

private:
    MainWindow *m_mainWindow;
    CameraController *m_cameraController;
    NetworkController *m_networkController;
    DeviceController *m_deviceController;
    
    NetworkConfigModel *m_networkConfigModel;
    DeviceConfigModel *m_deviceConfigModel;
    ConnectionStatusModel *m_connectionStatusModel;
    WifiInfoModel *m_wifiInfoModel;
    
    void setupControllers();
    void setupModels();
    void connectSignals();
    void connectViews();
};

#endif // MAINCONTROLLER_H

