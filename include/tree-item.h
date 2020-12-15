#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVector>
#include <QVariant>

class TreeItem
{
public:
    static int GLOBAL_ID_INCREMENT;

    explicit TreeItem(const QVector<QVariant> &data, TreeItem *parent = nullptr,
                      QVariant itemId = QVariant(),
                      QVariant parentId = QVariant());
    ~TreeItem();

    TreeItem *child(int number);

    int childNumber() const;
    int childCount() const;
    int columnCount() const;
    QVariant getParentId() const;
    QVariant getId() const;
    Qt::ItemFlags getFlags() const;
    bool isRemoved() const;

    QVariant data(int column) const;

    bool insertChildren(int position, int count, int columns);
    bool insertColumns(int position, int columns);
    bool removeChildren(int position, int count);
    bool removeColumns(int position, int columns);

    void resetFlags();
    bool setData(int column, const QVariant &value);
    void setChildren(const QVector<TreeItem*>& children);

    QVector<TreeItem*> getSubtreeItems();
    void addChild(TreeItem* child);
    TreeItem *parent();
    TreeItem *clone();
    TreeItem* getNodeById(const QVariant id);
    void removeRecursively();

private:
    QVector<TreeItem*> childItems;
    QVector<QVariant> itemData;
    TreeItem *parentItem;
    QVariant id;
    QVariant parentId;
    bool removed = false;
};

#endif // TREEITEM_H
