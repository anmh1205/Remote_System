#include "MainControlView.h"
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QSlider>
#include <QGridLayout>
#include <QTextEdit>
#include <QDateTime>

MainControlView::MainControlView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainControlView)
    , m_previousZoomValue(50)
{
    ui->setupUi(this);
    
    // Configure size policies for resizable layout
    // Camera widget should expand and fill available space
    ui->cameraWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // Set stretch factors: video gets most space (3), controls get less (1)
    // This allows the video area to expand/contract when window is resized
    ui->topHorizontalLayout->setStretch(0, 3);  // Camera widget (index 0)
    ui->topHorizontalLayout->setStretch(1, 1);  // Control layout (index 1)
    
    // Log area should expand horizontally but have preferred height
    ui->groupSystemLog->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    
    // Set stretch for vertical layout: top area gets most space, log gets less
    ui->mainVerticalLayout->setStretch(0, 3);  // Top horizontal layout (video + controls)
    ui->mainVerticalLayout->setStretch(1, 0);  // Log area (fixed height, no stretch)
    
    setupConnections();
    applyStyles();
}

MainControlView::~MainControlView()
{
    delete ui;
}

void MainControlView::setupConnections()
{
    // Connect direction buttons
    connect(ui->btnUp, &QPushButton::clicked, this, &MainControlView::upButtonClicked);
    connect(ui->btnDown, &QPushButton::clicked, this, &MainControlView::downButtonClicked);
    connect(ui->btnLeft, &QPushButton::clicked, this, &MainControlView::leftButtonClicked);
    connect(ui->btnRight, &QPushButton::clicked, this, &MainControlView::rightButtonClicked);
    
    // Connect zoom slider
    connect(ui->sliderZoom, &QSlider::valueChanged, this, &MainControlView::zoomValueChanged);
    connect(ui->sliderZoom, &QSlider::valueChanged, [this](int value) {
        ui->lblZoomValue->setText(QString::number(value) + "%");
    });
    
    // Connect camera control buttons
    connect(ui->btnConnectCamera, &QPushButton::clicked, this, &MainControlView::connectCameraClicked);
    connect(ui->btnDisconnectCamera, &QPushButton::clicked, this, &MainControlView::disconnectCameraClicked);
}

void MainControlView::applyStyles()
{
    QString buttonStyle = 
        "QPushButton {"
        "background-color: #4CAF50;"
        "color: white;"
        "border: none;"
        "border-radius: 5px;"
        "font-size: 16px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "background-color: #3d8b40;"
        "}";
    
    ui->btnUp->setStyleSheet(buttonStyle);
    ui->btnDown->setStyleSheet(buttonStyle);
    ui->btnLeft->setStyleSheet(buttonStyle);
    ui->btnRight->setStyleSheet(buttonStyle);
    
    // Style for zoom slider
    QString sliderStyle =
        "QSlider::groove:horizontal {"
        "border: 1px solid #999999;"
        "height: 8px;"
        "background: #ddd;"
        "border-radius: 4px;"
        "}"
        "QSlider::handle:horizontal {"
        "background: #4CAF50;"
        "border: 1px solid #45a049;"
        "width: 18px;"
        "height: 18px;"
        "margin: -5px 0;"
        "border-radius: 9px;"
        "}"
        "QSlider::handle:horizontal:hover {"
        "background: #45a049;"
        "}";
    ui->sliderZoom->setStyleSheet(sliderStyle);
    
    QString statusStyle =
        "QLabel {"
        "font-size: 14px;"
        "padding: 5px;"
        "}";
    
    ui->lblCameraStatus->setStyleSheet(statusStyle);
    ui->lblPanTiltStatus->setStyleSheet(statusStyle);
    ui->lblJoystickStatus->setStyleSheet(statusStyle);
    
    // Style for log text area
    QString logStyle =
        "QTextEdit {"
        "background-color: #1e1e1e;"
        "color: #d4d4d4;"
        "border: 1px solid #3e3e3e;"
        "border-radius: 3px;"
        "font-family: 'Consolas', 'Courier New', monospace;"
        "font-size: 11px;"
        "}";
    ui->textLog->setStyleSheet(logStyle);
}

void MainControlView::updateCameraStatus(const QString &status)
{
    ui->lblCameraStatus->setText("Camera: " + status);
}

void MainControlView::updatePanTiltStatus(const QString &status)
{
    ui->lblPanTiltStatus->setText("Pan/Tilt: " + status);
}

void MainControlView::updateJoystickStatus(const QString &status)
{
    ui->lblJoystickStatus->setText("Joystick: " + status);
}

void MainControlView::appendLog(const QString &message)
{
    if (ui->textLog) {
        QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
        ui->textLog->append(QString("[%1] %2").arg(timestamp, message));
        // Auto-scroll to bottom
        QTextCursor cursor = ui->textLog->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->textLog->setTextCursor(cursor);
    }
}

void MainControlView::clearLog()
{
    if (ui->textLog) {
        ui->textLog->clear();
    }
}

