# Project Group Number: 3

## Group Members: 
- Gianni Guadagno - guada028 
- Jackson Reichow - reich656 
- Tyler Larson - lars6653

## Testing Machine: 
- login01.cselabs.umn.edu

## Changes to Makefile or Existing Files: 
- added "outfolder" command as a dependency of "all" and "inter", which was in TA given makefile from Piazza

## Actual Contributions:
- All: leaf_process.c
- Jackson: implement delete_duplicate_files and aid in redirection and create_symlinks and README
- Tyler: Implement the redirection function
- Gianni: Implement the create_symlinks

## Implementation Plan:
- Already finished most of nonleaf_process and root_process so the
pseudocode for those is not included here.
- All we need is to implement the helper functions below:
```
Function redirection(dup_list, size, root_dir):
    Determine the filename for the output file based on root_dir
    Make the full path for the output file in the output_file_folder
    Redirect s/o to the output file with a pipe
        For each symbolic link in dup_list:
            Read the content of the symbolic link
            Write the symbolic link's path to the output file
    Return


Function create_symlinks(dup_list, retain_list, size):
    For each index in the range of the size:
        Get the duplicate file path from dup_list
        Get the target file path from retain_list (linking file)
        Create a sym link at the dup file's spot that points to the retain file
        Delete the duplicate file
    Return


Function delete_duplicate_files(dup_list, size):
    For file in dup_list:
        Delete file
    Return 
```

AI use:
-used to help debug certain failed test cases, revealed missing space/bracket in printing process