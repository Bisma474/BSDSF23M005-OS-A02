## What is the crucial difference between the stat() and lstat() system calls?

## Answer:
stat() follows symbolic links and returns information about the target file, while lstat() returns information about the link itself without following it.

## In the context of the ls command, when is it more appropriate to use lstat()?

## Answer:
lstat() is used when ls needs to display symbolic links without following them — for example, showing link → target.

## How can you use bitwise operators and predefined macros to extract file type and permissions from st_mode?

## Answer:
Use the bitwise AND (&) operator with macros like S_IFDIR or S_IRUSR:
if ((st_mode & S_IFMT) == S_IFDIR)  // Check if directory
if (st_mode & S_IRUSR)              // Check read permission
## Explain the general logic for printing items in a "down then across" columnar format. Why is a simple single loop through the list of filenames insufficient for this task?

## Answer:
In a "down then across" format, items are printed column by column, not row by row. You first determine how many rows fit on the screen, then print the first column downwards, the next column beside it, and so on.
A single loop is insufficient because it prints filenames sequentially across rows, not in a columnar pattern — it cannot correctly align items into multiple columns.

## What is the purpose of the ioctl system call in this context? What would be the limitations of your program if you only used a fixed-width fallback (e.g., 80 columns) instead of detecting the terminal size?

## Answer:
ioctl() is used to get the terminal window size (number of columns and rows) so that the program can adjust column width and fit output neatly on the screen.
If you use a fixed width (like 80 columns), the output may look misaligned or wrapped incorrectly on different terminal sizes, reducing readability and wasting screen space.
## Compare the implementation complexity of the "down then across" (vertical) printing logic versus the "across" (horizontal) printing logic. Which one requires more pre-calculation and why?

## Answer:
The "down then across" (vertical) printing logic is more complex because it requires pre-calculating the number of rows and columns, determining how to evenly distribute filenames, and indexing elements correctly to print them column by column.
The "across" (horizontal) logic is simpler since filenames are printed in the order they are stored, row by row, without complex indexing.

## Describe the strategy you used in your code to manage the different display modes (-l, -x, and default). How did your program decide which function to call for printing?

## Answer:
The program checked command-line options using getopt() or manual flag checking. Based on the selected flag:

-l → called the long listing function,

-x → called the horizontal listing function,

no flag → called the default vertical display function.
This approach kept each display mode in a separate function, making the code modular and easy to maintain.
## Explain the purpose and signature of the comparison function required by qsort(). How does it work, and why must it take const void * arguments?

## Answer:
The comparison function defines how two elements are compared during sorting. Its signature is:

int compare(const void *a, const void *b);


It returns:

< 0 if a should come before b

0 if they are equal

> 0 if a should come after b

const void * is used because qsort() is generic — it can sort any data type, so it passes pointers of type void *. The const ensures the function does not modify the data being compare
## Why is it necessary to read all directory entries into memory before you can sort them? What are the potential drawbacks of this approach for directories containing millions of files?

## Answer:
All directory entries must be read into memory first because sorting requires random access to the entire list of filenames, which is only possible when all entries are stored in a data structure (like an array).
The drawback is high memory usage — for directories with millions of files, this can lead to slow performance, memory exhaustion, or even program crashes.
## How do ANSI escape codes work to produce color in a standard Linux terminal? Show the specific code sequence for printing text in green.

## Answer:
ANSI escape codes are special character sequences that control text formatting (like color, boldness, etc.) in terminals.
They start with \033[ (or \x1b[) followed by color codes and end with m.

Example to print text in green:

printf("\033[32mThis text is green\033[0m\n");


\033[32m → sets text color to green

\033[0m → resets color to default

## To color an executable file, you need to check its permission bits. Explain which bits in the st_mode field you need to check to determine if a file is executable by the owner, group, or others.

## Answer:
Check the execute permission bits in st_mode using bitwise AND:

if (st_mode & S_IXUSR) // Executable by owner
if (st_mode & S_IXGRP) // Executable by group
if (st_mode & S_IXOTH) // Executable by others


If any of these bits are set, the file is executable, and it can be displayed in a different color (e.g., green) in the output.
## In a recursive function, what is a "base case"? In the context of your recursive ls, what is the base case that stops the recursion from continuing forever?

## Answer:
A base case is the condition that terminates recursion, preventing it from running indefinitely.
In a recursive ls, the base case occurs when the program reaches a directory that has no subdirectories or when it encounters "." or ".." (the current and parent directories). At that point, the recursion stops instead of calling itself again.

## Explain why it is essential to construct a full path (e.g., "parent_dir/subdir") before making a recursive call. What would happen if you simply called do_ls("subdir") from within do_ls("parent_dir")?

## Answer:
It’s essential to build the full path because without it, the program would look for "subdir" in the current working directory, not inside "parent_dir".
If you just called do_ls("subdir"), it would likely fail to open the correct directory or process the wrong one, leading to incorrect results or errors when traversing nested directories.
