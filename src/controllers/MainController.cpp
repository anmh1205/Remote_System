#include "MainController.h"
#include "MainWindow.h"
#include "CameraController.h"
#include "NetworkController.h"
#include "DeviceController.h"
#include "models/NetworkConfigModel.h"
#include "models/DeviceConfigModel.h"
#include "models/ConnectionStatusModel.h"
#include "models/WifiInfoModel.h"
#include "views/MainControlView.h"
#include "views/WifiInfoView.h"
#include "views/IpConfigView.h"
#include <QDebug>

MainController::MainController(QObject *parent)
    : QObject(parent)
    , m_mainWindow(nullptr)
    , m_cameraController(nullptr)
    , m_networkController(nullptr)
    , m_deviceController(nullptr)
    , m_networkConfigModel(nullptr)
    , m_deviceConfigModel(nullptr)
    , m_connectionStatusModel(nullptr)
    , m_wifiInfoModel(nullptr)
{
    setupModels();
    setupControllers();
}

MainController::~MainController()
{
}

void MainController::setMainWindow(MainWindow *window)
{
    m_mainWindow = window;
    connectSignals();
    connectViews();
}

void MainController::setupModels()
{
    m_networkConfigModel = new NetworkConfigModel(this);
    m_deviceConfigModel = new DeviceConfigModel(this);
    m_connectionStatusModel = new ConnectionStatusModel(this);
    m_wifiInfoModel = new WifiInfoModel(this);
}

void MainController::setupControllers()
{
    m_cameraController = new CameraController(this);
    m_networkController = new NetworkController(this);
    m_deviceController = new DeviceController(this);
    
    // Set models for controllers
    m_cameraController->setDeviceConfigModel(m_deviceConfigModel);
    m_cameraController->setConnectionStatusModel(m_connectionStatusModel);
    m_networkController->setDeviceConfigModel(m_deviceConfigModel);
    m_networkController->setConnectionStatusModel(m_connectionStatusModel);
    
    // Connect controllers
    connect(m_deviceController, &DeviceController::commandToPanTilt,
            m_networkController, &NetworkController::sendPanTiltCommand);
    connect(m_networkController, &NetworkController::joystickCommandReceived,
            m_deviceController, &DeviceController::processJoystickCommand);
}

void MainController::connectSignals()
{
    if (!m_mainWindow) {
        return;
    }
    
    // Connect connection status to main control view
    connect(m_connectionStatusModel, &ConnectionStatusModel::cameraStateChanged,
            [this](ConnectionState state) {
                if (m_mainWindow) {
                    QString status = (state == ConnectionState::Connected) ? "Connected" : "Disconnected";
                    // Will be connected in connectViews
                }
            });
}

void MainController::connectViews()
{
    if (!m_mainWindow) {
        return;
    }
    
    // Connect MainControlView
    MainControlView *mainView = m_mainWindow->mainControlView();
    if (mainView) {
        m_cameraController->setCameraWidget(mainView->cameraWidget());
        
        // Connect control buttons
        connect(mainView, &MainControlView::upButtonClicked,
                [this]() {
                    QByteArray cmd = m_deviceController->createPanTiltCommand(PanTiltCommand::Up);
                    m_networkController->sendPanTiltCommand(cmd);
                });
        connect(mainView, &MainControlView::downButtonClicked,
                [this]() {
                    QByteArray cmd = m_deviceController->createPanTiltCommand(PanTiltCommand::Down);
                    m_networkController->sendPanTiltCommand(cmd);
                });
        connect(mainView, &MainControlView::leftButtonClicked,
                [this]() {
                    QByteArray cmd = m_deviceController->createPanTiltCommand(PanTiltCommand::Left);
                    m_networkController->sendPanTiltCommand(cmd);
                });
        connect(mainView, &MainControlView::rightButtonClicked,
                [this]() {
                    QByteArray cmd = m_deviceController->createPanTiltCommand(PanTiltCommand::Right);
                    m_networkController->sendPanTiltCommand(cmd);
                });
        connect(mainView, &MainControlView::zoomInButtonClicked,
                [this]() {
                    QByteArray cmd = m_deviceController->createPanTiltCommand(PanTiltCommand::ZoomIn);
                    m_networkController->sendPanTiltCommand(cmd);
                });
        connect(mainView, &MainControlView::zoomOutButtonClicked,
                [this]() {
                    QByteArray cmd = m_deviceController->createPanTiltCommand(PanTiltCommand::ZoomOut);
                    m_networkController->sendPanTiltCommand(cmd);
                });
        
        // Connect status updates
        connect(m_connectionStatusModel, &ConnectionStatusModel::cameraStateChanged,
                [mainView](ConnectionState state) {
                    QString status = (state == ConnectionState::Connected) ? "Connected" : "Disconnected";
                    mainView->updateCameraStatus(status);
                });
        connect(m_connectionStatusModel, &ConnectionStatusModel::panTiltStateChanged,
                [mainView](ConnectionState state) {
                    QString status = (state == ConnectionState::Connected) ? "Connected" : "Disconnected";
                    mainView->updatePanTiltStatus(status);
                });
        connect(m_connectionStatusModel, &ConnectionStatusModel::joystickStateChanged,
                [mainView](ConnectionState state) {
                    QString status = (state == ConnectionState::Connected) ? "Connected" : "Disconnected";
                    mainView->updateJoystickStatus(status);
                });
    }
    
    // Connect WifiInfoView
    WifiInfoView *wifiView = m_mainWindow->wifiInfoView();
    if (wifiView) {
        wifiView->setModel(m_wifiInfoModel);
    }
    
    // Connect IpConfigView
    IpConfigView *ipView = m_mainWindow->ipConfigView();
    if (ipView) {
        ipView->setModel(m_networkConfigModel);
    }
}

