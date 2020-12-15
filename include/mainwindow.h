#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>

#include "treemodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onCacheContextMenuRequested(const QPoint &pos);
    void onDbContextMenuRequested(const QPoint &pos);
    void onAddTreeNode();
    void onRemoveTreeNode();
    void onUploadTreeNode();

    void on_resetButton_clicked();

    void on_saveButton_clicked();

private:
    void initContextMenus();

    QMenu* dbContextMenu;
    QMenu* cacheContextMenu;

    TreeModel* cacheModel;
    TreeModel* dbModel;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
