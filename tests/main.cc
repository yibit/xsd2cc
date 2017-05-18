
/*
Copyright (C) 2013-2014  GuiQuan Zhang

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

#include <iostream>
#include <string>
#include "resource_test.h"

int main(int agrc, char* argv[]) {
  const std::string test1 =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
  <resource-lists>\
     <list>\
       <entry uri=\"sip:bill@example.com\" copyControl=\"to\" />\
       <entry uri=\"sip:randy@example.net\" copyControl=\"to\" anonymize=\"true\"/>\
       <entry uri=\"sip:eddy@example.com\" copyControl=\"to\" anonymize=\"true\"/>\
       <entry uri=\"sip:joe@example.org\" copyControl=\"cc\" />\
       <entry uri=\"sip:carol@example.net\" copyControl=\"cc\" anonymize=\"true\"/>\
       <entry uri=\"sip:ted@example.net\" copyControl=\"bcc\" />\
       <entry uri=\"sip:andy@example.com\" copyControl=\"bcc\" />\
     </list>\
   </resource-lists>";

  resource::Resource_lists resource;
  if (!resource.FromXml(test1)) {
    return -1;
  }

  std::string ns_prefix = "cp";
  std::string ns_decl =
      "xmlns=\"http://uri.etsi.org/ngn/params/xml/simservs/xcap\" "
      "xmlns:cp=\"urn:ietf:params:xml:ns:common-policy\"";

  resource.SetNsPrefix(ns_prefix);
  resource.SetNsDeclaration(ns_decl);
  std::cout << resource.ToString(true, "resource-lists") << std::endl;
  if (resource.IsSet() && resource.ListIsSet()) {
    resource::ListType list = resource.GetList();
    if (list.IsSet() && list.EntryIsSet()) {
      std::vector<resource::EntryType> ev = list.GetEntry();
      for (int i = 0, n = ev.size(); n > 0 && i < n; ++i) {
        if (ev[i].IsSet()) {
          std::cout << "uri: " << ev[i].GetUri() << std::endl;
          std::cout << "copyControl: " << ev[i].GetCopyControl() << std::endl;
        }
      }
    }
  }

  const std::string test2 =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
  <resource-lists>\
     <list>\
     </list>\
   </resource-lists>";

  resource::Resource_lists resource2;
  if (!resource2.FromXml(test2)) {
    return -1;
  }

  std::cout << resource2.ToString(true, "resource-lists") << std::endl;

  resource2.SetNsPrefix(ns_prefix);
  resource2.SetNsDeclaration(ns_decl);
  std::cout << resource2.ToString(true, "resource-lists") << std::endl;

  if (!resource2.GetList().IsSet()) {
    std::cout << "resource2.GetList().IsSet() is false" << std::endl;
  }

  if (resource2.GetList().IsSetted()) {
    std::cout << "resource2.GetList().IsSetted() is true" << std::endl;
  }

  return 0;
}
