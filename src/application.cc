
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xsd2cc/application.h>
#include <iostream>

namespace xsd2cc {

Application::Application(int argc, char* argv[])
    : lt_("std::vector"), todir_(".") {
  if (argc < 2) {
    printf("Usage: %s [options] <.xsd filename>\n", *argv);
    printf("    -namespace <ns>        Define a global namespace\n");
    printf("    -classname <name>      C++ Classname\n");
    printf("    -lt <type>             Vector type (default: std::vector)\n");
    printf(
        "    -ns <href> <prefix>    Use 'prefix' as C++ namespace for xml "
        "namespace 'href'\n");
    printf("    -todir <dir>           Create files in <dir>\n");
    exit(-1);
  }

  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-namespace") && i < argc - 1)
      global_ns_ = argv[++i];
    else if (!strcmp(argv[i], "-classname") && i < argc - 1)
      classname_ = argv[++i];
    else if (!strcmp(argv[i], "-lt") && i < argc - 1)
      lt_ = argv[++i];
    else if (!strcmp(argv[i], "-ns") && i < argc - 2) {
      std::string href = argv[++i];
      std::string prefix = argv[++i];
      nsmap_[href] = prefix;
    } else if (!strcmp(argv[i], "-todir") && i < argc - 1)
      todir_ = argv[++i];
    else
      filename_ = argv[i];
  }

  if (classname_.empty() && !filename_.empty()) {
    size_t pos = filename_.rfind(".");
    if (pos != std::string::npos) {
      classname_ = filename_.substr(0, pos);
    }
  }

  for (char* s = (char*)classname_.c_str(); *s; s++) {
    if (*s == '.' || *s == '-' || *s == '+' || *s == '%') *s = '_';
  }
}

FILE* Application::myfopen(const std::string& filename,
                           const std::string& mode) {
  unlink(filename.c_str());
  FILE* file = fopen(filename.c_str(), mode.c_str());
  if (!file) {
    std::cout << "Couldn't open file: " + filename << std::endl;
    return NULL;
  }

  return file;
}

const std::string Application::Href2Prefix(const std::string& ns_href) const {
  std::map<std::string, std::string>::const_iterator it = nsmap_.find(ns_href);
  if (it != nsmap_.end()) {
    return it->second;
  }

  return "";
}

}  // namespace xsd2cc
