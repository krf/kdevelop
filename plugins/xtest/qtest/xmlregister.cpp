/* KDevelop xUnit plugin
 *
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

#include "xmlregister.h"

#include "qtestcase.h"
#include "qtestcommand.h"
#include "qtestoutputparser.h"
#include "qtestsuite.h"

#include <KDebug>
#include <KLocalizedString>
#include <KProcess>

#include <QIODevice>

using QTest::QTestCase;
using QTest::QTestCommand;
using QTest::QTestOutputParser;
using QTest::XmlRegister;
using QTest::QTestSuite;

using Veritas::Test;

const QString XmlRegister::c_suite("suite");
const QString XmlRegister::c_case("case");
const QString XmlRegister::c_cmd("command");
const QString XmlRegister::c_root("root");
const QString XmlRegister::c_name("name");
const QString XmlRegister::c_dir("dir");
const QString XmlRegister::c_exe("exe");

XmlRegister::XmlRegister()
        : m_root(""), m_settings(0)
{
    // Data for column headers is stored in the root item.
    QList<QVariant> rootData;
    rootData << i18n("Test Name") << i18n("Result") << i18n("Message")
    << i18n("File Name") << i18n("Line Number");
    m_rootItem = new Test(rootData);
}

XmlRegister::~XmlRegister()
{
}

void XmlRegister::setSource(QIODevice* source)
{
    Q_ASSERT(source);
    setDevice(source);
}

void XmlRegister::setSettings(ISettings* s)
{
    m_settings = s;
}

Veritas::Test* XmlRegister::root() const
{
    return m_rootItem;
}

bool XmlRegister::isStartElement_(const QString& elem)
{
    return isStartElement() && (name() == elem);
}

bool XmlRegister::isEndElement_(const QString& elem)
{
    return isEndElement() && (name() == elem);
}

void XmlRegister::reload()
{
    Q_ASSERT(device());// Q_ASSERT(m_settings);
    device()->close();
    device()->open(QIODevice::ReadOnly);
    if (!device()->isOpen()) {
        kDebug() << "Failed to XML source device for reading.";
        return;
    }
    while (!atEnd()) {
        readNext();
        if (isStartElement_(c_root) && m_root.isEmpty()) {
            m_root = attributes().value(c_dir).toString();
        }
        if (isStartElement_(c_suite)) {
            processSuite();
        }
    }
    kError(hasError()) << errorString() << " @ " << lineNumber() << ":" << columnNumber();
}

void XmlRegister::setRootDir(const QString& root)
{
    m_root = root;
}

void XmlRegister::processSuite()
{
    QTestSuite* suite = new QTestSuite(fetchName(), fetchDir(), m_rootItem);
    m_rootItem->addChild(suite);
    kDebug() << suite->name();

    while (!atEnd() && !isEndElement_(c_suite)) {
        readNext();
        if (isStartElement_(c_case)) {
            QTestCase* caze = instantiateCase(suite);
            processCase(caze);
        }
    }
}

QTestCase* XmlRegister::instantiateCase(QTestSuite* parent)
{
    QTestCase* caze = new QTestCase(fetchName(), fetchExe(), parent);
    parent->addChild(caze);
    caze->setSettings(m_settings);
    caze->setProcess(new KProcess(caze));
    caze->setOutputParser(new QTestOutputParser);
    kDebug() << "Instantiated new QTestCase: " << caze->name();
    return caze;
}

void XmlRegister::processCase(QTestCase* caze)
{
    while (!atEnd() && !isEndElement_(c_case)) {
        readNext();
        if (isStartElement_(c_cmd)) {
            processCmd(caze);
        }
    }
}

void XmlRegister::processCmd(QTestCase* caze)
{
    QTestCommand* cmd = new QTestCommand(fetchName(), caze);
    caze->addChild(cmd);
    kDebug() << "  Instantiated new QTestCommand " 
             << cmd->name() << " " << cmd->command();
}

QString XmlRegister::fetchName()
{
    return attributes().value(c_name).toString();
}

QFileInfo XmlRegister::fetchDir()
{
    QString dir = attributes().value(c_dir).toString();
    if (!m_root.isEmpty()) {
        dir = m_root + dir;
    }
    return QFileInfo(dir);
}

QFileInfo XmlRegister::fetchExe()
{
    return QFileInfo(attributes().value(c_exe).toString());
}