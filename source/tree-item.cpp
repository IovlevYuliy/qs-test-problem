#include "include/tree-item.h"

int TreeItem::GLOBAL_ID_INCREMENT = 1;

TreeItem::TreeItem(const QVector<QVariant> &data, TreeItem *parent, QVariant itemId, QVariant parentId)
    : itemData(data),
      parentItem(parent),
      parentId(parentId)
{
    if (itemId.isNull()) {
        id = GLOBAL_ID_INCREMENT++;
    } else {
        id = itemId;
    }

    if (parent) {
        this->parentId = parent->id;
    }
}

TreeItem::~TreeItem() {
    qDeleteAll(childItems);
}

TreeItem *TreeItem::child(int number) {
    if (number < 0 || number >= childItems.size())
        return nullptr;
    return childItems.at(number);
}

int TreeItem::childCount() const {
    return childItems.count();
}

int TreeItem::childNumber() const {
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));
    return 0;
}

int TreeItem::columnCount() const {
    return itemData.count();
}

QVariant TreeItem::data(int column) const {
    if (column < 0 || column >= itemData.size())
        return QVariant();
    return itemData.at(column);
}

bool TreeItem::insertChildren(int position, int count, int columns) {
    if (position < 0 || position > childItems.size())
        return false;

    for (int row = 0; row < count; ++row) {
        QVector<QVariant> data(columns, "New node");
        TreeItem *item = new TreeItem(data, this);
        childItems.insert(position, item);
    }

    return true;
}

bool TreeItem::insertColumns(int position, int columns) {
    if (position < 0 || position > itemData.size())
        return false;

    for (int column = 0; column < columns; ++column)
        itemData.insert(position, QVariant());

    for (TreeItem *child : qAsConst(childItems))
        child->insertColumns(position, columns);

    return true;
}

QVariant TreeItem::getId() const {
    return id;
}

TreeItem *TreeItem::parent() {
    return parentItem;
}

QVariant TreeItem::getParentId() const {
    return parentId;
}

Qt::ItemFlags TreeItem::getFlags() const {
    if (isRemoved()) {
        return Qt::NoItemFlags;
    }

    Qt::ItemFlags flags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);

    return flags;
}

void TreeItem::resetFlags() {
    removed = false;
}

bool TreeItem::isRemoved() const {
    return removed;
}

bool TreeItem::removeChildren(int position, int count) {
    if (position < 0 || position + count > childItems.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete childItems.takeAt(position);

    return true;
}

bool TreeItem::removeColumns(int position, int columns) {
    if (position < 0 || position + columns > itemData.size())
        return false;

    for (int column = 0; column < columns; ++column)
        itemData.remove(position);

    for (TreeItem *child : qAsConst(childItems))
        child->removeColumns(position, columns);

    return true;
}

bool TreeItem::setData(int column, const QVariant &value) {
    if (column < 0 || column >= itemData.size())
        return false;

    itemData[column] = value;
    return true;
}

void TreeItem::addChild(TreeItem* child) {
    child->parentItem = this;
    childItems.push_back(child);
}

TreeItem* TreeItem::clone() {
    return new TreeItem(itemData, nullptr, id, parentId);
}

TreeItem* TreeItem::getNodeById(const QVariant id) {
    if (this->id == id) {
        return this;
    }

    for (auto child : childItems) {
        auto res = child->getNodeById(id);
        if (res) {
            return res;
        }
    }

    return nullptr;
}

void TreeItem::setChildren(const QVector<TreeItem*>& children) {
    childItems = children;
}

void TreeItem::removeRecursively() {
    removed = true;
    for (auto child : childItems) {
        child->removeRecursively();
    }
}

QVector<TreeItem*> TreeItem::getSubtreeItems() {
    QVector<TreeItem*> items = {this};

    for (auto child : childItems) {
        auto subtree = child->getSubtreeItems();
        items += subtree;
    }

    return items;
}

