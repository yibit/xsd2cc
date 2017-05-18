
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

#include <xsd2cc/attribute.h>
#include <xsd2cc/simple_type.h>
#include <xsd2cc/split.h>
#include <xsd2cc/type_base.h>
#include <xsd2cc/xml_node.h>
#include <xsd2cc/xsd.h>

namespace xsd2cc {

Attribute::Attribute(const Xsd& xsd, const XmlNode& node,
                     const std::string& name, const std::string& ns_prefix)
    : TypeBase(xsd, node, ns_prefix, Split(name).Type),
      name_(!name.empty()
                ? name
                : node.PropertyExists("name") ? node.GetProperty("name") : ""),
      b_default_is_set_(false),
      b_fixed_is_set_(false) {
  std::string type;
  if (node.PropertyExists("type")) {
    type = node.GetProperty("type");
  } else {
    XmlNode n(node, "simpleType");
    if (n) {
      type = ns_prefix + "::" + Split(name_).Type;
    }
  }

  if (!type.empty()) {
    SetType(type);
  }

  if (node.PropertyExists("use")) {
    use_ = node.GetProperty("use");
  }

  if (node.PropertyExists("default")) {
    default_ = node.GetProperty("default");
    b_default_is_set_ = true;
  }

  if (node.PropertyExists("fixed")) {
    fixed_ = node.GetProperty("fixed");
    b_fixed_is_set_ = true;
  }

  std::string t = TypeBase::CType();
  if (IsSimpleType()) {
    XmlNode n = xsd_.FindSimpleType(ns_href_, type_);
    SimpleType sim(xsd_, n, Split(type_).Type, Split(type_).NS);
    t = sim.CType();
  }

  base_type_ = t;
}

const std::string Attribute::Documentation() const {
  return TypeBase::Documentation(node_);
}

const std::string Attribute::CType() const { return TypeBase::CType(); }

const std::string Attribute::CName() const { return Split(name_).CName; }

const std::string Attribute::CName1up() const { return Split(name_).CName1up; }

bool Attribute::IsOptional() const { return use_ != "required"; }

const std::string Attribute::CType_decl() const {
  std::string t = TypeBase::CType();
  if (IsSimpleType()) {
    XmlNode n = xsd_.FindSimpleType(ns_href_, type_);
    SimpleType sim(xsd_, n, Split(type_).Type, Split(type_).NS);
    t = sim.CType();
  }

  return t;
}

const std::string Attribute::CType_in() const {
  std::string t = TypeBase::CType();
  if (IsSimpleType()) {
    XmlNode n = xsd_.FindSimpleType(ns_href_, type_);
    SimpleType sim(xsd_, n, Split(type_).Type, Split(type_).NS);
    t = sim.CType();
  }

  if (IsBuiltin() || IsSimpleType()) {
    if (t == "std::string") {
      t = "const " + t + "&";
    }
  } else {
    t = "const " + t + "&";
  }

  return t;
}

const std::string Attribute::CType_out() const {
  std::string t = TypeBase::CType();
  if (IsSimpleType()) {
    XmlNode n = xsd_.FindSimpleType(ns_href_, type_);
    SimpleType sim(xsd_, n, Split(type_).Type, Split(type_).NS);
    t = sim.CType();
  }

  if (IsBuiltin() || IsSimpleType()) {
    if (t == "std::string") {
      t = "const " + t + "&";
    }
  } else {
    t = t + "&";
  }

  return t;
}

const std::string Attribute::DefaultValue() const {
  switch (TypeClass()) {
    case kNUMBER:
      return "0";

    case kBOOLEAN:
      return "false";

    default:
      return "";
  }
}

bool Attribute::IsSimpleType() const {
  if (IsBuiltin()) {
    return false;
  }

  XmlNode n = xsd_.FindSimpleType(ns_href_, type_);
  if (!n.empty()) {
    return true;
  }

  return false;
}

}  // namespace xsd2cc
