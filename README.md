# tffm

tffm is a simple, keyboard-centric file manager intended for use with tiling
window managers such as [Xmonad](http://xmonad.org/). It uses similar keybindings
to those used in vim.

## Keybindings

Here, different keybindings that do the same thing are separated by spaces

| Keybindings | Action                              | Notes                    |
|:------------|:------------------------------------|:-------------------------|
| `j` &darr;  | Move down one item                  |                          |
| `k` &uarr;  | Move up one item                    |                          |
| `h` &larr;  | Move up one directory               | (`cd ..`)                |
| `l` &rarr;  | Enter selected directory            | If the selection is not a directory, nothing happens.|
| `o` space   | Open selected item                  | If the selection is a directory, it is entered (same as `l` or &rarr;). If it's a file, it's opened using `xdg-open`. |
| `/`         | Search current directory            | Opens a prompt and searches the current directory for the text entered. |
| `?`         | search current directory in reverse |                          |
| `n`         | Find next occurrences of search     | Finds the next occurrence of a search, in the same order as the command used to start the search (sing `/` or `?`) |
| `N`         | Find previous occurrence of search  | Whatever `n` does but backwards. |
| `gg` home   | Go to first item                    |                          |
| `G` end     | Go to last item                     |                          |

## License:

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
