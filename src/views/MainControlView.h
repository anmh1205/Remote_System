#ifndef MAINCONTROLVIEW_H
#define MAINCONTROLVIEW_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include "ui_MainControlView.h"
#include "CameraWidget.h"

class MainControlView : public QWidget
{
    Q_OBJECT

public:
    explicit MainControlView(QWidget *parent = nullptr);
    ~MainControlView();
    
    CameraWidget* cameraWidget() const { return ui->cameraWidget; }
    
    void updateCameraStatus(const QString &status);
    void updatePanTiltStatus(const QString &status);
    void updateJoystickStatus(const QString &status);
    void appendLog(const QString &message);
    void clearLog();
    void setCameraIp(const QString &ip);

signals:
    void upButtonClicked();
    void downButtonClicked();
    void leftButtonClicked();
    void rightButtonClicked();
    void zoomValueChanged(int value);
    void connectCameraClicked();
    void disconnectCameraClicked();

private slots:
    void onZoomSliderPressed();
    void onZoomSliderReleased();
    void onZoomSliderMoved(int value);
    void sendZoomRequest();

private:
    Ui::MainControlView *ui;
    int m_previousZoomValue;
    QNetworkAccessManager *m_networkManager;
    QTimer *m_zoomRequestTimer;
    QString m_cameraIp;
    int m_currentZoomValue;
    bool m_isDragging;
    
    void setupConnections();
    void applyStyles();
};

#endif // MAINCONTROLVIEW_H

