#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QVariant>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QItemDelegate>
#include <QLineEdit>

#include "tree-item.h"

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    TreeModel(QObject *parent = nullptr);
    ~TreeModel();

    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;
    bool setHeaderData(int section, Qt::Orientation orientation,
                       const QVariant &value, int role = Qt::EditRole) override;

    bool insertColumns(int position, int columns,
                       const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int position, int columns,
                       const QModelIndex &parent = QModelIndex()) override;
    bool insertRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex()) override;

    void addNode(const QModelIndex &index);
    void addNode(TreeItem* node);

    TreeItem* getRoot() const;
    QVector<TreeItem*> getItems() const;
    TreeItem *getItem(const QModelIndex &index) const;
    void setDefaultTree();
    void syncWith(TreeModel* model);

private:
    TreeItem *rootItem;
};

class Delegate : public QItemDelegate {
public:
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem&, const QModelIndex&) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
};

#endif // TREEMODEL_H
