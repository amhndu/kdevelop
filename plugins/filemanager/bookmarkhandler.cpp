/* This file is part of the KDE project
   Copyright (C) xxxx KFile Authors
   Copyright (C) 2002 Anders Lund <anders.lund@lund.tdcadsl.dk>
   Copyright (C) 2009 Dominik Haumann <dhaumann kde org>
   Copyright (C) 2007 Mirko Stocker <me@misto.ch>
   Copyright (C) 2012 Niko Sams <niko.sams@gmai.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "bookmarkhandler.h"
#include "filemanager.h"
#include "kdevfilemanagerplugin.h"
#include "debug.h"
#include <interfaces/icore.h>
#include <interfaces/isession.h>

#include <KActionCollection>


BookmarkHandler::BookmarkHandler( FileManager *parent, QMenu* kpopupmenu )
    : QObject( parent ),
    KBookmarkOwner(),
    m_parent( parent ),
    m_menu( kpopupmenu )
{
    setObjectName( QStringLiteral( "BookmarkHandler" ) );

    QUrl bookmarksPath = KDevelop::ICore::self()->activeSession()->pluginDataArea(parent->plugin());
    bookmarksPath.setPath(bookmarksPath.path() + QLatin1String("fsbookmarks.xml"));
    qCDebug(PLUGIN_FILEMANAGER) << bookmarksPath;

    KBookmarkManager *manager = KBookmarkManager::managerForFile( bookmarksPath.toLocalFile(), QStringLiteral( "kdevplatform" ) );
    manager->setUpdate( true );

    m_bookmarkMenu = new KBookmarkMenu( manager, this, m_menu, parent->actionCollection() );

     //remove shortcuts as they might conflict with others (eg. Ctrl+B)
    foreach (QAction *action, parent->actionCollection()->actions()) {
        action->setShortcut(QKeySequence());
    }
}

BookmarkHandler::~BookmarkHandler()
{
    delete m_bookmarkMenu;
}

QUrl BookmarkHandler::currentUrl() const
{
    return m_parent->dirOperator()->url();
}

QString BookmarkHandler::currentTitle() const
{
    return currentUrl().toDisplayString();
}

void BookmarkHandler::openBookmark( const KBookmark & bm, Qt::MouseButtons, Qt::KeyboardModifiers )
{
    emit openUrl(bm.url());
}
