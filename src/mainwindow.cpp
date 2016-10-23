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

#include "mainwindow.hpp"

tffm::MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    // allocate members
    _centralWidget = std::make_unique<QWidget>();
    _mainLayout = std::make_unique<QVBoxLayout>();
    _fileManager = std::make_unique<FileManager>(this);
    _inputLine = std::make_unique<InputLine>(this);

    // configure layout
    _mainLayout->setMargin(0);

    // connect signals to slots
    connect(_inputLine.get(), SIGNAL(textChanged(QString)), _fileManager.get(), SLOT(handleCommandUpdate(QString)));
    connect(_inputLine.get(), SIGNAL(commandEntered(QString)), _fileManager.get(), SLOT(handleCommand(QString)));

    // set widgets
    _mainLayout->addWidget(_fileManager.get());
    _mainLayout->addWidget(_inputLine.get());
    _centralWidget->setLayout(_mainLayout.get());
    this->setCentralWidget(_centralWidget.get());
}
