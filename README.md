## Table of content
- [Example view](#example-view-)
- [Description](#description)
- [History](#history)
- [Used tools and libs](#used-tools-and-libs-)
- [Installation/Compilation](#installation-compilation)
- [Licensing](#licensing)
- [Updater](#updater)
- [Setup update server](#setup-update-server)
- [Testing](#testing)
- [Potential further improvements](#potential-further-improvements)
- [Some screenshots:](#some-screenshots-)

## Example view
![](exampleScreen.gif?raw=true "")

## Description
Volbx is a graphical tool used for data manipulation and written in C++/Qt. User can:
 * load data - opens xlsx and ods spreadsheet files,
 * filter data - filters panel can be used to define data range on each column,
 * select data - user can select rows on main data table, 
 * visualize data - multiple types of diagrams built in (histogram, grouping, linear regression, quantiles), adjusts dynamically according to filtering and selection,
 * order data - each column can be ordered ascending and descending, 
 * export data - selected set of data, together with linked plots, can be exported to xlsx spreadsheet file or CSV,
 * and more. 

## History
Tool was originally created for real estate appraisal as it gives possibility, for people working in that area, to estimate past, current and future value of analyzed properties. When project/start-up was dropped, I've decided to release less specialized application as open source project. Most advanced and dedicated for real estates functionalities were cut out, basic and intermediate ones stayed.

## Used tools and libs
| Tool |  Windows | Lubuntu |
| --- | --- | --- |
| OS version | 10 v1909 | 20.04 |
| GCC | 7.3.0 | 9.3.0 |
| Cmake | 3.14 | 3.17 |
| Git | 2.20 | 2.25 |
| Svn | 1.11.1 | 1.13.0 |
| Qt | 5.12.6 | 5.12.7 |
| QtCreator | 4.12.0 | 4.13.0 |
| Qwt | 6.1.4 | 6.1.4 |
| QuaZip | 0.9 | 0.9 |
| Zlib | 1.2.11 | 1.2.11 |
| Eible | master | master |
| QwtBle | master | master |
| Wble | master | master |

## Installation/Compilation
Use directly Cmake or QtCreator. Cmake **should**:
- configure everything automatically, 
- download dependencies using Git (QuaZip, Zlib, Eible, Wble, Qwtble) and SVN (Qwt), 
- build dependencies, 
- compile Volbx and create binaries.  

This is of course happy path...  

**TIPS**: set `CMAKE_PREFIX_PATH` env variable (should point to Qt) and add `qmake` location to `PATH` variable (QWT building expects it).   

## Licensing
Volbx can be used using LGPLv3. 

Volbx uses following software and licenses:    
* Qt, Qwt, QuaZip, Eible, Wble and Qwtble libraries - LGPL licences (more on qt-project.org, quazip.sourceforge.net, qwt.sourceforge.net and github.com/przemek83),    
* Zlib - Zlib license (can be found on zlib.net).

## Updater
1) Build target `VersionCheck`.
2) Run `VersionCheck`.
3) Following window should be shown:    
![Alt text](updateScreen.gif?raw=true "")

## Setup update server
1) Get a domain and hosting :)
2) Create simple .php file which contains something similar to this:
```
<?
echo("Volbx-Update-Info\n");

echo("1.10");

chdir("current/");
foreach (glob("*") as $filename) {
    echo "\n$filename;" . filesize($filename);
}
?>
```
3) Copy .php file to hosting.
4) Create folder `current` in directory where .php file is located.
5) Add new/other version of Volbx and/or updater in `current` directory.
6) Change code of `Networking` namespace in Volbx to point to proper address - look for 
`QNetworkRequest Networking::getCurrentVersionRequest()` 
and 
`QNetworkRequest Networking::getDownloadFileRequest(const QString& file)`
7) Compile `VersionChecker` and run it.
8) Application should connect to given address and download content of `current` folder.

**Remarks**:   
- Checksum should be used instead of size of files in correctness checks.   
- When I've created VersionChecker I've decided to use simplest solution which I was able to create. I do not know if it is safe and 'proper' enough but it worked for me. I'm not web developer ;)   
- Windows needs admin rights (as of ~2012) when executing files having in name strings like "update", "install" or "setup". I've picked name `VersionChecker` to workaround that problem.   
- On Windows OS updater cannot overwrite file which are being used (as of ~2012). There is a special mechanism in place for replacing used/locked files: 
    * close Volbx, 
    * run updater, 
    * change not used/blocked binaries/libs, 
    * replace automatically used/blocked ones during next run of Volbx.

## Testing
1) Compile project.
2) Run target named `tests`.
3) Check output. All tests should have status `PASSED`.    

Tests are done using Qt test framework.

## Potential further improvements
* Upgrade code to use C++20.
* Measure and increase test coverage.
* Setup CI with static analysis (CppCheck, Clang Tidy, Clazy).

## Some screenshots:
![Alt text](Screenshot2.jpg?raw=true "")![Alt text](Screenshot3.jpg?raw=true "")![Alt text](Screenshot4.jpg?raw=true "")
