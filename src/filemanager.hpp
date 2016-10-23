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
#include <utility>

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

        void searchNext();
        /*  searches for the next occurrence of `_searchPattern` */

        void searchPrevious();
        /*  searches for the previous occurrence of `_searchPattern` */

        void toggleHidden();
        /*  toggle whether hidden files are shown */

        void handleCommandUpdate(const QString& command);
        /*  handles a command entery as it's being typed by the user */

        void handleCommand(const QString& command);
        /*  handles a command entered by the user */

    protected:
        void keyPressEvent(QKeyEvent* event) override;
        void moveSelection(QAbstractItemView::CursorAction action);

    private:
        std::unique_ptr<QFileSystemModel> _fsModel;
        KeyBindingTable _keyBindings;
        QString _pathWaitingToBeLoaded;
        QString _searchPattern;
        bool _searchInReverse;
        Qt::CaseSensitivity _searchCaseSensitivity;

        void change_directory(QString const& path);
        /*  changes the directory being displayed to `path` */

        static bool beginsWith(QString const& str, QString const& pattern);
        /*  returns true iff the first n characters of `str` are the same as the
            characters in `pattern` where n is the length of `pattern`
        */

        /*
        searches for the next occurrence of `_searchPattern`
        */
        template <typename Predicate, typename CircularIterator, typename NextSiblingFunction>
        static auto cirularSearch(Predicate&& pred, CircularIterator&& startPoint, NextSiblingFunction&& next) -> std::pair<bool, CircularIterator>{
            auto i = startPoint;
            while (i.isValid()) {
                if (pred(i)) {
                    return std::make_pair(true, i);
                }
                i = next(i);
                if (i == startPoint) { // if we're back to the start,
                    break;             // then we've found nothing and wrapped around
                }
            }
            return std::make_pair(false, i);
        }

        bool indexMatchsSearchPattern(const QModelIndex& i) {
            return i.data(Qt::DisplayRole).toString().startsWith(_searchPattern, _searchCaseSensitivity);
        }

        template <typename QSTRING>
        static bool isAllLower(QSTRING&& s){
            for (auto&& c : s)
                if (c.isUpper())
                    return false;
            return true;
        }

        //template <typename QMODELINDEX>
        //static QModelIndex nextSibling(QMODELINDEX&& i) {
        static QModelIndex nextSibling(QModelIndex const& i) {
            return i.sibling((i.row() + 1) % i.model()->rowCount(i.parent()), i.column());
        }

        //template <typename QMODELINDEX>
        //static QModelIndex previousSibling(QMODELINDEX&& i) {
        static QModelIndex previousSibling(QModelIndex const& i) {
            return i.sibling(i.row() > 0 ? (i.row() - 1) : i.model()->rowCount(i.parent()) - 1, i.column());
        }

    private slots:
        void selectFirstChildIfNeeded(const QString& path);
};

#endif // FILEMANAGER
