/* KDevelop Automake Support
 *
 * Copyright (C)  2005  Matt Rogers <mattr@kde.org>
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
 * Foundation, Inc., 51 Franklin Steet, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef MAKEFILEINTERFACE_H
#define MAKEFILEINTERFACE_H

#include <QObject>

class QFileInfo;
class QDir;

namespace AutoTools
{
    class ProjectAST;
    enum TargetType { Program, Library, LibtoolLibrary, Lisp, Python, Java,
                      Scripts, Data, Headers, ManPages, Texinfo };
    enum InstallLocation { Bin, Libexec, Lib, None, Check, Info, Man };
};

template <typename T> class QList;
template <typename T1, typename T2> class QMap;
template <typename T1, typename T2> class QHash;

typedef QHash<QFileInfo, AutoTools::ProjectAST*> ASTHash;
typedef QMap<QFileInfo, AutoTools::ProjectAST*> ASTMap;
typedef QList<AutoTools::ProjectAST*> ASTList;

uint qHash( const QFileInfo& key );



struct TargetInfo
{
    AutoTools::TargetType type;
    AutoTools::InstallLocation location;
    QString name;
};



class MakefileInterface : public QObject
{
    Q_OBJECT
public:
    enum ParserRecursion { Recursive, NonRecursive };
    enum TargetType { Program, Library, LibtoolLibrary, Lisp, Python, Java,
                      Scripts, Data, Headers, ManPages, Texinfo };
    enum InstallLocations { Bin, Libexec, Lib, None, Check, Info, Man };

    MakefileInterface( QObject* parent );
    ~MakefileInterface();

    static QString canonicalize( const QString& target );

    bool parse( const QDir& dir, ParserRecursion recursive = Recursive );

    QString projectRoot() const;
    void setProjectRoot( const QDir& dir );
    QStringList topSubDirs() const;
    QStringList subdirsFor( const QDir& dir ) const;

    QList<TargetInfo> targetsForFolder( const QDir& dir ) const;

private:
    QStringList subdirsFor( AutoTools::ProjectAST* ) const;
    AutoTools::ProjectAST* astForFolder( const QDir& dir ) const;
    bool isVariable( const QString& item ) const;
    QString resolveVariable( const QString& variable, AutoTools::ProjectAST* ast ) const;

private:
    class Private;
    Private* d;

};

#endif

