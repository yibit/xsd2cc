
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

#ifndef XSD2CC_COMPLEX_H_INCLUDED_
#define XSD2CC_COMPLEX_H_INCLUDED_

#include <xsd2cc/type_base.h>
#include <list>
#include <map>
#include <string>

namespace xsd2cc {

class Xsd;
class XmlNode;

class ComplexType : public TypeBase {
 public:
  ComplexType(const Xsd& xsd, const XmlNode& node, const std::string& name = "",
              const std::string& ns_prefix = "");

  ~ComplexType() {}

  const std::string& Name() const { return name_; }
  const std::string Documentation() const;

  int NumberOfMembers() const;
  bool GetRef(std::string& ref_) const;

  std::list<std::pair<std::string, XmlNode> > GetMembers() const;
  std::list<std::pair<std::string, XmlNode> > GetAttributes() const;

  void CreateInterface(FILE* file, int lvl = 1) const;
  void CreateImplementation(FILE* file) const;

  const std::string CName() const;

  bool OkToGenerate() const;
  void SetGenerated(bool x = true);
  bool IsGenerated() const;

  bool IsSequence() const { return b_is_sequence_; }
  bool IsAll() const { return b_is_all_; }
  bool IsComplexContent() const { return b_complexContent_; }
  bool IsArray() const { return b_is_array_; }
  bool IsMixed() const { return b_is_mixed_; }

  static void ShowGenerated();

 private:
  void reset_members(std::list<std::pair<std::string, XmlNode> >& members,
                     std::list<std::pair<std::string, XmlNode> >& attrs,
                     FILE* file) const;

  void clear_members(std::list<std::pair<std::string, XmlNode> >& members,
                     std::list<std::pair<std::string, XmlNode> >& attrs,
                     FILE* file) const;

  void from_xml(std::list<std::pair<std::string, XmlNode> >& members,
                std::list<std::pair<std::string, XmlNode> >& attrs,
                FILE* file) const;

  void to_string(std::list<std::pair<std::string, XmlNode> >& members,
                 std::list<std::pair<std::string, XmlNode> >& attrs,
                 FILE* file) const;

  XmlNode Start() const;

 private:
  std::string name_;
  bool b_is_sequence_;
  bool b_is_all_;
  bool b_complexContent_;
  bool b_is_array_;
  bool b_is_mixed_;
  std::string extends_;
  std::string extends_cls_;
  static std::map<std::string, bool> b_generated_;
};

}  // namespace xsd2cc

#endif  // XSD2CC_COMPLEX_H_INCLUDED_
