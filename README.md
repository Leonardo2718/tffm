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
| `:`         | Enter command mode                  | See Commands section.    |
| `yy`        | Copy currently selected items to clipboard | Put the paths of the currently selected items (separated by `:`s) into CLIPBOARD. |
| `p`         | Put/past items from clipboard       | Items are put in current directory. |

## Commands

Type `:` to enter command mode. In command mode, you can enter commands that will be executed
by tffm. The following commands are currently supported:

| Command     | Description                                                    |
|:------------|:---------------------------------------------------------------|
| `cd PATH`   | Changes the current directory to `PATH`. `PATH` can be a relative or absolute directory path. If `PATH` does not exist, nothing happens. Spaces in `PATH` *do not* need to be escaped. |

## License:

tffm is licensed under the terms of [The MIT License](https://opensource.org/) (see `LICENSE` file).
