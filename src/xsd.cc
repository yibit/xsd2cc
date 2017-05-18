
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

#include <xsd2cc/application.h>
#include <xsd2cc/complex_type.h>
#include <xsd2cc/simple_type.h>
#include <xsd2cc/split.h>
#include <xsd2cc/utility.h>
#include <xsd2cc/xml_document.h>
#include <xsd2cc/xml_node.h>
#include <xsd2cc/xsd.h>
#include <iostream>

namespace xsd2cc {

Xsd::Xsd(Application& app, XmlDocument& doc) : app_(app), doc_(doc) {
  ParseElements(doc);
}

Xsd::~Xsd() {
  std::list<XmlDocument*>::iterator it;
  for (it = import_.begin(); it != import_.end(); it++) {
    XmlDocument* p = *it;
    delete p;
  }
}

const std::string Xsd::GetTargetNamespace() const {
  XmlNode root(doc_);

  return root.GetProperty("targetNamespace");
}

const std::string Xsd::GetTargetNamespacePrefix() const {
  XmlNode root(doc_);

  return root.GetNsMapRe()[root.GetProperty("targetNamespace")];
}

std::list<std::string> Xsd::GetPorts() const {
  std::list<std::string> vec;
  XmlNode n0(doc_, "service");
  if (n0) {
    XmlNode n(n0, "port");
    while (n) {
      vec.push_back(n.GetProperty("name"));
      ++n;
    }
  }

  return vec;
}

const std::string Xsd::GetPortname() const {
  XmlNode n(doc_, "portType");
  if (n) {
    return n.GetProperty("name");
  }

  return "";
}

std::list<std::string> Xsd::GetOperations(const std::string& portname) const {
  std::list<std::string> vec;
  XmlNode n(doc_, "portType");
  while (n) {
    if (n.GetProperty("name") == portname || portname.empty()) {
      XmlNode n2(n, "operation");
      while (n2) {
        vec.push_back(n2.GetProperty("name"));
        ++n2;
      }
    }
    ++n;
  }

  return vec;
}

const std::string Xsd::GetMessageForOp(const std::string& portname,
                                       const std::string& op,
                                       const std::string& type) const {
  XmlNode n(doc_, "portType");
  while (n) {
    if (n.GetProperty("name") == portname || portname.empty()) {
      XmlNode n2(n, "operation");
      while (n2) {
        if (n2.GetProperty("name") == op) {
          XmlNode n3(n2, type);
          if (n3) {
            return n3.GetProperty("message");
          }
        }
        ++n2;
      }
    }
    ++n;
  }

  return "";
}

XmlNode Xsd::GetPortTypeOp(const std::string& portname,
                           const std::string& op) const {
  XmlNode n(doc_, "portType");
  while (n) {
    if (n.GetProperty("name") == portname || portname.empty()) {
      XmlNode n2(n, "operation");
      while (n2) {
        if (n2.GetProperty("name") == op) {
          return n2;
        }
        ++n2;
      }
    }
    ++n;
  }

  return XmlNode();
}

XmlNode Xsd::GetBindingOp(const std::string& portname,
                          const std::string& op) const {
  XmlNode n(doc_, "binding");
  while (n) {
    if (n.GetProperty("name") == portname || portname.empty()) {
      XmlNode n2(n, "operation");
      while (n2) {
        if (n2.GetProperty("name") == op) {
          return n2;
        }
        ++n2;
      }
    }
    ++n;
  }

  return XmlNode();
}

XmlNode Xsd::GetMessage(const std::string& msgname) const {
  XmlNode n(doc_, "message");
  while (n) {
    if (n.GetProperty("name") == Split(msgname).Type) {
      return n;
    }
    ++n;
  }

  return XmlNode();
}

void Xsd::ShowMessage(const std::string& msgname) {
  XmlNode message = GetMessage(msgname);
  XmlNode n(message, "part");
  while (n) {
    std::string name = n.GetProperty("name");
    printf("  part/ name: %s", name.c_str());

    std::string type = n.GetProperty("type");
    if (!type.empty()) {
      printf(" type: %s", type.c_str());
    }

    std::string element = n.GetProperty("element");
    if (!element.empty()) {
      printf(" element: %s", (element = n.GetProperty("element")).c_str());
    }
    printf("\n");

    if (!element.empty()) {
      XmlNode el = GetElement(element);
      printf("  {\n");
      ShowElement(el, "    ");
      printf("  }\n");
    }
    ++n;
  }
}

void Xsd::ShowElement(const XmlNode& element_node, const std::string& level) {
  std::string name = element_node.GetProperty("name");
  std::string ref = element_node.GetProperty("ref");

  XmlNode complex(element_node, "complexType");
  if (complex) {
    printf("%s{\n", level.c_str());
    ShowComplexType(complex, level + "  ");
    printf("%s}\n", level.c_str());
    return;

  } else if (!ref.empty()) {
    XmlNode n = GetElement(ref);
    if (n) {
      printf("%s{\n", level.c_str());
      ShowElement(n, level + "  ");
      printf("%s}\n", level.c_str());
    }
    return;
  }

  std::string type = element_node.GetProperty("type");
  std::string min = element_node.GetProperty("minOccurs");
  std::string max = element_node.GetProperty("maxOccurs");
  min = (min.empty() ? "1" : min);
  max = (max.empty() ? "1" : max);

  if (!type.empty()) {
    std::string tmp = level;
    tmp += type;
    while (tmp.size() < 24 + level.size()) tmp += " ";
    tmp += " \"" + name + "\"";
    while (tmp.size() < 40 + level.size()) tmp += " ";
    printf("%s %s .. %s\n", tmp.c_str(), min.c_str(), max.c_str());

    std::string href = element_node.GetNsMap()[Split(type).NS];
    if (href.empty()) {
      return;
    }

    XmlNode n = FindComplexType(href, type);
    if (n) {
      printf("%s{\n", level.c_str());
      ShowComplexType(n, level + "  ");
      printf("%s}\n", level.c_str());
    }
  }

  return;
}

void Xsd::ShowComplexType(const XmlNode& complex, const std::string& level) {
  XmlNode n(complex, "sequence");
  if (n) {
    XmlNode n2(n, "element");
    while (n2) {
      ShowElement(n2, level);
      ++n2;
    }
  }
}

XmlNode Xsd::GetSchema(const std::string& target_ns) const {
  XmlNode root(doc_);

  if (root.GetNodeName() == "schema" &&
      root.GetProperty("targetNamespace") == target_ns) {
    return root;
  }

  XmlNode n0(doc_, "types");
  if (n0) {
    XmlNode n(n0, "schema");
    while (n) {
      if (n.GetProperty("targetNamespace") == target_ns) {
        return n;
      }
      ++n;
    }
  }

  return XmlNode();
}

XmlNode Xsd::GetElement(const std::string& name) const {
  std::string ns_prefix = Split(name).NS;
  XmlNode root(doc_);
  std::string ns_href = root.GetNsMap()[ns_prefix];
  XmlNode n = GetSchema(ns_href);
  if (n) {
    return GetElement(n, name);
  }

  return XmlNode();
}

XmlNode Xsd::FindComplexType(const std::string& ns_href,
                             const std::string& type) const {
  std::map<std::string, std::map<std::string, XmlNode> >::const_iterator it;
  it = complexType_Map_.find(ns_href);
  if (it == complexType_Map_.end()) {
    return XmlNode();
  }

  const std::map<std::string, XmlNode>& ref = it->second;
  std::map<std::string, XmlNode>::const_iterator it2 =
      ref.find(Split(type).Type);
  if (it2 != ref.end()) {
    return it2->second;
  }

  return XmlNode();
}

XmlNode Xsd::FindSimpleType(const std::string& ns_href,
                            const std::string& type) const {
  std::map<std::string, std::map<std::string, XmlNode> >::const_iterator it;
  it = simpleType_Map_.find(ns_href);
  if (it == simpleType_Map_.end()) {
    return XmlNode();
  }

  const std::map<std::string, XmlNode>& ref = it->second;
  std::map<std::string, XmlNode>::const_iterator it2 =
      ref.find(Split(type).Type);
  if (it2 != ref.end()) {
    return it2->second;
  }

  return XmlNode();
}

XmlNode Xsd::GetElement(const XmlNode& node, const std::string& name) const {
  XmlNode n(node, "import");
  while (n) {
    std::string ns = n.GetProperty("namespace");

    XmlNode ns_node = GetSchema(ns);
    if (!ns_node.empty()) {
      return GetElement(ns_node, name);
    }
    ++n;
  }

  XmlNode n2(node, "element");
  while (n2) {
    if (n2.GetProperty("name") == Split(name).Type) {
      return n2;
    }
    ++n2;
  }

  return XmlNode();
}

std::list<std::pair<std::string, XmlNode> > Xsd::GetMessageParameters(
    const std::string& msgname, std::string& path) const {
  std::list<std::pair<std::string, XmlNode> > vec;
  XmlNode message = GetMessage(msgname);
  XmlNode n(message, "part");
  std::string element;
  path = "";

  while (n) {
    std::string name = n.GetProperty("name");
    if (n.PropertyExists("type")) {
      std::string type;
      vec.push_back(std::pair<std::string, XmlNode>(name, n));
    }

    if (n.PropertyExists("element")) {
      element = n.GetProperty("element");
    }
    ++n;
  }

  XmlNode el(doc_);

  // check
  if (vec.size() == 1) {
    std::list<std::pair<std::string, XmlNode> >::iterator it = vec.begin();
    XmlNode& ref = it->second;
    std::string type = ref.GetProperty("type");
    std::string href = ref.GetNsMap()[Split(type).NS];

    XmlNode n = FindComplexType(href, type);
    vec.erase(it);
    el = n;
  }

  if (!element.empty()) {  // go deeper
    el = GetElement(element);
  }

  if (el) {
    while (el) {
      printf("/// %s  <%s>\n", el.GetProperty("name").c_str(),
             el.GetNodeName().c_str());
      fflush(stdout);

      if (el.PropertyExists("name")) {
        if (!path.empty()) path += "/";
        path += el.GetProperty("name");
      }
      if (el.GetNodeName() == "element") {
        if (el.PropertyExists("ref")) {
          std::string ref = el.GetProperty("ref");
          el = GetElement(ref);
          continue;
        }

        if (el.PropertyExists("type")) {
          std::string type = el.GetProperty("type");
          std::string href = el.GetNsMap()[Split(type).NS];
          el = FindComplexType(href, type);
          continue;
        }
        XmlNode n(el, "complexType");
        if (n) {
          ComplexType cc(*this, n);
          std::string ref;
          if (cc.NumberOfMembers() == 1 && cc.GetRef(ref)) {
            el = GetElement(ref);
            continue;
          }
          vec = cc.GetMembers();
        }
        break;
      } else if (el.GetNodeName() == "complexType") {
        ComplexType cc(*this, el);
        std::string ref;
        if (cc.NumberOfMembers() == 1 && cc.GetRef(ref)) {
          el = GetElement(ref);
          continue;
        }
        vec = cc.GetMembers();
        break;
      }
    }
  }

  return vec;
}

void Xsd::ParseElements(XmlDocument& doc, const std::string& targetNs) {
  XmlNode root(doc);
  if (root.GetNodeName() == "schema") {
    XmlNode n(root, "import");  // "include" ?
    while (n) {
      std::string ns =
          n.PropertyExists("namespace") ? n.GetProperty("namespace") : "";
      std::string loc = n.PropertyExists("schemaLocation")
                            ? n.GetProperty("schemaLocation")
                            : "";

      std::cout << "Import " << ns << " schemaLocation " << loc << std::endl;
      Utility::Path current = Utility::CurrentDirectory();
      Utility::Path path(loc);
      if (!path.GetPath().empty()) {
        Utility::ChangeDirectory(path.GetPath());
      }

      XmlDocument* doc = new XmlDocument();
      doc->Load(path.GetFilename());
      import_.push_back(doc);
      ParseElements(*doc, ns);
      if (!path.GetPath().empty()) {
        Utility::ChangeDirectory(current);
      }
    }
    ++n;
  }

  std::string ns_href = root.GetProperty("targetNamespace");
  if (ns_href == targetNs || targetNs.empty()) {
    std::string ns_prefix = root.GetNsMapRe()[ns_href];
    ParseTypes(root, ns_href);
    ParseElements(root, ns_href);
  }

  XmlNode n(doc, "types");
  if (n) {
    XmlNode n2(n, "schema");
    while (n2) {
      XmlNode n3(n2, "import");
      while (n3) {
        std::string ns =
            n3.PropertyExists("namespace") ? n3.GetProperty("namespace") : "";
        std::string loc = n3.PropertyExists("schemaLocation")
                              ? n3.GetProperty("schemaLocation")
                              : "";
        std::cout << "Import " << ns << " schemaLocation " << loc << std::endl;
        Utility::Path current = Utility::CurrentDirectory();
        Utility::Path path(loc);
        if (!path.GetPath().empty()) {
          Utility::ChangeDirectory(path.GetPath());
        }
        XmlDocument* doc = new XmlDocument();
        doc->Load(path.GetFilename());
        import_.push_back(doc);
        ParseElements(*doc, ns);
        if (!path.GetPath().empty()) {
          Utility::ChangeDirectory(current);
        }
        ++n3;
      }

      std::string ns_href = n2.GetProperty("targetNamespace");
      if (ns_href == targetNs || targetNs.empty()) {
        std::string ns_prefix = root.GetNsMapRe()[ns_href];
        ParseTypes(n2, ns_href);
        ParseElements(n2, ns_href);
      }
      ++n2;
    }
  }
}

void Xsd::ParseTypes(const XmlNode& node, const std::string& ns_href) {
  XmlNode n(node, "complexType");
  while (n) {
    std::string name;
    name = n.GetProperty("name");
    complexType_[ns_href].push_back(name);
    complexType_Map_[ns_href][name] = ParseComplexType(n, ns_href, name);
    ++n;
  }

  XmlNode n2(node, "simpleType");
  while (n2) {
    std::string name;
    name = n2.GetProperty("name");
    simpleType_[ns_href].push_back(name);
    simpleType_Map_[ns_href][name] = n2;
    ++n2;
  }
}

void Xsd::ParseElements(const XmlNode& node, const std::string& ns_href,
                        const std::string& parent_class) {
  XmlNode n(node, "element");
  while (n) {
    std::string name;
    if (n.PropertyExists("name")) {
      name = n.GetProperty("name");
    }
    std::string type;
    if (n.PropertyExists("type")) {
      type = n.GetProperty("type");
    } else {
      // type = name + "_t";
      type = name;
      // test complex- / simpleType
      XmlNode complex(n, "complexType");
      if (complex) {
        complexType_[ns_href].push_back(type);
        complexType_Map_[ns_href][type] =
            ParseComplexType(complex, ns_href, type);
        parent_[ns_href][type] = parent_class;
      }

      XmlNode simple(n, "simpleType");
      if (simple) {
        simpleType_[ns_href].push_back(type);
        simpleType_Map_[ns_href][type] = simple;
        parent_[ns_href][type] = parent_class;
      }
    }
    ++n;
  }
}

XmlNode Xsd::ParseComplexType(const XmlNode& node, const std::string& ns_href,
                              const std::string& name) {
  XmlNode start(node);
  XmlNode complexContent(node, "complexContent");
  std::string extends;
  if (complexContent) {
    XmlNode extension(complexContent, "extension");
    if (extension) {
      if (extension.PropertyExists("base")) {
        extends = extension.GetProperty("base");
        start = extension;
      }
    }
  }

  ComplexType t(*this, start, name, ns_href);
  if (t.IsSequence() || t.IsAll()) {
    XmlNode n(start, t.IsSequence() ? "sequence" : "all");
    if (n) {
      ParseElements(n, ns_href, name);
    }
  }

  XmlNode n(start, "attribute");
  while (n) {
    std::string aname = n.GetProperty("name");
    if (n.PropertyExists("type")) {
      // do nothing
    } else {
      XmlNode n2(n, "simpleType");
      if (n2) {
        // std::string type = aname + "_t";
        std::string type = aname;
        simpleType_[ns_href].push_back(type);
        simpleType_Map_[ns_href][type] = n2;
        parent_[ns_href][type] = name;
      }
    }
    ++n;
  }

  return start;
}

bool Xsd::CreateInterface() {
  XmlNode root(doc_);

  std::string ns = app_.Namespace();
  std::string cn = app_.Classname();

  std::string h_fileename = cn + ".h";
  std::string cc_fileename = cn + ".cc";

  FILE* file = app_.myfopen(app_.ToDir() + "/" + h_fileename, "wt");
  if (!file) {
    return false;
  }

  AddLicense(file);
  fprintf(file, "\n#ifndef XSD2CC_%s_H_INCLUDED_\n",
          Utility::ToUpper(cn).c_str());
  fprintf(file, "#define XSD2CC_%s_H_INCLUDED_\n",
          Utility::ToUpper(cn).c_str());
  fprintf(file, "\n\n");
  fprintf(file, "#include <string>\n");
  fprintf(file, "#include <vector>\n");
  fprintf(file, "#include <list>\n");
  fprintf(file, "#include <xsd2cc/xsd2cc.h>\n\n\n");
  fprintf(file, "using namespace xsd2cc;\n\n");
  fflush(file);
  fprintf(file, "\n");
  {
    NSFile mns(file, ns);  // global namespace
    mns.Begin();

    // simpleType
    std::map<std::string, std::list<std::string> >::iterator it;
    for (it = simpleType_.begin(); it != simpleType_.end(); it++) {
      std::string ns_href = it->first;
      std::list<std::string>& ref = it->second;
      if (!ref.empty()) {
        std::string href = ns_href;
        NSFile mns(file, GetNamespace(ns_href), href);
        for (std::list<std::string>::iterator it = ref.begin(); it != ref.end();
             it++) {
          std::string name = *it;
          std::string parent = parent_[ns_href][name];
          if (parent.empty()) {
            SimpleType t(*this, simpleType_Map_[ns_href][name], name, ns_href);
            mns.Begin();
            t.CreateInterface(file);
          }
        }
      }
      fprintf(file, "\n");
    }

    // create class definitions
    bool repeat = false;
    bool gen = false;
    do {
      repeat = false;
      gen = false;
      std::map<std::string, std::list<std::string> >::iterator it;
      for (it = complexType_.begin(); it != complexType_.end(); it++) {
        std::string ns_href = it->first;
        std::list<std::string>& ref = it->second;
        if (!ref.empty()) {
          std::string href = ns_href;
          NSFile mns(file, GetNamespace(ns_href), href);
          for (std::list<std::string>::iterator it = ref.begin();
               it != ref.end(); it++) {
            std::string name = *it;
            std::string parent = parent_[ns_href][name];
            fflush(file);
            if (parent.empty()) {
              ComplexType t(*this, complexType_Map_[ns_href][name], name,
                            ns_href);
              if (!t.IsGenerated()) {
                if (t.OkToGenerate()) {
                  std::string doc = t.Documentation();
                  mns.Begin();
                  fprintf(file, "\n");
                  if (!doc.empty()) {
                    fprintf(file, "\t/** %s */\n", doc.c_str());
                  }
                  t.CreateInterface(file);
                  t.SetGenerated();
                  gen = true;
                } else {
                  repeat = true;
                }
              }
            }
          }
        }
      }
    } while (repeat && gen);

    fprintf(file, "\n\n");
  }

  fprintf(file, "#endif // XSD2CC_%s_H_INCLUDED_\n\n",
          Utility::ToUpper(cn).c_str());
  fclose(file);

  return true;
}

bool Xsd::CreateImplementation() {
  XmlNode root(doc_);

  std::string ns = app_.Namespace();
  std::string cn = app_.Classname();

  std::string h_fileename = cn + ".h";
  std::string cc_fileename = cn + ".cc";

  FILE* file = app_.myfopen(app_.ToDir() + "/" + cc_fileename, "wt");
  if (!file) {
    return false;
  }

  AddLicense(file);
  fprintf(file, "\n#include <string.h>\n");
  fprintf(file, "#include \"%s\"\n\n", h_fileename.c_str());
  fprintf(file, "\n");

  NSFile mns(file, ns);  // global namespace
  mns.Begin();

  // create class implementations
  std::map<std::string, std::list<std::string> >::iterator it;
  for (it = complexType_.begin(); it != complexType_.end(); it++) {
    std::string ns_href = it->first;
    std::list<std::string>& ref = it->second;
    if (!ref.empty()) {
      std::string href = ns_href;
      NSFile mns(file, GetNamespace(ns_href), href);
      mns.Begin();
      std::list<std::string>::iterator it;
      for (it = ref.begin(); it != ref.end(); it++) {
        std::string name = *it;
        ComplexType t(*this, complexType_Map_[ns_href][name], name, ns_href);
        t.CreateImplementation(file);
      }
    }
  }
  fclose(file);

  return true;
}

const std::string Xsd::GetParent(const std::string& ns_href,
                                 const std::string& classname) const {
  if (ns_href.empty()) {
    printf("GetParent( %s, %s )\n", ns_href.c_str(), classname.c_str());
  }
  fflush(stdout);

  std::map<std::string, std::map<std::string, std::string> >::const_iterator
      it1;
  it1 = parent_.find(ns_href);
  if (it1 == parent_.end()) {
    return "";
  }

  std::map<std::string, std::string>::const_iterator it;
  if ((it = it1->second.find(classname)) != it1->second.end()) {
    return it->second;
  }

  return "";
}

const std::list<std::string>& Xsd::ComplexTypes(
    const std::string& ns_href) const {
  std::map<std::string, std::list<std::string> >::const_iterator it;
  it = complexType_.find(ns_href);
  if (it != complexType_.end()) return it->second;

  return empty_;
}

const std::list<std::string>& Xsd::SimpleTypes(
    const std::string& ns_href) const {
  std::map<std::string, std::list<std::string> >::const_iterator it;
  it = simpleType_.find(ns_href);
  if (it != simpleType_.end()) return it->second;

  return empty_;
}

const std::string Xsd::NamespaceFromHref(const std::string& href) const {
  std::string prefix = app_.Href2Prefix(href);
  if (!prefix.empty()) {
    return prefix;
  }

  size_t pos = href.find("://");
  if (pos != std::string::npos) {
    std::string hreft = href.substr(pos + 3);
    pos = hreft.find("/");
    std::string host = hreft;
    std::string path;
    if (pos != std::string::npos) {
      host = hreft.substr(0, pos);
      path = hreft.substr(pos + 1);
    }

    Parse pa(host, ".:");
    std::string tmp = pa.getword();
    std::string ns;
    std::string ns2;
    bool first = true;
    while (!tmp.empty()) {
      if (first) {
        first = false;
      } else {
        ns = ns2;
        ns2 = tmp + "_" + ns2;
      }
      tmp = pa.getword();
    }

    for (size_t i = 0; i < path.size(); i++) {
      ns2 += (isalnum(path[i]) ? path[i] : '_');
    }
    return ns2;
  }

  std::string ns;
  for (size_t i = 0; i < href.size(); i++) {
    ns += (!isalnum(href[i]) ? '_' : href[i]);
  }

  return ns;
}

const std::string Xsd::GetNamespace(const std::string& ns_href) const {
  return NamespaceFromHref(ns_href);
}

void Xsd::AddLicense(FILE* file) {
#if 1
  fprintf(
      file,
      "\n/*\n"
      "Copyright (C) 2013-2014  GuiQuan Zhang (guiqzhang@gmail.com)\n"
      "Copyright (C) 2008-2009  Anders Hedstrom (grymse@alhem.net)\n"
      "\n"
      "This program is made available under the terms of the GNU GPL.\n"
      "\n"
      "If you would like to use this program in a closed-source application,\n"
      "a separate license agreement is available. For information about \n"
      "the closed-source license agreement for this program, please\n"
      "visit http://www.alhem.net/Sockets/license.html and/or\n"
      "email license@alhem.net.\n"
      "\n"
      "This program is free software; you can redistribute it and/or\n"
      "modify it under the terms of the GNU General Public License\n"
      "as published by the Free Software Foundation; either version 2\n"
      "of the License, or (at your option) any later version.\n"
      "\n"
      "This program is distributed in the hope that it will be useful,\n"
      "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
      "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
      "GNU General Public License for more details.\n"
      "\n"
      "You should have received a copy of the GNU General Public License\n"
      "along with this program; if not, write to the Free Software\n"
      "Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, "
      "USA.\n"
      "*/\n");
#endif

  fprintf(file, "\n");
}

}  // namespace xsd2cc
