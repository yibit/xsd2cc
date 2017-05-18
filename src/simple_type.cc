
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

#include <xsd2cc/simple_type.h>
#include <xsd2cc/split.h>
#include <xsd2cc/type_base.h>
#include <xsd2cc/xml_node.h>
#include <xsd2cc/xsd.h>

namespace xsd2cc {

SimpleType::SimpleType(const Xsd& xsd, const XmlNode& node,
                       const std::string& name, const std::string& ns_prefix)
    : TypeBase(xsd, node, ns_prefix, Split(name).Type),
      name_(!name.empty()
                ? name
                : node.PropertyExists("name") ? node.GetProperty("name") : "") {
  XmlNode n(node, "restriction");
  if (n) {
    if (n.PropertyExists("base")) {
      std::string type = n.GetProperty("base");
      SetType(type);
    }

    XmlNode n2(n, "enumeration");
    while (n2) {
      std::string value = n2.GetProperty("value");
      enum_values_.push_back(value);
      ++n2;
    }

    XmlNode n3(n, "pattern");
    while (n3) {
      std::string value = n3.GetProperty("value");
      pattern_values_.push_back(value);
      ++n3;
    }
  }
}

const std::string SimpleType::Documentation() const {
  return TypeBase::Documentation(node_);
}

void SimpleType::CreateInterface(FILE* file, int lvl) const {
  for (int i = 0; i < lvl; i++) {
    fprintf(file, "\t");
  }

  fprintf(file, "typedef %s %s;\n\n", CType().c_str(),
          Split(name_).CName.c_str());

  // check code here
}

const std::string SimpleType::CType() const { return TypeBase::CType(); }

}  // namespace xsd2cc
