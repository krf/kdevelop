/**
 * The interface to programming language specific features
 */

#ifndef _KDEVLANGUAGESUPPORT_H_
#define _KDEVLANGUAGESUPPORT_H_

#include <qstring.h>
#include <qstringlist.h>
#include "kdevpart.h"


class KDevLanguageSupport : public KDevPart
{
    Q_OBJECT
    
public:

    enum Features {
        Classes=1, Structs=2, Functions=4, Variables=8,
        Namespaces=16, Signals=32, Slots=64,           /* features of the language itself       */
        NewClass=128, AddMethod=256, AddAttribute=512  /* features of the language support part */
    };
    
    KDevLanguageSupport( KDevApi *api, QObject *parent=0, const char *name=0 );
    ~KDevLanguageSupport();

    /**
     * Returns the feature set of the language. This is e.g. used
     * by the class view to decide which organizer items to display
     * and which not.
     */
    virtual Features features();
    /**
     * Opens a "New class" dialog and adds the configured
     * class to the sources.
     * TODO: Use KDevNode stuff for this
     */
    virtual void addClass();
    /**
     * Opens an "Add method" dialog and adds the configured
     * method to the sources.
     */
    virtual void addMethod(const QString &className);
    /**
     * Opens an "Add attribute" dialog and adds the configured
     * method to the sources.
     */
    virtual void addAttribute(const QString &className);

    /** 
     * returns a typical filterlist  for the support language
     * should be configurable in the languagesupport dialog
     * example "*.cpp;*.h;*.c";
     * used in grepview, adding files
     */
    virtual QStringList fileFilters();

signals:
    /**
     * Emitted when the content of the classtore has been
     * modified
     */
    void updatedSourceInfo();
};


#endif
