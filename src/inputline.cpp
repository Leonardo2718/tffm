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

// project headers
#include "inputline.hpp"

// Qt classes
#include <QKeyEvent>

tffm::InputLine::InputLine(QWidget* parent) : QLineEdit{parent}, _keyBindings{this} {
    // set key bindings
    _keyBindings.add(QKeySequence{Qt::Key_Escape}, this, &InputLine::leave);

    // connect signals to slots
    connect(this, &InputLine::textChanged, this, &InputLine::tryLeave);
    connect(this, &InputLine::returnPressed, this, &InputLine::sendCommand);

    // setup widget
    setHidden(true);
}

void tffm::InputLine::enterSearchMode() {
    grabKeyboard();
    setHidden(false);
    setText("/");
}

void tffm::InputLine::enterReverseSearchMode() {
    grabKeyboard();
    setHidden(false);
    setText("?");
}

void tffm::InputLine::enterCommandMode() {
    grabKeyboard();
    setHidden(false);
    setText(":");
}

void tffm::InputLine::leave() {
    setHidden(true);
    clear();
    releaseKeyboard();
}

void tffm::InputLine::tryLeave(QString const& text) {
    if (text == "") {
        leave();
    }
}

void tffm::InputLine::sendCommand() {
    emit commandEntered(text());
    leave();
}
