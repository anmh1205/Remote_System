#include "IpConfigView.h"
#include "models/NetworkConfigModel.h"
#include "network/NetworkUtils.h"
#include <QComboBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QUrl>

IpConfigView::IpConfigView(QWidget *parent)
    : QWidget(parent), ui(new Ui::IpConfigView), m_model(nullptr) {
  ui->setupUi(this);
  setupConnections();
}

IpConfigView::~IpConfigView() { delete ui; }

void IpConfigView::setupConnections() {
  connect(ui->btnSave, &QPushButton::clicked, this,
          &IpConfigView::onSaveClicked);

  // Connect quick URL input
  connect(ui->editQuickUrl, &QLineEdit::editingFinished, this,
          &IpConfigView::onQuickUrlEntered);
  connect(ui->editQuickUrl, &QLineEdit::returnPressed, this,
          &IpConfigView::onQuickUrlEntered);
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

  // Block signals to prevent multiple connectCamera() calls when updating
  // multiple properties at once
  m_model->blockSignals(true);

  // Update model (same as onApplyClicked)
  m_model->setCameraProtocol(cameraProtocol);
  m_model->setCameraIp(cameraIp);
  m_model->setCameraPort(cameraPort);
  m_model->setCameraRtspPath(cameraRtspPath);
  m_model->setCameraUsername(cameraUsername);
  m_model->setCameraPassword(cameraPassword);
  m_model->setPanTiltIp(panTiltIp);
  m_model->setJoystickIp(joystickIp);

  // Unblock signals after batch update
  m_model->blockSignals(false);

  // Emit save for persistence
  emit saveClicked();
}

void IpConfigView::onQuickUrlEntered() {
  QString url = ui->editQuickUrl->text().trimmed();
  if (!url.isEmpty()) {
    parseQuickUrl(url);
  }
}

void IpConfigView::parseQuickUrl(const QString &url) {
  QUrl qurl(url);

  if (!qurl.isValid() || qurl.scheme().isEmpty()) {
    QMessageBox::warning(this, "Invalid URL",
                         QString("Invalid URL format: %1").arg(url));
    return;
  }

  QString scheme = qurl.scheme().toLower();
  QString host = qurl.host();
  int port = qurl.port();
  QString path = qurl.path();
  QString username = qurl.userName();
  QString password = qurl.password();

  // Determine protocol
  if (scheme == "http" || scheme == "https") {
    ui->comboCameraProtocol->setCurrentIndex(0); // HTTP
  } else if (scheme == "rtsp") {
    ui->comboCameraProtocol->setCurrentIndex(1); // RTSP
  } else {
    QMessageBox::warning(this, "Unsupported Protocol",
                         QString("Unsupported protocol: %1\n"
                                 "Supported protocols: http, https, rtsp")
                             .arg(scheme));
    return;
  }

  // Set IP
  if (!host.isEmpty()) {
    ui->editCameraIp->setText(host);
  }

  // Set port (use default if not specified)
  if (port > 0) {
    ui->spinCameraPort->setValue(port);
  } else {
    // Use default port based on protocol
    if (scheme == "rtsp") {
      ui->spinCameraPort->setValue(554); // RTSP default
    } else {
      ui->spinCameraPort->setValue(8080); // HTTP default
    }
  }

  // Set path
  if (!path.isEmpty()) {
    ui->editCameraRtspPath->setText(path);
  } else {
    ui->editCameraRtspPath->clear();
  }

  // Set username and password
  if (!username.isEmpty()) {
    ui->editCameraUsername->setText(
        QUrl::fromPercentEncoding(username.toUtf8()));
  } else {
    ui->editCameraUsername->clear();
  }

  if (!password.isEmpty()) {
    ui->editCameraPassword->setText(
        QUrl::fromPercentEncoding(password.toUtf8()));
  } else {
    ui->editCameraPassword->clear();
  }

  // Clear quick URL field after parsing
  ui->editQuickUrl->clear();
}
