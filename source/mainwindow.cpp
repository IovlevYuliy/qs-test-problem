#include "include/mainwindow.h"
#include "./ui_mainwindow.h"
#include "include/treemodel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initContextMenus();
    cacheModel = new TreeModel(this);
    ui->cacheTreeView->setItemDelegate(new Delegate());
    ui->cacheTreeView->setModel(cacheModel);

    dbModel = new TreeModel(this);
    dbModel->setDefaultTree();
    ui->dbTreeView->setModel(dbModel);
    ui->dbTreeView->expandAll();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::initContextMenus() {
    cacheContextMenu = new QMenu();
    cacheContextMenu->addAction(QString("Add"), this, SLOT(onAddTreeNode()));
    cacheContextMenu->addAction(QString("Remove"), this, SLOT(onRemoveTreeNode()));

    ui->cacheTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->cacheTreeView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCacheContextMenuRequested(const QPoint &)));

    dbContextMenu = new QMenu();
    dbContextMenu->addAction(QString("Upload"), this, SLOT(onUploadTreeNode()));

    ui->dbTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->dbTreeView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onDbContextMenuRequested(const QPoint &)));
}

void MainWindow::onCacheContextMenuRequested(const QPoint &pos) {
    QModelIndex index = ui->cacheTreeView->indexAt(pos);
    if (index.isValid() && !cacheModel->getItem(index)->isRemoved()) {
        cacheContextMenu->exec(ui->cacheTreeView->viewport()->mapToGlobal(pos));
    }
}

void MainWindow::onDbContextMenuRequested(const QPoint &pos) {
    QModelIndex index = ui->dbTreeView->indexAt(pos);
    if (index.isValid() && !dbModel->getItem(index)->isRemoved()) {
        dbContextMenu->exec(ui->dbTreeView->viewport()->mapToGlobal(pos));
    }
}

void MainWindow::onAddTreeNode() {
    QModelIndex index = ui->cacheTreeView->currentIndex();
    cacheModel->addNode(index);
}

void MainWindow::onRemoveTreeNode() {
    QModelIndex index = ui->cacheTreeView->currentIndex();
    auto node = cacheModel->getItem(index);
    node->removeRecursively();
}

void MainWindow::onUploadTreeNode() {
    QModelIndex index = ui->dbTreeView->currentIndex();
    auto node = dbModel->getItem(index);
    cacheModel->addNode(node->clone());
    ui->cacheTreeView->expandAll();
}

void MainWindow::on_resetButton_clicked() {
    delete dbModel;
    delete cacheModel;

    cacheModel = new TreeModel(this);
    ui->cacheTreeView->setModel(cacheModel);

    dbModel = new TreeModel(this);
    dbModel->setDefaultTree();
    ui->dbTreeView->setModel(dbModel);
    ui->dbTreeView->expandAll();
}

void MainWindow::on_saveButton_clicked() {
    dbModel->syncWith(cacheModel);
    ui->dbTreeView->expandAll();
    ui->cacheTreeView->expandAll();
}
