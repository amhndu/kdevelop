/***************************************************************************
                     ckdevelop.h - the mainclass in kdevelop   
                             -------------------                                         

    begin                : 20 Jul 1998                                        
    copyright            : (C) 1998 by Sandy Meier                         
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

#ifndef CKDEVELOP_H
#define CKDEVELOP_H

#include "cproject.h"
#include "ctreehandler.h"
#include "ctags/cctags_interf.h"
#include "ctoolclass.h"

#include "resource.h"
#include "kpp/kpp.h"
#include <kdialog.h>
#include <kiconloader.h>
#include <kmainwindow.h>
#include <khelpmenu.h>
#include <kcompletion.h>
#include "widgets/qextmdi/include/qextmdimainfrm.h"

#include <kopenwith.h>
#include <kprocess.h>

#include <qguardedptr.h>
#include <qlist.h>
#include <qstrlist.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

class QSplitter;
class QWhatsThis;
class QTimer;
class QProgressBar;

class CProject;
class CConfigA2psDlg;
class CConfigEnscriptDlg;
class CDocBrowser;
class CClassView;
class DocTreeView;
class CRealFileView;
class CLogFileView;
class CAddExistingFileDlg;
class QListViewItem;
class GrepDialog;
class searchTagsDialogImpl;
class KHTMLView;
class KHTMLPart;
class CParsedMethod;
class CParsedContainer;
class CParsedClass;
class CParsedAttribute;
class KStatusBar;
class CEditWidget;
class COutputWidget;
class CMakeOutputWidget;
class CKDevAccel;
class KProcess;
class KWriteView;
class CKonsoleWidget;
class QStringList;
class KPrinter;

// Debugger classes
class VarViewer;
class DbgController;
class FrameStack;
class BreakpointManager;
class Breakpoint;
class Disassemble;
class DbgToolbar;
class KStatusBarLabel;
class Kpp;

// MDI and related classes
class DocViewMan;
class KDevSession;

// SaveAllDialog class
class SaveAllDialog : public KDialog
{
  Q_OBJECT

  public:
    enum SaveAllResult{Yes=1, No=2, SaveAll=3, Cancel=4};

    SaveAllDialog(const QString& filename, CProject* prj);
    ~SaveAllDialog();

    SaveAllResult result();

  private slots:
    void yes();
    void no();
    void saveAll();
    void cancel();

  private:
    SaveAllResult m_result;

};

/** the mainclass in kdevelop
  *@author Sandy Meier
  */
class CKDevelop : public QextMdiMainFrm {
  Q_OBJECT
public:
  /**constructor*/
  CKDevelop();

  /**destructor*/
  ~CKDevelop();

  /** initializer functions called by the constructor */
  void initView();
  void initConnections();
  void initMenuBar();
  void initToolBar();
  void initStatusBar();
  void initWhatsThis();
  CProject* initProject(bool ignoreLastProject);
  void initDebugger();
  void completeStartup(bool ignoreLastProject);


  /** Change a text string for search in a way,
   *  which makes it useable for a regular expression search.
   *  This means converting reg. exp. special chars like $, [, !, ecc.
   *  to \$, \[, \!
   *  @param szOldString   the string, you want to convert.
   *  @param bForGrep      special handling for using resultstring with grep
   */
  QString realSearchText2regExp(const char *szOldString, bool bForGrep=false);

  /*
     parses only the files listed in the stringlist
  */
  void refreshClassViewByFileList(QStrList * iFileList);
  /** Checks if the filename passed as param
   *   is an Untitled (means new file generated by KDevelop)
   *
   *  @param szFilename   the filename you want to test
   */
  static bool isUntitled(const char *szFilename);
  /** The complete file save as handling
   *  @return true if it succeeded
   */
  bool fileSaveAs();

  void refreshTrees(QStrList * iFileList = NULL);
  void refreshTrees(TFileInfo *info);

  void setKeyAccel();
  void setToolmenuEntries();
	
  /** sets the Main window caption for KDevelop */
  void setMainCaption(int item=-1);
  			
  void newFile(bool add_to_project,const char* dir=0);

  /** prepare readProjectFile(..), creates and inits the project object */
  CProject* prepareToReadProjectFile(QString file);
  /** read the projectfile from the disk (time consuming) */
  void readProjectFile(QString file, CProject* lNewProject);

  /** Add a file with a specified type to the project. 
   *  
   *  @param complete_filename   The absolute filename.
   *  @param type                Type of file.
   *  @param refresh             If to refresh the trees.
   *  @return true if a new subdir was added.
   */
  bool addFileToProject(QString complete_filename, ProjectFileType type, bool refreshTrees=true);

  // Mewthods for manipulating the recent project menu
  void addRecentProject(const QString& file);
  void shuffleProjectToTop(int id);
  QString getProjectAsString(int id);
  void setupRecentProjectMenu();
  void saveRecentProjectMenu();

  void switchToWorkspace(int id);

  /** Switch the view to a certain file.
   * @param filename the absolute filename
   * @param bForceReload if true then enforce updating widget text from file
   * @param bShowModifiedBox if true no messagebox is shown, if the file was modified outside the editor
   */
  void switchToFile(QString filename, int line = -1, int col = 0,
                    bool bForceReload=false,bool bShowModifiedBox=true);

  /** Helper function to get the config */
  KConfig* getConfig() { return config; };

  /** Switch to a certain line in a certain file.
   *  @param filename Absolute filename of the file to switch to.
   *  @param lineNo   The line in the file to switch to.
   */
//  void switchToFile(QString filename, int lineNo);

  /** set the correct toolbar and menubar,if a process is running
    * @param enable if true than enable,otherwise disable
    */
  void setToolMenuProcess(bool enable);
  void setDebugMenuProcess(bool enable);

  /** Get the current project. */
  CProject* getProject() { return prj; };

  /** Get the name of the current project if any */
  QString getProjectName();

  /** Is there a current project */
  bool hasProject() { return project; };

  /** Create a SaveAllDialog to save project files */
  SaveAllDialog::SaveAllResult doProjectSaveAllDialog(const QString &filename);

  /** checks the project files
   *  if there is one file modified or younger than the binary
   */
  bool isProjectDirty();

  /** Get the breakpoint manager */
  BreakpointManager* getBrkptManager() { return brkptManager; };
  VarViewer* getVarViewer() { return var_viewer; };
  QWhatsThis* getWhatsThis() { return whats_this; };

  /** shared helper method for slotViewTTreeView and slotViewTOutputView */
  void toggleGroupOfToolViewCovers(int type, QList<KDockWidget>* pToolViewCoverList);
  /** First part of opening a .kdevprj project from file using prjfile as filename.
   *  Between part 1 and 2 we can place other stuff
   *  (e.g.: as it is needed during KDevelop's startup)
   */
  CProject* projectOpenCmdl_Part1(QString prjfile);
  /** Second part of opening a .kdevprj project from file using prjfile as filename. */
  void projectOpenCmdl_Part2(CProject* pProj);

 public slots:

  void enableCommand(int id_);
  void disableCommand(int id_);

  ////////////////////////
  // FILE-Menu entries
  ///////////////////////
 
  /** generate a new file*/
  void slotFileNew();
  /** generate a new file in dir*/
  void slotFileNew(const char* dir);
  /**open a file*/
  void slotFileOpen();
  /** opens a file from the file_open_popup that is a delayed popup menu 
   *installed in the open file button of the toolbar */
  void slotFileOpen( int id_ );
  /** close the current file*/
  void slotFileClose();
  /** closes all KWrite documents and their views but not the document browser views */
  void slotFileCloseAll();
  /** save the current file,if Untitled a dialog ask for a valid name*/
  void slotFileSave();
  /** save all files*/
  void slotFileSaveAll();
  /** save the current file under a different filename*/
  void slotFileSaveAs();
  /** opens the printing dialog */
  void slotFilePrint();
  /** quit kdevelop*/
  void slotFileQuit();

  ////////////////////////
  // EDIT-Menu entries
  ///////////////////////
  /** acts on grep to search the selected word by keyboard shortcut */
  void slotEditSearchText();
  /** search in files, use grep and find*/
  void slotEditSearchInFiles();
  /** called by popups in the edit and brwoser widgets to grep a string */
  void slotEditSearchInFiles(QString);
  /** runs ispell check on the actual editwidget */
  /*  void slotEditSpellcheck(); */
  /** opens the search and replace dialog */
  void slotEditReplace();
  void slotEditIndent();
  void slotEditUnindent();
  void slotEditComment();
  void slotEditUncomment();
  
  ////////////////////////
  // VIEW-Menu entries
  ///////////////////////
  /** opens the goto line dialog */
  void slotViewGotoLine();
  /** jump to the next error, based on the make output*/
  void slotViewNextError();
  /** jump to the previews error, based on the make output*/
  void slotViewPreviousError();
  /** dis-/enables the treeview */
  void slotViewTTreeView();
  /** dis-/enables the outputview */
  void slotViewTOutputView();
  void showOutputView(bool show=true);
  /** en-/disable the toolbar */
  void slotViewTStdToolbar();
  /** en-/disable the browser toolbar */
  void slotViewTBrowserToolbar();
  /** en-/disable the statusbar */
  void slotViewTStatusbar();
  /** en-/disable the MDI-view taskbar */
  void slotViewMdiViewTaskbar();
  /** overridden from its base class to set the menu item additionally */
  void showViewTaskBar();
  /** overridden from its base class to set the menu item additionally */
  void hideViewTaskBar();
  /** refresh all trees and other widgets*/
  void slotViewRefresh();

  void slotViewTabIcons();
  void slotViewTabText();
  void slotViewTabTextIcons();

  ////////////////////////
  // PROJECT-Menu entries
  ///////////////////////
  /** generates a new project with KAppWizard*/
  void slotProjectNewAppl();
  /** calls kimport to generate a new project by
    * requesting a project directory, writes project file
    * and loads the new project */
  void slotProjectGenerate();
  /** opens a projectfile and close the old one*/
  void slotProjectOpen();
  /** opens a project file from the recent project menu in the project menu by getting the project entry and
   * calling projectOpenCmdl()
   */
  void slotProjectOpenRecent(int id_);
  /** opens a project committed by comandline or kfm */
  //MB  cannot guard with #ifdefs here - moc skips this :(
   /** switchs between kdoc1 and doxygen as API documentation tool
   */
  void slotSwitchDocTool();
  /** Configures Doxygen */
  void slotConfigureDoxygen();
  //MB end
  /** close the current project,return false if  canceled*/
  bool slotProjectClose();
  /** add a new file to the project-same as file new */
  void slotProjectAddNewFile();
  /** opens the add existing files dialog */
  void slotProjectAddExistingFiles();
  /** helper methods for slotProjectAddExistingFiles() */
  void slotAddExistingFiles();
  /** add a new po file to the project*/
  void slotProjectAddNewTranslationFile();
  /** remove a project file */
  void slotProjectRemoveFile();
  /** opens the New class dialog */
  void slotProjectNewClass();
  /** opens the new class dialog when called from file/classviewer with directory where to create the new class*/
  void slotProjectNewClass(const char* folder);
  /** opens the properties dialog for the project files */
  void slotProjectFileProperties();
  /** opens the properties dialog for project files,rel_name is selected, used by RFV,LFV*/
  void slotShowFileProperties(const QString& rel_name);
  /** opens the project options dialog */
  void slotProjectOptions();
  /** selects the project workspace */
  void slotProjectMessages();
  void slotProjectAPI();
  void slotProjectManual();
  void slotProjectMakeDistSourceTgz();
  void slotProjectMakeDistRPM();
  void slotConfigMakeDistRPM();
  void slotProjectMakeTags();
  void slotProjectLoadTags();
  ////////////////////////
  // BUILD-Menu entries
  ///////////////////////
  /** compile the actual sourcefile using preset options */
  bool RunMake(const CMakefile::Type type, const QString& target);
	/** run configure. config is the configuration, ask=true opens the
			configure arguments dialog to edit the configure options.*/
	void RunConfigure(const QString& conf, bool ask);

  void slotBuildMake();
  void slotBuildMakeClean();
  void slotBuildCompileFile();
  void slotBuildCompileDir(const QString& target);
  void slotBuildRebuildAll();
  void slotBuildDistClean();
  void slotBuildAutoconf();
  void slotBuildCleanRebuildAll();

  void slotBuildStop();
  void slotBuildRun();
  void slotBuildRunWithArgs();
  void slotStartRun(bool bWithArgs=false);
  void slotBuildDebug(bool bWithArgs=false);
  void slotBuildDebugStart();
  void slotBuildConfigure();
  //   void slotBuildMakeWith();

  /** connects the menu AND toolbar up to the debug functions
      Most functions are in dbgController which is constructed when
      the user wants the debugger */
  void slotDebugActivator(int id);
  /** this starts the "real" debug session
      it is a successor for slotBuildDebug(true)
      if you start this version instead of slotBuildDebug()
      no check for rebuilding will be made
  */
  void slotStartDebugRunWithArgs();
  /** this starts the "real" debug session
      it is a successor for slotBuildDebug(false)
      if you start this version instead of slotBuildDebug()
      no check for rebuilding will be made
  */
  void slotStartDebug();
  /** Starts up the debugger, and gets it running. This may
      instantiate a debugger if it doesn't exist */
  void slotDebugRun();
  /** Stops and kills the debugger */
  void slotDebugStop();
  /** display the given breakpoint*/
  void slotDebugRefreshBPState(const Breakpoint* BP);
  /** BP state has changed do something (maybe) */
  void slotDebugBPState(Breakpoint* BP);
   /** Dialog showing various views of the debugged program
      memory, libraries, disassembled code */
  void slotDebugMemoryView();
  /** follows the source position in the editor of the debugger*/
  void slotDebugShowStepInSource(const QString& filename, int linenumber,
                                  const QString& address);
  /** follows the source position in the editor of the debugger*/
  void slotDebugGoToSourcePosition(const QString& filename,int linenumber);
  /** Shows the debugger status on the status line */
  void slotDebugStatus(const QString& status, int statusFlag);
  /** Shows the debugger output */
  void slotDebugReceivedStdout(const QString& buffer);
  /** Enter a pid and get the debugger to attach to it */
  void slotDebugAttach();
  /** Set the internal debugger arguments */
  void slotDebugRunWithArgs();
  /** Setect a core file and examine the contents with the debugger */
  void slotDebugExamineCore();
  /** Allow the user to select an arbitary executable to debug */
  void slotDebugNamedFile();
  /** Intial debugger setup */
  void setupInternalDebugger();

  void slotDebugRunToCursor();
  void slotDebugStepInto();
  void slotDebugStepIntoIns();
  void slotDebugStepOver();
  void slotDebugStepOverIns();
  void slotDebugBreakInto();
  void slotDebugStepOutOff();
  void slotDebugToggleBreakpoint();
  void slotDebugInterrupt();

  void slotDebugRunUntil(const QString& filename, int line);

  ////////////////////////
  // TOOLS-Menu entries
  ///////////////////////
  void slotToolsTool(int tool);

  ////////////////////////
  // OPTIONS-Menu entries
  ///////////////////////
  void slotOptionsEditor();
  void slotOptionsEditorColors();
  void slotOptionsSyntaxHighlightingDefaults();
  void slotOptionsSyntaxHighlighting();
  /** shows the Browser configuration dialog */
  void slotOptionsDocBrowser();
  /** shows the Tools-menu configuration dialog */
  void slotOptionsToolsConfigDlg();
  /** shows the spellchecker config dialog */
  void slotOptionsSpellchecker();
//  /** shows the configuration dialog for enscript-printing */
//  void slotOptionsConfigureEnscript();
//  /** shows the configuration dialog for a2ps printing */
//  void slotOptionsConfigureA2ps();
  /** show a configure-dialog for kdevelop*/
  void slotOptionsKDevelop();
  /** sets the make command after it is changed in the Setup dialog */
  void slotOptionsMake();
  /** dis-/enables autosaving by setting in the Setup dialog */
  void slotOptionsAutosave(bool);
  /** sets the autosaving time intervall */
  void slotOptionsAutosaveTime(int);
  /** dis-/enalbes autoswitch by setting bAutoswitch */
  void slotOptionsAutoswitch(bool);
  /** toggles between autoswitching to CV or LFV */
  void slotOptionsDefaultCV(bool);
  /** shows the Update dialog and sends output to the messages widget */
  void slotOptionsUpdateKDEDocumentation();
  /** shows the create search database dialog called by the setup button */
  void  slotOptionsCreateSearchDatabase();
  
  ////////////////////////
  // BOOKMARKS-Menu entries
  ///////////////////////
  /** toggle bookmark on the current document */
  void slotBookmarksToggle();
  /** clear bookmarks on the current document */
  void slotBookmarksClear();
  /** goto the next bookmark in the current document */
  void slotBookmarksNext();
  /** goto the previous bookmark in the current document */
  void slotBookmarksPrevious();
	/**
	 * Open an URL in the Documentation Browser.
	 *
	 * @param file The URL to open.
	 */
	void openBrowserBookmark(const QString& file);

  ////////////////////////
  // HELP-Menu entries
  ///////////////////////
  /** goes one page back in the documentation browser */
  void slotHelpBack();
  /** goes one page forward in the documentatio browser */
  void slotHelpForward();
  /** goes to the page in the history list by delayed popup menu on the 
   *  back-button on the browser toolbar */
  void slotHelpHistoryBack( int id_);
  /** goes to the page in the history list by delayed popup menu on the
   * forward-button on the browser toolbar */
  void slotHelpHistoryForward(int id_);
  /** reloads the currently opened page */
  void slotHelpBrowserReload();
  /** search marked text */
  void slotHelpSearchText();
  /** search marked text with a text string */
  void slotHelpSearchText(QString text);
  /** shows the Search for Help on.. dialog to insert a search expression */
  void slotHelpSearch();
  /** display manpage */
  void slotHelpManpage(QString text);
  /** shows the Display Manpage.. dialog */
  void slotManpage();
  /** shows the KDevelop manual */
  void slotHelpContents();
  /** shows the KDevelop Programming handbook */
  void slotHelpProgramming();
  /** shows the Tutorial handbook */
  void slotHelpTutorial();
  /** shows the KDE library reference */
  void slotHelpKDELibRef();
  /** shows the C/C++-referenc */
  void slotHelpReference();
  /** calls showHelpTipOfDay() */
  void slotHelpTipOfDay() { showHelpTipOfDay(true); };
  /**  open the KDevelop Homepage with kfm/konqueror*/
  void slotHelpHomepage();
  /** shows the API of the current project */
  void slotHelpAPI();
  /** shows the manual of the current project */
  void slotHelpManual();
  /** switches to the handbook selected: manual, programming, tutorial... */
  void showDocHelp(const QString& bookname);

  void slotGrepDialogItemSelected(QString filename,int linenumber);
  
  //////////////////////////////////
  // Classbrowser wizardbutton slots
  //////////////////////////////////
  /** View the class header */
  void slotClassbrowserViewClass();
  /** View the graphical classtree. */
  void slotClassbrowserViewTree();
  /** View the declaration of a class/method. */
  void slotClassbrowserViewDeclaration();
  /** View the definition of a class/method. */
  void slotClassbrowserViewDefinition();
  /** Add a new method to a class. */
  void slotClassbrowserNewMethod();
  /** Add a new attribute to a class. */
  void slotClassbrowserNewAttribute();
  /** Add a new signal to a class - called from the toolbar */
  void slotClassbrowserNewSignal();
  /** Add a new slot to a class - called from the toolbar */
  void slotClassbrowserNewSlot();

  ////////////////////////
  // All slots which are used if the user clicks or selects something in the view
  ///////////////////////
  /** swich construction for the toolbar icons, selecting the right slots */
  void slotToolbarClicked(int);
  /** click on the main window tabs: header, source,documentation or tools*/
  void slotViewSelected(QWidget* pView/*, int docType*/);

  /** set the tree tab automatically without click */
  void slotTCurrentTab(int item);
	
  ///////////// -- the methods for the treeview selection
  /** click action on LFV */
  void slotLogFileTreeSelected(QString file);
  /** click action on RFV */
  void slotRealFileTreeSelected(QString file);
  /** click action on DOC */
  void slotDocTreeSelected(QString url_file);
  /** selection of classes in the browser toolbar */
  void slotClassChoiceCombo(const QString& text);
  /** selection of methods in the browser toolbar */
  void slotMethodChoiceCombo(const QString& text);
  /** add a file to the project */
  void slotAddFileToProject(QString abs_filename);
  void delFileFromProject(QString rel_filename);

  /////////some slots for VCS interaction
  
  void slotUpdateFileFromVCS(QString file);
  void slotCommitFileToVCS(QString file);
  void slotUpdateDirFromVCS(QString dir);
  void slotCommitDirToVCS(QString dir);

  //////////////// -- the methods for the statusbar items
  /** change the status message to text */
  void slotStatusMsg(const QString& text);
  /** change the status message of the whole statusbar temporary */
  void slotStatusHelpMsg(const QString& text);
  /** switch argument for Statusbar help entries on slot selection */
  void statusCallback(int id_);
  /** change Statusbar status of INS and OVR */
  void slotNewStatus();
  /** change copy & cut status */
  void slotCPPMarkStatus(KWriteView *, bool);
  void slotHEADERMarkStatus(KWriteView *, bool);
  void slotMarkStatus(KWriteView *, bool);
  void slotBROWSERMarkStatus(KHTMLPart *, bool);
  /** recognize change of Clipboard data */
  void slotClipboardChanged(KWriteView *, bool);
  /** change Statusbar status of Line and Column */
  void slotNewLineColumn();
  void slotNewUndo();

  //  void slotClickedOnMessagesWidget();  

  void slotURLSelected(const QString& url);
  void slotDocumentDone();
  void slotURLonURL(const QString& url);

  void slotReceivedStdout(KProcess* proc,char* buffer,int buflen);
  void slotReceivedStderr(KProcess* proc,char* buffer,int buflen);

  void slotApplReceivedStdout(KProcess* proc,char* buffer,int buflen);
  void slotApplReceivedStderr(KProcess* proc,char* buffer,int buflen);
  void slotApplReceivedStdout(const char* buffer);
  void slotApplReceivedStderr(const char* buffer);

//  void switchToKDevelop();
  void startDesigner();

  void slotSearchReceivedStdout(KProcess* proc,char* buffer,int buflen);
  void slotProcessExited(KProcess* proc);
  void slotSearchProcessExited(KProcess*);
  
  //////////////// -- the methods for signals generated from the CV
  /** Add a method to a class. Brings up a dialog and lets the
   * user fill it out.
   * @param aClassName      The class to add the method to.
   */
  void slotCVAddMethod( const char *aClassName );

  /** Add a method to a class.
   * user fill it out.
   * @param aClassName      The class to add the method to.
   * @param aMethod         The method to add to the class.
   */
  void slotCVAddMethod( const char *aClassName, CParsedMethod *aMethod );

  /** Add an attribute to a class.
   * @param aClassName      The class to add the attribute to.
   */
  void slotCVAddAttribute( const char *aClassName );
  
  void slotCVAddAttribute( const char *aClassName, CParsedAttribute* aAttr );
  /**  */
  void slotCVSigSlotMapImplement ( CParsedClass*, const QString&, CParsedMethod* );
  /** Delete an method.
   * @param aClassName Name of the class holding the method. NULL for global functions.
   * @param aMethodName Name of the method(with arguments) to delete.
   */
  void slotCVDeleteMethod( const char *aClassName,const char *aMethodName );

  /** The user wants to view the declaration of a method. 
   * @param className Name of the class holding the declaration. NULL for globals.
   * @param declName Name of the declaration item.
   * @param type Type of declaration item
   */
  void slotCVViewDeclaration( const char *parentPath, 
                              const char *itemName, 
                              THType parentType,
                              THType itemType );

  /** slot to views the class definition from
      the graphical classbrowser. */
  void slotSwitchToFile( const QString &, int );

  /** The user wants to view the definition of a method/attr...
   * @param className Name of the class holding the definition. NULL for globals.
   * @param declName Name of the definition item.
   * @param type Type of definition item.
   */
  void slotCVViewDefinition( const char *parentPath, 
                             const char *itemName, 
                             THType parentType,
                             THType itemType );

  /** Reimplemented from base class QextMdiMainFrm.
   *  Dispatches this 'event' to m_docViewMan which will delete the closed view
   */
  virtual void closeWindow(QextMdiChildView *pWnd, bool layoutTaskBar = true);
  /**
   * Overridden from its base class QextMdiMainFrm. Adds additional entries to the "Window" menu.
   */
  virtual void fillWindowMenu();
  /**
   * Creates and shows a new MDI view window depending on the last focused view type
   */
  void slotCreateNewViewWindow();

  /**
   * Goto the line in the file of the tag
   */
  // void slotTagGotoFile(const CTag* tag);
  /**
   * Open the file that corresponds to tag
   */
  // void slotTagOpenFile(QString tag);
  /**
   * Switch between corresponding source and header files. Assumes that
   * the files exist and that they have the same basename.
   */
  void slotTagSwitchTo();
  /**
   * Show and raise the CTags search dialog
   */
  void slotTagSearch();
  /**
   * Find and open files that contain definition corresponding to tag
   */
  //void slotTagDefinition(QString tag);
  /**
   * Find and open files that contain declaration corresponding to tag
   */
  //void slotTagDeclaration(QString tag);

  // activate certain tool-views (slots for accels)
  void slotActivateTView_Class();
  void slotActivateTView_LFV();
  void slotActivateTView_RFV();
  void slotActivateTView_Doc();
  void slotActivateTView_VAR();
  void slotActivateOView_Messages();
  void slotActivateOView_StdInStdOut();
  void slotActivateOView_StdErr();
  void slotActivateOView_Konsole();
  void slotActivateOView_BrkptManager();
  void slotActivateOView_FrameStack();
  void slotActivateOView_Disassemble();
  void slotActivateOView_Dbg();

protected: // Protected methods

  /** The user selected a class in the classcombo.
   * @param aName Name of the class.
   */
  void CVClassSelected( const char *aName );

  /** The user selected a method in the methodcombo.
   * @param aName Name of the method.
   */
  void CVMethodSelected( const char *aName );

  /** 
   * Goto the definition of the specified item.
   * 
   * @param parentPath Path of the container. Empty for globals.
   * @param itemName   Name of the selected item. Empty for containers.
   * @param parentType Type of container. Ignored for globals.
   * @param itemType   Type of the selected item. Ignored for containers.
   */
  void CVGotoDefinition( const char *parentPath, 
                         const char *itemName, 
                         THType parentType,
                         THType itemType );

  /** 
   * Goto the declaration of the specified item.
   *
   * @param parentPath Path of the container. Empty for globals.
   * @param itemName   Name of the selected item. Empty for containers.
   * @param parentType Type of container. Ignored for globals.
   * @param itemType   Type of the selected item. Ignored for containers.
   */
  void CVGotoDeclaration( const char *parentPath, 
                         const char *itemName, 
                         THType parentType,
                         THType itemType );
  
  /** 
   * Returns the class for the supplied classname. 
   *
   * @param parentPath Path of the container.
   * @param parentType Type of container.
   */
  CParsedContainer *CVGetContainer( const char *parentPath, 
                                    THType parentType );

  /** Update the class combo with all classes in alpabetical order. */
  void CVRefreshClassCombo();

  /** Update the method combo with the methods from the selected
   * class.
   * @param aClass Class to update the methodcombo from.
   */
  void CVRefreshMethodCombo( CParsedClass *aClass );

public: // Public methods

  /** create a ctags file for the current CProject */
  void create_tags();

  /** return the instance of the CTags search dialog */
  searchTagsDialogImpl* getCTagsDialog() const ;

  bool isFileInBuffer(QString abs_filename);

  /** a tool meth,used in the search engine*/
  int searchToolGetNumber(QString str);
  QString searchToolGetTitle(QString str);
  QString searchToolGetURL(QString str);
  void saveCurrentWorkspaceIntoProject();

  /** Get the progress bar */
  QProgressBar* getProgressBar() { return statProg; };

	/** Get the last search text */
	QString & getDocSearchText() { return doc_search_text; };
	
  /** called if a new subdirs was added to the project, shows a messagebox and start autoconf...*/
  void newSubDir();

protected:
  /** reads all options and initializes values*/
  void readOptions();
  /** saves all options on queryExit() */
  void saveOptions();
  /** save the project of the current window
   * If project closing is cancelled, returns false */
  bool queryClose();
  /** saves all options by calling saveOptions() */
  bool queryExit();
  /** saves the currently opened project by the session manager and write 
   * the project file to the session config*/
  //void saveProperties(KConfig*);
  /** initializes the session windows and opens the projects of the last
   * session */
  //void readProperties(KConfig* );

  /** overridden from it's base class method, additionally moves the MDI system buttons (in maximized mode)
   */
  virtual void resizeEvent( QResizeEvent *pRSE);

private:
  void printImpl(QStringList& list, KPrinter* printer);
  /** */
  bool isToolViewVisible(QWidget* pToolView);
  /** */
	void adjustTTreesToolButtonState();
  /** */
  void adjustTOutputToolButtonState();
  /** shows the tip of the day */
  void showHelpTipOfDay(bool force=true);

public:
  /** this attribute is true from construction until qApp->exec().
    * In this way we can prevent some actions in queryClose() and queryExit() */
  bool bStartupIsPending;

private:
  KFileOpenWithHandler fowh;
  //the menus for kdevelop main
  QPopupMenu* file_menu;				
  QPopupMenu* recent_projects_menu;
  //MB
  QPopupMenu* doctool_menu;
  enum DOCTOOL {DT_KDOC, DT_DOX};
  int doctool;
  //MB end
  QPopupMenu* edit_menu;
  QPopupMenu* view_menu;
  QPopupMenu* view_tab_menu;
  QPopupMenu* bookmarks_menu;
	//  QPopupMenu* doc_bookmarks;

  KStatusBarLabel *m_statusLabel;

  QPopupMenu* project_menu;
  QPopupMenu* build_menu;
  QPopupMenu* debug_menu;
  QPopupMenu* debugPopup;
  QPopupMenu* tools_menu;
  QPopupMenu* options_menu;
  KHelpMenu* help_menu;
  QWhatsThis* whats_this;
	
  QPopupMenu* history_prev;
  QPopupMenu* history_next;
  QPopupMenu* file_open_popup;
  /** Popup menu for the classbrowser wizard button. */
  QPopupMenu* classbrowser_popup;
  /** Popup menu to show/hide single tree tool views */
  QPopupMenu* toggletreeviews_popup;
  /** Popup menu to show/hide single output tool views */
  QPopupMenu* toggleoutputviews_popup;

  /** Tells if the next click on the classwizard toolbar button should show
   * the declaration or the definition of the selected item. */
  bool cv_decl_or_impl;

  QStrList file_open_list;	

  /** QValueList containing the Tool Apps */
  ToolAppList toolList;

  KCompletion* compile_comp;
  KCompletion* class_comp;
  KCompletion* method_comp;	
  /** If this to true, the user wants a beep after a
   *  process,slotProcessExited() */
  bool beep; 
  
  
  KIconLoader icon_loader;
  /** for tools,compiler,make,kodc */
  KShellProcess process;
  /** only for selfmade appl */
  KShellProcess appl_process;
  /**  for kdoc,sgmltools ... */
  KShellProcess shell_process;
  /** search with glimpse */
  KShellProcess search_process;
//  /** print process */
//  KShellProcess m_print_process;
//  /** print preview process */
//  KShellProcess m_preview_process;

  /** at the moment only one project at the same time */
  CProject* prj;

  CKDevAccel *accel;

  /** global configuration file for KDevelop contains the project */
  KConfig* config;

  int act_outbuffer_len;

//  QStringList recent_projects;
  // for the browser
  QStrList history_list;
  QStrList history_title_list;
	//  QStrList doc_bookmarks_list;
  //  QStrList doc_bookmarks_title_list;
	
  ///////////////////////////////
  //some widgets for the mainview
  ///////////////////////////////

  KDockWidget* maindock;

  /** The classview. */
  CClassView* class_tree;
  /** The logical fileview. */
  CLogFileView* log_file_tree;
  /** The real fileview. */
  CRealFileView* real_file_tree;
  /** The documentation tree. */
  DocTreeView* doc_tree;

  /** Output from the compiler ... */
  CMakeOutputWidget* messages_widget;
  /** stdin and stdout output. */
  COutputWidget* stdin_stdout_widget;
  /** stderr output. */
  COutputWidget* stderr_widget;

  /** A konsole */
  CKonsoleWidget *konsole_widget;
  
  int workspace;

  QString version;
  bool project;

  bool  prev_was_search_result;

  // Autosaving elements
  /** The autosave timer. */
  QTimer* saveTimer;
  /** Tells if autosaving is enabled. */
  bool bAutosave;
  /** The autosave interval. */
  int saveTimeout;

  bool bAutoswitch;
  bool bDefaultCV;
//  bool bKDevelop;
//  KProgress* statProg;
  QGuardedPtr<QProgressBar> statProg;
  //some vars for the searchengine
  QString search_output;
  QString doc_search_display_text, doc_search_text;
  // for more then one job in proc;checked in slotProcessExited(KProcess* proc);
  // values are "run","make" "refresh";
  QString next_job;
  QString make_cmd;
//   QString make_with_cmd;

  CConfigEnscriptDlg* enscriptconf;
  CConfigA2psDlg* a2psconf;

  CAddExistingFileDlg* add_dlg;
  GrepDialog* grep_dlg;
  searchTagsDialogImpl* ctags_dlg;

  QString lastfile;
  QString lastOpenDir;

  /*********************Debugger additions *******************/
  /** The controller for the debugging program (eg gdb) */
  DbgController* dbgController;
  /** The floating toolbar - always on top */
  DbgToolbar* dbgToolbar;
  /** The debugger's tree of local variables - on tree view tab */
  VarViewer* var_viewer;
  /** Manages a list of breakpoints - Always active - on output tab */
  BreakpointManager* brkptManager;
  /** Manages a frame stack list - on output tab */
  FrameStack* frameStack;
  /** show disassembled code being run - on output tab */
   Disassemble* disassemble;
 
  /** debug aid. Switch on using compile switch GDB_MONITOR
      or DBG_MONITOR  - on output tab */
  COutputWidget* dbg_widget;
  /** */
  bool dbgInternal;
  /** The external debugger command to run */
  QString dbgExternalCmd;
  /** Protect the gdbcontroller deletion. */
  bool dbgShuttingDown;
  /** Have both float and normal toolbar */
  bool dbgEnableFloatingToolbar;
  /************* END Debugger additions ********************/

  /** memory effect on open file dialog box*/
  QString _LastOpenDir;

  /** true if we have and use Exuberant CTags for enhanced browsing capabilities */
  bool bCTags;
  /** true if we are using Glimpse */
  bool useGlimpse;
  /** true if we are using HtDig */
  bool useHtDig;

  bool lastShutdownOK;
  KStatusBar* m_statusBar;

/** RPM stuff **/
      Kpp *rpmbuilder;

  /***************** MDI additions ********************/
  DocViewMan* m_docViewManager;

  KDevSession* m_pKDevSession;
  /************* END MDI additions ********************/

  /** command and command line arguments to create a ctags file */
  CTagsCommandLine m_CTagsCmdLine;

  /** This list stores all finished Processes that have been started via the "Tool" Menu */
  QList<KProcess> m_FinishedToolProcesses;

  bool m_bToggleToolViewsIsPending;

private slots:
  void slotdoneWithKpp();
  void slotrpmBuildProcess();
  void slotGetRPMBuildSTDOUT(QString sstdout);
  void slotGetRPMBuildSTDERR(QString sstderr);
  void slotAddSpec(QString path);
  void slotDockWidgetHasUndocked(KDockWidget*);
  void fillToggleTreeViewsMenu();
  void fillToggleOutputViewsMenu();
  void slotToolProcessExited (KProcess* proc);
  void cleanUpToolProcesses();

  // en-/disable the single tool views
  void slotViewTClassesView();
  void slotViewTGroupsView();
  void slotViewTFilesView();
  void slotViewTBooksView();
  void slotViewTWatchView();
  void slotViewOMessagesView();
  void slotViewOStdOutView();
  void slotViewOStdErrView();
  void slotViewOKonsoleView();
  void slotViewOBreakpointView();
  void slotViewODisassembleView();
  void slotViewOFrameStackView();
  void slotViewODebuggerView();

  void slotCompileCombo(const QString&) {};
};

#endif

