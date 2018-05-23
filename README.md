Izzy
====

A digital clock for X Windows

![Izzy Screenshot](https://github.com/rogerallen/izzy/raw/master/izzy.png "Izzy Screenshot")

By Roger Allen

This is probably the oldest working code I have.  Looks like I started this in November, 1991 and I found the code & decided to do some cleanup in May, 2018 -- 26 years later.

All I had to do to get this going on Ubuntu was `sudo apt-get install libmotif-dev` and compile it.  That was a surprise.

I removed some crufty and downright unsafe code that allowed for running programs on a schedule.  So, now all this does is display an nice digital display.  But that's what I wanted it to do.

Usage
-----

`./izzy -fg red -bg black &`

License
-------

Copyright (c) 1991-2018 Roger Allen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
