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
    void applyClicked();
    void saveClicked();

private slots:
    void onApplyClicked();
    void onSaveClicked();
    void onModelChanged();

private:
    Ui::IpConfigView *ui;
    NetworkConfigModel *m_model;
    
    void setupConnections();
    void updateUI();
};

#endif // IPCONFIGVIEW_H

