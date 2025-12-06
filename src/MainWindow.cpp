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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_stackedWidget(nullptr)
    , m_mainControlView(nullptr)
    , m_wifiInfoView(nullptr)
    , m_ipConfigView(nullptr)
    , m_mainController(nullptr)
{
    setupUI();
    setupMenuBar();
    setupStatusBar();
    
    m_mainController = new MainController(this);
    m_mainController->setMainWindow(this);
    
    // Show main control view by default
    showMainControl();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);
    
    m_mainControlView = new MainControlView(this);
    m_wifiInfoView = new WifiInfoView(this);
    m_ipConfigView = new IpConfigView(this);
    
    m_stackedWidget->addWidget(m_mainControlView);
    m_stackedWidget->addWidget(m_wifiInfoView);
    m_stackedWidget->addWidget(m_ipConfigView);
    
    setWindowTitle("Remote System Control");
    resize(1200, 800);
}

void MainWindow::setupMenuBar()
{
    QMenu *viewMenu = menuBar()->addMenu("View");
    
    QAction *mainControlAction = viewMenu->addAction("Main Control");
    connect(mainControlAction, &QAction::triggered, this, &MainWindow::showMainControl);
    
    QAction *wifiInfoAction = viewMenu->addAction("WiFi Info");
    connect(wifiInfoAction, &QAction::triggered, this, &MainWindow::showWifiInfo);
    
    QAction *ipConfigAction = viewMenu->addAction("IP Config");
    connect(ipConfigAction, &QAction::triggered, this, &MainWindow::showIpConfig);
}

void MainWindow::setupStatusBar()
{
    statusBar()->showMessage("Ready");
}

void MainWindow::showMainControl()
{
    m_stackedWidget->setCurrentWidget(m_mainControlView);
    statusBar()->showMessage("Main Control View");
}

void MainWindow::showWifiInfo()
{
    m_stackedWidget->setCurrentWidget(m_wifiInfoView);
    statusBar()->showMessage("WiFi Info View");
}

void MainWindow::showIpConfig()
{
    m_stackedWidget->setCurrentWidget(m_ipConfigView);
    statusBar()->showMessage("IP Config View");
}

