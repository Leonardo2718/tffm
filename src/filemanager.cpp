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

#include "filemanager.hpp"

#include <QKeyEvent>
#include <QScrollBar>
#include <QKeySequence>
#include <QDesktopServices>
#include <QRegularExpression>
#include <QDebug>

tffm::FileManager::FileManager(QWidget* parent) : QListView{parent}, _keyBindings{this} {
    _fsModel = std::make_unique<QFileSystemModel>();
    _searchPattern = QString{};

    // configure widget
    setModel(_fsModel.get());
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSelectionMode(QAbstractItemView::SingleSelection);

    // configure key bindings
    _keyBindings.add(QKeySequence{Qt::Key_Right}, SLOT(enterSelectedDirectory()));
    _keyBindings.add(QKeySequence{Qt::Key_L}, SLOT(enterSelectedDirectory()));

    _keyBindings.add(QKeySequence{Qt::Key_J}, SLOT(moveSelectionDown()));
    _keyBindings.add(QKeySequence{Qt::Key_K}, SLOT(moveSelectionUp()));

    _keyBindings.add(QKeySequence{Qt::Key_Left}, SLOT(cdUp()));
    _keyBindings.add(QKeySequence{Qt::Key_H}, SLOT(cdUp()));

    _keyBindings.add(QKeySequence{Qt::Key_G, Qt::Key_G}, SLOT(moveSelectionTop()));
    _keyBindings.add(QKeySequence{Qt::SHIFT + Qt::Key_G}, SLOT(moveSelectionBottom()));

    _keyBindings.add(QKeySequence{Qt::Key_O}, SLOT(openCurrent()));
    _keyBindings.add(QKeySequence{Qt::Key_Space}, SLOT(openCurrent()));

    _keyBindings.add(QKeySequence{Qt::Key_N}, SLOT(searchNext()));

    // connect signals to slots
    connect(_fsModel.get(), SIGNAL(directoryLoaded(QString)), this, SLOT(selectFirstChildIfNeeded(QString)));

    // move to home directory
    change_directory(QDir::homePath());
}

void tffm::FileManager::keyPressEvent(QKeyEvent* event) {
    switch(event->key()) {
    case Qt::Key_Enter: openCurrent(); break;
    default: QListView::keyPressEvent(event);
    }
}

void tffm::FileManager::moveSelectionUp() {
    moveSelection(QAbstractItemView::MoveUp);
}

void tffm::FileManager::moveSelectionDown() {
    moveSelection(QAbstractItemView::MoveDown);
}

void tffm::FileManager::moveSelectionTop() {
    moveSelection(QAbstractItemView::MoveHome);
}

void tffm::FileManager::moveSelectionBottom() {
    moveSelection(QAbstractItemView::MoveEnd);
}

void tffm::FileManager::enterSelectedDirectory() {
    auto selectedPath = _fsModel->filePath(currentIndex());
    change_directory(selectedPath);

    // row count is only 0 if current directory hasn't
    // been loaded yet or if it's empty; assume the first
    if (_fsModel->rowCount(rootIndex()) == 0) {
        _pathWaitingToBeLoaded = selectedPath;
    }
    else {
        setCurrentIndex(rootIndex().child(0,0));
    }
}

void tffm::FileManager::cdUp() {
    auto i = rootIndex();
    auto pwd = _fsModel->rootDirectory();
    pwd.cdUp();
    change_directory(pwd.absolutePath());
    setCurrentIndex(i);
}

void tffm::FileManager::openCurrent() {
    auto ci = currentIndex();
    if (_fsModel->isDir(ci)) {
        enterSelectedDirectory();
    }
    else {
        QDesktopServices::openUrl(QUrl{tr("file:///") + _fsModel->filePath(ci)});
    }
}

/*
searches for the next of occurence of `pattern`, processing it
(removing leading character, assuming it came from a command)
and assigning it to `_searchPAttern`
*/
void tffm::FileManager::searchFor(QString const& pattern) {
    if (pattern.isEmpty()) return;

    // process `pattern`
    _searchPattern = pattern;
    _searchPattern.remove(0, 1); // remove first character (the slash)

    // search for an occurrence of the pattern
    Qt::CaseSensitivity caseSensitivity = isAllLower(_searchPattern) ? Qt::CaseInsensitive : Qt::CaseSensitive;
    auto i = currentIndex();
    while (true) {
        if (!i.isValid()) {
            break;
        }
        else if (i.data(Qt::DisplayRole).toString().startsWith(_searchPattern, caseSensitivity)) {
            // found a match, so select it
            setCurrentIndex(i);
            break;
        }
        else if (nextSibling(i) != currentIndex()) { // if the next sibling (row) is the current index,
            i = nextSibling(i);                      // then we've found nothing and wrapped around
        }
        else {
            break;
        }
    }
}

/*
searches for the next occurrence of `_searchPattern`
*/
void tffm::FileManager::searchNext() {
    // search for the next occurrence of the pattern
    Qt::CaseSensitivity caseSensitivity = isAllLower(_searchPattern) ? Qt::CaseInsensitive : Qt::CaseSensitive;
    auto i = currentIndex();
    while (true) {
        i = nextSibling(i);
        if (!i.isValid() || i == currentIndex()) { // if we're back to the current index,
            break;                                 // then we've found nothing and wrapped around
        }
        else if (i.data(Qt::DisplayRole).toString().startsWith(_searchPattern, caseSensitivity)) {
            // found a match, so select it
            setCurrentIndex(i);
            break;
        }
    }
}

/*
changes the directory being displayed to `path`
*/
void tffm::FileManager::change_directory(QString const& path) {
    auto rootIndex = _fsModel->setRootPath(path);
    setRootIndex(rootIndex);
}

void tffm::FileManager::moveSelection(QAbstractItemView::CursorAction action) {
    setCurrentIndex(moveCursor(action, Qt::NoModifier));
}

void tffm::FileManager::selectFirstChildIfNeeded(const QString& path) {
    if (path == _pathWaitingToBeLoaded) {
        auto pathIndex = _fsModel->index(path);
        if (_fsModel->rowCount(pathIndex) > 0) { // not strictly required but doesn't hurt
            _fsModel->sort(0);
            setCurrentIndex(pathIndex.child(0,0));
            _pathWaitingToBeLoaded = QString{};
        }
    }
}
