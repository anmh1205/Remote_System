#include "MainWindow.h"
#include "views/MainControlView.h"
#include "views/WifiInfoView.h"
#include "views/IpConfigView.h"
#include "controllers/MainController.h"
#include "controllers/CameraController.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>

/**
 * @brief MainWindow constructor - Initializes the main application window
 * 
 * Initialization sequence:
 * 1. setupUI() - Creates QStackedWidget and all three views
 * 2. setupMenuBar() - Creates navigation menu with actions
 * 3. setupStatusBar() - Creates status bar for messages
 * 4. Create MainController - This initializes all models and controllers
 * 5. Connect MainWindow to MainController - Establishes all signal/slot connections
 * 6. Show default view - Displays MainControlView
 * 
 * @param parent Parent widget (nullptr for top-level window)
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_stackedWidget(nullptr)      // Will be created in setupUI()
    , m_mainControlView(nullptr)     // Will be created in setupUI()
    , m_wifiInfoView(nullptr)        // Will be created in setupUI()
    , m_ipConfigView(nullptr)        // Will be created in setupUI()
    , m_mainController(nullptr)      // Will be created below
{
    // Step 1: Create UI components (QStackedWidget and all views)
    setupUI();
    
    // Step 2: Create menu bar with navigation actions
    setupMenuBar();
    
    // Step 3: Create status bar for displaying messages
    setupStatusBar();
    
    // Step 4: Create MainController - This will:
    //   - Create all 4 models (NetworkConfigModel, DeviceConfigModel, etc.)
    //   - Create all 3 controllers (CameraController, NetworkController, DeviceController)
    //   - Set up models for controllers
    //   - Connect signals between controllers
    m_mainController = new MainController(this);
    
    // Step 5: Connect MainWindow to MainController - This will:
    //   - Store reference to MainWindow
    //   - Connect general signals (connectSignals)
    //   - Connect views to models and controllers (connectViews)
    //   - Set camera widget for CameraController
    //   - Connect button signals to DeviceController
    //   - Connect status signals to MainControlView
    m_mainController->setMainWindow(this);
    
    // Step 6: Display MainControlView as the default view
    showMainControl();
}

MainWindow::~MainWindow()
{
}

/**
 * @brief Sets up the main UI components
 * 
 * Creates QStackedWidget to manage multiple views and allows switching between them.
 * All three views are created and added to the stacked widget:
 * - MainControlView: Main control interface with camera display and control buttons
 * - WifiInfoView: WiFi network information display
 * - IpConfigView: IP configuration interface
 */
void MainWindow::setupUI()
{
    // Create QStackedWidget to manage multiple views (only one visible at a time)
    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);  // Set as the central widget of QMainWindow
    
    // Create all three views
    m_mainControlView = new MainControlView(this);    // Main control interface
    m_wifiInfoView = new WifiInfoView(this);          // WiFi information view
    m_ipConfigView = new IpConfigView(this);          // IP configuration view
    
    // Add all views to the stacked widget (index 0, 1, 2)
    m_stackedWidget->addWidget(m_mainControlView);
    m_stackedWidget->addWidget(m_wifiInfoView);
    m_stackedWidget->addWidget(m_ipConfigView);
    
    // Set window properties
    setWindowTitle("Remote System Control");
    resize(1200, 800);  // Default window size
}

/**
 * @brief Sets up the menu bar with navigation actions
 * 
 * Creates a "View" menu with three actions to switch between different views:
 * - Main Control: Switches to MainControlView
 * - WiFi Info: Switches to WifiInfoView
 * - IP Config: Switches to IpConfigView
 */
void MainWindow::setupMenuBar()
{
    // Create "View" menu in the menu bar
    QMenu *viewMenu = menuBar()->addMenu("View");
    
    // Create "Main Control" action and connect to showMainControl slot
    QAction *mainControlAction = viewMenu->addAction("Main Control");
    connect(mainControlAction, &QAction::triggered, this, &MainWindow::showMainControl);
    
    // Create "WiFi Info" action and connect to showWifiInfo slot
    QAction *wifiInfoAction = viewMenu->addAction("WiFi Info");
    connect(wifiInfoAction, &QAction::triggered, this, &MainWindow::showWifiInfo);
    
    // Create "IP Config" action and connect to showIpConfig slot
    QAction *ipConfigAction = viewMenu->addAction("IP Config");
    connect(ipConfigAction, &QAction::triggered, this, &MainWindow::showIpConfig);
}

/**
 * @brief Sets up the status bar
 * 
 * Creates and initializes the status bar with a default "Ready" message.
 */
void MainWindow::setupStatusBar()
{
    statusBar()->showMessage("Ready");
}

/**
 * @brief Switches to MainControlView
 * 
 * Changes the current view in QStackedWidget to MainControlView
 * and updates the status bar message.
 */
void MainWindow::showMainControl()
{
    m_stackedWidget->setCurrentWidget(m_mainControlView);
    statusBar()->showMessage("Main Control View");
}

/**
 * @brief Switches to WifiInfoView
 * 
 * Changes the current view in QStackedWidget to WifiInfoView
 * and updates the status bar message.
 */
void MainWindow::showWifiInfo()
{
    m_stackedWidget->setCurrentWidget(m_wifiInfoView);
    statusBar()->showMessage("WiFi Info View");
}

/**
 * @brief Switches to IpConfigView
 * 
 * Changes the current view in QStackedWidget to IpConfigView
 * and updates the status bar message.
 */
void MainWindow::showIpConfig()
{
    m_stackedWidget->setCurrentWidget(m_ipConfigView);
    statusBar()->showMessage("IP Config View");
}

