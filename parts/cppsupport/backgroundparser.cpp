/***************************************************************************
 *   Copyright (C) 2002 by Roberto Raggi                                   *
 *   roberto@kdevelop.org                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "backgroundparser.h"
#include "cppsupportpart.h"
#include "cppsupport_events.h"
#include "codeinformationrepository.h"
#include "cppcodecompletion.h"
#include "driver.h"
#include "ast_utils.h"
#include "kdevdeepcopy.h"

#if QT_VERSION < 0x030100
#include <kdevmutex.h>
#else
#include <qmutex.h>
#endif

#include <kparts/part.h>
#include <ktexteditor/editinterface.h>
#include <ktexteditor/document.h>
#include <ktexteditor/view.h>

#include <kdevpartcontroller.h>
#include <kdevproject.h>

#include <kurl.h>
#include <kdebug.h>
#include <kapplication.h>

#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <qprocess.h>

#include <stdlib.h>
#include <unistd.h>

class KDevDriver: public Driver
{
public:
    KDevDriver( CppSupportPart* cppSupport )
        : m_cppSupport( cppSupport )
    {
	//setupProject();
	//setup();

	addMacro( Macro( "__cplusplus", "1" ) );
    }

    void setupProject()
    {
	QMap<QString, bool> map;

	{
	    QStringList fileList = m_cppSupport->project()->allFiles();
	    QStringList::ConstIterator it = fileList.begin();
	    while( it != fileList.end() ){
		QFileInfo info( *it );
		++it;

		map.insert( info.dirPath(true), true );
	    }
	}

	{
	    QMap<QString, bool>::Iterator it = map.begin();
	    while( it != map.end() ){
		addIncludePath( it.key() );
		++it;
	    }
	}
    }

    // setup the preprocessor
    // code provided by Reginald Stadlbauer <reggie@trolltech.com>
    void setup()
    {
	QString kdedir = getenv( "KDEDIR" );
	if( !kdedir.isNull() )
	    addIncludePath( kdedir + "/include" );

	QString qtdir = getenv( "QTDIR" );
	if( !qtdir.isNull() )
	    addIncludePath( qtdir + "/include" );

	QString qmakespec = getenv( "QMAKESPEC" );
	if ( qmakespec.isNull() )
	    qmakespec = "linux-g++";
	// #### implement other mkspecs and find a better way to find the
	// #### proper mkspec (althoigh this will be no fun :-)

	addIncludePath( qtdir + "/mkspecs/" + qmakespec );
	if ( qmakespec == "linux-g++" ) {
	    addIncludePath( "/include" );
	    addIncludePath( "/usr/include" );
	    addIncludePath( "/ust/local/include" );
#if KDE_VERSION <= 305
	    return; // FIXME!!! Roberto, please review! ;-)
	    // If the QProcess from below is executed,
	    // it somehow breaks the gcc call in parts/outputviews/makewidget.cpp. :-(
	    // It then has the effect that KProcess will never exit, at least on KDE-3.0
#endif // KDE_VERSION
	    QProcess proc;
	    proc.addArgument( "gcc" );
	    proc.addArgument( "-print-file-name=include" );
	    if ( !proc.start() ) {
		qWarning( "Couldn't start gcc" );
		return;
	    }
	    while ( proc.isRunning() )
		usleep( 1 );

	    addIncludePath( proc.readStdout() );
	    addIncludePath( "/usr/include/g++-3" );
	    addIncludePath( "/usr/include/g++" );
	    proc.clearArguments();
	    proc.addArgument( "gcc" );
	    proc.addArgument( "-E" );
	    proc.addArgument( "-dM" );
	    proc.addArgument( "-ansi" );
	    proc.addArgument( "-" );
	    if ( !proc.start() ) {
		qWarning( "Couldn't start gcc" );
		return;
	    }
	    while ( !proc.isRunning() )
		usleep( 1 );
	    proc.closeStdin();
	    while ( proc.isRunning() )
		usleep( 1 );
	    while ( proc.canReadLineStdout() ) {
		QString l = proc.readLineStdout();
		QStringList lst = QStringList::split( ' ', l );
		if ( lst.count() != 3 )
		    continue;
		addMacro( Macro( lst[1], lst[2] ) );
	    }
	    addMacro( Macro( "__cplusplus", "1" ) );
	} else if ( qmakespec == "win32-borland" ) {
	    QString incl = getenv( "INCLUDE" );
	    QStringList includePaths = QStringList::split( ';', incl );
	    QStringList::Iterator it = includePaths.begin();
	    while( it != includePaths.end() ){
		addIncludePath( *it );
		++it;
	    }
	    // ### I am sure there are more standard include paths on
	    // ### windows. I will fix that soon
	    // ### Also do the compiler specific defines on windows
	}
    }

private:
    CppSupportPart* m_cppSupport;
};


class KDevSourceProvider: public SourceProvider
{
public:
    KDevSourceProvider( CppSupportPart* cppSupport ): m_cppSupport( cppSupport ) {}

    virtual QString contents( const QString& fileName )
    {
        //kdDebug(9007) << "-------> kapp is locked = " << kapp->locked() << endl;
        bool needToLock = kapp->locked() == false;

        if( needToLock )
	    kapp->lock();

        //kdDebug(9007) << "-------> kapp locked" << endl;

	QPtrList<KParts::Part> parts( *m_cppSupport->partController()->parts() );
	QPtrListIterator<KParts::Part> it( parts );
	while( it.current() ){
	    KTextEditor::Document* doc = dynamic_cast<KTextEditor::Document*>( it.current() );
	    ++it;

	    KTextEditor::EditInterface* editIface = dynamic_cast<KTextEditor::EditInterface*>( doc );
	    if( !doc || !editIface || doc->url().path() != fileName )
		continue;

	    QString contents = QString( editIface->text().ascii() ); // deep copy

            if( needToLock )
                kapp->unlock();

            //kdDebug(9007) << "-------> kapp unlocked" << endl;

	    return contents;
	}

        if( needToLock )
	    kapp->unlock();
        //kdDebug(9007) << "-------> kapp unlocked" << endl;

	QFile f( fileName );
	QTextStream stream( &f );
	if( f.open(IO_ReadOnly) ){
	    QString contents = stream.read();
	    f.close();
	    return contents;
	}

	return QString::null;
    }

    virtual bool isModified( const QString& fileName )
    {
	Q_UNUSED( fileName );
	return true;
    }

private:
    CppSupportPart*  m_cppSupport;

private:
    KDevSourceProvider( const KDevSourceProvider& source );
    void operator = ( const KDevSourceProvider& source );
};

BackgroundParser::BackgroundParser( CppSupportPart* part, QWaitCondition* consumed )
    : m_consumed( consumed ), m_cppSupport( part ), m_close( false )
{
    m_consumed = 0;
    m_driver = new KDevDriver( m_cppSupport );
    m_driver->setSourceProvider( new KDevSourceProvider(m_cppSupport) );
    //disabled for now m_driver->setResolveDependencesEnabled( true );
}

BackgroundParser::~BackgroundParser()
{
    removeAllFiles();
    delete( m_driver );
    m_driver = 0;
}

void BackgroundParser::addFile( const QString& fileName )
{
    QMutexLocker locker( &m_mutex );
    QString fn = deepCopy( fileName );
    bool added = false;
    if( m_fileList.find(fn) == m_fileList.end() ){
        m_fileList.push_back( fn );
	added = true;
    }

    if( added )
        m_canParse.wakeAll();
}

void BackgroundParser::removeAllFiles()
{
    kdDebug(9007) << "BackgroundParser::removeAllFiles()" << endl;
    QMutexLocker locker( &m_mutex );

    QMap<QString, Unit*>::Iterator it = m_unitDict.begin();
    while( it != m_unitDict.end() ){
        Unit* unit = it.data();
	++it;
	delete( unit );
    }
    m_unitDict.clear();
    m_driver->reset();
    m_fileList.clear();

    m_isEmpty.wakeAll();
}

void BackgroundParser::removeFile( const QString& fileName )
{
    QMutexLocker locker( &m_mutex );

    if( Unit* unit = findUnit(fileName) ){
        m_driver->remove( fileName );
        m_unitDict.remove( fileName );
        delete( unit );
	unit = 0;
    }

    if( m_fileList.isEmpty() )
        m_isEmpty.wakeAll();
}

Unit* BackgroundParser::parseFile( const QString& fileName )
{
    m_driver->remove( fileName );
    m_driver->parseFile( fileName );
    m_driver->removeAllMacrosInFile( fileName );  // romove all macros defined by this
						  // translation unit.
    TranslationUnitAST::Node translationUnit = m_driver->takeTranslationUnit( fileName );

    Unit* unit = new Unit;
    unit->fileName = fileName;
    unit->translationUnit = translationUnit.release();
    unit->problems = m_driver->problems( fileName );

    return unit;
}

Unit* BackgroundParser::findUnit( const QString& fileName )
{
    QMap<QString, Unit*>::Iterator it = m_unitDict.find( fileName );
    return it != m_unitDict.end() ? *it : 0;
}

TranslationUnitAST* BackgroundParser::translationUnit( const QString& fileName )
{
    Unit* u = findUnit( fileName );
    return u ? u->translationUnit : 0;
}

QValueList<Problem> BackgroundParser::problems( const QString& fileName )
{
    Unit* u = findUnit( fileName );
    return u ? u->problems : QValueList<Problem>();
}

void BackgroundParser::close()
{
    QMutexLocker locker( &m_mutex );
    m_close = true;
    m_canParse.wakeAll();
}

bool BackgroundParser::filesInQueue()
{
    QMutexLocker locker( &m_mutex );

    return m_fileList.count();
}

void BackgroundParser::run()
{
    (void) m_cppSupport->codeCompletion()->repository()->getEntriesInScope( QStringList(), false );

    while( !m_close ){

        m_mutex.lock();
	while( !m_fileList.size() ){
            m_canParse.wait( &m_mutex );

            if( m_close ){
                break;
            }
        }

        if( m_close ){
            m_mutex.unlock();
            break;
        }

        QString fileName = deepCopy( m_fileList.front() );
        m_fileList.pop_front();

        m_mutex.unlock();

	Unit* unit = parseFile( fileName );
        {
            QMutexLocker locker( &m_mutex );

	    if( m_unitDict.find(fileName) != m_unitDict.end() ){
	        Unit* u = m_unitDict[ fileName ];
		m_unitDict.remove( fileName );
		delete( u );
		u = 0;
	    }

	    m_unitDict.insert( fileName, unit );

            KApplication::postEvent( m_cppSupport, new FileParsedEvent(fileName, unit->problems) );

	    if( m_consumed )
	        m_consumed->wait();

	    if( m_fileList.isEmpty() )
	        m_isEmpty.wakeAll();
	}
        m_cppSupport->emitFileParsed( fileName );
    }

    kdDebug(9007) << "!!!!!!!!!!!!!!!!!! BG PARSER DESTROYED !!!!!!!!!!!!" << endl;

    QThread::exit();
}

