[![Build & test](https://github.com/przemek83/volbx/actions/workflows/buld-and-test.yml/badge.svg)](https://github.com/przemek83/volbx/actions/workflows/buld-and-test.yml)
[![CodeQL](https://github.com/przemek83/volbx/actions/workflows/codeql.yml/badge.svg)](https://github.com/przemek83/volbx/actions/workflows/codeql.yml)
[![codecov](https://codecov.io/gh/przemek83/volbx/graph/badge.svg?token=4E63PV9WM1)](https://codecov.io/gh/przemek83/volbx)

[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=przemek83_volbx&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=przemek83_volbx)
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=przemek83_volbx&metric=bugs)](https://sonarcloud.io/summary/new_code?id=przemek83_volbx)
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=przemek83_volbx&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=przemek83_volbx)
[![Coverage](https://sonarcloud.io/api/project_badges/measure?project=przemek83_volbx&metric=coverage)](https://sonarcloud.io/summary/new_code?id=przemek83_volbx)
[![Duplicated Lines (%)](https://sonarcloud.io/api/project_badges/measure?project=przemek83_volbx&metric=duplicated_lines_density)](https://sonarcloud.io/summary/new_code?id=przemek83_volbx)

## Table of content
- [Example view](#example-view)
- [Description](#description)
- [History](#history)
- [Used tools and libs](#used-tools-and-libs)
- [Compilation and installation](#Compilation-and-installation)
- [Licensing](#licensing)
- [Updater](#updater)
- [Setup update server](#setup-update-server)
- [Testing](#testing)
- [Potential further improvements](#potential-further-improvements)
- [Screenshots](#screenshots)

## Example view
![](exampleScreen.gif?raw=true "")

## Description
Volbx is a graphical tool used for data manipulation written in C++/Qt. User can:
 * load data - opens XLSX and ODS spreadsheet files,
 * filter data - filters panel can be used to define data range on each column,
 * select data - user can select rows on main data table, 
 * visualize data - multiple types of built-in diagrams  (histogram, grouping, linear regression, quantiles) which adjusts dynamically according to user actions,
 * order data - each column can be ordered ascending or descending, 
 * export data - selected set of data, together with linked plots, can be exported to XLSX spreadsheet file or CSV. 

## History
The tool was originally created for real estate appraisal as it gives the possibility, for people working in that area, to estimate past, current and future value of analyzed properties. When project/start-up was dropped, I've decided to release a less specialized application as an open source project. Most advanced and dedicated for real estates functionalities were cut out, basic and intermediate ones stayed.

## Used tools and libs
| Tool |  Windows | Lubuntu |
| --- | --- | --- |
| OS version | 10 22H2 | 22.04 |
| GCC | 11.2.0 | 11.3.0 |
| CMake | 3.25.0 | 3.25.0 |
| Git | 2.38.1 | 2.34.1 |
| Qt | 6.5.2 | 6.5.2 |
| Qt Creator | 10.0.2 | 10.0.2 |
| Qwt | 6.2 | 6.2 |
| QuaZip | 1.4 | 1.4 |
| Zlib | 1.3.1 | 1.3.1 |
| Eible | 1.2.0 | 1.2.0 |
| QwtBle | 1.1.0 | 1.1.0 |
| Wble | 1.2.0 | 1.2.0 |

## Compilation and installation
Use directly CMake or Qt Creator. CMake **should**:
- configure everything automatically, 
- download dependencies using Git (QuaZip, Zlib, Eible, Wble, Qwtble, Qwt), 
- build dependencies, 
- compile Volbx and create binaries.  

This is of course a happy path...  

**TIPS**: set `CMAKE_PREFIX_PATH` env variable (should point to Qt) and add `qmake` location to `PATH` variable (QWT building expects it).   

## Licensing
Volbx is published using LGPLv3 license. 

The project uses the following software and licenses:    
* Qt, Qwt, QuaZip, Eible, Wble and Qwtble libraries - LGPL licenses (more on qt-project.org, quazip.sourceforge.net, qwt.sourceforge.net and github.com/przemek83),    
* Zlib - Zlib license (can be found on zlib.net).

## Updater
1) Build target `VersionCheck`.
2) Run `VersionCheck`.
3) Following window should be shown:    

![Alt text](updateScreen.gif?raw=true "")

## Setup update server
1) Get a domain and hosting :)
2) Create a simple .php file which contains something similar to this:
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
6) Change code of `Networking` namespace in Volbx to point to proper address. Look for 
`QNetworkRequest Networking::getCurrentVersionRequest()` 
and 
`QNetworkRequest Networking::getDownloadFileRequest(const QString& file)`
7) Compile `VersionChecker` and run it.
8) Application should connect to given address and download content of `current` folder.

**Remarks**:   
- Updater is doing correctness checks using file sizes. Checksums should be used instead.   
- When I've created VersionChecker I've decided to use the simplest known by me solution. I do not know if it is safe and 'proper' enough, but it worked for me. I'm not web developer ;)   
- Windows needs admin rights (as of ~2012) to execute files having in name strings like "update", "install" or "setup". I've picked name `VersionChecker` to work around that problem.   
- On Windows OS, updater cannot overwrite file which are being used (as of ~2012). There is a special mechanism in place for replacing used/locked files: 
    * close Volbx, 
    * run updater, 
    * download files,
    * change not used binaries/libs, 
    * on next run of Volbx replace automatically used/blocked ones.

## Testing
1) Compile project.
2) Run target named `tests`.
3) Check output. All tests should have status `PASSED`.    

Tests are done using Qt test framework.

## Potential further improvements
* Upgrade code to use C++20.
* Measure and increase test coverage.
* Setup static analysis (CppCheck, Clang Tidy, Clazy).

## Screenshots
![Alt text](groupingAndHistogram.gif?raw=true "")

![Alt text](analysing.gif?raw=true "")

![Alt text](exportData.gif?raw=true "")

![Alt text](regression.gif?raw=true "")

![Alt text](groupingPlot.gif?raw=true "")

![Alt text](importWindow.gif?raw=true "")
