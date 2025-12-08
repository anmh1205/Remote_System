#ifndef IPCONFIGVIEW_H
#define IPCONFIGVIEW_H

#include "models/NetworkConfigModel.h"
#include "ui_IpConfigView.h"
#include <QWidget>

class IpConfigView : public QWidget {
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
  void onQuickUrlEntered();

private:
  Ui::IpConfigView *ui;
  NetworkConfigModel *m_model;

  void setupConnections();
  void updateUI();
  void parseQuickUrl(const QString &url);
};

#endif // IPCONFIGVIEW_H
