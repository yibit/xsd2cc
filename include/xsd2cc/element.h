
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


#ifndef XSD2CC_ELEMENT_H_INCLUDED_
#define XSD2CC_ELEMENT_H_INCLUDED_


#include <string>
#include <xsd2cc/type_base.h>


namespace xsd2cc {

class Xsd;
class Application;

class Element : public TypeBase {
 public:
  Element(const Xsd& xsd,
          const XmlNode& node, 
          bool in_sequence, 
          bool in_choice, 
          const std::string& name = "", 
          const std::string& ns_prefix = "");

  ~Element() {}

  const std::string& Name() const { return name_; }
  const std::string& BaseType() const { return base_type_; }
  const std::string Documentation() const;

  void CreateInterface(FILE *file) const;

  bool IsOptional() const { return minOccurs_ == 0; }
  bool IsVector() const { return maxOccurs_ > 1 || maxOccurs_ == -1; }

  const std::string CName() const;
  const std::string CName1up() const;

  const std::string CType_decl() const;
  const std::string CType_in() const;
  const std::string CType_out() const;

  const std::string DefaultValue() const;
  const std::string& Default() const;

  bool HasDefault() const;
  bool IsComplexType() const;
  bool IsSimpleType() const;

 private:
  Application& app_;
  std::string  name_;
  std::string  base_type_;
  int          minOccurs_;
  int          maxOccurs_;
  bool         has_default_;
  std::string  default_;
};

} // namespace xsd2cc

#endif // XSD2CC_ELEMENT_H_INCLUDED_

