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
    Windows 10 v1809
    Cmake 3.12.3
    Git 2.19.1
    Svn 1.11.1
    Zlib 1.2.11

## Installation/Compilation
Use Cmake directly or via QtCreator. Cmake **should** configure everything automatically, download dependencies using Git (QuaZip, Zlib) and SVN (Qwt), build dependencies, compile Volbx and create binaries. This is of course "happy path" ;)    
To launch and work properly Volbx needs dynamically linked Qwt and QuaZip libs in lib search path or in working directory (usually directory where Volbx binary is and it is launched from).

## Licensing
Volbx can be used using LGPLv3. 
Volbx uses following software and licenses:    
* Qt, Qwt and QuaZip libraries - LGPL licences (more on qt-project.org, quazip.sourceforge.net and qwt.sourceforge.net),    
* Zlib - Zlib license (can be found on zlib.net).

## Updater
1) Build target VersionCheck.
2) Run VersionCheck, following window should be shown:
![Alt text](updater.jpg?raw=true "")

## Setup update server
1) Get a domain and hosting ;)
2) Add simple .php file which contains
```
<?
echo("Volbx-Updade-Info\n");

echo("1.10");

chdir("current/");
foreach (glob("*") as $filename) {
    echo "\n$filename;" . filesize($filename);
}
?>
```
3) Create folder `current` in directory where PHP file from point above is located.
4) Add new/other version of Volbx and/or updater in `current` directory.
5) Change code of networking module in Volbx to point to proper adress - look for `Networking::getCurrentVersionRequest()` and `QNetworkRequest Networking::getDownloadFileRequest(QString file)`
6) Compile VersionChecker and run it - it should connect to given adress and download content of `current` folder.

**Remarks**:   
- Instead of size of files returned by PHP script checksums should be returned and used for checking of correctenss of downloaded files.   
- When I've created VersionChecker I've decided to use simplest solution which I was able to create. I do not know if it is safe and 'proper' enough but it worked :D I'm not web deweloper ;)   
- Windows needs admin rights when executing files having in name strings like "update", "install" or "setup". Quite ugly name `VersionChecker` was picked to workaround that problem. ;)   
- On Windows updater cannot overwrite file which is being used (executed) so ther is a special mechanism closing Volbx, running updater (Volbx binary and some libs can be replaced by new ones) and when running again Volbx replacing updater binary.

## Testing
1) Compile project.
2) Run target named tests.
3) Check test output, all tests should have status `PASSED`.
Tests are done using Qt test framework.

## Potential further improvements
* Clean and simplify whole code (it was created between 2010 and 2013 when I had only few years of experience and veeery little knowledge how good code should look like...).
* Upgrade whole code to use C++17.
* Migrate testing and Updater parts to Cmake.
* Increase test coverage.

## More screenshots:
![Alt text](Screenshot2.jpg?raw=true "")![Alt text](Screenshot3.jpg?raw=true "")![Alt text](Screenshot4.jpg?raw=true "")
