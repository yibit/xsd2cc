Name
======

xsd2cc - a Xml Schema code generator, which converting a .xsd file to C++ classes.


Dependencies
==============

__libxml2__ is required. Also see [http://xmlsoft.org](http://xmlsoft.org).


Build and Install
===================

    git clone https://github.com/xsd2cc/xsd2cc.git
    cd xsd2cc
    sudo make install

__Note: outputs are xsd2cc and libxsd2cc.a__


Test and Usage
================

    cd tests
    make
    ./resource_test


Authors
=========

GuiQuan "guiquanz" Zhang (张桂权) <guiqzhang@gmail.com>


Copyright and License
=======================

Part of this code is from [wsdl2cpp](http://www.alhem.net/project/wsdl2cpp/)
but heavily refactored.

Copyright (C) 2013-2014  GuiQuan Zhang

Copyright (C) 2008-2009  Anders Hedstrom

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

