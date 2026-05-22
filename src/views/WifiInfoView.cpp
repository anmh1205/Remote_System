#include "WifiInfoView.h"
#include "models/WifiInfoModel.h"
#include <QTableWidget>
#include <QPushButton>
#include <QHeaderView>

WifiInfoView::WifiInfoView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WifiInfoView)
    , m_model(nullptr)
{
    ui->setupUi(this);
    setupUI();
}

WifiInfoView::~WifiInfoView()
{
    delete ui;
}

void WifiInfoView::setupUI()
{
    ui->tableWifi->setColumnCount(3);
    ui->tableWifi->setHorizontalHeaderLabels(QStringList() << "SSID" << "Password" << "Status");
    ui->tableWifi->horizontalHeader()->setStretchLastSection(true);
    ui->tableWifi->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWifi->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    connect(ui->btnRefresh, &QPushButton::clicked, this, &WifiInfoView::onRefreshClicked);
}

void WifiInfoView::setModel(WifiInfoModel *model)
{
    if (m_model) {
        disconnect(m_model, &WifiInfoModel::networksChanged, this, &WifiInfoView::onNetworksChanged);
    }
    
    m_model = model;
    
    if (m_model) {
        connect(m_model, &WifiInfoModel::networksChanged, this, &WifiInfoView::onNetworksChanged);
        updateTable();
    }
}

void WifiInfoView::refreshNetworks()
{
    if (m_model) {
        m_model->refreshNetworks();
    }
}

void WifiInfoView::onRefreshClicked()
{
    refreshNetworks();
}

void WifiInfoView::onNetworksChanged()
{
    updateTable();
}

void WifiInfoView::updateTable()
{
    ui->tableWifi->setRowCount(0);
    
    if (!m_model) {
        return;
    }
    
    QList<WifiNetwork> networks = m_model->networks();
    ui->tableWifi->setRowCount(networks.size());
    
    for (int i = 0; i < networks.size(); ++i) {
        const WifiNetwork &network = networks[i];
        
        ui->tableWifi->setItem(i, 0, new QTableWidgetItem(network.ssid));
        
        QString passwordDisplay = network.password.isEmpty() ? "N/A" : "****";
        ui->tableWifi->setItem(i, 1, new QTableWidgetItem(passwordDisplay));
        
        QString status = network.isConnected ? "Connected" : "Available";
        ui->tableWifi->setItem(i, 2, new QTableWidgetItem(status));
    }
}

