
/*
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
*/


#include <xsd2cc/xml_document.h>
#include <xsd2cc/xsd.h>
#include <xsd2cc/application.h>
#include <xsd2cc/complex_type.h>
#include <xsd2cc/utility.h>
#include <iostream>


using namespace xsd2cc;

int 
main(int argc, char *argv[])
{
  Application app(argc, argv);

  XmlDocument doc;
  bool ret = doc.Load(app.Filename());
  if (!ret) {
    std::cout << "xml.Load(" + app.Filename() + ") failed!" << std::endl;
    return -1;
  }

  Xsd xsd(app, doc);

  std::cout << "---------------------------------------------" << std::endl;
  std::cout << "@@@@ generate types" << std::endl;
  if (!xsd.CreateInterface() || !xsd.CreateImplementation())
    return -1;
  std::cout << "---------------------------------------------" << std::endl;
  std::cout << "@@@@ view_xsd" << std::endl;

  {
    std::string target_ns = xsd.GetTargetNamespace();
    std::string prefix_ns = xsd.GetTargetNamespacePrefix();
    std::cout << "Target Namespace: " << target_ns << std::endl;
    std::cout << "Target Ns Prefix: " << prefix_ns << std::endl;
  }

  std::cout << "---------------------------------------------" << std::endl;
  std::cout << "@@@@ Types::generate()" << std::endl;

  ComplexType::ShowGenerated();

  return 0;
}

