**fprmck**
==========

File permission check

======================================================================

fprmck - File Permission Checker v. 1.0
by Tymon Zagawa
Thu, Aug 6, 2021 

======================================================================

By this program you can check if permissions had changes during
runtime of this program.

======================================================================

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

======================================================================

I know that code might be dirty, but it works. In future versions
some problems will be eliminated.

======================================================================







Introduction
============

fpmrck is a opem-source tool for permissions monitoring.


fprmck is created  by Tymon Zagawa (LinuxTechnician on Github).
If you want to contact with me just send me an email on address:
tymon.zagawa@gmail.com



License
=======

fprmck is distributed under the General Public License.



Instalation
==============

Supported Platforms
-------------------

* Linux (tested on CentOS7)

In future version AIX will be supported.


Setup
--------

You need to have a C compiler (gcc)

If you simply want to build and install CMake from source, just use compiler:

  $ gcc fprmck.c -o [DESTINATION]

For example (if you want to use fprmck as a system-wide tool):


  $ gcc fprmck.c -o /bin/fprmck


Reporting Bugs
==============

If you will find a bug just send email on address:
tymon.zagawa@gmail.com


Usage
=====

If you want to use fprmck you must use option -f and provide path to monitored 
file. For example:

  $ ./fprmck -f /mnt/public_files/text.txt

There is option of enabling logging with option -l and providing path to log file:

  $ ./fprmck -f /mnt/public_files/text.txt -l /mnt/public_files/log.txt
  $ ./fprmck -f text.txt -l log.txt


You can define time interval between permission checks. You can define it in 
microseconds (-m option) or just in seconds (-s options):

  $ ./fprmck -f /mnt/public_files/text.file -s 360

