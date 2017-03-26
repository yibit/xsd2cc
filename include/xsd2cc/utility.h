
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


#ifndef XSD2CC_UTILITY_H_INCLUDED_
#define XSD2CC_UTILITY_H_INCLUDED_


#include <inttypes.h>
#include <string>


namespace xsd2cc {

class Utility {
 public:

 class Path {
  public:
    Path(const std::string& );

    const std::string& GetPath() const { return path_; }
    const std::string& GetFilename() const { return file_; }
    const std::string& GetExtension() const { return ext_; }

  private:
    std::string path_;
    std::string file_;
    std::string ext_;
  };

 public:
  static std::string base64(const std::string& str_in);
  static std::string base64d(const std::string& str_in);
  static std::string l2string(long l);
  static std::string bigint2string(int64_t l);
  static std::string bigint2string(uint64_t l);

  static std::string ToLower(const std::string& str);
  static std::string ToUpper(const std::string& str);
  static std::string ToString(double d);;

  static const std::string FromUtf8(const std::string& );
  static const std::string ToUtf8(const std::string&);

  static const Path CurrentDirectory();
  static bool ChangeDirectory(const Path& to_dir);
};

} // namespace

#endif // XSD2CC_UTILITY_H_INCLUDED_

