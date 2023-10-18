Project Group Number: 3

Group Members: Gianni Guadagno - guada028 Jackson Reichow - reich656 Tyler Larson - lars6653

Testing Machine: login01.cselabs.umn.edu

Changes to Makefile or Existing Files: 
added "out" folder command, makeOutFolder, used TA given makefile from piazza

Contributions:
all: helped complete leaf process
Jackson: created readme, delete duplicate files
Tyler: helped with leaf process, creating symlinks
Gianni: making root process, helped with redirection

redirection, create symlinks and delete duplicate files still needed

Implementation Plan:

redirection:
Function redirection(dup_list, size, root_dir):
    - Determine the filename for the output file based on root_dir (e.g., "root1.txt" for "./root_directories/root1")
- Construct the full path for the output file in the output_file_folder
- - Redirect standard output to the output file (using dup2 or freopen)
 - - For each symbolic link in dup_list:
        - Read the content of the symbolic link (which should be the path to the original file)
        - Write the symbolic link's path and its content (original file path) to the output file
    - Reset standard output to its original state if necessary
    - Return success/failure status
endfunction


create symlinks:
Function create_symlinks(dup_list, retain_list, size):
- For each index in the range of the size:
  - Get the duplicate file path from dup_list
    - Get the target file path (the file to link to) from retain_list
    - Create a symbolic link at the duplicate file's location that points to the target file
      - Delete the duplicate file if it exists
        - Use symlink() to create a new symbolic link
  - Return success/failure status
EndFunction


delete duplicate files:
Function delete_duplicate_files(dup_list, size):
- For each file in dup_list:
   - Delete the file at the current path
     - Use unlink() or remove() to delete the file
- Return success/failure status
EndFunction

