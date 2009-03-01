/*
 * This file is part of KDevelop
 * Copyright 2008 Manuel Breugelmans <mbr.nxi@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef VERITAS_CONFIGMODULE_H
#define VERITAS_CONFIGMODULE_H

#include <QList>
#include <QMap>
#include <project/projectkcmodule.h>
#include "veritasconfig.h" // generated by KConfigXT

namespace Veritas
{
class ConfigWidget;
class ITestFramework;

class ConfigModule : public ProjectKCModule<VeritasConfig>
{
Q_OBJECT

public:
    explicit ConfigModule(QWidget* parent = 0, const QVariantList& args = QVariantList());
    virtual ~ConfigModule();

public Q_SLOTS:
    virtual void save();
    virtual void load();

private Q_SLOTS:
    void setDetailsWidgetFor(const QString& framework);

private:
    void initWidgetFrameworkSelectionBox(const QList<Veritas::ITestFramework*>& frameworks);
    void initFrameworkSpecificConfigs(const QList<Veritas::ITestFramework*>& frameworks, const QVariantList& args);

private:
    ConfigWidget* m_widget;
    QMap<QString, QWidget*> m_specificWidgetFor; // additional framework specific config widgets.
                                                 // key is the framework name
};

}

#endif // VERITAS_CONFIGMODULE_H