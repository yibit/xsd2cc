
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

#ifndef XSD2CC_XML_NODE_H_INCLUDED_
#define XSD2CC_XML_NODE_H_INCLUDED_

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include <map>
#include <string>

namespace xsd2cc {

class XmlDocument;

class XmlNode {
 public:
  XmlNode() : doc_(NULL), current_(NULL) {}

  XmlNode(XmlDocument& doc);

  XmlNode(XmlDocument& doc, const std::string& nodepath);

  XmlNode(const XmlNode& node, const std::string& nodepath);

  XmlNode(XmlDocument& doc, xmlNodePtr ptr);

  XmlNode(xmlDocPtr doc, xmlNodePtr ptr);

  ~XmlNode() {}

  operator xmlNodePtr() const;

  XmlNode operator[](const std::string& name) const;

  operator std::string() const { return GetContent(); }

  void operator++() const;

  bool empty() { return doc_ == NULL && current_ == NULL; }

  xmlDocPtr GetDocument() const { return doc_; }

  bool Valid() const { return current_ ? true : false; }

  xmlNodePtr GetRootElement() const;

  std::string GetProperty(const std::string& propname) const;
  bool PropertyExists(const std::string& propname) const;

  xmlNodePtr GetChildrenNode() const;
  xmlNodePtr GetNextNode() const;

  const std::string& GetNodeName() const;
  const std::string& GetContent() const;

  void SetCurrent(xmlNodePtr p) const { current_ = p; }

  xmlNsPtr GetNodeNs() const;

  const std::string& GetNodeNsPrefix() const;

  const std::string& GetNodeNsHref() const;

  xmlNodePtr GetFirstElement(const std::string& name) const;
  xmlNodePtr GetFirstElement(xmlNodePtr parent, const std::string& name) const;
  xmlNodePtr GetNextElement(xmlNodePtr node, const std::string& name) const;

  bool Exists(const std::string& name) const;

  std::map<std::string, std::string> GetNsMap() const;

  std::map<std::string, std::string> GetNsMapRe() const;

  const std::string FindProperty(const std::string& propname,
                                 bool climb = false) const;

 private:
  xmlDocPtr doc_;
  mutable xmlNodePtr current_;
  mutable std::string current_name_;
  mutable std::string ns_prefix_;
  mutable std::string ns_href_;
  mutable std::string content_;
  mutable std::string lookup_name_;
};

}  // namespace xsd2cc

#endif  // XSD2CC_XML_NODE_H_INCLUDED_
