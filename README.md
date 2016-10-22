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
| `o` space enter | Open selected item                  | If the selection is a directory, it is entered (same as `l` or &rarr;). If it's a file, it's opened using `xdg-open`. |
| `/`         | Search current directory            | Opens a prompt and searches the current directory for the text entered. |
| `?`         | search current directory in reverse |                          |
| `n`         | Find next occurrences of search     | Finds the next occurrence of a search, in the same order as the command used to start the search (sing `/` or `?`) |
| `N`         | Find previous occurrence of search  | Whatever `n` does but backwards. |
| `gg` home   | Go to first item                    |                          |
| `G` end     | Go to last item                     |                          |
| `..`        | Toggle hidden items                 | Show/hide hidden files and directories. |

## License:

tffm is licensed under the terms of [The MIT License](https://opensource.org/) (see `LICENSE` file).
