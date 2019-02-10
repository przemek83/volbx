## Example view:
![](Screenshot1.jpg?raw=true "")

## Description
Volbx is a graphical tool for data manipulation written in C++/Qt. User can load, filter, select, visualize, export data and more using few clicks. Volbx reads, along with inner application format, xlsx and ods spreadsheet files. User can save data set as csv or xlsx and plots as png files. Dynamic and interactive plots can be zoomed, moved, reset to initial state, quickly exported to clipboard and more. Application calculate dynamically handy values as quantiles, standard deviation, average and more for given data set.

This tool was originally created for real estate appraisal to give possibility, for people working in that area, to estimate past, current and future value of analyzed properties. Project/start-up was dropped and decision about release of less specialized application was made by me. Most advanced and dedicated for real estates functionalities were cut out but still some are quite useful.

## Built using:
    Qt 5.12
    QtCreator 4.8.1
    Qwt 6.1
    QuaZip 0.7.6
    MinGW with GCC 7.3.0, 64 bit
    Windows 10, v. 1809
    Cmake 3.12.3
    Git 2.19.1
    Svn 1.11.1
    Zlib 1.2.11

## Installation/Compilation
Use Cmake directly or via QtCreator. Cmake **should** configure everything automatically, download dependencies using Git (QuaZip, Zlib) and SVN (Qwt), build dependencies, compile Volbx and create binaries. This is of course "happy path" ;)    
To launch and work properly Volbx needs dynamically linked Qwt and QuaZip libs in lib search path or in working directory (usually directory where Volbx binary is and it is launched from).

## Licensing
Volbx can be used using LGPLv2 or LGPLv3. 
Volbx uses following software and licenses:    
* Qt, Qwt and QuaZip libraries - LGPL licences (more on qt-project.org, quazip.sourceforge.net and qwt.sourceforge.net),    
* Zlib - zlib license (can be found on zlib.net).

## Updater (not migrated to Cmake yet)
TODO

## Testing (not migrated to Cmake yet)
1) Compile project UnitTests.pro.
2) Download test files from projects page on SourceForge or create own files to match expected by unit test content.
3) Add downloaded/created directories and file to binary run time directory. There should be 2 directories ("Data" for inner format files and "TestSpreadsheets" for spreadsheets tests) and file named "config" with configuration to test.
4) Run "UnitTests" binary and check results for "PASS" and "FAIL" statuses.
Directory "TestSpreadsheets" has some sub directories in which test files are located. Names of those sub directories are hard coded in "UnitTests" binary. If you add new test files, or you need to update comparison results, you can modify "UnitTests" to generate new comparison data by uncomment one line in method "initTestCase()".

## Potential further improvements
* Clean and simplify whole code (it was created between 2010 and 2013 when I had only few years of experience and veeery little knowledge how good code should look like...).
* Upgrade whole code to use C++17.
* Migrate testing and Updater parts to Cmake.
* Increase test coverage.

## More screenshots:
![Alt text](Screenshot2.jpg?raw=true "")![Alt text](Screenshot3.jpg?raw=true "")![Alt text](Screenshot4.jpg?raw=true "")
