/***************************************************************************
 *   Copyright (C) 2001 by Bernd Gehrmann                                  *
 *   bernd@kdevelop.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qwhatsthis.h>
#include <qvbox.h>
#include <kaction.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <klocale.h>

#include "kdevcore.h"

#include "fileviewfactory.h"
#include "fileviewwidget.h"
#include "fileviewconfigwidget.h"
#include "fileviewpart.h"


FileViewPart::FileViewPart(KDevApi *api, QObject *parent, const char *name)
    : KDevPart(api, parent, name)
{
    setInstance(FileViewFactory::instance());
    //    setXMLFile("kdevfileview.rc");
    
    m_filetree = new FileViewWidget(this);
    m_filetree->setCaption(i18n("File view"));
    QWhatsThis::add(m_filetree, i18n("File View\n\n"
                                     "The file viewer shows all files of the project, "
                                     "ins groups which can be configured by you."));

    core()->embedWidget(m_filetree, KDevCore::SelectView, i18n("Files"));

    connect( core(), SIGNAL(projectOpened()), this, SLOT(refresh()) );
    connect( core(), SIGNAL(projectClosed()), this, SLOT(refresh()) );
    connect( core(), SIGNAL(projectConfigWidget(KDialogBase*)),
             this, SLOT(projectConfigWidget(KDialogBase*)) );
}


FileViewPart::~FileViewPart()
{
    delete m_filetree;
}


void FileViewPart::refresh()
{
    // This method may be called from m_filetree's slot,
    // so make sure not to modify the list view during
    // the execution of the slot
    QTimer::singleShot(0, m_filetree, SLOT(refresh()));
}


void FileViewPart::projectConfigWidget(KDialogBase *dlg)
{
    QVBox *vbox = dlg->addVBoxPage(i18n("File View"));
    FileViewConfigWidget *w = new FileViewConfigWidget(this, vbox, "documentation tree config widget");
    connect( dlg, SIGNAL(okClicked()), w, SLOT(accept()) );
}

#include "fileviewpart.moc"
