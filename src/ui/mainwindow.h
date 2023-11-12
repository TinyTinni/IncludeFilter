#pragma once

#include "../ParseIncludes.h"
#include <QMainWindow>

class QSortFilterProxyModel;

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() noexcept;

  private:
    Ui::MainWindow *ui;
    QSortFilterProxyModel *mSearchModel;
    void loadFromFile(QString filename) noexcept;
    void loadFromMemory(std::string_view mem);
    Parsing parsingMethod() const noexcept;
};
