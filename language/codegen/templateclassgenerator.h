/*  This file is part of KDevelop
    Copyright 2012 Miha Čančula <miha@noughmad.eu>

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

#ifndef KDEVELOP_TEMPLATECLASSGENERATOR_H
#define KDEVELOP_TEMPLATECLASSGENERATOR_H

#include "classgenerator.h"

namespace KDevelop
{

class KDEVPLATFORMLANGUAGE_EXPORT TemplateClassGenerator : public ClassGenerator
{
public:
    TemplateClassGenerator(const KUrl& baseUrl);
    ~TemplateClassGenerator();
    
    void setTemplateDescription(const QString& templateDescription);
    
    virtual DocumentChangeSet generate();
    virtual QStringList fileLabels();
    virtual QHash< QString, KUrl > fileUrlsFromBase (const KUrl& baseUrl, bool toLower = true);
    virtual QVariantHash templateVariables();
    
    bool hasCustomOptions();
    QByteArray customOptions();
    
    void addVariables(const QVariantHash& variables);
    
    // TODO: Might want to make this a generic utility
    QString renderString(const QString& text);
    
private:
    class TemplateClassGeneratorPrivate* const d;
};

}

#endif // KDEVELOP_TEMPLATECLASSGENERATOR_H
