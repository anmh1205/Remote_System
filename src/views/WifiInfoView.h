#ifndef WIFIINFOVIEW_H
#define WIFIINFOVIEW_H

#include <QWidget>
#include "ui_WifiInfoView.h"
#include "models/WifiInfoModel.h"

class WifiInfoView : public QWidget
{
    Q_OBJECT

public:
    explicit WifiInfoView(QWidget *parent = nullptr);
    ~WifiInfoView();
    
    void setModel(WifiInfoModel *model);
    void refreshNetworks();

private slots:
    void onRefreshClicked();
    void onNetworksChanged();

private:
    Ui::WifiInfoView *ui;
    WifiInfoModel *m_model;
    
    void setupUI();
    void updateTable();
};

#endif // WIFIINFOVIEW_H

