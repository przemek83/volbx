Description
Volbx is a graphical tool for data manipulation written in C++/QT. User can 
load data, filter, select, visualize, export and more using few clicks. 
Volbx reads, along with inner application format, xlsx and ods spreadsheet files. 
User can save output stream as csv or xlsx and plots as png files.
Dynamic and interactive plots can be zoomed, moved, reset to initial state, 
quickly exported to clipboard and more. Application calculate dynamically such 
handy values as quantiles, standard deviation, average and more for given sample.

This tool was originally created for real estate appraisal to give possibility, 
for people working in that area, to estimate past, current and future value of 
analysed properties. Project/start-up was dropped and decision about release of 
less specialized application was made. Most advanced and dedicated for real 
estates functionalities were cut but still some are quite useful.

Used QT 5.3.2, QWT 6.1.1, QuaZip 0.7, Boost 1.55.0, MinGW 4.8.2


Installation/Compilation
First make sure you have QT, QWT, QuaZip, Boost and compiler. Set environment 
variables to Boost (BOOST_ROOT), QWT (QWT_ROOT) and QuaZip (QUAZIP_ROOT) or 
modify .pro files by directly set correct paths to mentioned libs.
After that use QT Creator to compile each project or by hand in root directory execute:
qmake <.pro file>
make
where <.pro file> = {volbx.pro, Update.pro, UnitTests.pro}


Licensing
Volbx can be used using LGPLv2 or LGPLv3. 
Volbx uses following software and licenses:
    * QT, QWT and QuaZIP libraries - LGPL licences (more on qt-project.org, 
    quazip.sourceforge.net and qwt.sourceforge.net),
    * Boost - Boost Software License (visit www.boost.org),
    * Zlib - zlib license (can be found on zlib.net).


Testing
1) Compile project UnitTests.pro,
2) Download test files from projects page on SourceForge or create own files to 
match expected by unit test content,
3) Add downloaded/created directories and file to binary runtime directory. 
There should be 2 directories ("Data" for inner format files and "TestSpreadsheets" 
for spreadsheets tests) and file named "config" with configuration to test.
4) Run "UnitTests" binary and check results for "PASS" and "FAIL" statuses.

Directory "TestSpreadsheets" has some subdirectories in which test files are located. 
Names of those subdirectories are hardcoded in "UnitTests" binary. If you add new test 
files, or you need to update comparison results, you can modify "UnitTests" to generate 
new comparison data by uncomment one line in method "initTestCase()".


Contact
Feel free to contact me in any matter using prz3m3k.urban@gmail.com

