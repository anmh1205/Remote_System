#include "CameraWidget.h"
#include <QVideoWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QVideoSink>

CameraWidget::CameraWidget(QWidget *parent)
    : QWidget(parent)
    , m_videoWidget(nullptr)
    , m_videoSink(nullptr)
    , m_placeholderLabel(nullptr)
    , m_layout(nullptr)
{
    setupUI();
}

CameraWidget::~CameraWidget()
{
}

void CameraWidget::setupUI()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    m_videoWidget = new QVideoWidget(this);
    m_videoSink = m_videoWidget->videoSink();
    
    m_placeholderLabel = new QLabel("No camera stream", this);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);
    m_placeholderLabel->setStyleSheet(
        "QLabel {"
        "background-color: #2b2b2b;"
        "color: #ffffff;"
        "font-size: 18px;"
        "}"
    );
    
    m_layout->addWidget(m_videoWidget);
    m_layout->addWidget(m_placeholderLabel);
    
    m_videoWidget->hide();
    m_placeholderLabel->show();
}

void CameraWidget::setPlaceholderText(const QString &text)
{
    if (m_placeholderLabel) {
        m_placeholderLabel->setText(text);
    }
}

