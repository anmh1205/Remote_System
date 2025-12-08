#include "MjpegStreamHandler.h"
#include <QDebug>
#include <QNetworkRequest>

MjpegStreamHandler::MjpegStreamHandler(QObject *parent)
    : QObject(parent), m_networkManager(nullptr), m_reply(nullptr),
      m_imageLabel(nullptr), m_isStreaming(false), m_hasConnected(false) {
  m_networkManager = new QNetworkAccessManager(this);
  m_buffer.clear();
  m_boundary.clear();
}

MjpegStreamHandler::~MjpegStreamHandler() { stopStream(); }

void MjpegStreamHandler::startStream(const QUrl &url, const QString &username,
                                     const QString &password) {
  if (m_isStreaming) {
    emit logMessage("MjpegStreamHandler: Already streaming, stopping first");
    stopStream();
  }

  if (!url.isValid()) {
    emit error("Invalid URL");
    return;
  }

  emit logMessage(QString("MjpegStreamHandler: Starting stream from %1")
                      .arg(url.toString()));

  m_currentUrl = url;
  m_buffer.clear();
  m_boundary.clear();
  m_hasConnected = false;

  // Create network request
  QNetworkRequest request(url);
  request.setRawHeader("User-Agent", "RemoteSystem/1.0");

  // Set authentication if provided
  if (!username.isEmpty() && !password.isEmpty()) {
    QString concatenated = username + ":" + password;
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    QString headerData = "Basic " + data;
    request.setRawHeader("Authorization", headerData.toLocal8Bit());
    emit logMessage("MjpegStreamHandler: Using Basic authentication");
  }

  // Start the request
  m_reply = m_networkManager->get(request);

  if (!m_reply) {
    emit error("Failed to create network request");
    return;
  }

  // Connect signals
  connect(m_reply, &QNetworkReply::readyRead, this,
          &MjpegStreamHandler::onReadyRead);
  connect(m_reply, &QNetworkReply::finished, this,
          &MjpegStreamHandler::onFinished);
  connect(
      m_reply,
      QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
      this, &MjpegStreamHandler::onError);

  m_isStreaming = true;
  emit logMessage("MjpegStreamHandler: Stream request sent");

  // Extract boundary from Content-Type header if available
  QVariant contentType = m_reply->header(QNetworkRequest::ContentTypeHeader);
  if (contentType.isValid()) {
    QString contentTypeStr = contentType.toString();
    m_boundary = extractBoundary(contentTypeStr);
    if (!m_boundary.isEmpty()) {
      emit logMessage(
          QString("MjpegStreamHandler: Detected multipart boundary: %1")
              .arg(m_boundary));
    }
  }
}

void MjpegStreamHandler::stopStream() {
  if (m_reply) {
    emit logMessage("MjpegStreamHandler: Stopping stream");
    QNetworkReply *reply = m_reply;
    m_reply = nullptr;

    // Disconnect to avoid slots firing on a deleted reply
    reply->disconnect(this);
    reply->abort();
    reply->deleteLater();
  }

  m_isStreaming = false;
  m_buffer.clear();
  m_boundary.clear();
  emit disconnected();
}

bool MjpegStreamHandler::isStreaming() const {
  return m_isStreaming && m_reply != nullptr;
}

void MjpegStreamHandler::setImageOutput(QLabel *label) { m_imageLabel = label; }

void MjpegStreamHandler::onReadyRead() {
  if (!m_reply) {
    return;
  }

  // First data received -> consider connection established
  if (!m_hasConnected) {
    m_hasConnected = true;
    emit connected();
  }

  QByteArray newData = m_reply->readAll();
  if (newData.isEmpty()) {
    return;
  }

  // Check for Content-Type header on first read
  if (m_boundary.isEmpty()) {
    QVariant contentType = m_reply->header(QNetworkRequest::ContentTypeHeader);
    if (contentType.isValid()) {
      QString contentTypeStr = contentType.toString();
      m_boundary = extractBoundary(contentTypeStr);
      if (!m_boundary.isEmpty()) {
        emit logMessage(
            QString("MjpegStreamHandler: Detected multipart boundary: %1")
                .arg(m_boundary));
      }
    }
  }

  // Process the data
  processMjpegData(newData);
}

void MjpegStreamHandler::onFinished() {
  emit logMessage("MjpegStreamHandler: Stream finished");

  if (m_reply) {
    // If we had already connected and got data, treat as normal stop
    if (m_reply->error() != QNetworkReply::NoError) {
      emit error(QString("Network error: %1").arg(m_reply->errorString()));
    }
    m_reply->deleteLater();
    m_reply = nullptr;
  }

  m_isStreaming = false;
  emit disconnected();
}

void MjpegStreamHandler::onError(QNetworkReply::NetworkError networkError) {
  QString errorString = m_reply ? m_reply->errorString() : "Unknown error";
  emit logMessage(QString("MjpegStreamHandler: Network error %1: %2")
                      .arg(networkError)
                      .arg(errorString));
  emit error(QString("Network error: %1").arg(errorString));

  m_isStreaming = false;
  emit disconnected();
}

void MjpegStreamHandler::processMjpegData(const QByteArray &data) {
  m_buffer.append(data);

  // If we have a boundary, parse multipart frames
  if (!m_boundary.isEmpty()) {
    parseMultipartFrame(m_buffer);
  } else {
    // No boundary - treat as simple JPEG stream
    // Try to find JPEG markers (FF D8 = start, FF D9 = end)
    int startPos = m_buffer.indexOf("\xFF\xD8");
    int endPos = m_buffer.indexOf("\xFF\xD9", startPos);

    if (startPos != -1 && endPos != -1) {
      // Found a complete JPEG frame
      QByteArray jpegData = m_buffer.mid(startPos, endPos - startPos + 2);
      QImage frame = decodeJpegFrame(jpegData);

      if (!frame.isNull()) {
        emit frameReceived(frame);

        // Update label if set
        if (m_imageLabel) {
          QPixmap pixmap = QPixmap::fromImage(frame);
          m_imageLabel->setPixmap(pixmap.scaled(m_imageLabel->size(),
                                                Qt::KeepAspectRatio,
                                                Qt::SmoothTransformation));
        }
      }

      // Remove processed data from buffer
      m_buffer.remove(0, endPos + 2);
    }
  }
}

void MjpegStreamHandler::parseMultipartFrame(const QByteArray &data) {
  if (m_boundary.isEmpty()) {
    return;
  }

  // Look for boundary markers
  QByteArray boundaryMarker = "--" + m_boundary.toUtf8();
  int boundaryPos = m_buffer.indexOf(boundaryMarker);

  while (boundaryPos != -1) {
    // Find the next boundary or end of buffer
    int nextBoundaryPos =
        m_buffer.indexOf(boundaryMarker, boundaryPos + boundaryMarker.size());

    if (nextBoundaryPos == -1) {
      // No complete frame yet, wait for more data
      break;
    }

    // Extract frame data between boundaries
    int frameStart = boundaryPos + boundaryMarker.size();

    // Skip headers (look for double CRLF which separates headers from image
    // data)
    int headerEnd = m_buffer.indexOf("\r\n\r\n", frameStart);
    if (headerEnd == -1) {
      headerEnd = m_buffer.indexOf("\n\n", frameStart);
    }

    if (headerEnd != -1) {
      int imageStart =
          headerEnd + (m_buffer.indexOf("\r\n\r\n", frameStart) != -1 ? 4 : 2);
      int imageEnd = nextBoundaryPos;

      if (imageStart < imageEnd) {
        QByteArray jpegData = m_buffer.mid(imageStart, imageEnd - imageStart);

        // Find JPEG markers
        int jpegStart = jpegData.indexOf("\xFF\xD8");
        int jpegEnd = jpegData.lastIndexOf("\xFF\xD9");

        if (jpegStart != -1 && jpegEnd != -1 && jpegEnd > jpegStart) {
          QByteArray cleanJpeg =
              jpegData.mid(jpegStart, jpegEnd - jpegStart + 2);
          QImage frame = decodeJpegFrame(cleanJpeg);

          if (!frame.isNull()) {
            emit frameReceived(frame);

            // Update label if set
            if (m_imageLabel) {
              QPixmap pixmap = QPixmap::fromImage(frame);
              m_imageLabel->setPixmap(pixmap.scaled(m_imageLabel->size(),
                                                    Qt::KeepAspectRatio,
                                                    Qt::SmoothTransformation));
            }
          }
        }
      }
    }

    // Move to next boundary
    boundaryPos = nextBoundaryPos;
  }

  // Keep only the last incomplete frame in buffer (everything after the last
  // boundary)
  if (boundaryPos != -1) {
    int lastBoundaryPos = m_buffer.lastIndexOf(boundaryMarker);
    if (lastBoundaryPos != -1) {
      m_buffer.remove(0, lastBoundaryPos);
    }
  }
}

QString MjpegStreamHandler::extractBoundary(const QString &contentTypeHeader) {
  // Content-Type: multipart/x-mixed-replace; boundary=--myboundary
  if (contentTypeHeader.contains("multipart", Qt::CaseInsensitive)) {
    int boundaryPos =
        contentTypeHeader.indexOf("boundary=", Qt::CaseInsensitive);
    if (boundaryPos != -1) {
      int start = boundaryPos + 9; // "boundary=" length
      QString boundary = contentTypeHeader.mid(start).trimmed();

      // Remove quotes if present
      if (boundary.startsWith("\"") && boundary.endsWith("\"")) {
        boundary = boundary.mid(1, boundary.length() - 2);
      }

      // Remove leading dashes if present
      while (boundary.startsWith("-")) {
        boundary = boundary.mid(1);
      }

      return boundary;
    }
  }

  return QString();
}

QImage MjpegStreamHandler::decodeJpegFrame(const QByteArray &jpegData) {
  if (jpegData.isEmpty()) {
    return QImage();
  }

  QImage image = QImage::fromData(jpegData, "JPEG");

  if (image.isNull()) {
    emit logMessage("MjpegStreamHandler: Failed to decode JPEG frame");
  }

  return image;
}
