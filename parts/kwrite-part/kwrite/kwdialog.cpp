// Dialogs

#include <stdio.h>
#include <stdlib.h>

#include <qgrid.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qptrcollection.h>
#include <qpushbutton.h>
#include <qobjectlist.h>
#include <qradiobutton.h>
#include <qwhatsthis.h>

#include <klocale.h>
#include <kcolorbutton.h>
#include <knuminput.h>

#include "kwdoc.h"
#include "kwdialog.h"

/*
Please keep the coding style of existing files
*/

SearchDialog::SearchDialog( QWidget *parent, QStringList &searchFor, QStringList &replaceWith, int flags )
  : KDialogBase( parent, 0L, true, i18n( "Find Text" ), Ok | Cancel, Ok )
  , m_replace( 0L )
{
  QWidget *page = new QWidget( this );
  setMainWidget( page );

  QVBoxLayout *topLayout = new QVBoxLayout( page, 0, spacingHint() );

  m_search = new QComboBox( true, page );
  m_search->insertStringList( searchFor );
  m_search->setMinimumWidth( m_search->sizeHint().width() );
  m_search->lineEdit()->selectAll();
  QLabel *label = new QLabel( m_search, i18n( "&Text to find:" ), page );
  m_optRegExp = new QCheckBox( i18n( "Regular expression" ), page );
  topLayout->addWidget( label );
  topLayout->addWidget( m_search );
  topLayout->addWidget( m_optRegExp );

  if( flags & KWrite::sfReplace )
  {
    // make it a replace dialog
    setCaption( i18n( "Replace Text" ) );
    m_replace = new QComboBox( true, page );
    m_replace->insertStringList( replaceWith );
    m_replace->setMinimumWidth( m_search->sizeHint().width() );
    label = new QLabel( m_replace, i18n( "&Replace with:" ), page );
    //m_optPlaceholders = new QCheckBox( i18n( "&Use placeholders" ), page );
    topLayout->addWidget( label );
    topLayout->addWidget( m_replace );
    //topLayout->addWidget( m_optPlaceholders );
  }

  QGroupBox *group = new QGroupBox( i18n( "Options" ), page );
  topLayout->addWidget( group, 10 );

  QGridLayout *gbox = new QGridLayout( group, 5, 2, spacingHint() );
  gbox->addRowSpacing( 0, fontMetrics().lineSpacing() );
  gbox->setRowStretch( 4, 10 );

  m_opt1 = new QCheckBox( i18n( "C&ase sensitive" ), group );
  gbox->addWidget( m_opt1, 1, 0 );

  m_opt2 = new QCheckBox(i18n("&Whole words only" ), group );
  gbox->addWidget( m_opt2, 2, 0 );

  m_opt3 = new QCheckBox(i18n("&From cursor" ), group );
  gbox->addWidget( m_opt3, 3, 0 );

  m_opt4 = new QCheckBox(i18n("Find &backwards" ), group );
  gbox->addWidget( m_opt4, 1, 1 );

  m_opt5 = new QCheckBox(i18n("&Selected text" ), group );
  gbox->addWidget( m_opt5, 2, 1 );

  m_opt1->setChecked( flags & KWrite::sfCaseSensitive );
  m_opt2->setChecked( flags & KWrite::sfWholeWords );
  m_opt3->setChecked( flags & KWrite::sfFromCursor );
  m_optRegExp->setChecked( flags & KWrite::sfRegularExpression );
  m_opt4->setChecked( flags & KWrite::sfBackward );
  m_opt5->setChecked( flags & KWrite::sfSelected );

  if( m_replace )
  {
    m_opt6 = new QCheckBox( i18n( "&Prompt on replace" ), group );
    m_opt6->setChecked( flags & KWrite::sfPrompt );
    gbox->addWidget( m_opt6, 3, 1 );
  }

  m_search->setFocus();
}

QString SearchDialog::getSearchFor()
{
  return m_search->currentText();
}

QString SearchDialog::getReplaceWith()
{
  return m_replace->currentText();
}

int SearchDialog::getFlags()
{
  int flags = 0;

  if( m_opt1->isChecked() ) flags |= KWrite::sfCaseSensitive;
  if( m_opt2->isChecked() ) flags |= KWrite::sfWholeWords;
  if( m_opt3->isChecked() ) flags |= KWrite::sfFromCursor;
  if( m_opt4->isChecked() ) flags |= KWrite::sfBackward;
  if( m_opt5->isChecked() ) flags |= KWrite::sfSelected;
  if( m_optRegExp->isChecked() ) flags |= KWrite::sfRegularExpression;
  if( m_replace )
  {
    if( m_opt6->isChecked() )
      flags |= KWrite::sfPrompt;

    flags |= KWrite::sfReplace;
  }

  return flags;
}

void SearchDialog::slotOk()
{
  if ( !m_search->currentText().isEmpty() )
  {
    if ( !m_optRegExp->isChecked() )
    {
      accept();
    }
    else
    {
      // Check for a valid regular expression.

      QRegExp regExp( m_search->currentText() );

      if ( regExp.isValid() )
        accept();
    }
  }
}

// this dialog is not modal
ReplacePrompt::ReplacePrompt( QWidget *parent )
  : KDialogBase(parent, 0L, false, i18n( "Replace Text" ),
  User3 | User2 | User1 | Close, User3, true,
  i18n("&All"), i18n("&No"), i18n("&Yes")) {

  QWidget *page = new QWidget(this);
  setMainWidget(page);

  QBoxLayout *topLayout = new QVBoxLayout( page, 0, spacingHint() );
  QLabel *label = new QLabel(i18n("Replace this occurence?"),page);
  topLayout->addWidget(label );
}

void ReplacePrompt::slotUser1( void ) { // All
  done(KWrite::srAll);
}

void ReplacePrompt::slotUser2( void ) { // No
  done(KWrite::srNo);
}

void ReplacePrompt::slotUser3( void ) { // Yes
  accept();
}

void ReplacePrompt::done(int r) {
  setResult(r);
  emit clicked();
}

void ReplacePrompt::closeEvent(QCloseEvent *) {
  reject();
}

GotoLineDialog::GotoLineDialog(QWidget *parent, int line, int max)
  : KDialogBase(parent, 0L, true, i18n("Goto Line"), Ok | Cancel, Ok) {

  QWidget *page = new QWidget(this);
  setMainWidget(page);

  QVBoxLayout *topLayout = new QVBoxLayout( page, 0, spacingHint() );
  e1 = new KIntNumInput(line, page);
  e1->setRange(1, max);
  e1->setEditFocus(true);

  QLabel *label = new QLabel( e1,i18n("&Goto line:"), page );
  topLayout->addWidget(label);
  topLayout->addWidget(e1);
  topLayout->addSpacing(spacingHint()); // A little bit extra space
  topLayout->addStretch(10);
  e1->setFocus();
}

int GotoLineDialog::getLine() {
  return e1->value();
}

const int IndentConfigTab::flags[] = {KWrite::cfAutoIndent, KWrite::cfSpaceIndent,
  KWrite::cfBackspaceIndents,KWrite::cfTabIndents, KWrite::cfKeepIndentProfile, KWrite::cfKeepExtraSpaces};

IndentConfigTab::IndentConfigTab(QWidget *parent, KWrite *kWrite)
  : QWidget(parent, 0L)
{
  QVBoxLayout *layout = new QVBoxLayout(this, 0, KDialog::spacingHint() );
  int configFlags = kWrite->config();

  opt[0] = new QCheckBox(i18n("&Auto indent"), this);
  layout->addWidget(opt[0], 0, AlignLeft);
  opt[0]->setChecked(configFlags & flags[0]);

  opt[1] = new QCheckBox(i18n("Indent with &spaces"), this);
  layout->addWidget(opt[1], 0, AlignLeft);
  opt[1]->setChecked(configFlags & flags[1]);

  opt[2] = new QCheckBox(i18n("&Backspace key indents"), this);
  layout->addWidget(opt[2], 0, AlignLeft);
  opt[2]->setChecked(configFlags & flags[2]);

  opt[3] = new QCheckBox(i18n("&Tab key indents"), this);
  layout->addWidget(opt[3], 0, AlignLeft);
  opt[3]->setChecked(configFlags & flags[3]);

  opt[4] = new QCheckBox(i18n("Keep indent &profile"), this);
  layout->addWidget(opt[4], 0, AlignLeft);
  opt[4]->setChecked(configFlags & flags[4]);

  opt[5] = new QCheckBox(i18n("&Keep extra spaces"), this);
  layout->addWidget(opt[5], 0, AlignLeft);
  opt[5]->setChecked(configFlags & flags[5]);

  layout->addStretch();

  // What is this? help
  QWhatsThis::add(opt[0], i18n("When <b>Auto indent</b> is on, KWrite will indent new lines to equal the indent on the previous line.<p>If the previous line is blank, the nearest line above with text is used"));
  QWhatsThis::add(opt[1], i18n("Check this if you want to indent with spaces rather than tabs.<br>A Tab will be converted to <u>Tab-width</u> as set in the <b>edit</b> options"));
  QWhatsThis::add(opt[2], i18n("This allows the <b>backspace</b> key to be used to indent."));
  QWhatsThis::add(opt[3], i18n("This allows the <b>tab</b> key to be used to indent."));
  QWhatsThis::add(opt[4], i18n("This retains current indentation settings for future documents."));
  QWhatsThis::add(opt[5], i18n("Indentations of more than the selected number of spaces will not be shortened."));
}

void IndentConfigTab::getData(KWrite *kWrite) {
  int configFlags, z;

  configFlags = kWrite->config();
  for (z = 0; z < numFlags; z++) {
    configFlags &= ~flags[z];
    if (opt[z]->isChecked()) configFlags |= flags[z];
  }
  kWrite->setConfig(configFlags);
}

const int SelectConfigTab::flags[] = {KWrite::cfPersistent, KWrite::cfDelOnInput,
  KWrite::cfMouseAutoCopy, KWrite::cfSingleSelection, KWrite::cfVerticalSelect, KWrite::cfXorSelect};

SelectConfigTab::SelectConfigTab(QWidget *parent, KWrite *kWrite)
  : QWidget(parent, 0L)
{
  QVBoxLayout *layout = new QVBoxLayout(this, 0, KDialog::spacingHint() );
  int configFlags = kWrite->config();

  opt[0] = new QCheckBox(i18n("&Persistent selections"), this);
  layout->addWidget(opt[0], 0, AlignLeft);
  opt[0]->setChecked(configFlags & flags[0]);

  opt[1] = new QCheckBox(i18n("&Overwrite selections"), this);
  layout->addWidget(opt[1], 0, AlignLeft);
  opt[1]->setChecked(configFlags & flags[1]);

  opt[2] = new QCheckBox(i18n("Mouse &autocopy"), this);
  layout->addWidget(opt[2], 0, AlignLeft);
  opt[2]->setChecked(configFlags & flags[2]);

  opt[3] = new QCheckBox(i18n("&X11-like single selection"), this);
  layout->addWidget(opt[3], 0, AlignLeft);
  opt[3]->setChecked(configFlags & flags[3]);

  opt[4] = new QCheckBox(i18n("&Vertical selections"), this);
  layout->addWidget(opt[4], 0, AlignLeft);
  opt[4]->setChecked(configFlags & flags[4]);

  opt[5] = new QCheckBox(i18n("&Toggle old"), this);
  layout->addWidget(opt[5], 0, AlignLeft);
  opt[5]->setChecked(configFlags & flags[5]);

  layout->addStretch();

  // What is this? help
  QWhatsThis::add(opt[0], i18n("Enabling this prevents key input or cursor movement by way of the arrow keys from causing the elimination of text selection.<p><b>Note:</b> If the Overwrite Selections option is activated then any typed character input or paste operation will replace the selected text."));
  QWhatsThis::add(opt[1], i18n("When this is on, any keyed character input or paste operation will replace the selected text."));
  QWhatsThis::add(opt[2], i18n("When this is on, any text selected with the mouse will be automatically copied to the clipboard."));
  QWhatsThis::add(opt[3], i18n("Not implemented yet."));
  QWhatsThis::add(opt[4], i18n("Enabling this allows you to make vertical selections."));
  QWhatsThis::add(opt[5], i18n("Not yet implemented."));
}

void SelectConfigTab::getData(KWrite *kWrite) {
  int configFlags, z;

  configFlags = kWrite->config();
  for (z = 0; z < numFlags; z++) {
    configFlags &= ~flags[z]; // clear flag
    if (opt[z]->isChecked()) configFlags |= flags[z]; // set flag if checked
  }
  kWrite->setConfig(configFlags);
}

const int EditConfigTab::flags[] = {KWrite::cfWordWrap, KWrite::cfReplaceTabs, KWrite::cfRemoveSpaces,
  KWrite::cfAutoBrackets, KWrite::cfGroupUndo, KWrite::cfShowTabs, KWrite::cfSmartHome,
  KWrite::cfPageUDMovesCursor, KWrite::cfWrapCursor};

EditConfigTab::EditConfigTab(QWidget *parent, KWrite *kWrite)
  : QWidget(parent, 0L) {

  QHBoxLayout *mainLayout;
  QVBoxLayout *cbLayout, *leLayout;
  int configFlags;

  mainLayout = new QHBoxLayout(this, 0, KDialog::spacingHint() );

  // checkboxes
  cbLayout = new QVBoxLayout( mainLayout );
  configFlags = kWrite->config();

  opt[0] = new QCheckBox(i18n("&Word wrap"), this);
  cbLayout->addWidget(opt[0], 0, AlignLeft);
  opt[0]->setChecked(configFlags & flags[0]);

  opt[1] = new QCheckBox(i18n("Replace &tabs with spaces"), this);
  cbLayout->addWidget(opt[1], 0, AlignLeft);
  opt[1]->setChecked(configFlags & flags[1]);

  opt[2] = new QCheckBox(i18n("&Remove trailing spaces"), this);
  cbLayout->addWidget(opt[2], 0, AlignLeft);
  opt[2]->setChecked(configFlags & flags[2]);

  opt[3] = new QCheckBox(i18n("&Auto brackets"), this);
  cbLayout->addWidget(opt[3], 0, AlignLeft);
  opt[3]->setChecked(configFlags & flags[3]);

  opt[4] = new QCheckBox(i18n("Group &undos"), this);
  cbLayout->addWidget(opt[4], 0, AlignLeft);
  opt[4]->setChecked(configFlags & flags[4]);

  opt[5] = new QCheckBox(i18n("&Show tabs"), this);
  cbLayout->addWidget(opt[5], 0, AlignLeft);
  opt[5]->setChecked(configFlags & flags[5]);

  opt[6] = new QCheckBox(i18n("Smart &home"), this);
  cbLayout->addWidget(opt[6], 0, AlignLeft);
  opt[6]->setChecked(configFlags & flags[6]);

  opt[7] = new QCheckBox(i18n("&Page up/down moves cursor"), this);
  cbLayout->addWidget(opt[7], 0, AlignLeft);
  opt[7]->setChecked(configFlags & flags[7]);

  opt[8] = new QCheckBox(i18n("Wrap &cursor"), this);
  cbLayout->addWidget(opt[8], 0, AlignLeft);
  opt[8]->setChecked(configFlags & flags[8]);

  cbLayout->addStretch();

  // edit lines
  leLayout = new QVBoxLayout();
  mainLayout->addLayout(leLayout,10);

  e1 = new KIntNumInput(kWrite->wordWrapAt(), this);
  e1->setRange(20, 200, 1, false);
  e1->setLabel(i18n("Wrap words at:"));

  e2 = new KIntNumInput(e1, kWrite->tabWidth(), this);
  e2->setRange(1, 16, 1, false);
  e2->setLabel(i18n("Tab width:"));

  e3 = new KIntNumInput(e2, kWrite->undoSteps(), this);
  e3->setRange(5, 30000, 1, false);
  e3->setLabel(i18n("Undo steps:"));

  leLayout->addWidget(e1, 0, AlignLeft);
  leLayout->addWidget(e2, 0, AlignLeft);
  leLayout->addWidget(e3, 0, AlignLeft);
  leLayout->addStretch();

  // What is this? help
  QWhatsThis::add(opt[0], i18n("Word wrap is a feature that causes the editor to automatically start a new line of text and move (wrap) the cursor to the beginning of that new line. KWrite will automatically start a new line of text when the current line reaches the length specified by the Wrap Words At: option.<p><b>NOTE:<b> Word Wrap will not change existing lines or wrap them for easy reading as in some applications."));
  QWhatsThis::add(e1, i18n("If the Word Wrap option is selected this entry determines the length (in characters) at which the editor will automatically start a new line."));
  QWhatsThis::add(opt[1], i18n("KWrite will replace any tabs with the number of spaces indicated in the Tab Width: entry."));
  QWhatsThis::add(e2, i18n("If the Replace Tabs By Spaces option is selected this entry determines the number of spaces with which the editor will automatically replace tabs."));
  QWhatsThis::add(opt[2], i18n("KWrite will automatically eliminate extra spaces at the ends of lines of text."));
  QWhatsThis::add(opt[3], i18n("When the user types a left bracket ([,(, or {) KWrite automatically enters the right bracket (}, ), or ]) to the right of the cursor."));
  QWhatsThis::add(opt[4], i18n("Checking this will cause sequences of similar actions to be undone at once."));
  QWhatsThis::add(opt[5], i18n("The editor will display a symbol to indicate the presence of a tab in the text."));
  QWhatsThis::add(opt[6], i18n("Not yet implemented."));
  QWhatsThis::add(opt[7], i18n("If this is selected, the insertion cursor will be moved to the first/last line when pressing the page up/down buttons.<p>If not selected, it will remain at its relative position in the visible text."));
  QWhatsThis::add(e3, i18n("Sets the number of undo/redo steps to record. More steps uses more memory."));
  QWhatsThis::add(opt[8], i18n("When on, moving the insertion cursor using the <b>Left</b> and <b>Right</b> keys will go on to previous/next line at beginning/end of the line, similar to most editors.<p>When off, the insertion cursor cannot be moved left of the line start, but it can be moved off the line end, which can be very handy for programmers."));
}

void EditConfigTab::getData(KWrite *kWrite)
{
  int configFlags, z;
  
  configFlags = kWrite->config();
  for (z = 0; z < numFlags; z++) {
    configFlags &= ~flags[z];
    if (opt[z]->isChecked()) configFlags |= flags[z];
  }
  kWrite->setConfig(configFlags);

  kWrite->setWordWrapAt(e1->value());
  kWrite->setTabWidth(e2->value());
  kWrite->setUndoSteps(e3->value());
}

/*
SettingsDialog::SettingsDialog(int flags, int wrapAt, int tabWidth, int undoSteps,
  QWidget *parent, const char *name)
  : QDialog(parent,name,true) {

  QLabel *label1;
  QLabel *label2;
  QLabel *label3;
  QPushButton *button1, *button2;

  QGroupBox *g1 = new QGroupBox(i18n("Edit Options"),this);
  opt1 = new QCheckBox(i18n("Auto &indent"),g1);
//  opt1->setFixedSize(opt1->sizeHint());
  opt2 = new QCheckBox(i18n("&Backspace indent"),g1);
//  opt2->setFixedSize(opt2->sizeHint());
  opt3 = new QCheckBox(i18n("&Word wrap"),g1);
//  opt3->setFixedSize(opt3->sizeHint());
  opt4 = new QCheckBox(i18n("Replace &tabs"),g1);
//  opt4->setFixedSize(opt4->sizeHint());
  opt5 = new QCheckBox(i18n("Remove trailing &spaces"),g1);
//  opt5->setFixedSize(opt5->sizeHint());
  opt6 = new QCheckBox(i18n("Wrap &cursor"),g1);
//  opt6->setFixedSize(opt6->sizeHint());
  opt7 = new QCheckBox(i18n("&Auto brackets"),g1);
//  opt7->setFixedSize(opt7->sizeHint());
  opt13 = new QCheckBox(i18n("Group &undos"),g1);
//  opt13->setFixedSize(opt13->sizeHint());

  g1->setMinimumHeight(8+8+8+7*4+8*(opt1->sizeHint().height()));

  opt1->setChecked(flags & cfAutoIndent);
  opt2->setChecked(flags & cfBackspaceIndent);
  opt3->setChecked(flags & cfWordWrap);
  opt4->setChecked(flags & cfReplaceTabs);
  opt5->setChecked(flags & cfRemoveSpaces);
  opt6->setChecked(flags & cfWrapCursor);
  opt7->setChecked(flags & cfAutoBrackets);
  opt13->setChecked(flags & cfGroupUndo);

  QGroupBox *g2 = new QGroupBox(i18n("Select Options"),this);
  opt8 = new QCheckBox(i18n("&Persistent selections"),g2);
  opt8->setFixedSize( opt8->sizeHint() );
  opt9 = new QCheckBox(i18n("&Multiple selections"),g2);
  opt9->setFixedSize( opt9->sizeHint() );
  opt10 = new QCheckBox(i18n("&Vertical selections"),g2);
  opt10->setFixedSize( opt10->sizeHint() );
  opt11 = new QCheckBox(i18n("&Delete on input"),g2);
  opt11->setFixedSize( opt11->sizeHint() );
  opt12 = new QCheckBox(i18n("&Toggle old"),g2);
  opt12->setFixedSize( opt12->sizeHint() );

  g2->setMinimumHeight(8+8+8+4*4+5*(opt8->sizeHint().height()));

  opt8->setChecked(flags & cfPersistent);
  opt9->setChecked(flags & cfKeepSelection);
  opt10->setChecked(flags & cfVerticalSelect);
  opt11->setChecked(flags & cfDelOnInput);
  opt12->setChecked(flags & cfXorSelect);


  e1 = new KIntLineEdit(wrapAt, this, 20, 200);
  label1 = new QLabel(e1,i18n("Wrap words at:"),this);
  int max = label1->sizeHint().width();

  e2 = new KIntLineEdit(tabWidth, this, 1, 16);
  label2 = new QLabel(e2,i18n("Tab width:"),this);
  if (label2->sizeHint().width()>max)
    max = label2->sizeHint().width();

  e3 = new KIntLineEdit(undoSteps, this, 5, 30000);
  label3 = new QLabel(e3,i18n("Undo steps:"),this);
  if (label3->sizeHint().width()>max)
    max = label3->sizeHint().width();

  label1->setFixedSize( max, label1->sizeHint().height() );
  label2->setFixedSize( max, label2->sizeHint().height() );
  label3->setFixedSize( max, label3->sizeHint().height() );

  e1->setFixedSize( max, e1->sizeHint().height() );
  e2->setFixedSize( max, e2->sizeHint().height() );
  e3->setFixedSize( max, e3->sizeHint().height() );

  button1 = new QPushButton(i18n("&OK"),this);
  button1->setFixedSize(button1->sizeHint());
  button1->setDefault(true);
  connect(button1,SIGNAL(clicked()),this,SLOT(accept()));

  button2 = new QPushButton(i18n("Cancel"),this);
  button2->setFixedSize(button2->sizeHint());
  connect(button2,SIGNAL(clicked()),this,SLOT(reject()));


  QVBoxLayout *mainLayout = new QVBoxLayout(this, 8, 4);

  QHBoxLayout *vbl6 = new QHBoxLayout();
  mainLayout->addLayout( vbl6 );

  QVBoxLayout *vbl3 = new QVBoxLayout();
  vbl6->addLayout( vbl3 );

  vbl3->addWidget( g1 );

  QVBoxLayout *vbl1 = new QVBoxLayout(g1, 8, 4);
  vbl1->addSpacing(8);
  vbl1->addWidget( opt1,0,AlignLeft );
  vbl1->addWidget( opt2,0,AlignLeft );
  vbl1->addWidget( opt3,0,AlignLeft );
  vbl1->addWidget( opt4,0,AlignLeft );
  vbl1->addWidget( opt5,0,AlignLeft );
  vbl1->addWidget( opt6,0,AlignLeft );
  vbl1->addWidget( opt7,0,AlignLeft );
  vbl1->addWidget( opt13,0,AlignLeft );

  vbl3->addSpacing( 10 );
  vbl3->addWidget( g2 );

  QVBoxLayout *vbl2 = new QVBoxLayout(g2, 8, 4);
  vbl2->addSpacing(8);
  vbl2->addWidget( opt8,0,AlignLeft );
  vbl2->addWidget( opt9,0,AlignLeft );
  vbl2->addWidget( opt10,0,AlignLeft );
  vbl2->addWidget( opt11,0,AlignLeft );
  vbl2->addWidget( opt12,0,AlignLeft );

  vbl6->addSpacing( 10 );

  QVBoxLayout *vbl5 = new QVBoxLayout();
  vbl6->addLayout( vbl5 );

  vbl5->addWidget( label1,0,AlignLeft );
  vbl5->addWidget( e1,0,AlignLeft );
  vbl5->addSpacing( 20 );
  vbl5->addWidget( label2,0,AlignLeft );
  vbl5->addWidget( e2,0,AlignLeft );
  vbl5->addSpacing( 20 );
  vbl5->addWidget( label3,0,AlignLeft );
  vbl5->addWidget( e3,0,AlignLeft );

  mainLayout->addSpacing( 10 );
  mainLayout->addStretch( 1 );

  QHBoxLayout *vbl4 = new QHBoxLayout();
  mainLayout->addLayout( vbl4 );
  vbl4->addStretch( 1 );
  vbl4->addWidget(button1);
  vbl4->addWidget(button2);

  resize(minimumSize());
}

int SettingsDialog::getFlags() {
  int flags;

  flags = 0;
  if (opt1->isChecked()) flags |= cfAutoIndent;
  if (opt2->isChecked()) flags |= cfBackspaceIndent;
  if (opt3->isChecked()) flags |= cfWordWrap;
  if (opt4->isChecked()) flags |= cfReplaceTabs;
  if (opt5->isChecked()) flags |= cfRemoveSpaces;
  if (opt6->isChecked()) flags |= cfWrapCursor;
  if (opt7->isChecked()) flags |= cfAutoBrackets;
  if (opt13->isChecked()) flags |= cfGroupUndo;

  if (opt8->isChecked()) flags |= cfPersistent;
  if (opt9->isChecked()) flags |= cfKeepSelection;
  if (opt10->isChecked()) flags |= cfVerticalSelect;
  if (opt11->isChecked()) flags |= cfDelOnInput;
  if (opt12->isChecked()) flags |= cfXorSelect;
  return flags;
}

int SettingsDialog::getWrapAt() {
  return e1->value();
}

int SettingsDialog::getTabWidth() {
  return e2->value();
}

int SettingsDialog::getUndoSteps() {
  return e3->value();
}
*/

ColorConfig::ColorConfig( QWidget *parent, char *name )
  : QWidget( parent, name )
{
  QGridLayout *glay = new QGridLayout( this, 6, 2, 0, KDialog::spacingHint());
  glay->setColStretch(1,1);
  glay->setRowStretch(5,1);

  QLabel *label;

  label = new QLabel( i18n("Background:"), this);
  label->setAlignment( AlignRight|AlignVCenter );
  m_back = new KColorButton( this );
  glay->addWidget( label, 0, 0 );
  glay->addWidget( m_back, 0, 1 );

  label = new QLabel( i18n("Text background:"), this);
  label->setAlignment( AlignRight|AlignVCenter );
  m_textBack = new KColorButton( this );
  glay->addWidget( label, 1, 0 );
  glay->addWidget( m_textBack, 1, 1 );

  label = new QLabel( i18n("Selected:"), this);
  label->setAlignment( AlignRight|AlignVCenter );
  m_selected = new KColorButton( this );
  glay->addWidget( label, 2, 0 );
  glay->addWidget( m_selected, 2, 1 );

  label = new QLabel( i18n("Found:"), this);
  label->setAlignment( AlignRight|AlignVCenter );
  m_found = new KColorButton( this );
  glay->addWidget( label, 3, 0 );
  glay->addWidget( m_found, 3, 1 );

  label = new QLabel( i18n("Selected + found:"), this);
  label->setAlignment( AlignRight|AlignVCenter );
  m_selFound = new KColorButton( this );
  glay->addWidget( label, 4, 0 );
  glay->addWidget( m_selFound, 4, 1 );

  // QWhatsThis help
  QWhatsThis::add(m_back, i18n("Sets the background color of the editing area"));
  QWhatsThis::add(m_textBack, i18n("Sets the background color of the text"));
  QWhatsThis::add(m_selected, i18n("Sets the background color of the selection. To set the text color for selected text, use the &quot;<b>Configure Highlightning</b>&quot; dialog."));
  QWhatsThis::add(m_found, i18n("Sets the background color of text found by the <b>Find</b> functions"));
  QWhatsThis::add(m_selFound, i18n("Sets the background color of text found within a selection"));

}


ColorConfig::~ColorConfig()
{
}

void ColorConfig::setColors(QColor *colors)
{
  m_textBack->setColor( colors[0] );
  m_selected->setColor( colors[1] );
  m_found->setColor( colors[2] );
  m_selFound->setColor( colors[3] );
  m_back->setColor( colors[4] );
}

void ColorConfig::getColors(QColor *colors)
{
  colors[4] = m_back->color();
  colors[0] = m_textBack->color();
  colors[1] = m_selected->color();
  colors[2] = m_found->color();
  colors[3] = m_selFound->color();
}


#include "kwdialog.moc"
