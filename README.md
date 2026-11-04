# application-tracker

A desktop app for keeping track of internship applications, written in C++ with Qt.

Each application has a title, company, link, due date, applied date, status, notes, and any files you attached (resume, cover letter). The table shows them with colored status labels, and applications that are past their deadline and not submitted yet turn red.

- Search by title, company, or notes
- Filter by status with the buttons under the search box
- Click a column header to sort
- Right-click an application to edit it, open its link or files, change its status, or delete it
- Everything is saved in a local SQLite file, so it's still there next time

## Build

You need Qt 6.5 or newer and CMake.

```
cmake -B build -DCMAKE_PREFIX_PATH=<path to your Qt install>
cmake --build build
```

Run it with `build/tracker` (`build\tracker.exe` on Windows).

The tests are in `tests/`. On Windows, run them from a terminal that has Qt's `bin` folder on the PATH:

```
ctest --test-dir build --output-on-failure
```

## Code

- `src/logic.cpp`: the rules that don't need Qt's interface code (overdue, search, filtering, counts)
- `src/applicationstore.cpp`: saving and loading in SQLite
- `src/applicationmodel.cpp` and `src/applicationdelegate.cpp`: the table and how its cells are drawn
- `src/mainwindow.cpp` and `src/editdialog.cpp`: the window and the edit form
