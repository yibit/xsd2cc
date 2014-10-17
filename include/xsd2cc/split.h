
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


#ifndef XSD2CC_SPLIT_H_INCLUDED_
#define XSD2CC_SPLIT_H_INCLUDED_


#include <string>
#include <xsd2cc/parse.h>


namespace xsd2cc {

class Split {
 public:
  Split(const std::string& name) {
    if (!name.empty() && name[0] == ':') {
      Type = name.substr(1);

    } else if (name.find(":") != std::string::npos) {
      Parse pa(name, ":");
      NS = pa.getword();
      Type = pa.getword();

    } else {
      Type = name;
    }

    for (size_t i = 0; i < Type.size(); i++) {
      switch (Type[i]) {
      default:
        CName += (!isalnum(Type[i]) ? '_' : Type[i]); // "_" ?
      }

    CName1up = CName;
    if (!CName1up.empty() && CName1up[0] >= 'a' && CName1up[0] <= 'z') {
      CName1up[0] = (char)CName1up[0] - 32;
    }
  }
}

 private:
  Split(const Split&);
  Split& operator=(const Split&);

 public:
  std::string NS;
  std::string Type;
  std::string CName;
  std::string CName1up;
};

} // namespace xsd2cc

#endif // XSD2CC_SPLIT_H_INCLUDED_

