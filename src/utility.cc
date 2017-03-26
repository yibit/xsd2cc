
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


#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <xsd2cc/utility.h>


namespace xsd2cc {

std::string Utility::l2string(long l)
{
  std::string str;
  char tmp[64];
  snprintf(tmp, sizeof(tmp), "%ld", l);
  str = tmp;

  return str;
}


std::string Utility::bigint2string(int64_t l)
{
  std::string str;
  int64_t tmp = l;
  if (l < 0) {
    str = "-";
    tmp = -l;
  }

  while (tmp) {
    uint64_t a = tmp % 10;
    str = (char)(a + 48) + str;
    tmp /= 10;
  }

  if (!str.size()) {
    str = "0";
  }

  return str;
}


std::string Utility::bigint2string(uint64_t l)
{
  std::string str;
  uint64_t tmp = l;
  while (tmp) {
    uint64_t a = tmp % 10;
    str = (char)(a + 48) + str;
    tmp /= 10;
  }

  if (!str.size()) {
    str = "0";
  }

  return str;
}


std::string Utility::ToLower(const std::string& str)
{
  std::string r;
  for (size_t i = 0; i < str.size(); ++i) {
    if (str[i] >= 'A' && str[i] <= 'Z') {
      r += str[i] | 32;
    } else {
      r += str[i];
    }
  }

  return r;
}


std::string Utility::ToUpper(const std::string& str)
{
  std::string r;
  for (size_t i = 0; i < str.size(); ++i) {
    if (str[i] >= 'a' && str[i] <= 'z') {
      r += (char)(str[i] - 32);
    } else {
      r += str[i];
    }
  }

  return r;
}


std::string Utility::ToString(double d)
{
  char tmp[100];
  snprintf(tmp, sizeof(tmp), "%f", d);
  return tmp;
}


Utility::Path::Path(const std::string& _str)
{
  std::string str = _str;
  for (size_t i = 0; i < str.size(); ++i) {
    if (str[i] == '\\') {
      str[i] = '/';
    }
  }

  size_t x = 0;
  for (size_t i = 0; i < str.size(); ++i) {
    if (str[i] == '/' || str[i] == '\\') {
      x = i + 1;
    }
  }

  path_ = str.substr(0, x);
  file_ = str.substr(x);
  for (size_t i = 0; i < file_.size(); ++i) {
    if (file_[i] == '.') {
      ext_ = file_.substr(i + 1);
    }
  }

}


const std::string Utility::FromUtf8(const std::string& str)
{
  if (!str.size()) {
    return "";
  }

  std::string r;
  for (size_t i = 0; i < str.size(); ++i) {
    if (i < str.size() - 1 && (str[i] & 0xe0) == 0xc0
      && (str[i + 1] & 0xc0) == 0x80)
     {
      int c1 = str[i] & 0x1f;
      int c2 = str[++i] & 0x3f;
      int c = (c1 << 6) + c2;
      r += (char)c;
    } else {
      r += str[i];
    }
  }

  return r;
}


const std::string Utility::ToUtf8(const std::string& str)
{
  if (str.empty()) {
    return "";
  }

  std::string r;
  for (size_t i = 0; i < str.size(); ++i) {
    if (((unsigned)str[i] & 0x80) == 0x80) {
      r += (str[i] >> 6) | 0xc0;
      r += (str[i] & 0x3f) | 0x80;
    } else {
      r += str[i];
    }
  }

  return r;
}


const Utility::Path Utility::CurrentDirectory()
{
  char slask[32000];
  if (!getcwd(slask, 32000)) {
    return Path(".");
  }

  return Path(slask);
}


bool Utility::ChangeDirectory(const Utility::Path& to_dir)
{
  if (chdir( to_dir.GetPath().c_str() ) == -1){
    return false;
  }

  return true;
}


} // namespace xsd2cc

