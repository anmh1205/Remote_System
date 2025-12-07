#ifndef IPCONFIGVIEW_H
#define IPCONFIGVIEW_H

#include <QWidget>
#include "ui_IpConfigView.h"
#include "models/NetworkConfigModel.h"

class IpConfigView : public QWidget
{
    Q_OBJECT

public:
    explicit IpConfigView(QWidget *parent = nullptr);
    ~IpConfigView();
    
    void setModel(NetworkConfigModel *model);
    void updateFromModel();

signals:
    void saveClicked();

private slots:
    void onSaveClicked();
    void onModelChanged();

private:
    Ui::IpConfigView *ui;
    NetworkConfigModel *m_model;
    bool m_isCheckingConnection;  // Flag to prevent duplicate connection checks
    
    void setupConnections();
    void updateUI();
    void checkCameraConnection();
};

#endif // IPCONFIGVIEW_H

