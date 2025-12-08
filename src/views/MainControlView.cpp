#include "MainControlView.h"
#include <QDateTime>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QTextEdit>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>

MainControlView::MainControlView(QWidget *parent)
    : QWidget(parent), ui(new Ui::MainControlView), m_previousZoomValue(0),
      m_networkManager(new QNetworkAccessManager(this)),
      m_zoomRequestTimer(new QTimer(this)),
      m_cameraIp(""),
      m_currentZoomValue(0),
      m_isDragging(false) {
  ui->setupUi(this);

  // Configure size policies for resizable layout
  // Camera widget should expand and fill available space
  ui->cameraWidget->setSizePolicy(QSizePolicy::Expanding,
                                  QSizePolicy::Expanding);

  // Set stretch factors: video gets most space (3), controls get less (1)
  // This allows the video area to expand/contract when window is resized
  if (ui->horizontalLayout) {
    ui->horizontalLayout->setStretch(0, 3); // Camera widget (index 0)
    ui->horizontalLayout->setStretch(1, 1); // Control layout (index 1)
  }

  // Set stretch for vertical layout: top area gets most space, log gets less
  if (ui->mainVerticalLayout) {
    ui->mainVerticalLayout->setStretch(0, 3); // Top widget (video + controls)
    ui->mainVerticalLayout->setStretch(1, 0); // Log area (fixed height, no stretch)
  }

  // Log area should expand horizontally but have preferred height
  ui->groupSystemLog->setSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::Preferred);

  setupConnections();
  applyStyles();
}

MainControlView::~MainControlView() { 
  if (m_zoomRequestTimer) {
    m_zoomRequestTimer->stop();
  }
  delete ui; 
}

void MainControlView::setupConnections() {
  // Connect direction buttons
  connect(ui->btnUp, &QPushButton::clicked, this,
          &MainControlView::upButtonClicked);
  connect(ui->btnDown, &QPushButton::clicked, this,
          &MainControlView::downButtonClicked);
  connect(ui->btnLeft, &QPushButton::clicked, this,
          &MainControlView::leftButtonClicked);
  connect(ui->btnRight, &QPushButton::clicked, this,
          &MainControlView::rightButtonClicked);

  // Connect zoom slider
  connect(ui->sliderZoom, &QSlider::valueChanged, this,
          &MainControlView::zoomValueChanged);
  connect(ui->sliderZoom, &QSlider::valueChanged, [this](int value) {
    ui->lblZoomValue->setText(QString::number(value) + "%");
  });
  
  // Connect zoom slider drag events for continuous HTTP requests
  connect(ui->sliderZoom, &QSlider::sliderPressed, this,
          &MainControlView::onZoomSliderPressed);
  connect(ui->sliderZoom, &QSlider::sliderReleased, this,
          &MainControlView::onZoomSliderReleased);
  connect(ui->sliderZoom, &QSlider::sliderMoved, this,
          &MainControlView::onZoomSliderMoved);
  
  // Setup timer for continuous zoom requests (every 40ms while dragging)
  m_zoomRequestTimer->setInterval(40); // 40ms = ~25 requests per second
  m_zoomRequestTimer->setSingleShot(false);
  connect(m_zoomRequestTimer, &QTimer::timeout, this,
          &MainControlView::sendZoomRequest);

  // Connect camera control buttons
  connect(ui->btnConnectCamera, &QPushButton::clicked, this,
          &MainControlView::connectCameraClicked);
  connect(ui->btnDisconnectCamera, &QPushButton::clicked, this,
          &MainControlView::disconnectCameraClicked);

  // Connect clear log button
  connect(ui->btnClearLog, &QPushButton::clicked, this,
          &MainControlView::clearLog);
}

void MainControlView::applyStyles() {
  QString buttonStyle = "QPushButton {"
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
  QString sliderStyle = "QSlider::groove:horizontal {"
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

  QString statusStyle = "QLabel {"
                        "font-size: 14px;"
                        "padding: 5px;"
                        "}";

  ui->lblCameraStatus->setStyleSheet(statusStyle);
  ui->lblPanTiltStatus->setStyleSheet(statusStyle);
  ui->lblJoystickStatus->setStyleSheet(statusStyle);

  // Style for log text area
  QString logStyle = "QTextEdit {"
                     "background-color: #1e1e1e;"
                     "color: #d4d4d4;"
                     "border: 1px solid #3e3e3e;"
                     "border-radius: 3px;"
                     "font-family: 'Consolas', 'Courier New', monospace;"
                     "font-size: 11px;"
                     "}";
  ui->textLog->setStyleSheet(logStyle);
}

void MainControlView::updateCameraStatus(const QString &status) {
  ui->lblCameraStatus->setText("Camera: " + status);
}

void MainControlView::updatePanTiltStatus(const QString &status) {
  ui->lblPanTiltStatus->setText("Pan/Tilt: " + status);
}

void MainControlView::updateJoystickStatus(const QString &status) {
  ui->lblJoystickStatus->setText("Joystick: " + status);
}

void MainControlView::appendLog(const QString &message) {
  if (ui->textLog) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->textLog->append(QString("[%1] %2").arg(timestamp, message));
    // Auto-scroll to bottom
    QTextCursor cursor = ui->textLog->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textLog->setTextCursor(cursor);
  }
}

void MainControlView::clearLog() {
  if (ui->textLog) {
    ui->textLog->clear();
  }
}

void MainControlView::setCameraIp(const QString &ip) {
  m_cameraIp = ip;
}

void MainControlView::onZoomSliderPressed() {
  m_isDragging = true;
  m_currentZoomValue = ui->sliderZoom->value();
  // Start sending requests immediately and then continuously via timer
  sendZoomRequest();
  m_zoomRequestTimer->start();
}

void MainControlView::onZoomSliderReleased() {
  m_isDragging = false;
  m_zoomRequestTimer->stop();
  // Send final request with current value
  m_currentZoomValue = ui->sliderZoom->value();
  sendZoomRequest();
}

void MainControlView::onZoomSliderMoved(int value) {
  m_currentZoomValue = value;
  // Update label immediately
  ui->lblZoomValue->setText(QString::number(value) + "%");
  // The timer will handle sending the HTTP request
}

void MainControlView::sendZoomRequest() {
  if (m_cameraIp.isEmpty()) {
    qDebug() << "MainControlView: Camera IP not set, skipping zoom request";
    return;
  }
  
  // Build URL: http://{camera_ip}:8080/ptz?zoom={value}
  QUrl url;
  url.setScheme("http");
  url.setHost(m_cameraIp);
  url.setPort(8080);
  url.setPath("/ptz");
  
  QUrlQuery query;
  query.addQueryItem("zoom", QString::number(m_currentZoomValue));
  url.setQuery(query);
  
  QNetworkRequest request(url);
  request.setRawHeader("User-Agent", "RemoteSystem/1.0");
  
  // Send GET request
  QNetworkReply *reply = m_networkManager->get(request);
  
  // Handle response (optional - just for logging)
  connect(reply, &QNetworkReply::finished, [this, reply]() {
    if (reply->error() == QNetworkReply::NoError) {
      int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
      if (statusCode == 200) {
        qDebug() << "MainControlView: Zoom request successful, zoom =" << m_currentZoomValue;
      } else {
        qDebug() << "MainControlView: Zoom request returned status" << statusCode;
      }
    } else {
      qDebug() << "MainControlView: Zoom request error:" << reply->errorString();
    }
    reply->deleteLater();
  });
}
