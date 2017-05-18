
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

#ifndef XSD2CC_TYPE_BASE_H_INCLUDED_
#define XSD2CC_TYPE_BASE_H_INCLUDED_

#include <string>

namespace xsd2cc {

class Xsd;
class XmlNode;

enum BuiltType { kUNKNOWN = 0, kSTRING, kNUMBER, kBOOLEAN, kCOMPLEX };

class TypeBase {
 public:
  TypeBase(const Xsd& xsd, const XmlNode& node, const std::string& ns_prefix,
           const std::string& type);

  ~TypeBase() {}

  const std::string& Type() const { return type_; }
  const std::string Documentation(const XmlNode&) const;
  const std::string CType() const;

  const std::string& NsHref() const { return ns_href_; }

  void SetType(const std::string& x);

  bool IsBuiltin() const;
  BuiltType TypeClass() const { return t_type_; }

 protected:
  const Xsd& xsd_;
  const XmlNode& node_;
  std::string ns_href_;
  std::string type_;
  BuiltType t_type_;
  std::string c_type_;

 private:
  void SetCType();
};

}  // namespace xsd2cc

#endif  // XSD2CC_TYPE_BASE_H_INCLUDED_
