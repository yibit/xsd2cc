
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


#include <xsd2cc/xsd.h>
#include <xsd2cc/xml_node.h>
#include <xsd2cc/split.h>
#include <xsd2cc/type_base.h>
#include <iostream>


namespace xsd2cc {

namespace {

#define nelems(a) ((int)(sizeof(a)/sizeof((a)[0])))

struct TypeItem {
  std::string  name_;
  std::string  type_;
  BuiltType    btype_;
};

static TypeItem kTypeMap[] = {
  {"string",             "std::string"   , kSTRING},
  {"date",               "std::string"   , kSTRING},
  {"dateTime",           "std::string"   , kSTRING},
  {"anySimpleType",      "std::string"   , kSTRING},
  {"anyURI",             "std::string"   , kSTRING},
  {"any",                "std::string"   , kSTRING},
  {"time",               "std::string"   , kSTRING},
  {"boolean",            "bool"          , kBOOLEAN},
  {"byte",               "char"          , kNUMBER},
  {"decimal",            "double"        , kNUMBER},
  {"int",                "int"           , kNUMBER},
  {"integer",            "int"           , kNUMBER},
  {"long",               "long"          , kNUMBER},
  {"short",              "short"         , kNUMBER},
  {"unsignedByte",       "unsigned char" , kNUMBER},
  {"unsignedInt",        "unsigned int"  , kNUMBER},
  {"unsignedShort",      "unsigned short", kNUMBER},
  {"unsignedLong",       "unsigned long" , kNUMBER},
  {"negativeInteger",    "int"           , kNUMBER},
  {"nonNegativeInteger", "unsigned int"  , kNUMBER},
  {"nonPositiveInteger", "int"           , kNUMBER},
  {"positiveInteger",    "unsigned int"  , kNUMBER},
};

} // namespace 

TypeBase::TypeBase(const Xsd& xsd, 
                   const XmlNode& node, 
                   const std::string& ns_prefix,
                   const std::string& type)
    : xsd_(xsd),
      node_(node),
      ns_href_(ns_prefix),
      type_(type),
      t_type_(kUNKNOWN)
{

  if (!type_.empty()) {
    SetType(type_);
  }

}


const std::string TypeBase::Documentation(const XmlNode& node) const
{
  XmlNode n(node, "annotation");
  if (n) {
    XmlNode n2(n, "documentation");
    if (n2) {
      return n2.GetContent();
    }
  }

  return "";
}


const std::string TypeBase::CType() const
{
  return c_type_;
}


void TypeBase::SetCType()
{
  std::string t;
  if (IsBuiltin()) {

    t = Split(type_).CName;
    for (int i = 0, n = nelems(kTypeMap); n > 0 && i < n; ++i) {
      if (kTypeMap[i].name_ == t) {
        t = kTypeMap[i].type_;
        t_type_ = kTypeMap[i].btype_;
        }
     }

  } else {
    std::string parent = xsd_.GetParent(ns_href_, type_);
    std::string tmp;
    while (!parent.empty()) {
      tmp = Split(parent).CName1up + "::" + tmp;
      parent = xsd_.GetParent(ns_href_, parent);
    }

    if (xsd_.GetNamespace(ns_href_).empty()) {
      t = tmp + Split(type_).CName1up;
    } else {
      t = xsd_.GetNamespace(ns_href_) + "::" + tmp + Split(type_).CName1up;
    }

   t_type_ = kCOMPLEX;
  }

  c_type_ = t;
}


void TypeBase::SetType(const std::string& x)
{
  std::string href = node_.GetNsMap()[Split(x).NS];
  if (Split(x).NS.empty()) {
    href = xsd_.GetTargetNamespace();
    XmlNode n(node_);
    std::string alt = n.FindProperty("targetNamespace", true);
    if (!alt.empty() && alt != href) {
      href = alt;
    }
  }

  if (href.empty()) {
    std::map<std::string, std::string> mmap = node_.GetNsMap();
    std::cout << "Empty href, type = \"" + x + "\"" << std::endl;
    return ;
  }

  ns_href_ = href;
  type_ = Split(x).Type;
  SetCType();
}


bool TypeBase::IsBuiltin() const
{
  return ns_href_ == "http://www.w3.org/2001/XMLSchema";
}

} // namespace xsd2cc

