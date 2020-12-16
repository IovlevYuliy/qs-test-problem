#include <QtWidgets>

#include "include/treemodel.h"

TreeModel::TreeModel(QObject *parent) : QAbstractItemModel(parent) {
    rootItem = new TreeItem(QVector<QVariant>{""});
}

TreeModel::~TreeModel() {
    delete rootItem;
}

int TreeModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    TreeItem *item = getItem(index);

    if (role == Qt::FontRole && item->isRemoved()) {
        QFont font;
        font.setStrikeOut(true);
        return QVariant(font);
    }
    if (role == Qt::EditRole) {
        return item->data(index.column());
    }
    if (role != Qt::DisplayRole)
        return QVariant();

    return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;
    return getItem(index)->getFlags();
}

TreeItem *TreeModel::getItem(const QModelIndex &index) const {
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return rootItem;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const {
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();
    TreeItem *parentItem = getItem(parent);
    if (!parentItem)
        return QModelIndex();

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

bool TreeModel::insertColumns(int position, int columns, const QModelIndex &parent) {
    beginInsertColumns(parent, position, position + columns - 1);
    const bool success = rootItem->insertColumns(position, columns);
    endInsertColumns();

    return success;
}

bool TreeModel::insertRows(int position, int rows, const QModelIndex &parent) {
    TreeItem *parentItem = getItem(parent);
    if (!parentItem)
        return false;

    beginInsertRows(parent, position, position + rows - 1);
    const bool success = parentItem->insertChildren(position,
                                                    rows,
                                                    rootItem->columnCount());
    endInsertRows();

    return success;
}

QModelIndex TreeModel::parent(const QModelIndex &index) const {
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = getItem(index);
    TreeItem *parentItem = childItem ? childItem->parent() : nullptr;

    if (parentItem == rootItem || !parentItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool TreeModel::removeColumns(int position, int columns, const QModelIndex &parent) {
    beginRemoveColumns(parent, position, position + columns - 1);
    const bool success = rootItem->removeColumns(position, columns);
    endRemoveColumns();

    if (rootItem->columnCount() == 0)
        removeRows(0, rowCount());

    return success;
}

bool TreeModel::removeRows(int position, int rows, const QModelIndex &parent) {
    TreeItem *parentItem = getItem(parent);
    if (!parentItem)
        return false;

    beginRemoveRows(parent, position, position + rows - 1);
    const bool success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

int TreeModel::rowCount(const QModelIndex &parent) const {
    const TreeItem *parentItem = getItem(parent);

    return parentItem ? parentItem->childCount() : 0;
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid()) {
        return false;
    }

    TreeItem *item = getItem(index);
    bool result = item->setData(index.column(), value);

    if (result)
        emit dataChanged(index, index, {role});

    return result;
}

bool TreeModel::setHeaderData(int section, Qt::Orientation orientation,
                              const QVariant &value, int role) {
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    const bool result = rootItem->setData(section, value);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}

TreeItem* TreeModel::getRoot() const {
    return rootItem;
}

void TreeModel::addNode(const QModelIndex &index) {
    insertRows(0, 1, index);
}

void TreeModel::addNode(TreeItem* node) {
    beginResetModel();

    auto sameNode = rootItem->getNodeById(node->getId());
    if (sameNode) {
        sameNode->setData(0, node->data(0));
        if (!sameNode->parent()->isRemoved()) {
            sameNode->resetFlags();
        }
        delete node;
        return;
    }

    auto parent = rootItem->getNodeById(node->getParentId());
    if (parent) {
        parent->addChild(node);
    } else {
        rootItem->addChild(node);
    }

    QVector<TreeItem*> rootChildren;
    for (uint childIndex = 0; childIndex < rootItem->childCount(); ++childIndex) {
        auto child = rootItem->child(childIndex);
        if (child->getParentId() == node->getId()) {
            node->addChild(child);
        } else {
            rootChildren.push_back(child);
        }
    }
    rootItem->setChildren(rootChildren);

    if (parent && parent->isRemoved()) {
        node->removeRecursively();
    }

    endResetModel();
}

void TreeModel::setDefaultTree(){
    rootItem->insertChildren(0, 1, rootItem->columnCount());
    uint counter = 0;
    for (int i = 0; i < 1; ++i) {
        auto rootChild = rootItem->child(i);
        rootChild->setData(0, "Node " + QString::number(++counter));
        rootChild->insertChildren(0, 2, rootChild->columnCount());
        for (int j = 0; j < 2; ++j) {
            auto firstLevelChild = rootChild->child(j);
            firstLevelChild->setData(0, "Node " + QString::number(++counter));
            firstLevelChild->insertChildren(0, 2, rootChild->columnCount());
            for (int k = 0; k < 2; ++k) {
                auto secondLevelChild = firstLevelChild->child(k);
                secondLevelChild->setData(0, "Node " + QString::number(++counter));
            }
            auto secondLevelChild = firstLevelChild->child(0);
            secondLevelChild->insertChildren(0, 2, rootChild->columnCount());
            for (int k = 0; k < 2; ++k) {
                auto thirdLevelChild = secondLevelChild->child(k);
                thirdLevelChild->setData(0, "Node " + QString::number(++counter));
            }
        }
    }
}

QVector<TreeItem*> TreeModel::getItems() const {
    return rootItem->getSubtreeItems();
}

void TreeModel::syncWith(TreeModel* model) {
    QVector<TreeItem*> treeItems = model->getItems();
    beginResetModel();
    for (auto item: treeItems) {
        if (item->getId() == model->getRoot()->getId()) {
            continue;
        }

        auto syncItem = rootItem->getNodeById(item->getId());
        if (syncItem && syncItem->isRemoved() && !item->isRemoved()) {
            item->removeRecursively();
            model->endResetModel();
            continue;
        }

        if (syncItem && !syncItem->isRemoved()) {
            syncItem->setData(0, item->data(0));
            if (!syncItem->isRemoved() && item->isRemoved()) {
                syncItem->removeRecursively();
            }
        } else if (!syncItem && !item->isRemoved()){
            auto updatedItem = rootItem->getNodeById(item->getParentId());
            if (updatedItem  && !updatedItem->isRemoved()) {
                updatedItem->addChild(item->clone());
            }
        }
    }
    endResetModel();
}

void Delegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
    if (!lineEdit->isModified()) {
        return;
    }
    QString text = lineEdit->text();
    text = text.trimmed();
    if (text.isEmpty()) {
        return;
    } else {
        QItemDelegate::setModelData(editor, model, index);
    }
}

QWidget* Delegate::createEditor(QWidget *parent, const QStyleOptionViewItem&, const QModelIndex&) const {
    QLineEdit* editor = new QLineEdit(parent);
    editor->setFrame(false);
    editor->setStyleSheet("QLineEdit { background-color: white; selection-background-color: rgb(70, 111, 213); }");

    return editor;
}

void Delegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QString value = index.model()->data(index, Qt::EditRole).toString();

    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
    lineEdit->setText(value);
}
