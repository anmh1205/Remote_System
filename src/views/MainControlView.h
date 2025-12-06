#ifndef MAINCONTROLVIEW_H
#define MAINCONTROLVIEW_H

#include <QWidget>
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

signals:
    void upButtonClicked();
    void downButtonClicked();
    void leftButtonClicked();
    void rightButtonClicked();
    void zoomInButtonClicked();
    void zoomOutButtonClicked();

private:
    Ui::MainControlView *ui;
    
    void setupConnections();
    void applyStyles();
};

#endif // MAINCONTROLVIEW_H

