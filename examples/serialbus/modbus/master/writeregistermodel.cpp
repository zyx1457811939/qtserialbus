/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the QtSerialBus module.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "writeregistermodel.h"

#define ROW_COUNT 10
#define COLUMN_COUNT 2

WriteRegisterModel::WriteRegisterModel(QObject *parent)
    : QAbstractTableModel(parent),
      m_coils(10, false), m_holdingRegisters(10, 0u)
{
}

int WriteRegisterModel::rowCount(const QModelIndex &/*parent*/) const
{
    return ROW_COUNT;
}

int WriteRegisterModel::columnCount(const QModelIndex &/*parent*/) const
{
    return COLUMN_COUNT;
}

QVariant WriteRegisterModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= ROW_COUNT || index.column() >= COLUMN_COUNT)
        return QVariant();

    Q_ASSERT(m_coils.count() == ROW_COUNT);
    Q_ASSERT(m_holdingRegisters.count() == ROW_COUNT);

    if (index.column() == 0 && role == Qt::CheckStateRole) // coils
        return m_coils.at(index.row()) ? Qt::Checked : Qt::Unchecked;
    else if (index.column() == 1 && role != Qt::CheckStateRole) //holding registers
        return QString("0x%1").arg(QString::number(m_holdingRegisters.at(index.row()), 16));

    return QVariant();

}

QVariant WriteRegisterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if (section == 0)
            return QStringLiteral("Coils  ");
        else
            return QStringLiteral("Holding Registers");
    } else {
        return QString::number(section);
    }
}

bool WriteRegisterModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() ||  index.row() >= ROW_COUNT || index.column() >= COLUMN_COUNT)
        return false;

    Q_ASSERT(m_coils.count() == ROW_COUNT);
    Q_ASSERT(m_holdingRegisters.count() == ROW_COUNT);

    if (index.column() == 0 && role == Qt::CheckStateRole) { // coils
        Qt::CheckState s = static_cast<Qt::CheckState>(value.toUInt());
        s == Qt::Checked ? m_coils.setBit(index.row()) : m_coils.clearBit(index.row());
        emit dataChanged(index, index);
        return true;
    } else if (index.column() == 1 && Qt::EditRole) { // holding registers
        bool result = false;
        quint16 newValue = value.toString().toUShort(&result, 16);
        if (result)
            m_holdingRegisters[index.row()] = newValue;

        emit dataChanged(index, index);
        return result;
    }

    return false;
}

Qt::ItemFlags WriteRegisterModel::flags(const QModelIndex &index) const
{
    if (!index.isValid() ||  index.row() >= ROW_COUNT || index.column() >= COLUMN_COUNT)
        return QAbstractTableModel::flags(index);

    if (index.column() == 0) //coils
        return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable;

    if (index.column() == 1) //holding registers
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;

    return QAbstractTableModel::flags(index);
}

