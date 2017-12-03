## FAT File System Simulator

This is implementation of utilities that perform operations on a file system similar to Microsoft's FAT file system.
The file system has 3 major components: the super block, the File Allocation Table (FAT) and the directory structure.

*There are four file system images: disk1.img, disk1_empty.img, disk2.img and disk2_empty.img that could be used for testing as well as some sample files.


### Implemented Utilities

#### PART 1. 

'./diskinfo disk1_empty.img'  displays information about the file system.

Sample output:
Super block information:
Block size: 512
Block count: 15360
FAT starts: 1
FAT blocks: 120
Root directory start: 121
Root directory blocks: 4
FAT information:
Free Blocks: 15235
Reserved Blocks: 121
Allocated Blocks: 4

#### PART 2

'./disklist disk1.img' displays the contents of the root directory in the file system.

Sample output:
F   91        readme.txt 2016/03/02 12:20:19
F   56        bar.txt 2016/03/02 12:20:43


#### PART 3

'./diskget disk1.img foo.txt' will copy a file (in this case foo.txt) from the file system to the current directory in Unix. If the specified file is not found in the root directory of the file system, you will get the message “File not found”.

#### PART 4

'./diskput disk1.img foo.txt' will copy a file from the current Unix directory into the file system. If the specified file is not found, you will get the message “File not found”.
