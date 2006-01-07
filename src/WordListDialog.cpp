//---------------------------------------------------------------------------
// WordListDialog.cpp
//
// The dialog for editing a list of words.
//
// Copyright 2005, 2006 Michael W Thelen <mike@pietdepsi.com>.
//
// This file is part of Zyzzyva.
//
// Zyzzyva is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Zyzzyva is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---------------------------------------------------------------------------

#include "WordListDialog.h"
#include "ZPushButton.h"
#include "Auxil.h"
#include "Defs.h"
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QVBoxLayout>

const QString DIALOG_CAPTION = "Edit Word List";
const QString LIST_HEADER_PREFIX = "Word List : ";

using namespace Defs;

//---------------------------------------------------------------------------
//  WordListDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
WordListDialog::WordListDialog (QWidget* parent, Qt::WFlags f)
    : QDialog (parent, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this, MARGIN, SPACING);
    Q_CHECK_PTR (mainVlay);

    wordList = new Q3ListView (this);
    Q_CHECK_PTR (wordList);
    wordList->setResizeMode (Q3ListView::LastColumn);
    wordList->addColumn ("");
    updateListHeader();
    mainVlay->addWidget (wordList);

    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING);
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    ZPushButton* openFileButton = new ZPushButton ("Open &File...");
    Q_CHECK_PTR (openFileButton);
    openFileButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (openFileButton, SIGNAL (clicked()), SLOT (openFileClicked()));
    buttonHlay->addWidget (openFileButton);

    ZPushButton* clearButton = new ZPushButton ("&Clear");
    Q_CHECK_PTR (clearButton);
    clearButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (clearButton, SIGNAL (clicked()), SLOT (clearClicked()));
    buttonHlay->addWidget (clearButton);

    buttonHlay->addStretch (1);

    ZPushButton* okButton = new ZPushButton ("&OK");
    Q_CHECK_PTR (okButton);
    okButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    okButton->setDefault (true);
    connect (okButton, SIGNAL (clicked()), SLOT (accept()));
    buttonHlay->addWidget (okButton);

    ZPushButton* cancelButton = new ZPushButton ("Cancel");
    Q_CHECK_PTR (cancelButton);
    cancelButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (cancelButton, SIGNAL (clicked()), SLOT (reject()));
    buttonHlay->addWidget (cancelButton);

    setCaption (DIALOG_CAPTION);
}

//---------------------------------------------------------------------------
//  ~WordListDialog
//
//! Destructor.
//---------------------------------------------------------------------------
WordListDialog::~WordListDialog()
{
}

//---------------------------------------------------------------------------
//  setWords
//
//! Set the contents of the word list to be the words in a space-separated
//! string of words.
//
//! @param string the word list string
//---------------------------------------------------------------------------
void
WordListDialog::setWords (const QString& string)
{
    wordList->clear();
    QStringList words = QStringList::split (" ", string);
    QStringList::iterator it;
    for (it = words.begin(); it != words.end(); ++it)
        new Q3ListViewItem (wordList, *it);
    updateListHeader();
}

//---------------------------------------------------------------------------
//  getWords
//
//! Return the word list, with words separated by single spaces.
//
//! @return the word list string
//---------------------------------------------------------------------------
QString
WordListDialog::getWords() const
{
    QString str;
    Q3ListViewItem* item = 0;
    for (item = wordList->firstChild(); item; item = item->nextSibling()) {
        if (!str.isEmpty())
            str += " ";
        str += item->text (0);
    }
    return str;
}

//---------------------------------------------------------------------------
//  openFileClicked
//
//! Called when the Open File button is clicked.  Open a file chooser dialog.
//---------------------------------------------------------------------------
void
WordListDialog::openFileClicked()
{
    QString filename = QFileDialog::getOpenFileName (this,
        "Open Word List File", Auxil::getWordsDir(), "Text Files (*.txt)");

    if (filename.isEmpty())
        return;

    QFile file (filename);
    if (!file.open (QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning (this, "Error Opening Word List File",
                              "Cannot open file '" + filename + "': " +
                              file.errorString());
        return;
    }

    QApplication::setOverrideCursor (Qt::waitCursor);
    QString line;
    char* buffer = new char [MAX_INPUT_LINE_LEN];
    while (file.readLine (buffer, MAX_INPUT_LINE_LEN) > 0) {
        QString line (buffer);
        line = line.simplifyWhiteSpace();
        if (!line.length() || (line.at (0) == '#'))
            continue;
        QString word = line.section (' ', 0, 0).upper();
        new Q3ListViewItem (wordList, word);
    }
    delete[] buffer;

    QApplication::restoreOverrideCursor();

    updateListHeader();
}

//---------------------------------------------------------------------------
//  clearClicked
//
//! Called when the Clear button is clicked.  Clears the word list.
//---------------------------------------------------------------------------
void
WordListDialog::clearClicked()
{
    wordList->clear();
    updateListHeader();
}

//---------------------------------------------------------------------------
//  updateListHeader
//
//! Update the list header with the current number of words.
//---------------------------------------------------------------------------
void
WordListDialog::updateListHeader()
{
    wordList->setColumnText (0, LIST_HEADER_PREFIX + QString::number
                             (wordList->childCount()) + " words");
}
