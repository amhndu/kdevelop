/***************************************************************************
 *   Copyright (C) 2001-2002 by Bernd Gehrmann                             *
 *   bernd@kdevelop.org                                                    *
 *   Copyright (C) 2002 by Jakob Simon-Gaarde                              *
 *   jakob@jsg.dk                                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "trollprojectpart.h"

#include <qfileinfo.h>
#include <qwhatsthis.h>
#include <kdebug.h>
#include <kdialogbase.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <qmessagebox.h>
#include <kgenericfactory.h>
#include <kaction.h>

#include "domutil.h"
#include "kdevcore.h"
#include "kdevmainwindow.h"
#include "kdevmakefrontend.h"
#include "kdevappfrontend.h"
#include "kdevpartcontroller.h"
#include "trollprojectwidget.h"
#include "runoptionswidget.h"
#include "config.h"


typedef KGenericFactory<TrollProjectPart> TrollProjectFactory;
K_EXPORT_COMPONENT_FACTORY( libkdevtrollproject, TrollProjectFactory( "kdevtrollproject" ) );

TrollProjectPart::TrollProjectPart(QObject *parent, const char *name, const QStringList& args )
    : KDevProject(parent, name ? name : "TrollProjectPart")
{
    setInstance(TrollProjectFactory::instance());
    
    if ( args.count() == 1 && args[0] == "TMake" )
        m_tmakeProject = true;

    setXMLFile("kdevtrollproject.rc");

    m_widget = new TrollProjectWidget(this);
    m_widget->setIcon(SmallIcon("make"));
    m_widget->setCaption(i18n("Project"));
    QWhatsThis::add(m_widget, i18n("Project Tree\n\n"
                                   "The project tree consists of two parts. The 'overview' "
                                   "in the upper half shows the subprojects, each one having a "
                                   ".pro file. The 'details' view in the lower half shows the "
                                   "targets for the active subproject selected in the overview."));

    mainWindow()->embedSelectViewRight(m_widget, i18n("QMake Manager"), i18n("project manager"));

    KAction *action;

    action = new KAction( i18n("&Build Project"), "make_kdevelop", Key_F8,
                          this, SLOT(slotBuild()),
                          actionCollection(), "build_build" );
    
    action = new KAction( i18n("&Clean Project"), 0,
                          this, SLOT(slotClean()),
                          actionCollection(), "build_clean" );

    action = new KAction( i18n("Execute Program"), "exec", 0,
                          this, SLOT(slotExecute()),
                          actionCollection(), "build_execute" );

    connect( core(), SIGNAL(projectConfigWidget(KDialogBase*)),
             this, SLOT(projectConfigWidget(KDialogBase*)) );
    m_qmakeHeader = i18n("# File generated by kdevelop's qmake manager. \n"
                         "# ------------------------------------------- \n"
                         "# Subdir relative project main directory: %s\n"
                         "# Target is %s %s\n");
}


TrollProjectPart::~TrollProjectPart()
{
    if (m_widget)
        mainWindow()->removeView(m_widget);
    delete m_widget;
}


void TrollProjectPart::projectConfigWidget(KDialogBase *dlg)
{
    QVBox *vbox;
    vbox = dlg->addVBoxPage(i18n("Run Options"));
    RunOptionsWidget *optdlg = new RunOptionsWidget(*projectDom(), "/kdevtrollproject",projectDirectory(), vbox);
    connect( dlg, SIGNAL(okClicked()), optdlg, SLOT(accept()) );
}


void TrollProjectPart::openProject(const QString &dirName, const QString &projectName)
{
    m_widget->openProject(dirName);
    m_projectName = projectName;
}


void TrollProjectPart::closeProject()
{
    m_widget->closeProject();
}


QString TrollProjectPart::projectDirectory()
{
  return m_widget->projectDirectory();
}


QString TrollProjectPart::buildDirectory()
{
  return m_widget->projectDirectory();
}

QString TrollProjectPart::projectName()
{
    return m_projectName;
}


QString TrollProjectPart::mainProgram()
{
   
    QDomDocument &dom = *projectDom();
    return DomUtil::readEntry(dom, "/kdevtrollproject/run/mainprogram");
}


QString TrollProjectPart::activeDirectory()
{
    QDomDocument &dom = *projectDom();

    return DomUtil::readEntry(dom, "/kdevtrollproject/general/activedir");
}


QStringList TrollProjectPart::allFiles()
{
    return m_widget->allFiles();
}


void TrollProjectPart::addFile(const QString &fileName)
{
	QStringList fileList;
	fileList.append ( fileName );
	
	this->addFiles ( fileList );
}

void TrollProjectPart::addFiles ( const QStringList &fileList )
{
	QStringList::ConstIterator it;

	for ( it = fileList.begin(); it != fileList.end(); ++it )
	{
		m_widget->addFile ( *it );
	}

	emit addedFilesToProject ( fileList );
}

void TrollProjectPart::removeFile(const QString & /* fileName */)
{
    // FIXME
/*	QStringList fileList;
	fileList.append ( fileName );
	
	this->removeFiles ( fileList );*/
}

void TrollProjectPart::removeFiles ( const QStringList& fileList )
{
// FIXME
// 	QStringList::ConstIterator it;	
// 	
// 	it = fileList.begin();
//
// 	for ( ; it != fileList.end(); ++it )
// 	{
// 		FIXME
// 	}

	emit removedFilesFromProject ( fileList );
}

void TrollProjectPart::startMakeCommand(const QString &dir, const QString &target)
{
    partController()->saveAllFiles();

    QFileInfo fi(dir + "/Makefile");
    if (!fi.exists()) {
        int r = KMessageBox::questionYesNo(m_widget, i18n("There is no Makefile in this directory. Run qmake first?"));
        if (r == KMessageBox::No)
            return;
        startQMakeCommand(dir);
    }
    QDomDocument &dom = *projectDom();

    if (target=="clean")
    {
      QString cmdline = DomUtil::readEntry(dom, "/kdevtrollproject/make/makebin");
      if (cmdline.isEmpty())
          cmdline = MAKE_COMMAND;
      cmdline += " clean";
      QString dircmd = "cd ";
      dircmd += dir;
      dircmd += " && ";
      makeFrontend()->queueCommand(dir, dircmd + cmdline);
    }

    QString cmdline = DomUtil::readEntry(dom, "/kdevtrollproject/make/makebin");
    if (cmdline.isEmpty())
        cmdline = MAKE_COMMAND;
    if (!DomUtil::readBoolEntry(dom, "/kdevtrollproject/make/abortonerror"))
        cmdline += " -k";
    int jobs = DomUtil::readIntEntry(dom, "/kdevtrollproject/make/numberofjobs");
    if (jobs != 0) {
        cmdline += " -j";
        cmdline += QString::number(jobs);
    }
    if (DomUtil::readBoolEntry(dom, "/kdevtrollproject/make/dontact"))
        cmdline += " -n";

    cmdline += " ";
    cmdline += target;

    QString dircmd = "cd ";
    dircmd += dir;
    dircmd += " && ";

    makeFrontend()->queueCommand(dir, dircmd + cmdline);
}


void TrollProjectPart::startQMakeCommand(const QString &dir)
{
    QFileInfo fi(dir);

    QString cmdline = QString::fromLatin1( isTMakeProject() ? "tmake " : "qmake " );
    cmdline += fi.baseName() + ".pro";
    cmdline += QString::fromLatin1( " -o Makefile" );

    QString dircmd = "cd ";
    dircmd += dir;
    dircmd += " && ";

    makeFrontend()->queueCommand(dir, dircmd + cmdline);
}


void TrollProjectPart::slotBuild()
{
    mainWindow()->raiseView(makeFrontend()->widget());
    startMakeCommand(m_widget->projectDirectory(), QString::fromLatin1(""));
}


void TrollProjectPart::slotClean()
{
    startMakeCommand(m_widget->projectDirectory(), QString::fromLatin1("clean"));
}


void TrollProjectPart::slotExecute()
{
    QString program = mainProgram();
    program += " " + DomUtil::readEntry(*projectDom(), "/kdevtrollproject/run/programargs");

    if ( program.isEmpty() ) {
        KMessageBox::sorry(m_widget, i18n("Please specify the executable name in the "
            "project options dialog first."), i18n("No executable name given"));
        return;
    }

    QDomElement docEl = projectDom()->documentElement();
    QDomElement trollprojectEl = docEl.namedItem("kdevtrollproject").toElement();
    QDomElement envvarsEl = trollprojectEl.namedItem("envvars").toElement();

    QString environstr;
    QDomElement childEl = envvarsEl.firstChild().toElement();
    while (!childEl.isNull()) {
        if (childEl.tagName() == "envvar") {
            environstr += childEl.attribute("name");
            environstr += "=";
            environstr += childEl.attribute("value");
            environstr += " ";
        }
        childEl = childEl.nextSibling().toElement();
    }
    program.prepend(environstr);

    QString dircmd = "cd "+m_widget->projectDirectory() + " && " ;

    bool inTerminal = DomUtil::readBoolEntry(*projectDom(), "/kdevtrollproject/run/terminal");
    appFrontend()->startAppCommand(dircmd + program, inTerminal);
}

void TrollProjectPart::execute(const QString &command)
{
    bool inTerminal = DomUtil::readBoolEntry(*projectDom(), "/kdevtrollproject/run/terminal");
    appFrontend()->startAppCommand(command, inTerminal);
}

void TrollProjectPart::queueCmd(const QString &dir, const QString &cmd)
{
    makeFrontend()->queueCommand(dir, cmd);
}


#include "trollprojectpart.moc"
