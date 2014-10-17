
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


#include <xsd2cc/xml_node.h>
#include <xsd2cc/xml_document.h>
#include <xsd2cc/utility.h>
#include <iostream>


namespace xsd2cc {

XmlNode::XmlNode(XmlDocument& doc)
    : doc_(doc), current_(GetRootElement())
{

}


XmlNode::XmlNode(XmlDocument& doc, const std::string& nodepath)
    : doc_(doc), current_(GetRootElement())
{
  xmlNodePtr p = GetFirstElement(nodepath);
  SetCurrent(p);
}


XmlNode::XmlNode(const XmlNode& node, const std::string& nodepath)
    : doc_(node.GetDocument()), current_(node)
{
  xmlNodePtr p = GetFirstElement(node, nodepath);
  SetCurrent(p);
}


XmlNode::XmlNode(XmlDocument& doc, xmlNodePtr ptr)
    : doc_(doc), current_(ptr)
{

}


XmlNode::XmlNode(xmlDocPtr doc, xmlNodePtr ptr)
    : doc_(doc), current_(ptr)
{

}


xmlNodePtr XmlNode::GetRootElement() const
{
  current_ = xmlDocGetRootElement(doc_);
  return current_;
}


std::string XmlNode::GetProperty(const std::string& name) const
{
  xmlChar *p = current_ ? xmlGetProp(current_, (const xmlChar *)name.c_str()) : NULL;
  if (!p) {
    std::cout << "Property '" + name + "' not found in node: " + GetNodeName() << std::endl;
    return "";
  }

  std::string str = (char *)p;
  xmlFree(p);

  return Utility::FromUtf8(str);
}


bool XmlNode::PropertyExists(const std::string& name) const
{
  xmlChar *p = current_ ? xmlGetProp(current_, (const xmlChar *)name.c_str()) : NULL;
  if (!p) {
    return false;
  }

  xmlFree(p);

  return true;
}


xmlNodePtr XmlNode::GetChildrenNode() const
{
  current_ = current_ ? current_->xmlChildrenNode : NULL;
  return current_;
}


xmlNodePtr XmlNode::GetNextNode() const
{
  do {
    current_ = current_ ? current_->next : NULL;
  } while (current_ && xmlIsBlankNode( current_ ));

  return current_;
}


const std::string& XmlNode::GetNodeName() const
{
  if (current_) {
    current_name_ = Utility::FromUtf8((char *)current_->name);
  } else {
    current_name_ = "";
  }

  return current_name_;
}


const std::string& XmlNode::GetContent() const
{
  content_ = "";

  if (current_) {
    xmlNodePtr p = current_;
    xmlNodePtr p2 = GetChildrenNode();
    if (p2 && p2->content) {
      content_ = Utility::FromUtf8((char *)p2->content);
    }
    SetCurrent(p);
  }

  return content_;
}


xmlNsPtr XmlNode::GetNodeNs() const
{
  if (current_) {
    return current_->ns;
  }

  return NULL;
}


const std::string& XmlNode::GetNodeNsPrefix() const
{
  if (current_ && current_->ns && current_->ns->prefix) {
    ns_prefix_ = Utility::FromUtf8((char *)current_->ns->prefix);
  } else {
    ns_prefix_ = "";
  }

  return ns_prefix_;
}


const std::string& XmlNode::GetNodeNsHref() const
{
  if (current_ && current_->ns && current_->ns->href) {
    ns_href_ = Utility::FromUtf8((char *)current_->ns->href);
  } else {
    ns_href_ = "";
  }

  return ns_href_;
}


xmlNodePtr XmlNode::GetFirstElement(const std::string& name) const
{
  if (lookup_name_.empty()) {
    lookup_name_ = name;
  }

  GetRootElement();
  xmlNodePtr p = GetChildrenNode();
  while (p) {
    if (name == GetNodeName()) {
      return p;
    }
    p = GetNextNode();
  }

  return NULL;
}


xmlNodePtr XmlNode::GetFirstElement(xmlNodePtr base,
                                    const std::string& name) const
{
  if (lookup_name_.empty()) {
    lookup_name_ = name;
  }

  SetCurrent(base);

  xmlNodePtr p = GetChildrenNode();
  while (p) {
    if (name == GetNodeName()) {
      return p;
    }
    p = GetNextNode();
  }

  return NULL;
}


xmlNodePtr XmlNode::GetNextElement(xmlNodePtr p,const std::string& name) const
{
  SetCurrent(p);
  p = GetNextNode();
  while (p) {
    if (name == GetNodeName()) {
      return p;
    }

    p = GetNextNode();
  }

  return NULL;
}


XmlNode::operator xmlNodePtr() const
{
  return current_;
}


XmlNode XmlNode::operator[](const std::string& name) const
{
  xmlNodePtr p0 = current_;
  xmlNodePtr p = GetFirstElement(current_, name);
  SetCurrent(p0);
  if (p) {
    return XmlNode(doc_, p);
  }

  return XmlNode();
}


bool XmlNode::Exists(const std::string& name) const
{
  xmlNodePtr p0 = current_;
  xmlNodePtr p = GetFirstElement(current_, name);
  SetCurrent(p0);
  if (!p) {
    return false;
  }

  return true;
}


void XmlNode::operator++() const
{
  GetNextNode();
  while (current_) {
    if (lookup_name_ == GetNodeName()) {
      return;
    }
    GetNextNode();
  }

}


std::map<std::string, std::string> XmlNode::GetNsMap() const
{
  xmlNsPtr *p = xmlGetNsList(doc_, current_);
  std::map<std::string, std::string> vec;

  for (int i = 0; p && p[i]; ++i) {
    std::string href = Utility::FromUtf8((char *)p[i]->href);
    std::string prefix = p[i]->prefix ? Utility::FromUtf8((char *)p[i]->prefix) : "";
    vec[prefix] = href;
    if (!p[i]->next) {
      break;
    }
  }

  return vec;
}


std::map<std::string, std::string> XmlNode::GetNsMapRe() const
{
  xmlNsPtr *p = xmlGetNsList(doc_, current_);
  std::map<std::string, std::string> vec;

  for (int i = 0; p && p[i]; ++i) {
    std::string href = Utility::FromUtf8((char *)p[i]->href);
    std::string prefix = p[i]->prefix ? Utility::FromUtf8((char *)p[i]->prefix) : "";
    vec[href] = prefix;
    if (!p[i]->next) {
      break;
    }
  }

  return vec;
}


const std::string XmlNode::FindProperty(const std::string& propname, 
                                        bool climb) const
{
  while (current_) {

   if (PropertyExists(propname)) {
      return GetProperty(propname);
    }

    if (!climb) {
      break;
    }

    current_ = current_->parent;
  }

  return "";
}

} // namespace xsd2cc

