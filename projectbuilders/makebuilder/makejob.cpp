/* This file is part of KDevelop
    Copyright 2004 Roberto Raggi <roberto@kdevelop.org>
    Copyright 2007 Andreas Pakulat <apaku@gmx.de>
    Copyright 2007 Dukju Ahn <dukjuahn@gmail.com>
    Copyright 2008 Hamish Rodda <rodda@kde.org>
    Copyright 2012 Ivan Shapovalov <intelfx100@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "makejob.h"

#include <QtCore/QFileInfo>

#include <KDebug>
#include <KShell>
#include <KConfig>
#include <KConfigGroup>
#include <KGlobal>
#include <KLocalizedString>

#include <interfaces/iproject.h>
#include <project/projectmodel.h>
#include <project/interfaces/ibuildsystemmanager.h>

#include "makebuilder.h"

using namespace KDevelop;

MakeJob::MakeJob(MakeBuilder* builder, KDevelop::ProjectBaseItem* item,
                 CommandType c,  const QStringList& overrideTargets,
                 const MakeVariables& variables )
    : OutputExecuteJob(builder)
    , m_builder(builder)
    , m_item(item)
    , m_command(c)
    , m_overrideTargets(overrideTargets)
    , m_variables(variables)
{
    setCapabilities( Killable );
    setFilteringStrategy( OutputModel::CompilerFilter );
    setProperties( NeedWorkingDirectory | PortableMessages | DisplayStderr | IsBuilderHint );

    QString title;
    if( !m_overrideTargets.isEmpty() )
        title = i18n("Make (%1): %2", m_item->text(), m_overrideTargets.join(" "));
    else
        title = i18n("Make (%1)", m_item->text());
    setJobName( title );
    setToolTitle( i18n("Make") );
}

MakeJob::~MakeJob()
{
}

void MakeJob::start()
{
    kDebug(9037) << "Building with make" << m_command << m_overrideTargets.join(" ");
    if (!m_item)
    {
        setError(ItemNoLongerValidError);
        setErrorText(i18n("Build item no longer available"));
        return emitResult();
    }

    if( m_item->type() == KDevelop::ProjectBaseItem::File ) {
        setError(IncorrectItemError);
        setErrorText(i18n("Internal error: cannot build a file item"));
        return emitResult();
    }

    setStandardToolView(IOutputView::BuildView);
    setBehaviours(KDevelop::IOutputView::AllowUserClose | KDevelop::IOutputView::AutoScroll);

    OutputExecuteJob::start();
}

KDevelop::ProjectBaseItem * MakeJob::item() const
{
    return m_item;
}

MakeJob::CommandType MakeJob::commandType()
{
    return m_command;
}

QStringList MakeJob::customTargets() const
{
    return m_overrideTargets;
}

KUrl MakeJob::workingDirectory() const
{
    KDevelop::IBuildSystemManager *bldMan = m_item->project()->buildSystemManager();
    if( bldMan )
        return bldMan->buildDirectory( m_item ); // the correct build dir
    else
    {
        // Just build in-source, where the build directory equals the one with particular target/source.
        for( ProjectBaseItem* item = m_item; item; item = item->parent() ) {
            switch( item->type() ) {
            case KDevelop::ProjectBaseItem::Folder:
            case KDevelop::ProjectBaseItem::BuildFolder:
                return static_cast<KDevelop::ProjectFolderItem*>(item)->url();
                break;
            case KDevelop::ProjectBaseItem::Target:
            case KDevelop::ProjectBaseItem::File:
                break;
            }
        }
        return KUrl();
    }
}

QStringList MakeJob::privilegedExecutionCommand() const
{
    KSharedConfig::Ptr configPtr = m_item->project()->projectConfiguration();
    KConfigGroup builderGroup( configPtr, "MakeBuilder" );

    bool runAsRoot = builderGroup.readEntry( "Install As Root", false );
    if ( runAsRoot && m_command == InstallCommand )
    {
        int suCommand = builderGroup.readEntry( "Su Command", 0 );
        QStringList arguments;
        QString suCommandName;
        switch( suCommand ) {
            case 1:
                return QStringList() << "kdesudo" << "-t";

            case 2:
                return QStringList() << "sudo";

            default:
                return QStringList() << "kdesu" << "-t";
        }
    }
    return QStringList();
}

QStringList MakeJob::commandLine() const
{
    QStringList cmdline;

    KSharedConfig::Ptr configPtr = m_item->project()->projectConfiguration();
    KConfigGroup builderGroup( configPtr, "MakeBuilder" );

#ifdef _MSC_VER
    QString makeBin = builderGroup.readEntry("Make Binary", "nmake");
#else
    QString makeBin = builderGroup.readEntry("Make Binary", "make");
#endif
    cmdline << makeBin;

    if( ! builderGroup.readEntry("Abort on First Error", true) )
    {
        cmdline << "-k";
    }

    int jobnumber = builderGroup.readEntry("Number Of Jobs", 1);
    if(jobnumber>1) {
        QString jobNumberArg = QString("-j%1").arg(jobnumber);
        cmdline << jobNumberArg;
    }

    if( builderGroup.readEntry("Display Only", false) )
    {
        cmdline << "-n";
    }

    QString extraOptions = builderGroup.readEntry("Additional Options", QString());
    if( ! extraOptions.isEmpty() )
    {
        foreach(const QString& option, KShell::splitArgs( extraOptions ) )
            cmdline << option;
    }

    MakeVariables::const_iterator it = m_variables.constBegin();
    while ( it != m_variables.constEnd() )
    {
        cmdline += QString("%1=%2").arg(it->first).arg(it->second);
        ++it;
    }

    if( m_overrideTargets.isEmpty() )
    {
        QString target;
        switch (m_item->type()) {
            case KDevelop::ProjectBaseItem::Target:
            case KDevelop::ProjectBaseItem::ExecutableTarget:
            case KDevelop::ProjectBaseItem::LibraryTarget:
                Q_ASSERT(m_item->target());
                cmdline << m_item->target()->text();
                break;
            case KDevelop::ProjectBaseItem::BuildFolder:
                target = builderGroup.readEntry("Default Target", QString());
                if( !target.isEmpty() )
                    cmdline << target;
                break;
            default: break;
        }
    }else
    {
        cmdline += m_overrideTargets;
    }

    return cmdline;
}

QString MakeJob::environmentProfile() const
{
    KSharedConfig::Ptr configPtr = m_item->project()->projectConfiguration();
    KConfigGroup builderGroup( configPtr, "MakeBuilder" );
    return builderGroup.readEntry( "Default Make Environment Profile", "default" );
}

void MakeJob::setItem( ProjectBaseItem* item )
{
    m_item = item;
}

#include "makejob.moc"
