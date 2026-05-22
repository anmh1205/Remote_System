#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QMenuBar>
#include <QStatusBar>

class MainControlView;
class WifiInfoView;
class IpConfigView;
class MainController;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    MainControlView* mainControlView() const { return m_mainControlView; }
    WifiInfoView* wifiInfoView() const { return m_wifiInfoView; }
    IpConfigView* ipConfigView() const { return m_ipConfigView; }

private slots:
    void showMainControl();
    void showWifiInfo();
    void showIpConfig();

private:
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();
    
    QStackedWidget *m_stackedWidget;
    MainControlView *m_mainControlView;
    WifiInfoView *m_wifiInfoView;
    IpConfigView *m_ipConfigView;
    
    MainController *m_mainController;
};

#endif // MAINWINDOW_H

