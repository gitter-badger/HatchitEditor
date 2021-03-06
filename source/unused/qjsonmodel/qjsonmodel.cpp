/***********************************************
    Copyright (C) 2014  Schutz Sacha
    This file is part of QJsonModel (https://github.com/dridk/QJsonmodel).

    QJsonModel is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QJsonModel is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QJsonModel.  If not, see <http://www.gnu.org/licenses/>.

**********************************************/

#include "qjsonmodel.h"
#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QIcon>
#include <QFont>

QJsonModel::QJsonModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    mRootItem = new QJsonTreeItem;
    mHeaders.append("key");
    mHeaders.append("value");
}

Qt::ItemFlags QJsonModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return 0;

    
    QJsonTreeItem* item = static_cast<QJsonTreeItem*>(index.internalPointer());
    if (item)
    {
        if (item->type() != QJsonValue::Array)
        {
            if (index.column() == 1)
                return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
        }
    }
   
    

    return QAbstractItemModel::flags(index);
}

bool QJsonModel::load(const QString &fileName)
{
    QFile file(fileName);
    bool success = false;
    if (file.open(QIODevice::ReadOnly)) {
        success = load(&file);
        file.close();
    }
    else success = false;

    return success;
}

bool QJsonModel::load(QIODevice *device)
{
    return loadJson(device->readAll());
}

bool QJsonModel::loadJson(const QByteArray &json)
{
    mDocument = QJsonDocument::fromJson(json);

    if (!mDocument.isNull())
    {
        beginResetModel();
        if (mDocument.isArray()) {
            mRootItem = QJsonTreeItem::load(QJsonValue(mDocument.array()));
        } else {
            mRootItem = QJsonTreeItem::load(QJsonValue(mDocument.object()));
        }
        endResetModel();
        return true;
    }
    return false;
}

bool QJsonModel::write(const QString & fileName)
{
    /*Need to build a QJsonDocument containing the modified data*/
    QJsonDocument document;
    
    QJsonTreeItem* parent = mRootItem->child(0);
    if (mDocument.isObject())
    {
        document.setObject(buildObject(parent));
    }
    else if (mDocument.isArray())
    {
        document.setArray(buildArray(parent));
    }

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(document.toJson());
        file.close();
        return true;
    }

    return false;
}

QJsonObject QJsonModel::buildObject(QJsonTreeItem * item)
{
    QJsonObject obj;
    obj.insert(item->key(), item->value());
    
    return obj;
}

QJsonArray QJsonModel::buildArray(QJsonTreeItem * item)
{
    QJsonArray array;
    
    array.append(buildObject(item));
    int childCount = item->childCount();
    for (int i = 0; i < childCount; i++)
    {
        array.append(buildArray(item->child(i)));
    }

    return array;
}


QVariant QJsonModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid())
        return QVariant();


    QJsonTreeItem *item = static_cast<QJsonTreeItem*>(index.internalPointer());
    

    if ((role == Qt::DecorationRole) && (index.column() == 0)){

        return mTypeIcons.value(item->type());
    }


    if (role == Qt::DisplayRole) {

        if (index.column() == 0)
            return QString("%1").arg(item->key());

        if (index.column() == 1)
            return QString("%1").arg(item->value());
    }



    return QVariant();

}

QVariant QJsonModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {

        return mHeaders.value(section);
    }
    else
        return QVariant();
}

QModelIndex QJsonModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    QJsonTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = mRootItem;
    else
        parentItem = static_cast<QJsonTreeItem*>(parent.internalPointer());

    

    QJsonTreeItem *childItem = parentItem->child(row);
    if (childItem)
    {
        if (column == 1)
            childItem->setEditable(true);
        return createIndex(row, column, childItem);
    }
    else
        return QModelIndex();
}

QModelIndex QJsonModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    QJsonTreeItem *childItem = static_cast<QJsonTreeItem*>(index.internalPointer());
    QJsonTreeItem *parentItem = childItem->parent();

    if (parentItem == mRootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int QJsonModel::rowCount(const QModelIndex &parent) const
{
    QJsonTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = mRootItem;
    else
        parentItem = static_cast<QJsonTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int QJsonModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 2;
}

void QJsonModel::setIcon(const QJsonValue::Type &type, const QIcon &icon)
{
    mTypeIcons.insert(type,icon);
}

bool QJsonModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (role != Qt::EditRole)
        return false;

    if (value.toString().isEmpty())
        return false;

    QJsonTreeItem* item = static_cast<QJsonTreeItem*>(index.internalPointer());
    if (item)
    {
        if (index.column() == 0)
            item->setKey(value.toString());
        if (index.column() == 1)
            item->setValue(value.toString());

        emit dataChanged(index, index);
        
        return true;
    }

    return false;
}

