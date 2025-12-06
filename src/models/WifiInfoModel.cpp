#include "WifiInfoModel.h"

WifiInfoModel::WifiInfoModel(QObject *parent)
    : QObject(parent)
{
}

void WifiInfoModel::refreshNetworks()
{
    // Placeholder: Will be implemented using Windows API or Qt Network
    // For now, just clear and emit signal
    clearNetworks();
    emit networksChanged();
}

void WifiInfoModel::addNetwork(const WifiNetwork &network)
{
    m_networks.append(network);
    emit networksChanged();
}

void WifiInfoModel::clearNetworks()
{
    m_networks.clear();
    emit networksChanged();
}

