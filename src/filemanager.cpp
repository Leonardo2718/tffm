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
#include <QFileInfo>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QApplication>
#include <QClipboard>
#include <QPair>
#include <QMessageBox>
#include <QDebug>

tffm::FileManager::FileManager(QWidget* parent) : QListView{parent}, _keyBindings{this} {
    _fsModel = std::make_unique<QFileSystemModel>();
    _searchPattern = QString{};
    _searchInReverse = false;

    // configure widget
    setModel(_fsModel.get());
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSelectionMode(QAbstractItemView::SingleSelection);

    // configure key bindings
    _keyBindings.add(QKeySequence{Qt::Key_Right}, this, &FileManager::enterSelectedDirectory);
    _keyBindings.add(QKeySequence{Qt::Key_L}, this, &FileManager::enterSelectedDirectory);

    _keyBindings.add(QKeySequence{Qt::Key_J}, this, &FileManager::moveSelectionDown);
    _keyBindings.add(QKeySequence{Qt::Key_K}, this, &FileManager::moveSelectionUp);

    _keyBindings.add(QKeySequence{Qt::Key_Left}, this, &FileManager::cdUp);
    _keyBindings.add(QKeySequence{Qt::Key_H}, this, &FileManager::cdUp);

    _keyBindings.add(QKeySequence{Qt::Key_G, Qt::Key_G}, this, &FileManager::moveSelectionTop);
    _keyBindings.add(QKeySequence{Qt::SHIFT + Qt::Key_G}, this, &FileManager::moveSelectionBottom);

    _keyBindings.add(QKeySequence{Qt::Key_O}, this, &FileManager::openCurrent);
    _keyBindings.add(QKeySequence{Qt::Key_Space}, this, &FileManager::openCurrent);
    _keyBindings.addEnterKeyBinding( [this](){ openCurrent(); } );

    _keyBindings.add(QKeySequence{Qt::Key_N}, this, &FileManager::searchNext);
    _keyBindings.add(QKeySequence{Qt::SHIFT + Qt::Key_N}, this, &FileManager::searchPrevious);

    _keyBindings.add(QKeySequence{Qt::Key_Period, Qt::Key_Period}, this, &FileManager::toggleHidden);

    _keyBindings.add(QKeySequence{Qt::Key_Y, Qt::Key_Y}, this, &FileManager::copySelected);
    _keyBindings.add(QKeySequence{Qt::Key_P}, this, &FileManager::putCopy);

    _keyBindings.add(QKeySequence{Qt::Key_D, Qt::Key_D}, this, &FileManager::removeSelected);

    // connect signals to slots
    connect(_fsModel.get(), &QFileSystemModel::directoryLoaded, this, &FileManager::selectFirstChildIfNeeded);

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
    if (!_fsModel->isDir(currentIndex())) return;

    auto selectedPath = _fsModel->filePath(currentIndex());
    change_directory(selectedPath);

    updateCurrentIndex(selectedPath);
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
searches forward for the next occurrence of `_searchPattern`
*/
void tffm::FileManager::searchNext() {
    auto next = _searchInReverse ? previousSibling : nextSibling;

    // start search from the next node to ignore the current one in case it matches the search
    auto match = cirularSearch( [this](auto&& i){ return this->indexMatchsSearchPattern(i); }, next(currentIndex()), next);
    if (match.first) {
        setCurrentIndex(match.second);
    }
}

/*
searches backward for the next occurrence of `_searchPattern`
*/
void tffm::FileManager::searchPrevious() {
    auto next = _searchInReverse ? nextSibling : previousSibling;

    // start search from the next node to ignore the current one in case it matches the search
    auto match = cirularSearch( [this](auto&& i){ return this->indexMatchsSearchPattern(i); }, next(currentIndex()), next);
    if (match.first) {
        setCurrentIndex(match.second);
    }
}

/*
toggle whether hidden files are shown
*/
void tffm::FileManager::toggleHidden() {
    auto f = QDir::AllEntries | QDir::NoDotAndDotDot | ( _fsModel->filter() & QDir::Hidden ? (QDir::Filter)0x0 : QDir::Hidden);
    _fsModel->setFilter(f);
}

/*
copies the path(s) of the currently selected item(s) to the clipboard
*/
void tffm::FileManager::copySelected() {
    auto selection = QStringList{};
    for (auto&& i : selectedIndexes()) {
        selection << _fsModel->filePath(i);
    }

    auto clipboard = QApplication::clipboard();
    clipboard->setText(selection.join(':'));
}

/*
makes copys of the paths in the clipboard in the current directory
*/
void tffm::FileManager::putCopy() {
    auto paths = QApplication::clipboard()->text().split(':');
    auto info = QFileInfo{};
    for (auto&& path : paths) {
        info.setFile(path);
        if (info.exists()) {
            copyRecursively(info.absoluteFilePath(), _fsModel->rootPath() + QChar('/') + info.fileName());
        }
    }
}

/*  removes the selected items from the file system */
void tffm::FileManager::removeSelected() {
    auto indexes = selectedIndexes();
    auto paths = QStringList{};
    for (auto&& i : indexes) {
        paths << _fsModel->filePath(i);
    }
    auto message = tr("Are you sure you want to delete:\n  %0").arg(paths.join("\n  "));
    auto answer = QMessageBox::question(this, "Delete these items?", message);
    if (answer == QMessageBox::Yes) {
        for (auto&& i : indexes) {
            _fsModel->remove(i);
        }
    }
}

/*
handles a command entery as it's being typed by the user
*/
void tffm::FileManager::handleCommandUpdate(QString const& command) {
    if (command.isEmpty()) return;

    // process `command`
    if (command[0] == '/' || command[0] == '?') { // if is search command
        _searchPattern = command;
        _searchPattern.remove(0, 1); // remove first character (the '/' or '?')
        _searchInReverse = (command[0] == '?');
        _searchCaseSensitivity = isAllLower(_searchPattern) ? Qt::CaseInsensitive : Qt::CaseSensitive;

        // search for an occurrence of the pattern
        auto next = _searchInReverse ? previousSibling : nextSibling;
        auto match = cirularSearch( [this](auto&& i){ return this->indexMatchsSearchPattern(i); }, currentIndex(), next);
        if (match.first) {
            setCurrentIndex(match.second);
        }
    }
}

/*
handles a command entered by the user
*/
void tffm::FileManager::handleCommand(const QString& command) {
    if (command.isEmpty()) return;

    // process `command`
    if (command.startsWith(":cd")) {   // if is cd command
        auto path = command;
        path = path.remove(0, 3).simplified();
        QDir dir = _fsModel->rootDirectory();
        if (dir.cd(path)) {
            auto path = dir.absolutePath();
            change_directory(path);
            updateCurrentIndex(path);
        }
        else {
            qDebug() << "error:" << path << "does not exist";
        }
    }
}

void tffm::FileManager::moveSelection(QAbstractItemView::CursorAction action) {
    setCurrentIndex(moveCursor(action, Qt::NoModifier));
}

/*
changes the directory being displayed to `path`
*/
void tffm::FileManager::change_directory(QString const& path) {
    auto rootIndex = _fsModel->setRootPath(path);
    setRootIndex(rootIndex);
}

/*
copies a file system item from `src` to `dest`, recursively if needed
*/
bool tffm::FileManager::copyRecursively(QString const& src, QString const& dest) {
    QList<QPair<QString,QString>> copyops;
    copyops.push_back(qMakePair(src, dest));
    QFileInfo srcInfo;
    while (!copyops.empty()) {
        auto op = copyops.back();
        copyops.pop_back();
        srcInfo.setFile(op.first);
        if (srcInfo.isFile()) {
            if (!QFile::copy(op.first, op.second)) {
                return false;
            }
        }
        else if (srcInfo.isDir()) {
            QDir targetDir{op.second};
            targetDir.cdUp();
            if (!targetDir.mkdir(QFileInfo(op.second).fileName())) {
                return false;
            }
            QDir sourceDir{op.first};
            auto fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
            for (auto&& fileName : fileNames) {
                const QString newsrc = src + QChar('/') + fileName;
                const QString newdest = dest + QChar('/') + fileName;
                copyops.push_front(qMakePair(newsrc, newdest));
            }
        }
        else {
            return false;
        }
    }
    return true;
}

void tffm::FileManager::updateCurrentIndex(QString const& currentPath) {
    // row count is only 0 if current directory hasn't
    // been loaded yet or if it's empty; assume the first
    if (_fsModel->rowCount(rootIndex()) == 0) {
        _pathWaitingToBeLoaded = currentPath;
    }
    else {
        setCurrentIndex(rootIndex().child(0,0));
    }
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
