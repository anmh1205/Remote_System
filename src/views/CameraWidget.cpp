#include "CameraWidget.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QVideoSink>
#include <QVideoWidget>

CameraWidget::CameraWidget(QWidget *parent)
    : QWidget(parent), m_videoWidget(nullptr), m_videoSink(nullptr),
      m_mjpegLabel(nullptr), m_placeholderLabel(nullptr), m_layout(nullptr) {
  setupUI();
}

CameraWidget::~CameraWidget() {}

void CameraWidget::setupUI() {
  m_layout = new QVBoxLayout(this);
  m_layout->setContentsMargins(0, 0, 0, 0);

  // Create QVideoWidget for RTSP streams
  m_videoWidget = new QVideoWidget(this);
  m_videoSink = m_videoWidget->videoSink();

  // Create QLabel for MJPEG frames
  m_mjpegLabel = new QLabel(this);
  m_mjpegLabel->setAlignment(Qt::AlignCenter);
  m_mjpegLabel->setScaledContents(false);
  m_mjpegLabel->setStyleSheet("QLabel {"
                              "background-color: #000000;"
                              "}");

  // Create placeholder label
  m_placeholderLabel = new QLabel("No camera stream", this);
  m_placeholderLabel->setAlignment(Qt::AlignCenter);
  m_placeholderLabel->setStyleSheet("QLabel {"
                                    "background-color: #2b2b2b;"
                                    "color: #ffffff;"
                                    "font-size: 18px;"
                                    "}");

  // Add all widgets to layout
  m_layout->addWidget(m_videoWidget);
  m_layout->addWidget(m_mjpegLabel);
  m_layout->addWidget(m_placeholderLabel);

  // Initially show placeholder
  m_videoWidget->hide();
  m_mjpegLabel->hide();
  m_placeholderLabel->show();
}

void CameraWidget::setPlaceholderText(const QString &text) {
  if (m_placeholderLabel) {
    m_placeholderLabel->setText(text);
  }
}

void CameraWidget::showVideoWidget() {
  if (m_videoWidget) {
    m_videoWidget->show();
  }
  if (m_mjpegLabel) {
    m_mjpegLabel->hide();
  }
  if (m_placeholderLabel) {
    m_placeholderLabel->hide();
  }
}

void CameraWidget::showMjpegLabel() {
  if (m_mjpegLabel) {
    m_mjpegLabel->show();
  }
  if (m_videoWidget) {
    m_videoWidget->hide();
  }
  if (m_placeholderLabel) {
    m_placeholderLabel->hide();
  }
}

void CameraWidget::showPlaceholder() {
  if (m_placeholderLabel) {
    m_placeholderLabel->show();
  }
  if (m_videoWidget) {
    m_videoWidget->hide();
  }
  if (m_mjpegLabel) {
    m_mjpegLabel->hide();
  }
}
