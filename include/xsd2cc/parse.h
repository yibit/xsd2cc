
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


#ifndef XSD2CC_PARSE_H_INCLUDED_
#define XSD2CC_PARSE_H_INCLUDED_


#include <stddef.h>
#include <string>


namespace xsd2cc {

class Parse {
 public:
  Parse();
  Parse(const std::string& s);
  Parse(const std::string& s, const std::string& sp);
  Parse(const std::string& s, const std::string& sp, short nospace);

  ~Parse() {}

  short issplit(const char);
  void getsplit();
  void getsplit(std::string&);

  std::string getword();
  void getword(std::string&);
  void getword(std::string&, std::string&,int);
  std::string getrest();
  void getrest(std::string&);
  long getvalue();
  void setbreak(const char);
  int getwordlen();
  int getrestlen();

  void enablebreak(const char c) {
    enable_ = c;
  }

  void disablebreak(const char c) {
    disable_ = c;
  }

  void getline();
  void getline(std::string&);
  size_t getptr() { return the_ptr_; }
  void EnableQuote(bool b) { nospace_ = b; }

 private:
  char breakchar_;
  char enable_;
  char disable_;
  short nospace_;
  bool quote_;
  size_t the_ptr_;
  std::string the_str_;
  std::string splits_;
  std::string ord_;
};

} // namespace xsd2cc

#endif // XSD2CC_PARSE_H_INCLUDED_

