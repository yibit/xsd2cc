
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

#include <string.h>
#include <xsd2cc/xml_document.h>
#include <iostream>

namespace xsd2cc {

bool XmlDocument::Load(const std::string& filename,
                       const std::string& verify_ns,
                       const std::string& verify_root) {
  xmlNodePtr cur;
  xmlNsPtr ns;

  if (!(doc_ = xmlParseFile(filename.c_str()))) {
    std::cout << "Parse of file failed: " << filename << std::endl;
    return false;
  }

  if (!(cur = xmlDocGetRootElement(doc_))) {
    xmlFreeDoc(doc_);
    doc_ = NULL;
    std::cout << "PDocument is empty: " << filename << std::endl;
    return false;
  }

  if (verify_ns.size() &&
      !(ns = xmlSearchNsByHref(doc_, cur, (const xmlChar*)verify_ns.c_str()))) {
    xmlFreeDoc(doc_);
    doc_ = NULL;
    std::cout << "Document namespace !=" << verify_ns << std::endl;
    return false;
  }

  if (verify_root.size() &&
      xmlStrcmp(cur->name, (const xmlChar*)verify_root.c_str())) {
    xmlFreeDoc(doc_);
    doc_ = NULL;
    std::cout << "Document root != " << verify_root << std::endl;
    return false;
  }

  return true;
}

bool XmlDocument::Load(const char* buffer, int size) {
  xmlNodePtr cur;

  if (!buffer || strlen(buffer) < size) {
    return false;
  }

  if (!(doc_ = xmlParseMemory(buffer, size))) {
    std::cout << "Parse of Buffer failed!" << std::endl;
    return false;
  }

  if (!(cur = xmlDocGetRootElement(doc_))) {
    xmlFreeDoc(doc_);
    doc_ = NULL;
    std::cout << "Document is empty!" << std::endl;
    return false;
  }

  return true;
}

bool XmlDocument::Load(const std::string& str, int size) {
  return Load(str.c_str(), size);
}

XmlDocument::~XmlDocument() {
  if (doc_) {
    xmlFreeDoc(doc_);
  }
}

XmlDocument::operator xmlDocPtr() { return doc_; }

}  // namespace xsd2cc
