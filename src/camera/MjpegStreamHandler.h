#ifndef MJPEGSTREAMHANDLER_H
#define MJPEGSTREAMHANDLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QImage>
#include <QLabel>
#include <QUrl>
#include <QByteArray>
#include <QString>

/**
 * @brief Handles MJPEG (Motion JPEG) HTTP streams
 * 
 * This class fetches MJPEG streams from HTTP sources, parses multipart MJPEG frames,
 * and emits QImage signals for each decoded frame. It supports both simple JPEG streams
 * and multipart MJPEG streams with boundary markers.
 */
class MjpegStreamHandler : public QObject
{
    Q_OBJECT

public:
    explicit MjpegStreamHandler(QObject *parent = nullptr);
    ~MjpegStreamHandler();
    
    /**
     * @brief Start streaming from the given URL
     * @param url The stream URL
     * @param username Optional username for authentication
     * @param password Optional password for authentication
     */
    void startStream(const QUrl &url, const QString &username = QString(), const QString &password = QString());
    
    /**
     * @brief Stop the current stream
     */
    void stopStream();
    
    /**
     * @brief Check if currently streaming
     */
    bool isStreaming() const;
    
    /**
     * @brief Set the QLabel to display frames directly (optional)
     * @param label The label to display frames in
     */
    void setImageOutput(QLabel *label);

signals:
    /**
     * @brief Emitted when a new frame is received and decoded
     * @param frame The decoded QImage frame
     */
    void frameReceived(const QImage &frame);
    
    /**
     * @brief Emitted when an error occurs
     * @param error Error message
     */
    void error(const QString &error);
    
    /**
     * @brief Emitted when stream connection is established
     */
    void connected();
    
    /**
     * @brief Emitted when stream is disconnected
     */
    void disconnected();
    
    /**
     * @brief Emitted for logging messages
     * @param message Log message
     */
    void logMessage(const QString &message);

private slots:
    void onReadyRead();
    void onFinished();
    void onError(QNetworkReply::NetworkError networkError);

private:
    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_reply;
    QLabel *m_imageLabel;
    QByteArray m_buffer;
    QString m_boundary;
    bool m_isStreaming;
    bool m_hasConnected;
    QUrl m_currentUrl;
    
    /**
     * @brief Process incoming MJPEG data
     * @param data New data received
     */
    void processMjpegData(const QByteArray &data);
    
    /**
     * @brief Parse a multipart MJPEG frame
     * @param data Frame data including boundary markers
     */
    void parseMultipartFrame(const QByteArray &data);
    
    /**
     * @brief Extract boundary from Content-Type header
     * @param contentTypeHeader Content-Type header value
     * @return Boundary string or empty if not found
     */
    QString extractBoundary(const QString &contentTypeHeader);
    
    /**
     * @brief Decode JPEG data to QImage
     * @param jpegData JPEG image data
     * @return Decoded QImage or null if failed
     */
    QImage decodeJpegFrame(const QByteArray &jpegData);
};

#endif // MJPEGSTREAMHANDLER_H

