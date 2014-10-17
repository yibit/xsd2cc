
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


#ifndef XSD2CC_XSD_H_INCLUDED_
#define XSD2CC_XSD_H_INCLUDED_


#include <stdio.h>
#include <list>
#include <map>
#include <string>
#include <xsd2cc/xml_node.h>


namespace xsd2cc {

class Application;

namespace {

class NSFile {
 public:
  NSFile(FILE *file, const std::string& ns, const std::string& href = "") 
    : file_(file), 
      ns_(ns), 
      href_(href), 
      start_(false)
  {

  }

  ~NSFile() {
    if (start_ && !ns_.empty()) {
      fprintf(file_, "} // namespace %s\n\n", ns_.c_str());
      }
    }

  void Begin() {
    if (!start_) {
        if (!ns_.empty()) {
          if (!href_.empty()) {
            fprintf(file_, "/** %s */\n", href_.c_str());
            }
          fprintf(file_, "namespace %s {\n\n", ns_.c_str());
        }
        start_ = true;
      }
    }

  private:
    FILE *file_;
    std::string ns_;
    std::string href_;
    bool start_;
  };

} // namespace

class Xsd {
 public:
  Xsd(Application& app, XmlDocument& doc);
  ~Xsd();

  Application& GetApplication() const { return app_; }

  const std::string GetTargetNamespace() const;
  const std::string GetTargetNamespacePrefix() const;

  std::list<std::string> GetPorts() const;
  const std::string GetPortname() const;
  std::list<std::string> GetOperations(const std::string& portname) const;

  const std::string GetMessageForOp(const std::string& portname, 
                                    const std::string& op, 
                                    const std::string& type) const;

  void ShowMessage(const std::string& msgname);
  void ShowElement(const XmlNode& element_node, const std::string& level);
  void ShowComplexType(const XmlNode& complex, const std::string& level);

  std::list<std::pair<std::string, XmlNode> > GetMessageParameters(const std::string& msgname, 
                                                                   std::string& path) const;

  XmlNode GetPortTypeOp(const std::string& portname, const std::string& op) const;
  XmlNode GetBindingOp(const std::string& portname, const std::string& op) const;
  XmlNode GetMessage(const std::string& msgname) const;
  XmlNode GetSchema(const std::string& target_ns) const;
  XmlNode GetElement(const std::string& name) const;
  XmlNode FindComplexType(const std::string& ns_href, const std::string& type) const;
  XmlNode FindSimpleType(const std::string& ns_href, const std::string& type) const;
  XmlNode GetElement(const XmlNode& node, const std::string& name) const;

  bool CreateInterface();
  bool CreateImplementation();

  const std::string GetParent(const std::string& href, const std::string& classname) const;

  const std::list<std::string>& ComplexTypes(const std::string& ns_href) const;
  const std::list<std::string>& SimpleTypes(const std::string& ns_href) const;

  const std::string NamespaceFromHref(const std::string& href) const;

  const std::string GetNamespace(const std::string& ns_href) const;

 private:
  void ParseElements(XmlDocument& doc, const std::string& targetNs = "");
  void ParseElements(const XmlNode& node, const std::string& ns_href, const std::string& parent_class = "");
  void ParseTypes(const XmlNode& node, const std::string& ns_href);
  XmlNode ParseComplexType(const XmlNode& node, const std::string& ns_href, const std::string& name);
  void AddLicense(FILE *);

 private:
  Application& app_;
  XmlDocument& doc_;
  std::list<XmlDocument *> import_;
  std::list<std::string> empty_;
  std::map<std::string, std::list<std::string> > complexType_;
  std::map<std::string, std::list<std::string> > simpleType_;
  /** Map complexType names in each namespace to XmlNode of definition */
  std::map<std::string, std::map<std::string, XmlNode> > complexType_Map_;
  /** Map simpleType names in each namespace to XmlNode of definition */
  std::map<std::string, std::map<std::string, XmlNode> > simpleType_Map_;

  std::map<std::string, std::map<std::string, std::string> > parent_;
};

} // namespace xsd2cc

#endif // XSD2CC_XSD_H_INCLUDED_

