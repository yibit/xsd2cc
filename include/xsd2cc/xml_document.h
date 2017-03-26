
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


#ifndef XSD2CC_XML_DOCUMENT_H_INCLUDED_
#define XSD2CC_XML_DOCUMENT_H_INCLUDED_


#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include <string>


namespace xsd2cc {

class XmlDocument {
 public:
  XmlDocument() : doc_(NULL) {}

  virtual ~XmlDocument();

  bool Load(const std::string& filename,
            const std::string& verify_ns = "",
            const std::string& verify_root = "");

  bool Load(const char *buffer, int size);

  bool Load(const std::string& str, int size);

  bool empty() { return doc_ == NULL; }

  operator xmlDocPtr();

 protected:
  XmlDocument(const XmlDocument& ) {}

 private:
  XmlDocument& operator=(const XmlDocument& ) { return *this; }

 private:
  xmlDocPtr doc_;
};

} // namespace xsd2cc

#endif // XSD2CC_XML_DOCUMENT_H_INCLUDED_

