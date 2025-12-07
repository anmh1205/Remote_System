#include "IpConfigView.h"
#include "models/NetworkConfigModel.h"
#include "network/NetworkUtils.h"
#include <QApplication>
#include <QComboBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>

IpConfigView::IpConfigView(QWidget *parent)
    : QWidget(parent), ui(new Ui::IpConfigView), m_model(nullptr),
      m_isCheckingConnection(false) {
  ui->setupUi(this);
  setupConnections();
}

IpConfigView::~IpConfigView() { delete ui; }

void IpConfigView::setupConnections() {
  connect(ui->btnSave, &QPushButton::clicked, this,
          &IpConfigView::onSaveClicked);
}

void IpConfigView::setModel(NetworkConfigModel *model) {
  if (m_model) {
    disconnect(m_model, nullptr, this, nullptr);
  }

  m_model = model;

  if (m_model) {
    connect(m_model, &NetworkConfigModel::localIpChanged, this,
            &IpConfigView::onModelChanged);
    connect(m_model, &NetworkConfigModel::subnetMaskChanged, this,
            &IpConfigView::onModelChanged);
    connect(m_model, &NetworkConfigModel::defaultGatewayChanged, this,
            &IpConfigView::onModelChanged);
    connect(m_model, &NetworkConfigModel::dnsServerChanged, this,
            &IpConfigView::onModelChanged);
    connect(m_model, &NetworkConfigModel::cameraProtocolChanged, this,
            &IpConfigView::onModelChanged);
    connect(m_model, &NetworkConfigModel::cameraIpChanged, this,
            &IpConfigView::onModelChanged);
    connect(m_model, &NetworkConfigModel::cameraPortChanged, this,
            &IpConfigView::onModelChanged);
    connect(m_model, &NetworkConfigModel::cameraRtspPathChanged, this,
            &IpConfigView::onModelChanged);
    connect(m_model, &NetworkConfigModel::cameraUsernameChanged, this,
            &IpConfigView::onModelChanged);
    connect(m_model, &NetworkConfigModel::cameraPasswordChanged, this,
            &IpConfigView::onModelChanged);
    connect(m_model, &NetworkConfigModel::panTiltIpChanged, this,
            &IpConfigView::onModelChanged);
    connect(m_model, &NetworkConfigModel::joystickIpChanged, this,
            &IpConfigView::onModelChanged);

    updateFromModel();
  }
}

void IpConfigView::updateFromModel() {
  if (!m_model) {
    return;
  }

  updateUI();
}

void IpConfigView::onModelChanged() { updateUI(); }

void IpConfigView::updateUI() {
  if (!m_model) {
    return;
  }

  ui->editLocalIp->setText(m_model->localIp());
  ui->editSubnetMask->setText(m_model->subnetMask());
  ui->editGateway->setText(m_model->defaultGateway());
  ui->editDns->setText(m_model->dnsServer());
  ui->comboCameraProtocol->setCurrentIndex(
      static_cast<int>(m_model->cameraProtocol()));
  ui->editCameraIp->setText(m_model->cameraIp());
  ui->spinCameraPort->setValue(m_model->cameraPort());
  ui->editCameraRtspPath->setText(m_model->cameraRtspPath());
  ui->editCameraUsername->setText(m_model->cameraUsername());
  ui->editCameraPassword->setText(m_model->cameraPassword());
  ui->editPanTiltIp->setText(m_model->panTiltIp());
  ui->editJoystickIp->setText(m_model->joystickIp());
}

void IpConfigView::onSaveClicked() {
  // Apply first (which includes connection check)
  // But don't emit applyClicked signal to avoid duplicate calls
  if (!m_model) {
    return;
  }

  // Validate IP addresses (same as onApplyClicked)
  int cameraProtocol = ui->comboCameraProtocol->currentIndex();
  QString cameraIp = ui->editCameraIp->text();
  quint16 cameraPort = static_cast<quint16>(ui->spinCameraPort->value());
  QString cameraRtspPath = ui->editCameraRtspPath->text();
  QString cameraUsername = ui->editCameraUsername->text();
  QString cameraPassword = ui->editCameraPassword->text();
  QString panTiltIp = ui->editPanTiltIp->text();
  QString joystickIp = ui->editJoystickIp->text();

  bool valid = true;
  QString errorMsg;

  if (!cameraIp.isEmpty() && !NetworkUtils::isValidIpAddress(cameraIp)) {
    valid = false;
    errorMsg = "Invalid Camera IP address";
  } else if (cameraPort == 0 || cameraPort > 65535) {
    valid = false;
    errorMsg = "Invalid Camera Port (must be 1-65535)";
  } else if (!panTiltIp.isEmpty() &&
             !NetworkUtils::isValidIpAddress(panTiltIp)) {
    valid = false;
    errorMsg = "Invalid Pan/Tilt IP address";
  } else if (!joystickIp.isEmpty() &&
             !NetworkUtils::isValidIpAddress(joystickIp)) {
    valid = false;
    errorMsg = "Invalid Joystick IP address";
  }

  if (!valid) {
    QMessageBox::warning(this, "Invalid Input", errorMsg);
    return;
  }

  // Update model (same as onApplyClicked)
  m_model->setCameraProtocol(cameraProtocol);
  m_model->setCameraIp(cameraIp);
  m_model->setCameraPort(cameraPort);
  m_model->setCameraRtspPath(cameraRtspPath);
  m_model->setCameraUsername(cameraUsername);
  m_model->setCameraPassword(cameraPassword);
  m_model->setPanTiltIp(panTiltIp);
  m_model->setJoystickIp(joystickIp);

  // Check connection and show result (same as onApplyClicked)
  checkCameraConnection();

  // Then save (emit signal for persistence if needed)
  emit saveClicked();
}

void IpConfigView::checkCameraConnection() {
  // Prevent duplicate connection checks
  if (m_isCheckingConnection) {
    return;
  }

  if (!m_model) {
    return;
  }

  m_isCheckingConnection = true;

  QString cameraIp = m_model->cameraIp();
  quint16 cameraPort = m_model->cameraPort();

  if (cameraIp.isEmpty() || cameraPort == 0) {
    m_isCheckingConnection = false;
    QMessageBox::warning(
        this, "Connection Check",
        "Cannot check connection: Camera IP or port is not set.");
    return;
  }

  // Build test URL - MUST match CameraController::buildStreamUrl() logic
  // Use the same values that CameraController will use (from model)
  CameraProtocol protocol = m_model->cameraProtocol();
  QString protocolStr = (protocol == CameraProtocol::HTTP) ? "http" : "rtsp";
  QString path = m_model->cameraRtspPath(); // Already normalized by model
                                            // (starts with / if not empty)
  QString username = m_model->cameraUsername();
  QString password = m_model->cameraPassword();

  // Build URL with optional authentication (same logic as
  // CameraController::buildStreamUrl)
  QString testUrl;
  if (!username.isEmpty() && !password.isEmpty()) {
    // URL encode username and password to handle special characters
    QString encodedUsername = QUrl::toPercentEncoding(username);
    QString encodedPassword = QUrl::toPercentEncoding(password);
    if (path.isEmpty()) {
      testUrl =
          QString("%1://%2:%3@%4:%5")
              .arg(protocolStr, encodedUsername, encodedPassword, cameraIp)
              .arg(cameraPort);
    } else {
      testUrl =
          QString("%1://%2:%3@%4:%5%6")
              .arg(protocolStr, encodedUsername, encodedPassword, cameraIp)
              .arg(cameraPort)
              .arg(path);
    }
  } else {
    if (path.isEmpty()) {
      testUrl =
          QString("%1://%2:%3").arg(protocolStr, cameraIp).arg(cameraPort);
    } else {
      testUrl = QString("%1://%2:%3%4")
                    .arg(protocolStr, cameraIp)
                    .arg(cameraPort)
                    .arg(path);
    }
  }

  // Use QNetworkAccessManager to test HTTP connection
  if (protocol == CameraProtocol::HTTP) {
    // Create checking dialog as pointer so it can be properly closed
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setWindowTitle("Checking Connection");
    msgBox->setText(QString("Testing connection to %1...").arg(testUrl));
    msgBox->setStandardButtons(QMessageBox::NoButton);
    msgBox->setIcon(QMessageBox::Information);
    msgBox->setModal(true);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request = QNetworkRequest(QUrl(testUrl));
    request.setRawHeader("User-Agent", "RemoteSystem/1.0");

    // If username/password are provided, also set authentication header
    if (!username.isEmpty() && !password.isEmpty()) {
      QString concatenated = username + ":" + password;
      QByteArray data = concatenated.toLocal8Bit().toBase64();
      QString headerData = "Basic " + data;
      request.setRawHeader("Authorization", headerData.toLocal8Bit());
    }

    // Set timeout
    QTimer *timeoutTimer = new QTimer(this);
    timeoutTimer->setSingleShot(true);
    timeoutTimer->setInterval(5000); // 5 seconds timeout

    // Show checking dialog
    msgBox->show();
    QApplication::processEvents(); // Process events to show the dialog

    QNetworkReply *reply = manager->get(request);

    // Connect timeout
    connect(timeoutTimer, &QTimer::timeout,
            [msgBox, reply, timeoutTimer, manager, testUrl, this]() {
              timeoutTimer->stop();
              reply->abort();

              // Close checking dialog first
              if (msgBox) {
                msgBox->close();
                msgBox->deleteLater();
              }

              // Then show error
              QMessageBox::warning(
                  this, "Connection Failed",
                  QString("Connection timeout to %1\n\nPlease check:\n"
                          "- Camera IP and port are correct\n"
                          "- Camera is powered on and connected to network\n"
                          "- Stream path is correct (if required)\n"
                          "- Username/password are correct (if required)\n"
                          "- Firewall is not blocking the connection")
                      .arg(testUrl));

              // Reset flag after connection check is complete
              m_isCheckingConnection = false;

              reply->deleteLater();
              timeoutTimer->deleteLater();
              manager->deleteLater();
            });

    // Connect reply
    connect(reply, &QNetworkReply::finished,
            [msgBox, reply, timeoutTimer, manager, testUrl, this]() {
              timeoutTimer->stop();

              // Close checking dialog first
              if (msgBox) {
                msgBox->close();
                msgBox->deleteLater();
              }

              // Then show result
              if (reply->error() == QNetworkReply::NoError) {
                QMessageBox::information(
                    this, "Connection Success",
                    QString("Successfully connected to camera at %1\n\n"
                            "Configuration applied successfully.")
                        .arg(testUrl));
              } else {
                QMessageBox::warning(
                    this, "Connection Failed",
                    QString("Cannot connect to camera at %1\n\nError: "
                            "%2\n\nPlease check:\n"
                            "- Camera IP and port are correct\n"
                            "- Camera is powered on and connected to network\n"
                            "- Stream path is correct (if required)\n"
                            "- Username/password are correct (if required)\n"
                            "- Firewall is not blocking the connection")
                        .arg(testUrl, reply->errorString()));
              }

              // Reset flag after connection check is complete
              m_isCheckingConnection = false;

              reply->deleteLater();
              timeoutTimer->deleteLater();
              manager->deleteLater();
            });

    timeoutTimer->start();
  } else {
    // For RTSP, just show info that connection will be attempted
    QMessageBox::information(this, "Configuration Applied",
                             QString("RTSP configuration applied. Connection "
                                     "will be attempted automatically.\n\n"
                                     "URL: %1")
                                 .arg(testUrl));
    // Reset flag after connection check is complete
    m_isCheckingConnection = false;
  }
}
