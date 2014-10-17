
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


#ifndef XSD2CC_SIMPLE_TYPE_H_INCLUDED_
#define XSD2CC_SIMPLE_TYPE_H_INCLUDED_


#include <string>
#include <vector>
#include <xsd2cc/type_base.h>


namespace xsd2cc {

class Xsd;

class SimpleType : public TypeBase {
 public:
  SimpleType(const Xsd& xsd,
             const XmlNode& node, 
             const std::string& name = "", 
             const std::string& ns_prefix = "");

  ~SimpleType() {}

  const std::string& name() const { return name_; }

  const std::string Documentation() const;

  void CreateInterface(FILE *file, int lvl = 1) const;

  const std::string CType() const;

 private:
  std::string               name_;
  std::vector<std::string>  enum_values_;
  std::vector<std::string>  pattern_values_; // using re2 to check
};

} // namespace xsd2cc

#endif // XSD2CC_SIMPLE_TYPE_H_INCLUDED_

