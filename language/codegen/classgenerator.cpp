/* This file is part of KDevelop
    Copyright 2008 Hamish Rodda <rodda@kde.org>

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

#include "classgenerator.h"

#include "duchain/persistentsymboltable.h"
#include "duchain/duchainlock.h"
#include "duchain/duchain.h"
#include "duchain/types/structuretype.h"

#include "codegen/documentchangeset.h"
#include "codegen/codedescription.h"

#include <KLocalizedString>

namespace KDevelop {

struct ClassGeneratorPrivate
{
    QString name; ///< The name for the class to be generated (does not include namespace if relevant)
    QString license;
    QList<DeclarationPointer> inheritedClasses;   ///< Represent *ALL* of the inherited classes

    QMap<QString, KUrl> fileUrls;
    QMap<QString, SimpleCursor> filePositions;

    QString headerFileType;
    QString implementationFileType;
    ClassDescription description;
};

ClassGenerator::ClassGenerator()
: d(new ClassGeneratorPrivate)
{
    d->headerFileType = i18n("Header");
    d->implementationFileType = i18n("Implementation");
}

ClassGenerator::~ClassGenerator()
{
    delete d;
}

QString ClassGenerator::name() const
{
    return d->name;
}

void ClassGenerator::setName(const QString& newName)
{
    d->name = newName;
}

QString ClassGenerator::identifier() const
{
    return name();
}

void ClassGenerator::setIdentifier(const QString& identifier)
{
    setName(identifier);
}

void ClassGenerator::addDeclaration(const DeclarationPointer& newDeclaration)
{
    m_declarations << newDeclaration;
}

QList<DeclarationPointer> ClassGenerator::declarations() const
{
    return m_declarations;
}

QList<DeclarationPointer> ClassGenerator::addBaseClass(const QString&  newBaseClass)
{
    DUChainReadLocker lock;

    bool added = false;
    PersistentSymbolTable::Declarations decl = PersistentSymbolTable::self().getDeclarations(IndexedQualifiedIdentifier(QualifiedIdentifier(newBaseClass)));

    //Search for all super classes
    for(PersistentSymbolTable::Declarations::Iterator it = decl.iterator(); it; ++it)
    {
        DeclarationPointer declaration = DeclarationPointer(it->declaration());
        if(declaration->isForwardDeclaration())
            continue;

        // Check if it's a class/struct/etc
        if(declaration->type<StructureType>())
        {
            fetchSuperClasses(declaration);
            m_baseClasses << declaration;
            added = true;
            break;
        }
    }

    if(!added)
        m_baseClasses << DeclarationPointer(); //Some entities expect that there is always an item added to the list, so just add zero

    return m_baseClasses;
}

QList<DeclarationPointer> ClassGenerator::inheritanceList() const
{
    return d->inheritedClasses;
}

QList<DeclarationPointer> ClassGenerator::directInheritanceList() const
{
    return m_baseClasses;
}

void ClassGenerator::clearInheritance()
{
    m_baseClasses.clear();
    d->inheritedClasses.clear();
}

void ClassGenerator::clearDeclarations()
{
    m_declarations.clear();
}

QStringList ClassGenerator::fileLabels()
{
    return QStringList() << d->headerFileType << d->implementationFileType;
}

QHash< QString, KUrl > ClassGenerator::fileUrlsFromBase (const KUrl& baseUrl, bool toLower)
{
    QHash<QString, KUrl> map;
    map.insert(d->headerFileType, headerUrlFromBase(baseUrl, toLower));
    map.insert(d->implementationFileType, implementationUrlFromBase(baseUrl, toLower));
    return map;
}

KUrl ClassGenerator::headerUrlFromBase(const KUrl& baseUrl, bool toLower)
{
    Q_UNUSED(baseUrl);
    Q_UNUSED(toLower);
    KUrl url;
    url.addPath(d->name);
    return url;
}

KUrl ClassGenerator::implementationUrlFromBase(const KUrl& baseUrl, bool toLower)
{
    Q_UNUSED(baseUrl);
    Q_UNUSED(toLower);
    return KUrl();
}

void ClassGenerator::setFilePosition (const QString& fileType, const SimpleCursor& position)
{
    d->filePositions[fileType] = position;
}


void ClassGenerator::setHeaderPosition(const SimpleCursor& position)
{
    setFilePosition(d->headerFileType, position);
}

void ClassGenerator::setImplementationPosition(const SimpleCursor& position)
{
    setFilePosition(d->implementationFileType, position);
}

void ClassGenerator::setFileUrl (const QString& fileType, const KUrl url)
{
    d->fileUrls[fileType] = url;
}

void ClassGenerator::setHeaderUrl(const KUrl& header)
{
    kDebug() << "Header for the generated class: " << header;
    setFileUrl(d->headerFileType, header);
}

void ClassGenerator::setImplementationUrl(const KUrl& implementation)
{
    kDebug() << "Implementation for the generated class: " << implementation;
    setFileUrl(d->implementationFileType, implementation);
}

SimpleCursor ClassGenerator::headerPosition()
{
    return d->filePositions[d->headerFileType];
}

SimpleCursor ClassGenerator::implementationPosition()
{
    return d->filePositions[d->implementationFileType];
}

KUrl ClassGenerator::headerUrl()
{
    return d->fileUrls[d->headerFileType];
}

KUrl ClassGenerator::implementationUrl()
{
    return d->fileUrls[d->implementationFileType];
}

/// Specify license for this class
void ClassGenerator::setLicense(const QString& license)
{
    kDebug() << "New Class: " << d->name << "Set license: " << d->license;
    d->license = license;
}

/// Get the license specified for this classes
QString ClassGenerator::license() const
{
    return d->license;
}

void ClassGenerator::setDescription(const ClassDescription& description)
{
    d->description = description;
}

ClassDescription ClassGenerator::description()
{
    return d->description;
}

void ClassGenerator::fetchSuperClasses(const DeclarationPointer& derivedClass)
{
    DUChainReadLocker lock;

    //Prevent duplicity
    if(d->inheritedClasses.contains(derivedClass)) {
        return;
    }

    d->inheritedClasses.append(derivedClass);

    DUContext* context = derivedClass->internalContext();
    if (context) {
        foreach (const DUContext::Import& import, context->importedParentContexts()) {
            if (DUContext * parentContext = import.context(context->topContext())) {
                if (parentContext->type() == DUContext::Class) {
                    fetchSuperClasses( DeclarationPointer(parentContext->owner()) );
                }
            }
        }
    }
}

}
