/* KDevelop Custom Makefile Support
 *
 * Copyright 2007 Dukju Ahn <dukjuahn@gmail.com>
 * Copyright 2011 Milian Wolff <mail@milianw.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef CUSTOMMAKEMANAGER_H
#define CUSTOMMAKEMANAGER_H

#include <project/abstractfilemanagerplugin.h>
#include <project/interfaces/ibuildsystemmanager.h>
#include <interfaces/iplugin.h>
#include <QtCore/QVariant>

template <typename T> class QList;
class QString;

namespace KDevelop
{
    class IProject;
    class ProjectItem;
    class ProjectFolderItem;
    class ProjectBaseItem;
    class ProjectFileItem;
    class ProjectTargetItem;
    class IProjectBuilder;
}

class CustomMakeManager : public KDevelop::AbstractFileManagerPlugin,
                          public KDevelop::IBuildSystemManager
{
    Q_OBJECT
    Q_INTERFACES( KDevelop::IBuildSystemManager )
public:

    explicit CustomMakeManager( QObject *parent = NULL, const QVariantList& args = QVariantList()  );

    virtual ~CustomMakeManager();

    virtual Features features() const { return Features(Folders | Targets | Files); }
    virtual KDevelop::ProjectFolderItem* import(KDevelop::IProject* project);

    /**
     * Provide access to the builder
     */
    virtual KDevelop::IProjectBuilder* builder() const;

    /**
     * Provide a list of include directories.
     */
    virtual KUrl::List includeDirectories(KDevelop::ProjectBaseItem*) const;

    /**
     * Provide a list of files that contain the preprocessor defines for the
     * project
     */
    virtual QHash<QString,QString> defines(KDevelop::ProjectBaseItem*) const;

    /**
     * Create a new target
     *
     * Creates the target specified by @p target to the folder @p parent and
     * modifies the underlying build system if needed
     */
    virtual KDevelop::ProjectTargetItem* createTarget(const QString& target,
                                            KDevelop::ProjectFolderItem *parent);

    /**
     * Add a file to a target
     *
     * Adds the file specified by @p file to the target @p parent and modifies
     * the underlying build system if needed.
     */
    virtual bool addFilesToTarget(const QList<KDevelop::ProjectFileItem*> &files, KDevelop::ProjectTargetItem *parent);

    /**
     * Remove a target
     *
     * Removes the target specified by @p target and
     * modifies the underlying build system if needed.
     */
    virtual bool removeTarget(KDevelop::ProjectTargetItem *target);

    /**
     * Remove a file from a target
     *
     * Removes the file specified by @p file from the folder @p parent and
     * modifies the underlying build system if needed. The file is not removed
     * from the folder it is in
     */
    virtual bool removeFilesFromTargets(const QList<KDevelop::ProjectFileItem*>&);

    /**
     * Get the toplevel build directory for the project
     */
    virtual KUrl buildDirectory(KDevelop::ProjectBaseItem*) const;

    /**
     * Get a list of all the targets in this project
     *
     * The list returned by this function should be checked to verify it is not
     * empty before using it
     *
     * @return The list of targets for this project
     * @todo implement
     */
    virtual QList<KDevelop::ProjectTargetItem*> targets(KDevelop::ProjectFolderItem*) const;

protected:
    virtual bool isValid(const KUrl& url, const bool isFolder, KDevelop::IProject* project) const;
    virtual KDevelop::ProjectFileItem* createFileItem(KDevelop::IProject* project, const KUrl& url, KDevelop::ProjectBaseItem* parent);
    virtual KDevelop::ProjectFolderItem* createFolderItem(KDevelop::IProject* project, const KUrl& url, KDevelop::ProjectBaseItem* parent = 0);

private:
    /**
     * Initialize targets by reading Makefile in @arg dir
     * @return Target lists in Makefile at @arg dir.
     */
    QStringList parseCustomMakeFile( const KUrl &makefile );

private slots:
    void slotDirty(const QString& path);

private:
    class Private;
    Private *d;
};
#endif
