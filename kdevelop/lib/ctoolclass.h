/***************************************************************************
                 ctoolclass.h - some methods,that would be normal global,but I
		              hate global function :-)
                             -------------------                                         

    begin                : 20 Jan 1999                        
    copyright            : (C) 1999 by Sandy Meier                         
    email                : smeier@rz.uni-potsdam.de                                     
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/

#ifndef CTOOLCLASS_H
#define CTOOLCLASS_H

/**
 * some methods,that would be normaly global,but I hate global functions :-)
 * @author Sandy Meier
 */
class CToolClass {
public:
  /** search the program in the $PATH*/
  static bool searchProgram(QString name);
  /** same as above but without warning message*/
  static bool searchInstProgram(QString name);
  /** return the absolute filename if found, otherwise ""*/
  static QString findProgram(QString name);

  static QString locatehtml(const QString &filename);

  /** Change a text string for search in a way,
   *  which makes it useable for a regular expression search.
   *  This means converting reg. exp. special chars like $, [, !, ecc.
   *  to \$, \[, \!
   *  @param szOldString   the string, you want to convert.
   *  @param bForGrep      special handling for using resultstring with grep
   */
  static QString escapetext(const char *szOldText, bool bForGrep);
};

#endif


