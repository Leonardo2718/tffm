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
#include "keybindingtable.hpp"

tffm::EnterKeyReceiver::EnterKeyReceiver(Callable f) : _f{f} {}

bool tffm::EnterKeyReceiver::eventFilter(QObject* obj, QEvent* event) {
    if (event->type()==QEvent::KeyPress) {
    QKeyEvent* key = static_cast<QKeyEvent*>(event);
    if ( (key->key()==Qt::Key_Enter) || (key->key()==Qt::Key_Return) ) {
        _f();
    } else {
        return QObject::eventFilter(obj, event);
    }
        return true;
    } else {
        return QObject::eventFilter(obj, event);
    }
    return false;
}

void tffm::KeyBindingTable::add(QKeySequence const& keys, const char* handler) {
    add(keys, static_cast<QObject*>(_listener), handler);
}

void tffm::KeyBindingTable::add(QKeySequence const& keys, QObject* handler, const char* f) {
    auto s = std::make_unique<QShortcut>(keys, _listener);
    QObject::connect(s.get(), SIGNAL(activated), handler, f);
    //connect(s.get(), SIGNAL(activatedAmbiguously()), std::forward<Handler>(handler), std::forward<Callable>(f));
    _keyBindings.push_back(std::move(s));
}
