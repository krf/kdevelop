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

#ifndef QXCPPUNIT_TESTSUITE_H
#define QXCPPUNIT_TESTSUITE_H

#include <QString>
#include <QFileInfo>
#include <KProcess>

#include "testbase.h"
#include "testcase.h"

namespace QxCppUnit
{

class TestSuite : public TestBase
{
Q_OBJECT
public:
    TestSuite();
    TestSuite(const QString&, const QFileInfo& executable, TestBase* parent);
    virtual ~TestSuite();

    TestCase* testAt(unsigned i);

    virtual int run();
    virtual bool isRunnable() { 
        return true;
    }

private:
    QFileInfo m_exe;
};

} // end namespace QxCppUnit

#endif // QXCPPUNIT_TESTSUITE_H