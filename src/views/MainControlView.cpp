#include "MainControlView.h"
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>

MainControlView::MainControlView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainControlView)
{
    ui->setupUi(this);
    setupConnections();
    applyStyles();
}

MainControlView::~MainControlView()
{
    delete ui;
}

void MainControlView::setupConnections()
{
    connect(ui->btnUp, &QPushButton::clicked, this, &MainControlView::upButtonClicked);
    connect(ui->btnDown, &QPushButton::clicked, this, &MainControlView::downButtonClicked);
    connect(ui->btnLeft, &QPushButton::clicked, this, &MainControlView::leftButtonClicked);
    connect(ui->btnRight, &QPushButton::clicked, this, &MainControlView::rightButtonClicked);
    connect(ui->btnZoomIn, &QPushButton::clicked, this, &MainControlView::zoomInButtonClicked);
    connect(ui->btnZoomOut, &QPushButton::clicked, this, &MainControlView::zoomOutButtonClicked);
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
    ui->btnZoomIn->setStyleSheet(buttonStyle);
    ui->btnZoomOut->setStyleSheet(buttonStyle);
    
    QString statusStyle =
        "QLabel {"
        "font-size: 14px;"
        "padding: 5px;"
        "}";
    
    ui->lblCameraStatus->setStyleSheet(statusStyle);
    ui->lblPanTiltStatus->setStyleSheet(statusStyle);
    ui->lblJoystickStatus->setStyleSheet(statusStyle);
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

