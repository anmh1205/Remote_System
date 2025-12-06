#ifndef CAMERAWIDGET_H
#define CAMERAWIDGET_H

#include <QWidget>
#include <QVideoWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QVideoSink>

class CameraWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CameraWidget(QWidget *parent = nullptr);
    ~CameraWidget();
    
    QVideoWidget* videoWidget() const { return m_videoWidget; }
    QVideoSink* videoSink() const { return m_videoSink; }
    void setPlaceholderText(const QString &text);

signals:
    void streamStatusChanged(bool isActive);

private:
    QVideoWidget *m_videoWidget;
    QVideoSink *m_videoSink;
    QLabel *m_placeholderLabel;
    QVBoxLayout *m_layout;
    
    void setupUI();
};

#endif // CAMERAWIDGET_H

