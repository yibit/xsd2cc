
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

#include <xsd2cc/application.h>
#include <xsd2cc/element.h>
#include <xsd2cc/simple_type.h>
#include <xsd2cc/split.h>
#include <xsd2cc/type_base.h>
#include <xsd2cc/xml_node.h>
#include <xsd2cc/xsd.h>
#include <iostream>

namespace xsd2cc {

Element::Element(const Xsd& xsd, const XmlNode& node, bool in_sequence,
                 bool in_choice, const std::string& name,
                 const std::string& ns_prefix)
    : TypeBase(xsd, node, ns_prefix, ""),
      app_(xsd.GetApplication()),
      name_(!name.empty()
                ? name
                : node.PropertyExists("name") ? node.GetProperty("name") : ""),
      minOccurs_(node.PropertyExists("minOccurs")
                     ? atoi(node.GetProperty("minOccurs").c_str())
                     : 1),
      maxOccurs_(1),
      has_default_(false) {
  if (node.PropertyExists("type")) {
    std::string type = node.GetProperty("type");
    SetType(type);

  } else if (node.PropertyExists("ref")) {
    name_ = node.GetProperty("ref");
    XmlNode n = xsd.GetElement(name_);

    if (n && n.PropertyExists("type")) {
      std::string type = n.GetProperty("type");
      SetType(type);
    }

  } else {
    XmlNode n(node, "complexType");
    if (n) {
      std::string prefix = node.GetNsMapRe()[ns_prefix];
      std::string type = prefix + ":" + name_;
      SetType(type);
    }
  }

  if (node.PropertyExists("maxOccurs")) {
    std::string max = node.GetProperty("maxOccurs");
    maxOccurs_ = ((max == "unbounded") ? -1 : atoi(max.c_str()));
  }

  if (node.PropertyExists("default")) {
    has_default_ = true;
    default_ = node.GetProperty("default");
  }

  std::string t = TypeBase::CType();
  if (IsSimpleType()) {
    XmlNode n = xsd_.FindSimpleType(ns_href_, type_);
    if (!n.empty()) {
      SimpleType sim(xsd_, n, Split(type_).Type, Split(type_).NS);
      t = sim.CType();
    }
  }

  base_type_ = t;
}

const std::string Element::Documentation() const {
  XmlNode n(node_, "annotation");

  if (n) {
    XmlNode n2(n, "documentation");

    if (n2) {
      return n2.GetContent();
    }
  }

  return "";
}

void Element::CreateInterface(FILE* file) const {}

const std::string Element::CType_decl() const {
  std::string t = TypeBase::CType();
  if (IsSimpleType()) {
    XmlNode n = xsd_.FindSimpleType(ns_href_, type_);
    SimpleType sim(xsd_, n, Split(type_).Type, Split(type_).NS);
    t = sim.CType();
  }

  if (IsVector()) {
    t = app_.ListType() + "<" + t + ">";
  }

  return t;
}

const std::string Element::CType_in() const {
  std::string t = TypeBase::CType();
  if (IsSimpleType()) {
    XmlNode n = xsd_.FindSimpleType(ns_href_, type_);
    SimpleType sim(xsd_, n, Split(type_).Type, Split(type_).NS);
    t = sim.CType();
  }

  if (IsBuiltin() || IsSimpleType()) {
    if (t == "std::string" && !IsVector()) {
      t = "const " + t + "&";
    }

  } else if (!IsVector()) {
    t = "const " + t + "&";
  }

  if (IsVector()) {
    t = "const " + app_.ListType() + "<" + t + ">&";
  }

  return t;
}

const std::string Element::CType_out() const {
  std::string t = TypeBase::CType();
  if (IsSimpleType()) {
    XmlNode n = xsd_.FindSimpleType(ns_href_, type_);
    SimpleType sim(xsd_, n, Split(type_).Type, Split(type_).NS);
    t = sim.CType();
  }

  if (IsBuiltin() || IsSimpleType()) {
    if (t == "std::string" && !IsVector()) {
      t = "const " + t + "&";
    }
  } else if (!IsVector()) {
    t = t + "&";
  }

  if (IsVector()) {
    t = app_.ListType() + "<" + t + ">&";
  }

  return t;
}

const std::string Element::CName() const { return Split(name_).CName; }

const std::string Element::CName1up() const { return Split(name_).CName1up; }

const std::string Element::DefaultValue() const {
  if (IsBuiltin()) {
    std::string t = Split(type_).CName;
    if (TypeClass() == kNUMBER) {
      return "0";
    }
  }

  return "";
}

bool Element::IsComplexType() const {
  if (IsBuiltin()) {
    return false;
  }

  XmlNode n = xsd_.FindComplexType(ns_href_, type_);
  if (!n.empty()) {
    return true;
  }

  return false;
}

bool Element::IsSimpleType() const {
  if (IsBuiltin()) {
    return false;
  }

  XmlNode n = xsd_.FindSimpleType(ns_href_, type_);
  if (!n.empty()) {
    return true;
  }

  return false;
}

bool Element::HasDefault() const { return has_default_; }

const std::string& Element::Default() const { return default_; }

}  // namespace xsd2cc
