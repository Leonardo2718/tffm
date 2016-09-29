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

#ifndef KEYBINDINGTABLE_HPP
#define KEYBINDINGTABLE_HPP

// standard libraries
#include <memory>
#include <vector>

// Qt classes
#include <QWidget>
#include <QShortcut>

namespace tffm {
    class KeyBindingTable;
}

class tffm::KeyBindingTable {
    public:
        explicit KeyBindingTable(QWidget* parent) :_parent{parent} {}

        void add(QKeySequence const& keys, const char* slot) {
            auto s = std::make_unique<QShortcut>(keys, _parent);
            QObject::connect(s.get(), SIGNAL(activated()), _parent, slot);
            //connect(s.get(), SIGNAL(activatedAmbiguously()), _parent, slot);
            _keyBindings.push_back(std::move(s));
        }

    private:
        QWidget* _parent;
        std::vector<std::unique_ptr<QShortcut>> _keyBindings;
};

#endif // KEYBINDINGTABLE_HPP
