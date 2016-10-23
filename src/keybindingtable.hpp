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
#include <functional>
#include <utility>

// Qt classes
#include <QObject>
#include <QEvent>
#include <QKeyEvent>
#include <QWidget>
#include <QShortcut>

namespace tffm {
    class EnterKeyReceiver;
    class KeyBindingTable;
}

/*
even handler for `Enter` key events
*/
class tffm::EnterKeyReceiver : public QObject {
    Q_OBJECT
    public:
        using Callable = std::function<void(void)>;
        explicit EnterKeyReceiver(Callable f);

    protected:
        bool eventFilter(QObject* obj, QEvent* event) override;

    private:
        Callable _f;
};

class tffm::KeyBindingTable {
    public:
        explicit KeyBindingTable(QWidget* listener) : _listener{listener} {}

        template <typename CallableHandler>
        void add(QKeySequence const& keys, CallableHandler&& handler);

        template <typename Handler, typename Callable>
        void add(QKeySequence const& keys, Handler&& handler, Callable&& f);

        void add(QKeySequence const& keys, const char* handler);

        void add(QKeySequence const& keys, QObject* handler, const char* f);

        /*  a special function is needed to handle the `Enter` key, which need an event filter to be handled */
        void addEnterKeyBinding(EnterKeyReceiver::Callable f) {
            auto r = std::make_unique<EnterKeyReceiver>(f);
            _listener->installEventFilter(r.get());
            _enterKeyReceivers.push_back(std::move(r));
        }

    private:

        QWidget* _listener;
        std::vector<std::unique_ptr<QShortcut>> _keyBindings;
        std::vector<std::unique_ptr<EnterKeyReceiver>> _enterKeyReceivers;
};

template <typename CallableHandler>
void tffm::KeyBindingTable::add(QKeySequence const& keys, CallableHandler&& handler) {
    auto s = std::make_unique<QShortcut>(keys, _listener);
    QObject::connect(s.get(), &QShortcut::activated, std::forward<CallableHandler>(handler));
    //QObject::connect(s.get(), &QShortcut::activatedAmbiguously, std::forward<CallableHandler>(handler));
    _keyBindings.push_back(std::move(s));
}

template <typename Handler, typename Callable>
void tffm::KeyBindingTable::add(QKeySequence const& keys, Handler&& handler, Callable&& f) {
    auto s = std::make_unique<QShortcut>(keys, _listener);
    QObject::connect(s.get(), &QShortcut::activated, std::forward<Handler>(handler), std::forward<Callable>(f));
    //QObject::connect(s.get(), &QShortcut::activatedAmbiguously, std::forward<Handler>(handler), std::forward<Callable>(f));
    _keyBindings.push_back(std::move(s));
}

#endif // KEYBINDINGTABLE_HPP
