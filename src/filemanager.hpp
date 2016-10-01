/*
Project: tffm
Author: Leonardo Banderali

Description:
    tffm is a simple, keyboard-centric file manager intended for use with tiling
    window managers such as Xmonad.

License:

    MIT License

    Copyright (c) 2016 Leonardo Banderali

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP

// project headers
#include "keybindingtable.hpp"

// standard libraries
#include <memory>

// Qt classes
#include <QListView>
#include <QFileSystemModel>
#include <QString>

namespace tffm { class FileManager; }

class tffm::FileManager : public QListView {
    Q_OBJECT

    public:
        explicit FileManager(QWidget* parent = nullptr);

    public slots:
        void moveSelectionUp();
        void moveSelectionDown();
        void moveSelectionTop();
        void moveSelectionBottom();

        void enterSelectedDirectory();
        void cdUp();

        void openCurrent();

        void searchFor(const QString& pattern);
        /*  searches for the next of occurrence of `pattern`, processing it
            (removing leading character, assuming it came from a command)
            and assigning it to `_searchPattern`
        */

        void searchNext();
        /*  searches for the next occurrence of `_searchPattern`*/

    protected:
        void keyPressEvent(QKeyEvent* event) override;
        void moveSelection(QAbstractItemView::CursorAction action);

    private:
        std::unique_ptr<QFileSystemModel> _fsModel;
        KeyBindingTable _keyBindings;
        QString _pathWaitingToBeLoaded;
        QString _searchPattern;

        void change_directory(QString const& path);
        /*  changes the directory being displayed to `path` */

        // some convienience functions

        template <typename QSTRING>
        static bool isAllLower(QSTRING&& s){
            for (auto&& c : s)
                if (c.isUpper())
                    return false;
            return true;
        }

        template <typename QMODELINDEX>
        static QModelIndex nextSibling(QMODELINDEX&& i) {
            return i.sibling((i.row() + 1) % i.model()->rowCount(i.parent()), i.column());
        }

    private slots:
        void selectFirstChildIfNeeded(const QString& path);
};

#endif // FILEMANAGER
