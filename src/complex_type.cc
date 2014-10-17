
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


#include <string.h>
#include <xsd2cc/complex_type.h>
#include <xsd2cc/xml_node.h>
#include <xsd2cc/element.h>
#include <xsd2cc/split.h>
#include <xsd2cc/xsd.h>
#include <xsd2cc/attribute.h>
#include <xsd2cc/simple_type.h>
#include <xsd2cc/application.h>
#include <xsd2cc/utility.h>
#include <iostream>


namespace xsd2cc {

std::map<std::string, bool> ComplexType::b_generated_;

ComplexType::ComplexType(const Xsd& xsd, 
                         const XmlNode& node, 
                         const std::string& name, 
                         const std::string& ns_prefix)
    : TypeBase(xsd, node, ns_prefix, Split(name).Type), 
      name_(!name.empty() ? name : node.PropertyExists("name") ? node.GetProperty("name") : ""),
      b_is_sequence_(false),
      b_is_all_(false),
      b_complexContent_(false),
      b_is_array_(false),
      b_is_mixed_(false)
{
  XmlNode start = Start();
  if (node_.PropertyExists("mixed") && node_.GetProperty("mixed") == "true") {
    b_is_mixed_ = true;
  }

  if (start.PropertyExists("base")) {
    extends_ = start.GetProperty("base");
    std::string ns_href = node_.GetNsMap()[Split(extends_).NS];
    if (ns_href.empty()) {
      extends_cls_ = Split(extends_).CName1up;
    } else {
      extends_cls_ = xsd_.NamespaceFromHref(ns_href) + "::" + Split(extends_).CName1up;
    }
  }

  {
    XmlNode n(start, "sequence");
    if (n) {
      b_is_sequence_ = true;
    }
  }

  {
    XmlNode n(start, "all");
    if (n) {
      b_is_all_ = true;
    }
  }

  XmlNode n(node_, "complexContent");
  if (n) {
    b_complexContent_ = true;
    XmlNode n2(n, "restriction");
    if (n2 && n2.PropertyExists("base") && Split(n2.GetProperty("base")).Type == "Array") {
      XmlNode n3(n2, "attribute");
      if (n3 && n3.PropertyExists("arrayType")) {
        std::string tmp = n3.GetProperty("arrayType");
        Parse pa(tmp, "[]");
        tmp = pa.getword();
        SetType(tmp);
        b_is_array_ = true;
      }
    }
  }

}


int ComplexType::NumberOfMembers() const
{
  XmlNode n(Start(), b_is_sequence_ ? "sequence" : "all");
  int num = 0;
  if (n) {
    XmlNode n2(n, "element");
    while (n2) {
      num++;
      ++n2;
    }
  }

  return num;
}


bool ComplexType::GetRef(std::string& ref) const
{
  XmlNode n(Start(), b_is_sequence_ ? "sequence" : "all");
  if (n) {
    XmlNode n2(n, "element");
    while (n2) {
      if (n2.PropertyExists("ref")) {
        ref = n2.GetProperty("ref");
        return true;
      }
      ++n2;
    }
  }

  return false;
}


std::list<std::pair<std::string, XmlNode> > ComplexType::GetMembers() const
{
  std::list<std::pair<std::string, XmlNode> > vec;
  XmlNode n1(Start(), b_is_sequence_ ? "sequence" : "all");
  if (n1) {
    XmlNode n(n1, "element");
    while (n) {
        std::string name;
      if (n.PropertyExists("ref")) {
        name = n.GetProperty("ref");
      } else {
        name = n.GetProperty("name");
      }
      vec.push_back(std::pair<std::string, XmlNode>(name, n));
      ++n;
    }
  }

  return vec;
}


std::list<std::pair<std::string, XmlNode> > ComplexType::GetAttributes() const
{
  std::list<std::pair<std::string, XmlNode> > vec;
  XmlNode n(Start(), "attribute");
  while (n) {
    std::string name = n.GetProperty("name");
    vec.push_back(std::pair<std::string, XmlNode>(name, n));
    ++n;
  }

  return vec;
}


const std::string ComplexType::Documentation() const
{
  return TypeBase::Documentation(node_);
}


const std::string ComplexType::CName() const
{
  return Split(name_).CName1up;
}


bool ComplexType::OkToGenerate() const
{
  if (b_complexContent_ && b_is_array_) {
    XmlNode n = xsd_.FindComplexType( ns_href_, Type() );
    ComplexType t(xsd_, n, Split(Type()).Type, Split(Type()).NS);
    if (!t.IsGenerated()) {
      return false;
    }
    return true;
  }

  std::list<std::pair<std::string, XmlNode> > members = GetMembers();
  std::list<std::pair<std::string, XmlNode> > attrs = GetAttributes();
  for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = members.begin(); 
       it != members.end(); it++) {
    const std::pair<std::string, XmlNode>& ref = *it;
    Element el(xsd_, ref.second, b_is_sequence_, false, "", NsHref());

    if (el.IsComplexType()) {
      XmlNode n = xsd_.FindComplexType(el.NsHref(), el.Type());
      ComplexType t(xsd_, n, Split(el.Type()).Type, Split(el.Type()).NS);
      std::string parent = xsd_.GetParent(NsHref(), t.Name());
      if (!t.IsGenerated() && parent != Name()) {
        return false;
      }
    }
  }

  return true;
}


void ComplexType::SetGenerated(bool x)
{
  b_generated_[name_] = x; 
}


bool ComplexType::IsGenerated() const
{
  std::map<std::string, bool>::const_iterator it = b_generated_.find(name_);
  if (it == b_generated_.end()) {
    return false;
  }

  return true;
}


void ComplexType::CreateInterface(FILE *file, int lvl) const
{
  std::string prefix;
  for (int i = 0; i < lvl; i++)
    prefix += "  ";
  if (b_complexContent_ && b_is_array_) {
    fprintf(file, "%stypedef %s<%s> %s;\n", prefix.c_str(), TypeBase::CType().c_str(), 
            xsd_.GetApplication().ListType().c_str(), CName().c_str());
  }

  std::list<std::pair<std::string, XmlNode> > members = GetMembers();
  std::list<std::pair<std::string, XmlNode> > attrs = GetAttributes();

  // parent
  std::string parent = xsd_.GetParent(NsHref(), Name());

  // class
  if (extends_.empty()) {
    fprintf(file, "%sclass %s {\n", prefix.c_str(), CName().c_str());
  } else {
    fprintf(file, "%sclass %s : public %s {\n", prefix.c_str(), CName().c_str(), extends_cls_.c_str());
  }

  // internal types
  const std::list<std::string>& ref = xsd_.ComplexTypes(NsHref());
  for (std::list<std::string>::const_iterator it = ref.begin(); it != ref.end(); it++){
    std::string parent = xsd_.GetParent(NsHref(), *it);
    if (parent == Name()) {
      fprintf(file, "%spublic:\n", prefix.c_str());
      XmlNode n = xsd_.FindComplexType( ns_href_, *it );
      ComplexType t(xsd_, n, *it, NsHref());
      t.CreateInterface(file, lvl + 1);
    }
  }

  const std::list<std::string>& ref2 = xsd_.SimpleTypes(NsHref());
  for (std::list<std::string>::const_iterator it = ref2.begin(); it != ref2.end(); it++) {
     if ((*it).empty()) continue;
    std::string parent = xsd_.GetParent(NsHref(), *it);
    if (parent == Name()) {
      fprintf(file, "%spublic:\n", prefix.c_str());
      XmlNode n = xsd_.FindSimpleType( ns_href_, *it );
      SimpleType t(xsd_, n, *it, NsHref());
      t.CreateInterface(file, lvl + 1);
    }
  }

  // begin methods
  fprintf(file, "%spublic:\n", prefix.c_str());
  // default constructor
  fprintf(file, "%s  %s();\n", prefix.c_str(), CName().c_str());
  // destructor
  fprintf(file, "%s  virtual ~%s() {}\n", prefix.c_str(), CName().c_str());
  fprintf(file, "\n");
  // clear
  fprintf(file, "%s  void clear();\n", prefix.c_str());
  fprintf(file, "\n");
  // From Xml
  fprintf(file, "%s  bool FromXml(const XmlNode& it);\n", prefix.c_str());
  fprintf(file, "\n");
  fprintf(file, "%s  bool FromXml(const std::string& it);\n", prefix.c_str());
  fprintf(file, "\n");
  fprintf(file, "%s  bool FromXml(const char* it);\n", prefix.c_str());
  fprintf(file, "\n");
  // ToString
  fprintf(file, "%s  const std::string ToString(bool needXmlHead=false, const std::string& name=\"\", int partid=3) const;\n", prefix.c_str());
  fprintf(file, "\n");

  // IsSet
  {
    fprintf(file, "%s  bool IsSet() const;\n", prefix.c_str());
    fprintf(file, "\n");
  }

  fprintf(file, "%s  void SetNsPrefix(std::string& prefix);\n", prefix.c_str());
  fprintf(file, "%s  void SetNsDeclaration(std::string& decl);\n", prefix.c_str());

  // SetNodeState
  {
    fprintf(file, "%s  void SetNodeState(bool state);\n", prefix.c_str());
    fprintf(file, "\n");
  }

  // get/set methods
  for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = members.begin(); 
       it != members.end(); it++) {
    const std::pair<std::string, XmlNode>& ref = *it;
    Element el(xsd_, ref.second, b_is_sequence_, false, "", NsHref());
    if (el.IsVector() || el.IsComplexType()) {
      fprintf(file, "%s  const %s Get%s() const;\n", prefix.c_str(), el.CType_out().c_str(), 
              el.CName1up().c_str());
      fprintf(file, "%s  %s Get%s();\n", prefix.c_str(), el.CType_out().c_str(), el.CName1up().c_str());
      fprintf(file, "%s  void Set%s(%s %s);\n",prefix.c_str(), el.CName1up().c_str(),
              el.CType_in().c_str(), el.CName().c_str());
    } else {
      fprintf(file, "%s  %s Get%s() const;\n", prefix.c_str(), el.CType_out().c_str(), el.CName1up().c_str());
      fprintf(file, "%s  void Set%s(%s %s);\n", prefix.c_str(), el.CName1up().c_str(),
        el.CType_in().c_str(), el.CName().c_str());
    }
    fprintf(file, "\n");
  }

  for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = attrs.begin();
       it != attrs.end(); it++) {
    const std::pair<std::string, XmlNode>& ref = *it;
    Attribute attr(xsd_, ref.second, ref.first, NsHref());

    fprintf(file, "%s  %s Get%s() const;\n", prefix.c_str(), attr.CType_out().c_str(), 
            attr.CName1up().c_str());
    //if (!attr.FixedIsSet())
    {
      fprintf(file, "%s  void Set%s(%s %s);\n", prefix.c_str(), attr.CName1up().c_str(),
        attr.CType_in().c_str(), attr.CName().c_str());
    }
  }

  if (!attrs.empty()) {
    fprintf(file, "\n");
  }

  if (b_is_mixed_) {
    fprintf(file, "%s  const std::string& Get() const;\n", prefix.c_str());
    fprintf(file, "%s  void Set(const std::string&);\n", prefix.c_str());
    fprintf(file, "\n");
  }

  // is_set_ methods
  for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = members.begin(); 
       it != members.end(); it++) {
    const std::pair<std::string, XmlNode>& ref = *it;
    Element el(xsd_, ref.second, b_is_sequence_, false, "", NsHref());
    if (el.IsVector() || el.IsComplexType()) {
      fprintf(file, "%s  bool %sIsSet() const;\n", prefix.c_str(), el.CName1up().c_str());
    } else {
      fprintf(file, "%s  bool %sIsSet() const { return elem_%s_is_set_; }\n", 
              prefix.c_str(), el.CName1up().c_str(), el.CName().c_str());
    }
  }

  if (!members.empty()) {
    fprintf(file, "\n");
   }

  for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = attrs.begin(); 
       it != attrs.end(); it++) {
    const std::pair<std::string, XmlNode>& ref = *it;
    Attribute attr(xsd_, ref.second, ref.first, NsHref());

    fprintf(file, "%s  bool %sIsSet() const { return attr_%s_is_set_; }\n", 
            prefix.c_str(), attr.CName1up().c_str(), attr.CName().c_str());
  }

  if (!attrs.empty()) {
    fprintf(file, "\n");
  }

  fprintf(file, "%s  const std::string& GetNsPrefix() const { return ns_prefix_; }\n", prefix.c_str());
  fprintf(file, "%s  const std::string& GetNsDeclaration() const { return ns_declaration_; }\n", prefix.c_str());
  fprintf(file, "%s  bool IsSetted() const { return is_setted_; }\n\n", prefix.c_str());

  // private
  fprintf(file, "%sprivate:\n", prefix.c_str());
  // elements
  for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = members.begin();
       it != members.end(); it++) {
    const std::pair<std::string, XmlNode>& ref = *it;
    Element el(xsd_, ref.second, b_is_sequence_, false, "", NsHref());
    std::string doc = el.Documentation();
    if (!doc.empty()) {
      fprintf(file, "%s  /** %s */\n", prefix.c_str(), doc.c_str());
    }
    fprintf(file, "%s  %s elem_%s_;\n", prefix.c_str(), el.CType_decl().c_str(), el.CName().c_str());
  }

  for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = attrs.begin();
       it != attrs.end(); it++) {
    const std::pair<std::string, XmlNode>& ref = *it;
    Attribute attr(xsd_, ref.second, ref.first, NsHref());

    fprintf(file, "%s  %s attr_%s_;\n", prefix.c_str(), attr.CType_decl().c_str(),
            attr.CName().c_str());
  }

  if (b_is_mixed_) {
    fprintf(file, "%s  std::string mixed_content_;\n", prefix.c_str());
  }

  // is_set_ members
  for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = members.begin();
       it != members.end(); it++) {
    const std::pair<std::string, XmlNode>& ref = *it;
    Element el(xsd_, ref.second, b_is_sequence_, false, "", NsHref());
    if (!el.IsVector() && !el.IsComplexType()) {
      fprintf(file, "%s  bool elem_%s_is_set_;\n", prefix.c_str(), el.CName().c_str());
    }
  }

  for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = attrs.begin();
       it != attrs.end(); it++) {
    const std::pair<std::string, XmlNode>& ref = *it;
    Attribute attr(xsd_, ref.second, ref.first, NsHref());

    fprintf(file, "%s  bool attr_%s_is_set_;\n", prefix.c_str(), attr.CName().c_str());
  }

  fprintf(file, "%s  std::string ns_prefix_;\n", prefix.c_str());
  fprintf(file, "%s  std::string ns_declaration_;\n", prefix.c_str());
  fprintf(file, "%s  bool is_setted_;\n", prefix.c_str());

  fprintf(file, "%s};\n", prefix.c_str());
  fprintf(file, "\n");
}


void ComplexType::reset_members(std::list<std::pair<std::string, XmlNode> >& members,
                                std::list<std::pair<std::string, XmlNode> >& attrs, FILE *file) const
{
  std::string prefix = ": ";
  if (!extends_.empty()) {
    fprintf(file, "%s%s()\n", prefix.c_str(), extends_cls_.c_str());
    prefix = ", ";
  }

  for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = members.begin();
       it != members.end(); it++) {
    const std::pair<std::string, XmlNode>& ref = *it;

    Element el(xsd_, ref.second, b_is_sequence_, false, "", NsHref());
    std::string def = el.DefaultValue();
    if (el.IsBuiltin() && el.HasDefault()) {
      if (el.TypeClass() == kNUMBER) {
        fprintf(file, "%selem_%s_(%s)\n", prefix.c_str(), el.CName().c_str(),
                el.Default().c_str());
      } else {
        fprintf(file, "%selem_%s_(\"%s\")\n", prefix.c_str(), el.CName().c_str(),
                el.Default().c_str());
      }
      prefix = ", ";
    } else if (el.IsBuiltin() && !def.empty()) {
      fprintf(file, "%selem_%s_(%s)\n", prefix.c_str(), el.CName().c_str(), 
              el.DefaultValue().c_str());
      prefix = ", ";
    }
  }

  for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = members.begin();
       it != members.end(); it++) {
    const std::pair<std::string, XmlNode>& ref = *it;
    Element el(xsd_, ref.second, b_is_sequence_, false, "", NsHref());
    if (el.IsVector() || el.IsComplexType()) {
      // _is_set_ is calculated
    } else if (el.HasDefault()) {
      fprintf(file, "%selem_%s_is_set_(true)\n", prefix.c_str(), el.CName().c_str());
      prefix = ", ";
    } else {
      fprintf(file, "%selem_%s_is_set_(false)\n", prefix.c_str(), el.CName().c_str());
      prefix = ", ";
    }
  }

  for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = attrs.begin();
       it != attrs.end(); it++) {
    const std::pair<std::string, XmlNode>& ref = *it;
    Attribute attr(xsd_, ref.second, "", NsHref());
    if (attr.DefaultIsSet()) {
      if (attr.TypeClass() == kNUMBER || attr.TypeClass() == kBOOLEAN) {
        fprintf(file, "%sattr_%s_(%s)\n", prefix.c_str(), attr.CName().c_str(), 
                attr.Default().c_str());
      } else {
        fprintf(file, "%sattr_%s_(\"%s\")\n", prefix.c_str(), attr.CName().c_str(), 
                attr.Default().c_str());
        }
      prefix = ", ";
    } else if (attr.FixedIsSet()) {
      if (attr.TypeClass() == kNUMBER || attr.TypeClass() == kBOOLEAN) {
        fprintf(file, "%sattr_%s_(%s)\n", prefix.c_str(), attr.CName().c_str(), attr.Fixed().c_str());
      } else {
        fprintf(file, "%sattr_%s_(\"%s\")\n", prefix.c_str(), attr.CName().c_str(), 
                attr.Fixed().c_str());
      }
      prefix = ", ";
    } else {
      std::string def = attr.DefaultValue();
      if (attr.IsBuiltin() && !def.empty()) {
        fprintf(file, "%sattr_%s_(%s)\n", prefix.c_str(), attr.CName().c_str(), 
                attr.DefaultValue().c_str());
        prefix = ", ";
      }
    }
  }

  for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = attrs.begin();
       it != attrs.end(); it++) {
    const std::pair<std::string, XmlNode>& ref = *it;
    Attribute attr(xsd_, ref.second, "", NsHref());
    if (attr.DefaultIsSet() || attr.FixedIsSet()) {
      fprintf(file, "%sattr_%s_is_set_(true)\n", prefix.c_str(), attr.CName().c_str());
    } else {
      fprintf(file, "%sattr_%s_is_set_(false)\n", prefix.c_str(), attr.CName().c_str());
    }
    prefix = ", ";
  }

  fprintf(file, "%sns_prefix_(\"\")\n", prefix.c_str());
  prefix = ", ";
  fprintf(file, "%sns_declaration_(\"\")\n", prefix.c_str());
  // is_setted_
  fprintf(file, "%sis_setted_(false)\n", prefix.c_str());
}


void ComplexType::clear_members(std::list<std::pair<std::string, XmlNode> >& members,
                                std::list<std::pair<std::string, XmlNode> >& attrs, FILE *file) const
{
  if (!extends_.empty()) {
    fprintf(file, "  %s::clear();\n", extends_cls_.c_str());
  }

  for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = members.begin();
       it != members.end(); it++) {
    const std::pair<std::string, XmlNode>& ref = *it;
    Element el(xsd_, ref.second, b_is_sequence_, false, "", NsHref());
    std::string def = el.DefaultValue();
    if (el.IsVector()) {
      // %! clear vector
      fprintf(file, "  while (!elem_%s_.empty())\n", el.CName().c_str());
      fprintf(file, "    elem_%s_.erase( elem_%s_.begin() );\n", el.CName().c_str(), 
              el.CName().c_str());
    } else if (el.IsSimpleType()) {
      fprintf(file, "  // simpleType:\n");
      fprintf(file, "  // elem_%s_.clear();\n", el.CName().c_str());
    } else if (el.IsComplexType()) {
      fprintf(file, "  elem_%s_.clear();\n", el.CName().c_str());
    } else if (el.IsBuiltin() && el.HasDefault()) {
      if (el.TypeClass() == kNUMBER)
        fprintf(file, "  elem_%s_ = %s;\n", el.CName().c_str(), el.Default().c_str());
      else
        fprintf(file, "  elem_%s_ = \"%s\";\n", el.CName().c_str(), el.Default().c_str());
    } else if (el.IsBuiltin() && !def.empty()) {
      fprintf(file, "  elem_%s_ = %s;\n", el.CName().c_str(), el.DefaultValue().c_str());
    } else {
      if (el.TypeClass() == kNUMBER)
        fprintf(file, "  elem_%s_ = 0;\n", el.CName().c_str());
      else
      if (el.TypeClass() == kBOOLEAN)
        fprintf(file, "  elem_%s_ = false;\n", el.CName().c_str());
      else
        fprintf(file, "  elem_%s_.clear();\n", el.CName().c_str());
    }
  }

  for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = members.begin(); 
       it != members.end(); it++) {
    const std::pair<std::string, XmlNode>& ref = *it;
    Element el(xsd_, ref.second, b_is_sequence_, false, "", NsHref());
    if (el.IsVector() || el.IsComplexType()) {
      // _is_set_ is calculated
    } else if (el.HasDefault()) {
      fprintf(file, "  elem_%s_is_set_ = true;\n", el.CName().c_str());
    } else {
      fprintf(file, "  elem_%s_is_set_ = false;\n", el.CName().c_str());
    }
  }

  for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = attrs.begin(); 
       it != attrs.end(); it++) {
    const std::pair<std::string, XmlNode>& ref = *it;
    Attribute attr(xsd_, ref.second, "", NsHref());
    if (attr.DefaultIsSet()) {
      if (attr.TypeClass() == kNUMBER || attr.TypeClass() == kBOOLEAN)
        fprintf(file, "  attr_%s_ = %s;\n", attr.CName().c_str(), attr.Default().c_str());
      else
        fprintf(file, "  attr_%s_ = \"%s\";\n", attr.CName().c_str(), attr.Default().c_str());
    } else if (attr.FixedIsSet()) {
      if (attr.TypeClass() == kNUMBER || attr.TypeClass() == kBOOLEAN)
        fprintf(file, "  attr_%s_ = %s;\n", attr.CName().c_str(), attr.Fixed().c_str());
      else
        fprintf(file, "  attr_%s_ = \"%s\";\n", attr.CName().c_str(), attr.Fixed().c_str());
    } else {
      std::string def = attr.DefaultValue();
      if (attr.IsBuiltin() && !def.empty()) {
        fprintf(file, "  attr_%s_ = %s;\n", attr.CName().c_str(), attr.DefaultValue().c_str());
      } else {
        if (attr.TypeClass() == kNUMBER)
          fprintf(file, "  attr_%s_ = 0;\n", attr.CName().c_str());
        else
        if (attr.TypeClass() == kBOOLEAN)
          fprintf(file, "  attr_%s_ = false;\n", attr.CName().c_str());
        else
          fprintf(file, "  attr_%s_.clear();\n", attr.CName().c_str());
      }
    }
  }

  for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = attrs.begin(); 
       it != attrs.end(); it++) {
    const std::pair<std::string, XmlNode>& ref = *it;
    Attribute attr(xsd_, ref.second, "", NsHref());
    if (attr.DefaultIsSet() || attr.FixedIsSet())
      fprintf(file, "  attr_%s_is_set_ = true;\n", attr.CName().c_str());
    else
      fprintf(file, "  attr_%s_is_set_ = false;\n", attr.CName().c_str());
  }

  fprintf(file, "  ns_prefix_.clear();\n");
  fprintf(file, "  ns_declaration_.clear();\n");
  fprintf(file, "  is_setted_ = false;\n");

}


void ComplexType::from_xml(std::list<std::pair<std::string, XmlNode> >& members, 
                           std::list<std::pair<std::string, XmlNode> >& attrs, FILE *file) const
{
  fprintf(file, "  SetNodeState(true);\n");

  if (!extends_.empty()) {
    fprintf(file, "  %s::FromXml(it);\n", extends_cls_.c_str());
  }

  for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = members.begin();
       it != members.end(); it++) {
    const std::pair<std::string, XmlNode>& ref = *it;
    Element el(xsd_, ref.second, b_is_sequence_, false, "", NsHref());
    if (!el.IsOptional()) {
      fprintf(file, "  if (!it.Exists(\"%s\"))\n", el.Name().c_str());
      fprintf(file, "    return false;\n");
    }

    if (el.IsVector()) {
      fprintf(file, "  {\n");
      fprintf(file, "    XmlNode n(it, \"%s\");\n", el.Name().c_str());
      fprintf(file, "    while (n)\n");
      fprintf(file, "    {\n");
      if (el.IsComplexType()) {
        fprintf(file, "      %s tmp;\n", el.CType().c_str());
        fprintf(file, "      if (!tmp.FromXml(n)) return false;\n");
        fprintf(file, "      elem_%s_.push_back( tmp );\n", el.CName().c_str());
      } else {
        if (el.BaseType() == "std::string") {
          fprintf(file, "      elem_%s_.push_back( n.GetContent() );\n", el.CName().c_str());
        } else if (el.BaseType() == "double") {
          fprintf(file, "      elem_%s_.push_back( atof(n.GetContent().c_str()) );\n", el.CName().c_str());
        } else if (el.BaseType() == "bool") {
          fprintf(file, "      elem_%s_.push_back( n.GetContent() == \"true\" || atoi(n.GetContent().c_str()) != 0 || n.GetContent() == \"yes\" );\n", el.CName().c_str());
        } else {
          fprintf(file, "      elem_%s_.push_back( atoi(n.GetContent().c_str()) );\n", el.CName().c_str());
        }
      }

      fprintf(file, "      ++n;\n");
      fprintf(file, "    }\n");
      fprintf(file, "  }\n");
    } else if (el.IsComplexType()) {
      fprintf(file, "  {\n");
      fprintf(file, "    XmlNode n(it, \"%s\");\n", el.Name().c_str());
      fprintf(file, "    if (n) {\n");
      fprintf(file, "      bool ret = elem_%s_.FromXml( n );\n", el.CName().c_str());
      fprintf(file, "      if (!ret) return false;\n");
      fprintf(file, "    }\n");
      fprintf(file, "  }\n");
    } else {

      if (el.BaseType() == "std::string") {
        if (el.IsOptional())
          fprintf(file, "  if (it.Exists(\"%s\"))\n  ", el.Name().c_str());
        fprintf(file, "  Set%s( it[\"%s\"] );\n", el.CName1up().c_str(), el.Name().c_str());
      } else if (el.BaseType() == "double") {
        if (el.IsOptional())
          fprintf(file, "  if (it.Exists(\"%s\"))\n  ", el.Name().c_str());
        fprintf(file, "  Set%s( atof(std::string(it[\"%s\"]).c_str()) );\n", el.CName1up().c_str(), 
                el.Name().c_str());
      } else if (el.BaseType() == "bool") {
        if (el.IsOptional()) {
          fprintf(file, "  if (it.Exists(\"%s\"))\n", el.Name().c_str());
        }
        fprintf(file, "  {\n");
        fprintf(file, "    std::string tmp = std::string(it[\"%s\"]);\n", el.Name().c_str());
        fprintf(file, "    Set%s( tmp == \"true\" || atoi(tmp.c_str()) != 0 || tmp == \"yes\" );\n", 
                el.CName1up().c_str());
        fprintf(file, "  }\n");
      } else {
        if (el.IsOptional())
          fprintf(file, "  if (it.Exists(\"%s\"))\n  ", el.Name().c_str());
        fprintf(file, "  Set%s( atoi(std::string(it[\"%s\"]).c_str()) );\n", el.CName1up().c_str(), 
                el.Name().c_str());
      }
    }
  }

  for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = attrs.begin(); 
       it != attrs.end(); it++) {
    const std::pair<std::string, XmlNode>& ref = *it;
    Attribute attr(xsd_, ref.second, "", NsHref());
    if (attr.BaseType() == "std::string") {
      if (attr.IsOptional()) {
        fprintf(file, "  if (it.PropertyExists(\"%s\"))\n  ", attr.Name().c_str());
      }
      fprintf(file, "  Set%s(it.GetProperty(\"%s\"));\n", attr.CName1up().c_str(), attr.Name().c_str());
    } else if (attr.BaseType() == "double") {
      if (attr.IsOptional()) {
        fprintf(file, "  if (it.PropertyExists(\"%s\"))\n  ", attr.Name().c_str());
      }
      fprintf(file, "  Set%s( atof(it.GetProperty(\"%s\").c_str()) );\n", attr.CName1up().c_str(), 
              attr.Name().c_str());
    } else if (attr.BaseType() == "bool") {
      if (attr.IsOptional()) {
        fprintf(file, "  if (it.PropertyExists(\"%s\"))\n", attr.Name().c_str());
      }
      fprintf(file, "  {\n");
      fprintf(file, "    std::string tmp = it.GetProperty(\"%s\");\n", attr.Name().c_str());
      fprintf(file, "    Set%s( tmp == \"true\" || atoi(tmp.c_str()) != 0 || tmp == \"yes\" );\n", 
              attr.CName1up().c_str());
      fprintf(file, "  }\n");
    } else {
      if (attr.IsOptional()) {
        fprintf(file, "  if (it.PropertyExists(\"%s\"))\n  ", attr.Name().c_str());
      }
      fprintf(file, "  Set%s( atoi(it.GetProperty(\"%s\").c_str()) );\n", attr.CName1up().c_str(), 
              attr.Name().c_str());
    }
  }

  if (b_is_mixed_) {
    fprintf(file, "  mixed_content_ = it.GetContent();\n");
  }

  fprintf(file, "  return true;\n");
}


void ComplexType::to_string(std::list<std::pair<std::string, XmlNode> >& members,
                            std::list<std::pair<std::string, XmlNode> >& attrs, FILE *file) const
{
  fprintf(file,"  std::string xmlHead = \"<?xml version=\\\"1.0\\\" encoding=\\\"UTF-8\\\"?>\";\n");
  fprintf(file, "  std::string r;\n");

  fprintf(file,"  if (partid == 1 || partid > 2)\n  {\n");
  fprintf(file, "    if (needXmlHead)\n");
  fprintf(file, "      r += xmlHead;\n");

  if (attrs.empty()) {
    fprintf(file, "    if (!name.empty())\n  ");
    fprintf(file, "    r += \"<\" + (ns_prefix_.empty() ? \"\" : ns_prefix_ + \":\") + name + (ns_declaration_.empty()?\"\": \" \" + ns_declaration_) + \">\";\n");
  } else {
    fprintf(file, "    if (!name.empty())\n  ");
    fprintf(file, "    r += \"<\" + (ns_prefix_.empty() ? \"\" : ns_prefix_ + \":\") + name;\n");

    for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = attrs.begin(); 
         it != attrs.end(); it++) {
      const std::pair<std::string, XmlNode>& ref = *it;
      Attribute attr(xsd_, ref.second, "", NsHref());

      if (attr.IsOptional()) {
        fprintf(file, "    if (%sIsSet())\n", attr.CName1up().c_str());
      }
      if (attr.BaseType() == "std::string" && attr.IsOptional()) {
        fprintf(file, "      r += ((Get%s().empty())? \"\" : \" %s=\\\"\" + Get%s() + \"\\\"\");\n", 
                attr.CName1up().c_str(), attr.Name().c_str(), attr.CName1up().c_str());
      } else if (attr.BaseType() == "std::string") {
        fprintf(file, "    r += ((Get%s().empty())? \"\" : \" %s=\\\"\" + Get%s() + \"\\\"\");\n",
                attr.CName1up().c_str(), attr.Name().c_str(), attr.CName1up().c_str());
      } else {
        fprintf(file, "    {\n");
        if (attr.BaseType() == "char" || attr.BaseType() == "unsigned char")
          fprintf(file, "      r = r + \" %s=\\\"\" + std::string(sizeof(char),Get%s()) + \"\\\"\";\n", 
                  attr.Name().c_str(), attr.CName1up().c_str());
        else
          fprintf(file, "      r = r + \" %s=\\\"\" + Utility::l2string(Get%s()) + \"\\\"\";\n", 
                  attr.Name().c_str(), attr.CName1up().c_str());
        
        fprintf(file, "    }\n");
      }
    }
    fprintf(file, "    r += (ns_declaration_.empty()? \"\": \" \" + ns_declaration_) + \">\";\n");
  }

  if (!extends_.empty()) {
    fprintf(file, "    r += %s::ToString(false, \"%s\", 1);\n", extends_cls_.c_str(), 
            name_.c_str());
  }

  for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = members.begin(); 
       it != members.end(); it++) {
    const std::pair<std::string, XmlNode>& ref = *it;
    Element el(xsd_, ref.second, b_is_sequence_, false, "", NsHref());
    if (el.IsVector()) {
      if (el.IsSimpleType()) {
        fprintf(file, "    for (%s<%s>::const_iterator it = elem_%s_.begin(); it != elem_%s_.end(); it++)\n",
          xsd_.GetApplication().ListType().c_str(),
          el.BaseType().c_str(), el.CName().c_str(), el.CName().c_str());
        fprintf(file, "    {\n");

        if (el.BaseType() == "std::string")
          fprintf(file, "      r += \"<%s>\" + *it + \"</%s>\";\n",
            el.Name().c_str(), el.Name().c_str());
        else if (el.BaseType() == "char" || el.BaseType() == "unsigned char")
            fprintf(file, "        r = r + \"<%s>\" + std::string(sizeof(char),*it) + \"</%s>\";\n",
              el.Name().c_str(), el.Name().c_str());            
          else
            fprintf(file, "        r = r + \"<%s>\" + Utility::l2string(*it) + \"</%s>\";\n",
              el.Name().c_str(), el.Name().c_str());
      } else if (el.IsComplexType()) {
        fprintf(file, "    for (%s<%s>::const_iterator it = elem_%s_.begin(); it != elem_%s_.end(); it++)\n",
          xsd_.GetApplication().ListType().c_str(),
          el.CType().c_str(), el.CName().c_str(), el.CName().c_str());
        fprintf(file, "    {\n");

        fprintf(file, "      const %s& ref = *it;\n", el.CType().c_str());
        fprintf(file, "      r += ref.ToString(false, \"%s\");\n", el.Name().c_str());
      } else if (el.BaseType() == "std::string") {
        fprintf(file, "    for (%s<%s>::const_iterator it = elem_%s_.begin(); it != elem_%s_.end(); it++)\n",
          xsd_.GetApplication().ListType().c_str(),
          el.BaseType().c_str(), el.CName().c_str(), el.CName().c_str());
        fprintf(file, "    {\n");

        fprintf(file, "      r += \"<%s>\" + *it + \"</%s>\";\n",
          el.Name().c_str(), el.Name().c_str());
      } else {
        fprintf(file, "    for (%s<%s>::const_iterator it = elem_%s_.begin(); it != elem_%s_.end(); it++)\n",
          xsd_.GetApplication().ListType().c_str(),
          el.CType().c_str(), el.CName().c_str(), el.CName().c_str());
        fprintf(file, "    {\n");

        fprintf(file, "      {\n");
        if (el.BaseType() == "char" || el.BaseType() == "unsigned char")
          fprintf(file, "        r = r + \"<%s>\" + std::string(sizeof(char),*it) + \"</%s>\";\n",
            el.Name().c_str(), el.Name().c_str());
        else
          fprintf(file, "        r = r + \"<%s>\" + Utility::l2string(*it) + \"</%s>\";\n",
            el.Name().c_str(), el.Name().c_str());
        fprintf(file, "      }\n");
      }

      fprintf(file, "    }\n");
    } else if (el.IsComplexType()) {
      fprintf(file, "    if (%sIsSet())\n", el.CName1up().c_str());
      fprintf(file, "      r += elem_%s_.ToString(false, \"%s\");\n", el.CName().c_str(), el.Name().c_str());
    } else if (el.BaseType() == "std::string") {
      if (el.IsOptional())
        fprintf(file, "    if (elem_%s_is_set_)\n  ", el.CName().c_str());
      fprintf(file, "    r += ((elem_%s_.empty())? \"<%s/>\" : \"<%s>\" + elem_%s_ + \"</%s>\");\n",
         el.CName().c_str(),el.Name().c_str(),el.Name().c_str(), el.CName().c_str(), el.Name().c_str());
    } else if (el.BaseType() == "char" || el.BaseType() == "unsigned char") {
      if (el.IsOptional())
        fprintf(file, "    if (elem_%s_is_set_)\n  ", el.CName().c_str());
      fprintf(file, "    r += ((std::string(sizeof(char),elem_%s_).empty())? \"<%s/>\" : \"<%s>\" + std::string(sizeof(char),elem_%s_) + \"</%s>\");\n",
         el.CName().c_str(),el.Name().c_str(),el.Name().c_str(), el.CName().c_str(), el.Name().c_str());
    } else {
      if (el.IsOptional())
        fprintf(file, "    if (elem_%s_is_set_)\n", el.CName().c_str());

      fprintf(file, "    {\n");
      fprintf(file, "      r = r + \"<%s>\" + Utility::l2string(elem_%s_) + \"</%s>\";\n",
        el.Name().c_str(), el.CName().c_str(), el.Name().c_str());
      fprintf(file, "    }\n");
    }
  }

  if (b_is_mixed_) {
    fprintf(file, "    r += mixed_content_;\n");
  }

  fprintf(file,"  }\n");

  fprintf(file,"  if (partid >= 2)\n  {\n");
  if (!extends_.empty()) {
    fprintf(file, "    r += %s::ToString(false, \"%s\", 2);\n", extends_cls_.c_str(), name_.c_str());
  }

  fprintf(file, "    if (!name.empty())\n  ");
  fprintf(file, "    r += \"</\" + (ns_prefix_.empty() ? \"\" : ns_prefix_ + \":\") + name + \">\";\n");  
  fprintf(file,"  }\n");

  fprintf(file, "  return r;\n");
}


void ComplexType::CreateImplementation(FILE *file) const
{
  std::list<std::pair<std::string, XmlNode> > members = GetMembers();
  std::list<std::pair<std::string, XmlNode> > attrs = GetAttributes();

  std::string parent = xsd_.GetParent(NsHref(), Name());
  std::string tmp;
  while (!parent.empty()) {
    tmp = Split(parent).CName1up + "::" + tmp;
    parent = xsd_.GetParent(NsHref(), parent);
  }
  std::string classname = tmp + CName();

  // default constructor
  fprintf(file, "%s::%s()\n", classname.c_str(), CName().c_str());
  reset_members(members, attrs, file);
  fprintf(file, "{\n");
  fprintf(file, "}\n\n\n");

  // clear
  fprintf(file, "void %s::clear()\n{\n", classname.c_str());
  clear_members(members, attrs, file);
  fprintf(file, "}\n\n\n");

  // From Xml
  fprintf(file, "bool %s::FromXml(const XmlNode& it)\n{\n", classname.c_str());
  from_xml(members, attrs, file);
  fprintf(file, "}\n\n\n");

  // From Xml
  fprintf(file, "bool %s::FromXml(const std::string& it)\n{\n", classname.c_str());
  fprintf(file, "  return FromXml(it.c_str());\n");
  fprintf(file, "}\n\n\n");

  // From Xml
  fprintf(file, "bool %s::FromXml(const char* it)\n{\n", classname.c_str());
  fprintf(file, "  XmlDocument doc;\n");
  fprintf(file, "  bool ret = doc.Load(it, (int)strlen(it));\n");
  fprintf(file, "  if (!ret) return false;\n");
  fprintf(file, "  XmlNode n(doc);\n");
  fprintf(file, "  return FromXml(n);\n");
  fprintf(file, "}\n\n\n");

  // ToString
  fprintf(file, "const std::string %s::ToString(bool needXmlHead, const std::string& name, int partid) const\n{\n", classname.c_str());
  to_string(members, attrs, file);
  fprintf(file, "}\n\n\n");
  // IsSet
  {
    fprintf(file, "bool %s::IsSet() const\n{\n", classname.c_str());

    char isSetBuff[1024 * 5] = "";
    std::string prefix = "  return ";
    for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = members.begin(); 
         it != members.end(); it++) {
      const std::pair<std::string, XmlNode>& ref = *it;
      Element el(xsd_, ref.second, b_is_sequence_, false, "", NsHref());
      sprintf(isSetBuff + strlen(isSetBuff), "%s%sIsSet()", prefix.c_str(), el.CName1up().c_str());
      prefix = " || ";
    }

    for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = attrs.begin();
         it != attrs.end(); it++) {
      const std::pair<std::string, XmlNode>& ref = *it;
      Attribute attr(xsd_, ref.second, "", NsHref());
      sprintf(isSetBuff + strlen(isSetBuff), "%s%sIsSet()", prefix.c_str(), attr.CName1up().c_str());
      prefix = " || ";
    }

    if (b_is_mixed_) {
      sprintf(isSetBuff + strlen(isSetBuff), "%s!mixed_content_.empty()", prefix.c_str());
      prefix = " || ";
    }

    if (strlen(isSetBuff) > 0)
      sprintf(isSetBuff + strlen(isSetBuff), ";\n");
    else {
      sprintf(isSetBuff + strlen(isSetBuff), "  return false;\n");
    }

    sprintf(isSetBuff + strlen(isSetBuff), "}\n\n\n");  
    fprintf(file, isSetBuff);  
  }

  fprintf(file, "void %s::SetNsPrefix(std::string& prefix) \n{\n", classname.c_str());
  fprintf(file, "  ns_prefix_ = prefix; \n}\n\n\n");

  fprintf(file, "void %s::SetNsDeclaration(std::string& decl) \n{\n", classname.c_str());
  fprintf(file, "  ns_declaration_ = decl; \n}\n\n\n");

  // SetNodeState
  {
    fprintf(file, "void %s::SetNodeState(bool state) \n{\n", classname.c_str());
    fprintf(file, "  is_setted_ = state; \n}\n\n\n");
  }

  // get/set
  for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = members.begin();
       it != members.end(); it++) {
    const std::pair<std::string, XmlNode>& ref = *it;
    Element el(xsd_, ref.second, b_is_sequence_, false, "", NsHref());
    if (el.IsVector() || el.IsComplexType()) {
      fprintf(file, "const %s %s::Get%s() const\n{\n", el.CType_out().c_str(), 
              classname.c_str(), el.CName1up().c_str());
      fprintf(file, "  return elem_%s_;\n", el.CName().c_str());
      fprintf(file, "}\n\n\n");
      fprintf(file, "%s %s::Get%s()\n{\n", el.CType_out().c_str(), classname.c_str(), 
              el.CName1up().c_str());
      fprintf(file, "  return elem_%s_;\n", el.CName().c_str());
      fprintf(file, "}\n\n\n");
      fprintf(file, "void %s::Set%s(%s %s)\n{\n  elem_%s_ = %s;\n}\n\n\n",
              classname.c_str(), el.CName1up().c_str(), el.CType_in().c_str(), el.CName().c_str(),
              el.CName().c_str(), el.CName().c_str());
    } else {
      fprintf(file, "%s %s::Get%s() const\n{\n", el.CType_out().c_str(), classname.c_str(), 
              el.CName1up().c_str());
      fprintf(file, "  return elem_%s_;\n", el.CName().c_str());
      fprintf(file, "}\n\n\n");
      fprintf(file, "void %s::Set%s(%s %s)\n{\n", classname.c_str(), el.CName1up().c_str(),
        el.CType_in().c_str(), el.CName().c_str());
      fprintf(file, "  elem_%s_ = %s;\n", el.CName().c_str(), el.CName().c_str());
      fprintf(file, "  elem_%s_is_set_ = true;\n", el.CName().c_str());
      fprintf(file, "}\n\n\n");
    }
  }

  // is_set_
  for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = members.begin();
       it != members.end(); it++) {
    const std::pair<std::string, XmlNode>& ref = *it;
    Element el(xsd_, ref.second, b_is_sequence_, false, "", NsHref());
    if (el.IsVector() || el.IsComplexType()) {
      // _is_set_ is calculated
      fprintf(file, "bool %s::%sIsSet() const\n{\n", classname.c_str(), el.CName1up().c_str());
      if (el.IsVector())
        fprintf(file, "  return !elem_%s_.empty();\n", el.CName().c_str());
      else {
        fprintf(file, "  return elem_%s_.IsSet();\n", el.CName().c_str());
        }
       fprintf(file, "}\n\n\n");
    }
  }

  // attribute get/set
  for (std::list<std::pair<std::string, XmlNode> >::const_iterator it = attrs.begin(); 
       it != attrs.end(); it++) {
    const std::pair<std::string, XmlNode>& ref = *it;
    Attribute attr(xsd_, ref.second, "", NsHref());
    {
      fprintf(file, "%s %s::Get%s() const\n{\n", attr.CType_out().c_str(), classname.c_str(), attr.CName1up().c_str());
      fprintf(file, "  return attr_%s_;\n", attr.CName().c_str());
      fprintf(file, "}\n\n\n");
      {
        fprintf(file, "void %s::Set%s(%s %s)\n{\n", classname.c_str(), attr.CName1up().c_str(),
          attr.CType_in().c_str(), attr.CName().c_str());
        fprintf(file, "  attr_%s_ = %s;\n", attr.CName().c_str(), attr.CName().c_str());
        fprintf(file, "  attr_%s_is_set_ = true;\n", attr.CName().c_str());
        fprintf(file, "}\n\n\n");
      }
    }
  }

  if (b_is_mixed_) {
    fprintf(file, "const std::string& %s::Get() const\n", classname.c_str());
    fprintf(file, "{\n");
    fprintf(file, "  return mixed_content_;\n");
    fprintf(file, "}\n\n\n");
    fprintf(file, "void %s::Set(const std::string& x)\n", classname.c_str());
    fprintf(file, "{\n");
    fprintf(file, "  mixed_content_ = x;\n");
    fprintf(file, "}\n\n\n");
  }

}


XmlNode ComplexType::Start() const
{
  XmlNode complexContent(node_, "complexContent");
  if (complexContent) {
    XmlNode extension(complexContent, "extension");
    if (extension && extension.PropertyExists("base")) {
      return extension;
    }
  }

  return node_;
}


void ComplexType::ShowGenerated()
{
  std::map<std::string, bool>::iterator it;
  for (it = b_generated_.begin(); it != b_generated_.end(); it++) {
    std::cout << "generated: " << it->first << std::endl;
  }

}

} // namespace xsd2cc

