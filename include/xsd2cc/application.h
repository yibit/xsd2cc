
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


#ifndef XSD2CC_APPLICATION_H_INCLUDED_
#define XSD2CC_APPLICATION_H_INCLUDED_


#include <stdio.h>
#include <map>
#include <string>


namespace xsd2cc {

class Application {
 public:
  Application(int argc, char *argv[]);
  ~Application() {}

  const std::string& Filename() const { return filename_; }
  const std::string& Portname() const { return portname_; }
  const std::string& Namespace() const { return global_ns_; }
  const std::string& Classname() const { return classname_; }
  const std::string& ListType() const { return lt_; }
  const std::string& ToDir() const { return todir_; }

  FILE *myfopen(const std::string& filename, const std::string& mode);

  const std::string Href2Prefix(const std::string& ns_href) const;

 private:
  std::string filename_;
  std::string portname_;
  std::string global_ns_;
  std::string classname_;
  std::string lt_;
  std::string todir_;
  std::map<std::string, std::string> nsmap_;
};

} // namespace xsd2cc

#endif // XSD2CC_APPLICATION_H_INCLUDED_

