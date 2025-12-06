#include "IpConfigView.h"
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QMessageBox>
#include "network/NetworkUtils.h"

IpConfigView::IpConfigView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::IpConfigView)
    , m_model(nullptr)
{
    ui->setupUi(this);
    setupConnections();
}

IpConfigView::~IpConfigView()
{
    delete ui;
}

void IpConfigView::setupConnections()
{
    connect(ui->btnApply, &QPushButton::clicked, this, &IpConfigView::onApplyClicked);
    connect(ui->btnSave, &QPushButton::clicked, this, &IpConfigView::onSaveClicked);
}

void IpConfigView::setModel(NetworkConfigModel *model)
{
    if (m_model) {
        disconnect(m_model, nullptr, this, nullptr);
    }
    
    m_model = model;
    
    if (m_model) {
        connect(m_model, &NetworkConfigModel::localIpChanged, this, &IpConfigView::onModelChanged);
        connect(m_model, &NetworkConfigModel::subnetMaskChanged, this, &IpConfigView::onModelChanged);
        connect(m_model, &NetworkConfigModel::defaultGatewayChanged, this, &IpConfigView::onModelChanged);
        connect(m_model, &NetworkConfigModel::dnsServerChanged, this, &IpConfigView::onModelChanged);
        connect(m_model, &NetworkConfigModel::cameraIpChanged, this, &IpConfigView::onModelChanged);
        connect(m_model, &NetworkConfigModel::cameraPortChanged, this, &IpConfigView::onModelChanged);
        connect(m_model, &NetworkConfigModel::panTiltIpChanged, this, &IpConfigView::onModelChanged);
        connect(m_model, &NetworkConfigModel::joystickIpChanged, this, &IpConfigView::onModelChanged);
        
        updateFromModel();
    }
}

void IpConfigView::updateFromModel()
{
    if (!m_model) {
        return;
    }
    
    updateUI();
}

void IpConfigView::onModelChanged()
{
    updateUI();
}

void IpConfigView::updateUI()
{
    if (!m_model) {
        return;
    }
    
    ui->editLocalIp->setText(m_model->localIp());
    ui->editSubnetMask->setText(m_model->subnetMask());
    ui->editGateway->setText(m_model->defaultGateway());
    ui->editDns->setText(m_model->dnsServer());
    ui->editCameraIp->setText(m_model->cameraIp());
    ui->spinCameraPort->setValue(m_model->cameraPort());
    ui->editPanTiltIp->setText(m_model->panTiltIp());
    ui->editJoystickIp->setText(m_model->joystickIp());
}

void IpConfigView::onApplyClicked()
{
    if (!m_model) {
        return;
    }
    
    // Validate IP addresses
    QString cameraIp = ui->editCameraIp->text();
    quint16 cameraPort = static_cast<quint16>(ui->spinCameraPort->value());
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
    } else if (!panTiltIp.isEmpty() && !NetworkUtils::isValidIpAddress(panTiltIp)) {
        valid = false;
        errorMsg = "Invalid Pan/Tilt IP address";
    } else if (!joystickIp.isEmpty() && !NetworkUtils::isValidIpAddress(joystickIp)) {
        valid = false;
        errorMsg = "Invalid Joystick IP address";
    }
    
    if (!valid) {
        QMessageBox::warning(this, "Invalid Input", errorMsg);
        return;
    }
    
    // Update model
    m_model->setCameraIp(cameraIp);
    m_model->setCameraPort(cameraPort);
    m_model->setPanTiltIp(panTiltIp);
    m_model->setJoystickIp(joystickIp);
    
    emit applyClicked();
    QMessageBox::information(this, "Success", "Configuration applied successfully");
}

void IpConfigView::onSaveClicked()
{
    onApplyClicked();
    emit saveClicked();
    QMessageBox::information(this, "Success", "Configuration saved successfully");
}

