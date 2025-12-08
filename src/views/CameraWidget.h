#ifndef CAMERAWIDGET_H
#define CAMERAWIDGET_H

#include <QLabel>
#include <QVBoxLayout>
#include <QVideoSink>
#include <QVideoWidget>
#include <QWidget>

/**
 * @brief Widget for displaying camera streams
 *
 * Supports both RTSP streams (via QVideoWidget) and MJPEG streams (via QLabel).
 * Automatically switches between display modes based on stream type.
 */
class CameraWidget : public QWidget {
  Q_OBJECT

public:
  explicit CameraWidget(QWidget *parent = nullptr);
  ~CameraWidget();

  /**
   * @brief Get QVideoWidget for RTSP streams
   * @return QVideoWidget pointer
   */
  QVideoWidget *videoWidget() const { return m_videoWidget; }

  /**
   * @brief Get QVideoSink for RTSP streams
   * @return QVideoSink pointer
   */
  QVideoSink *videoSink() const { return m_videoSink; }

  /**
   * @brief Get QLabel for MJPEG frame display
   * @return QLabel pointer
   */
  QLabel *mjpegLabel() const { return m_mjpegLabel; }

  /**
   * @brief Set placeholder text
   * @param text Placeholder text
   */
  void setPlaceholderText(const QString &text);

  /**
   * @brief Show RTSP video widget
   */
  void showVideoWidget();

  /**
   * @brief Show MJPEG label
   */
  void showMjpegLabel();

  /**
   * @brief Show placeholder
   */
  void showPlaceholder();

signals:
  void streamStatusChanged(bool isActive);

private:
  QVideoWidget *m_videoWidget;
  QVideoSink *m_videoSink;
  QLabel *m_mjpegLabel;
  QLabel *m_placeholderLabel;
  QVBoxLayout *m_layout;

  void setupUI();
};

#endif // CAMERAWIDGET_H
