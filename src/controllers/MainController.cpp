#include "MainController.h"
#include "CameraController.h"
#include "DeviceController.h"
#include "MainWindow.h"
#include "NetworkController.h"
#include "models/ConnectionStatusModel.h"
#include "models/DeviceConfigModel.h"
#include "models/NetworkConfigModel.h"
#include "models/WifiInfoModel.h"
#include "views/IpConfigView.h"
#include "views/MainControlView.h"
#include "views/WifiInfoView.h"
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QStandardPaths>

/**
 * @brief MainController constructor - Initializes the core application logic
 *
 * This is the central controller that manages all models and controllers.
 * Initialization sequence:
 * 1. setupModels() - Creates all data models (NetworkConfigModel,
 * DeviceConfigModel, etc.)
 * 2. setupControllers() - Creates all controllers (CameraController,
 * NetworkController, DeviceController)
 *
 * Note: MainWindow connection happens later via setMainWindow() to avoid
 * circular dependencies.
 *
 * @param parent Parent QObject for memory management
 */
MainController::MainController(QObject *parent)
    : QObject(parent), m_mainWindow(nullptr) // Will be set via setMainWindow()
      ,
      m_cameraController(nullptr) // Will be created in setupControllers()
      ,
      m_networkController(nullptr) // Will be created in setupControllers()
      ,
      m_deviceController(nullptr) // Will be created in setupControllers()
      ,
      m_networkConfigModel(nullptr) // Will be created in setupModels()
      ,
      m_deviceConfigModel(nullptr) // Will be created in setupModels()
      ,
      m_connectionStatusModel(nullptr) // Will be created in setupModels()
      ,
      m_wifiInfoModel(nullptr) // Will be created in setupModels()
{
  // Step 1: Create all data models first
  // Models store application state and emit signals when data changes
  setupModels();

  // Step 2: Create all controllers and connect them
  // Controllers contain business logic and coordinate between models and views
  setupControllers();
}

MainController::~MainController() {}

/**
 * @brief Connects MainWindow to MainController
 *
 * This method is called after MainWindow is fully constructed.
 * It establishes all connections between views, models, and controllers.
 *
 * Sequence:
 * 1. Store reference to MainWindow
 * 2. connectSignals() - Connect general signals
 * 3. connectViews() - Connect views to models and controllers, set up button
 * handlers
 *
 * @param window Pointer to the MainWindow instance
 */
void MainController::setMainWindow(MainWindow *window) {
  // Store reference to MainWindow for later use
  m_mainWindow = window;

  // Connect general signals (currently minimal, most connections in
  // connectViews)
  connectSignals();

  // Connect views to models and controllers:
  //   - Set camera widget for CameraController
  //   - Connect button signals to DeviceController
  //   - Connect status signals to MainControlView
  //   - Set models for WifiInfoView and IpConfigView
  connectViews();
}

/**
 * @brief Creates all data models
 *
 * Models are responsible for storing application data and emitting signals
 * when data changes. They follow the Observer pattern via Qt's signal/slot
 * mechanism.
 *
 * Models created:
 * - NetworkConfigModel: Manages IP addresses, ports, and network configuration
 *                      (automatically detects local IP on construction)
 * - DeviceConfigModel: Manages camera and ESP32 device configurations
 * - ConnectionStatusModel: Tracks connection states (camera, pan/tilt,
 * joystick)
 * - WifiInfoModel: Manages WiFi network information
 */
void MainController::setupModels() {
  // NetworkConfigModel: Manages IP addresses and ports for all devices
  // Constructor automatically calls updateLocalNetworkInfo() to detect local IP
  m_networkConfigModel = new NetworkConfigModel(this);
  m_networkConfigModel->loadFromFile(networkConfigFilePath());

  // DeviceConfigModel: Manages camera type (RTSP/HTTP/Iriun) and ESP32
  // configurations
  m_deviceConfigModel = new DeviceConfigModel(this);

  // ConnectionStatusModel: Tracks connection states for camera, pan/tilt, and
  // joystick All states default to Disconnected
  m_connectionStatusModel = new ConnectionStatusModel(this);

  // WifiInfoModel: Manages WiFi network information
  m_wifiInfoModel = new WifiInfoModel(this);
}

/**
 * @brief Creates all controllers and establishes connections between them
 *
 * Controllers contain business logic and coordinate between models and views.
 *
 * Controllers created:
 * - CameraController: Manages camera streams (RTSP/HTTP/Iriun)
 *                     Creates CameraManager internally
 * - NetworkController: Manages TCP connections
 *                      Creates TcpServer (for joystick) and TcpClient (for
 * pan/tilt)
 * - DeviceController: Processes joystick commands and converts them to pan/tilt
 * commands
 *
 * Connections established:
 * - DeviceController::commandToPanTilt → NetworkController::sendPanTiltCommand
 *   (When DeviceController creates a command, send it via NetworkController)
 * - NetworkController::joystickCommandReceived →
 * DeviceController::processJoystickCommand (When NetworkController receives
 * joystick data, process it in DeviceController)
 */
void MainController::setupControllers() {
  // Create CameraController - Manages camera streams
  // Constructor creates CameraManager internally
  m_cameraController = new CameraController(this);

  // Create NetworkController - Manages TCP server/client
  // Constructor creates TcpServer and TcpClient internally
  m_networkController = new NetworkController(this);

  // Create DeviceController - Processes joystick commands
  m_deviceController = new DeviceController(this);

  // Assign models to controllers so they can access and update data
  m_cameraController->setDeviceConfigModel(m_deviceConfigModel);
  m_cameraController->setConnectionStatusModel(m_connectionStatusModel);
  m_cameraController->setNetworkConfigModel(
      m_networkConfigModel); // Connect NetworkConfigModel to CameraController
  m_networkController->setDeviceConfigModel(m_deviceConfigModel);
  m_networkController->setConnectionStatusModel(m_connectionStatusModel);

  // Connect controllers: DeviceController → NetworkController
  // When DeviceController creates a pan/tilt command, send it via
  // NetworkController
  connect(m_deviceController, &DeviceController::commandToPanTilt,
          m_networkController, &NetworkController::sendPanTiltCommand);

  // Connect controllers: NetworkController → DeviceController
  // When NetworkController receives joystick command, process it in
  // DeviceController
  connect(m_networkController, &NetworkController::joystickCommandReceived,
          m_deviceController, &DeviceController::processJoystickCommand);
}

/**
 * @brief Connects general signals (currently minimal)
 *
 * Most signal connections are done in connectViews() where views are available.
 * This method is reserved for general connections that don't require views.
 */
void MainController::connectSignals() {
  if (!m_mainWindow) {
    return;
  }

  // Note: Most connections are done in connectViews() where views are available
  // This method can be extended for general signal connections that don't
  // require views
}

/**
 * @brief Connects views to models and controllers
 *
 * This is where the MVC pattern is fully established:
 * - Views are connected to Models (data binding)
 * - Views are connected to Controllers (user actions)
 * - Models are connected to Views (status updates)
 *
 * Connections made:
 * 1. MainControlView:
 *    - CameraWidget → CameraController (for video display)
 *    - Button clicks → DeviceController → NetworkController (command flow)
 *    - ConnectionStatusModel → MainControlView (status updates)
 *
 * 2. WifiInfoView:
 *    - WifiInfoModel → WifiInfoView (data binding)
 *
 * 3. IpConfigView:
 *    - NetworkConfigModel → IpConfigView (data binding)
 */
void MainController::connectViews() {
  if (!m_mainWindow) {
    return;
  }

  // ============================================================
  // Connect MainControlView
  // ============================================================
  MainControlView *mainView = m_mainWindow->mainControlView();
  if (mainView) {
    // Set camera IP for zoom control
    if (m_networkConfigModel) {
      mainView->setCameraIp(m_networkConfigModel->cameraIp());
      // Update camera IP when it changes
      connect(m_networkConfigModel, &NetworkConfigModel::cameraIpChanged,
              mainView, &MainControlView::setCameraIp);
    }
    
    // Set camera widget for CameraController
    // This allows CameraManager to display video stream in the widget
    m_cameraController->setCameraWidget(mainView->cameraWidget());

    // Connect control button signals to command handlers
    // When user clicks a button, create command and send to pan/tilt unit

    // Up button: Create "UP" command and send via NetworkController
    connect(mainView, &MainControlView::upButtonClicked, [this]() {
      QByteArray cmd =
          m_deviceController->createPanTiltCommand(PanTiltCommand::Up);
      m_networkController->sendPanTiltCommand(cmd);
    });

    // Down button: Create "DOWN" command and send via NetworkController
    connect(mainView, &MainControlView::downButtonClicked, [this]() {
      QByteArray cmd =
          m_deviceController->createPanTiltCommand(PanTiltCommand::Down);
      m_networkController->sendPanTiltCommand(cmd);
    });

    // Left button: Create "LEFT" command and send via NetworkController
    connect(mainView, &MainControlView::leftButtonClicked, [this]() {
      QByteArray cmd =
          m_deviceController->createPanTiltCommand(PanTiltCommand::Left);
      m_networkController->sendPanTiltCommand(cmd);
    });

    // Right button: Create "RIGHT" command and send via NetworkController
    connect(mainView, &MainControlView::rightButtonClicked, [this]() {
      QByteArray cmd =
          m_deviceController->createPanTiltCommand(PanTiltCommand::Right);
      m_networkController->sendPanTiltCommand(cmd);
    });

    // Zoom slider: Now handled directly in MainControlView via HTTP requests
    // The zoomValueChanged signal is still emitted for compatibility,
    // but HTTP requests are sent continuously while dragging

    // Connect status update signals from ConnectionStatusModel to
    // MainControlView When connection state changes, update the status labels
    // in the UI

    // Camera connection state changed → update camera status label
    connect(m_connectionStatusModel, &ConnectionStatusModel::cameraStateChanged,
            [mainView](ConnectionState state) {
              QString status = (state == ConnectionState::Connected)
                                   ? "Connected"
                                   : "Disconnected";
              mainView->updateCameraStatus(status);
            });

    // Pan/Tilt connection state changed → update pan/tilt status label
    connect(m_connectionStatusModel,
            &ConnectionStatusModel::panTiltStateChanged,
            [mainView](ConnectionState state) {
              QString status = (state == ConnectionState::Connected)
                                   ? "Connected"
                                   : "Disconnected";
              mainView->updatePanTiltStatus(status);
            });

    // Joystick connection state changed → update joystick status label
    connect(m_connectionStatusModel,
            &ConnectionStatusModel::joystickStateChanged,
            [mainView](ConnectionState state) {
              QString status = (state == ConnectionState::Connected)
                                   ? "Connected"
                                   : "Disconnected";
              mainView->updateJoystickStatus(status);
            });
  }

  // ============================================================
  // Connect WifiInfoView to WifiInfoModel
  // ============================================================
  WifiInfoView *wifiView = m_mainWindow->wifiInfoView();
  if (wifiView) {
    // Bind WifiInfoModel to WifiInfoView
    // View will automatically update when model data changes
    wifiView->setModel(m_wifiInfoModel);
  }

  // ============================================================
  // Connect IpConfigView to NetworkConfigModel
  // ============================================================
  IpConfigView *ipView = m_mainWindow->ipConfigView();
  if (ipView) {
    // Bind NetworkConfigModel to IpConfigView
    // View will display IP addresses and allow editing
    ipView->setModel(m_networkConfigModel);

    // When Apply/Save is clicked, the model is updated and will automatically
    // trigger camera connection via signal-slot connection in CameraController
    // (cameraIpChanged/cameraPortChanged → connectCamera)
    // When Apply/Save is clicked, properties are already updated in model
    // CameraController will automatically reconnect via signal connections
    connect(ipView, &IpConfigView::saveClicked, this, [this, ipView]() {
      if (!m_networkConfigModel) {
        return;
      }
      QString path = networkConfigFilePath();
      if (!m_networkConfigModel->saveToFile(path)) {
        qWarning() << "MainController: failed to save network config to"
                   << path;
      } else {
        qDebug() << "MainController: network config saved to" << path;
        QMessageBox::information(ipView, "Saved",
                                 "Network configuration has been saved.");
      }
    });
  }

  // ============================================================
  // Connect CameraController signals to MainControlView log
  // ============================================================
  if (mainView && m_cameraController) {
    // Log camera connection errors to the system log
    connect(m_cameraController, &CameraController::streamError,
            [mainView](const QString &error) {
              mainView->appendLog(QString("Camera Error: %1").arg(error));
            });

    // Log camera connection status changes
    connect(m_cameraController, &CameraController::streamStatusChanged,
            [mainView](bool isActive) {
              QString status =
                  isActive ? "Camera connected" : "Camera disconnected";
              mainView->appendLog(status);
            });

    // Forward log messages from CameraController to MainControlView
    connect(
        m_cameraController, &CameraController::logMessage,
        [mainView](const QString &message) { mainView->appendLog(message); });

    // Connect camera control buttons
    connect(mainView, &MainControlView::connectCameraClicked,
            m_cameraController, &CameraController::connectCamera);
    connect(mainView, &MainControlView::disconnectCameraClicked,
            m_cameraController, &CameraController::disconnectCamera);
  }
}

QString MainController::networkConfigFilePath() const {
  QString dir =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  if (dir.isEmpty()) {
    dir = QDir::currentPath();
  }
  QDir().mkpath(dir);
  return QDir(dir).filePath("network_config.json");
}
