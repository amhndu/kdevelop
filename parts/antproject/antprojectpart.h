#ifndef _ANTPROJECTPART_H_
#define _ANTPROJECTPART_H_


#include <qstring.h>
#include <qstringlist.h>
#include <qmap.h>


class QPopupMenu;


class KAction;
class KDialogBase;


#include "kdevproject.h"


class AntOptionsWidget;


class AntOptions
{
public:

  AntOptions();

  enum Verbosity { Quiet, Verbose, Debug };

  QString m_buildXML;
  QString m_defaultTarget;
  QStringList m_targets;
  QMap<QString,QString> m_properties;
  QMap<QString,bool> m_defineProperties;
  Verbosity m_verbosity;
  
};


class AntProjectPart : public KDevProject
{
  Q_OBJECT

public:
	  
  AntProjectPart(QObject *parent, const char *name, const QStringList &args);
  ~AntProjectPart();


protected:

  virtual void openProject(const QString &dirName, const QString &projectName);
  virtual void closeProject();

  virtual QString projectDirectory();
  virtual QString projectName();
  virtual QString mainProgram();
  virtual QString activeDirectory();
  virtual QStringList allFiles();
  virtual QString buildDirectory();
  virtual void addFile(const QString &fileName);
  virtual void addFiles ( const QStringList &fileList );
  virtual void removeFile(const QString &fileName);
  virtual void removeFiles ( const QStringList& fileList );


private slots:

  void slotBuild();
  void slotTargetMenuActivated(int id);
  
  void projectConfigWidget(KDialogBase *dlg);
  void contextMenu(QPopupMenu *popup, const Context *context);

  void optionsAccepted();

  void slotAddToProject();
  void slotRemoveFromProject();


private:

  void parseBuildXML();
  void fillMenu();
  void populateProject();

  void ant(const QString &target);

  QString m_projectDirectory, m_projectName;
  QStringList m_classPath;

  QStringList m_sourceFiles;

  AntOptions m_antOptions;

  KAction *m_buildProjectAction;
 
  QPopupMenu *m_targetMenu;

  AntOptionsWidget *m_antOptionsWidget;
  ClassPathWidget *m_classPathWidget;

  QString m_contextFileName;

};


#endif
