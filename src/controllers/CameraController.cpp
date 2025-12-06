#include "CameraController.h"
#include "camera/CameraManager.h"
#include "views/CameraWidget.h"
#include "models/DeviceConfigModel.h"
#include "models/ConnectionStatusModel.h"
#include "network/NetworkUtils.h"
#include <QDebug>

CameraController::CameraController(QObject *parent)
    : QObject(parent)
    , m_cameraManager(nullptr)
    , m_cameraWidget(nullptr)
    , m_deviceConfigModel(nullptr)
    , m_connectionStatusModel(nullptr)
{
    m_cameraManager = new CameraManager(this);
    setupConnections();
}

CameraController::~CameraController()
{
}

void CameraController::setupConnections()
{
    connect(m_cameraManager, &CameraManager::streamStarted,
            this, &CameraController::onStreamStarted);
    connect(m_cameraManager, &CameraManager::streamStopped,
            this, &CameraController::onStreamStopped);
    connect(m_cameraManager, &CameraManager::streamError,
            this, &CameraController::onStreamError);
}

void CameraController::setCameraWidget(CameraWidget *widget)
{
    m_cameraWidget = widget;
    if (m_cameraWidget && m_cameraManager) {
        // Use QVideoWidget if available, otherwise use videoSink
        QVideoWidget *videoWidget = m_cameraWidget->videoWidget();
        if (videoWidget) {
            m_cameraManager->setVideoOutput(videoWidget);
        } else {
            m_cameraManager->setVideoOutput(m_cameraWidget->videoSink());
        }
    }
}

void CameraController::setVideoWidget(QVideoWidget *videoWidget)
{
    if (m_cameraManager && videoWidget) {
        m_cameraManager->setVideoOutput(videoWidget);
    }
}

void CameraController::setDeviceConfigModel(DeviceConfigModel *model)
{
    m_deviceConfigModel = model;
}

void CameraController::setConnectionStatusModel(ConnectionStatusModel *model)
{
    m_connectionStatusModel = model;
}

bool CameraController::startRtspStream(const QString &url)
{
    if (!m_cameraManager) {
        return false;
    }
    
    bool success = m_cameraManager->startRtspStream(url);
    if (success && m_deviceConfigModel) {
        m_deviceConfigModel->setRtspUrl(url);
        m_deviceConfigModel->setCameraType(CameraType::RTSP);
    }
    return success;
}

bool CameraController::startHttpStream(const QString &url)
{
    if (!m_cameraManager) {
        return false;
    }
    
    bool success = m_cameraManager->startHttpStream(url);
    if (success && m_deviceConfigModel) {
        m_deviceConfigModel->setHttpUrl(url);
        m_deviceConfigModel->setCameraType(CameraType::HTTP);
    }
    return success;
}

bool CameraController::startIriunStream(const QString &deviceId)
{
    if (!m_cameraManager) {
        return false;
    }
    
    bool success = m_cameraManager->startIriunStream(deviceId);
    if (success && m_deviceConfigModel) {
        m_deviceConfigModel->setIriunDeviceId(deviceId);
        m_deviceConfigModel->setCameraType(CameraType::Iriun);
    }
    return success;
}

void CameraController::stopStream()
{
    if (m_cameraManager) {
        m_cameraManager->stopStream();
    }
    
    if (m_deviceConfigModel) {
        m_deviceConfigModel->setCameraType(CameraType::None);
    }
}

void CameraController::scanIriunCameras()
{
    // Placeholder: Iriun camera scanning
    // Will use NetworkUtils to discover Iriun cameras
    NetworkUtils *utils = new NetworkUtils(this);
    utils->startIriunDiscovery();
}

void CameraController::onStreamStarted()
{
    if (m_connectionStatusModel) {
        m_connectionStatusModel->setCameraState(ConnectionState::Connected);
    }
    emit streamStatusChanged(true);
}

void CameraController::onStreamStopped()
{
    if (m_connectionStatusModel) {
        m_connectionStatusModel->setCameraState(ConnectionState::Disconnected);
    }
    emit streamStatusChanged(false);
}

void CameraController::onStreamError(const QString &error)
{
    if (m_connectionStatusModel) {
        m_connectionStatusModel->setCameraState(ConnectionState::Error);
    }
    emit streamError(error);
}

