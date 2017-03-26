
/*
Copyright (C) 2013-2014  GuiQuan Zhang (guiqzhang@gmail.com)
Copyright (C) 2008-2009  Anders Hedstrom (grymse@alhem.net)

This program is made available under the terms of the GNU GPL.

If you would like to use this program in a closed-source application,
a separate license agreement is available. For information about 
the closed-source license agreement for this program, please
visit http://www.alhem.net/Sockets/license.html and/or
email license@alhem.net.

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
#include "resource_test.h"


/** resource */
namespace resource {

ListType::ListType()
: attr_name_is_set_(false)
, ns_prefix_("")
, ns_declaration_("")
, is_setted_(false)
{
}


void ListType::clear()
{
  elem_display_name_.clear();
  while (!elem_external_.empty())
    elem_external_.erase( elem_external_.begin() );
  while (!elem_entry_.empty())
    elem_entry_.erase( elem_entry_.begin() );
  while (!elem_entry_ref_.empty())
    elem_entry_ref_.erase( elem_entry_ref_.begin() );
  attr_name_.clear();
  attr_name_is_set_ = false;
  ns_prefix_.clear();
  ns_declaration_.clear();
  is_setted_ = false;
}


bool ListType::FromXml(const XmlNode& it)
{
  SetNodeState(true);
  {
    XmlNode n(it, "display-name");
    if (n) {
      bool ret = elem_display_name_.FromXml( n );
      if (!ret) return false;
    }
  }
  {
    XmlNode n(it, "external");
    while (n)
    {
      resource::ExternalType tmp;
      if (!tmp.FromXml(n)) return false;
      elem_external_.push_back( tmp );
      ++n;
    }
  }
  {
    XmlNode n(it, "entry");
    while (n)
    {
      resource::EntryType tmp;
      if (!tmp.FromXml(n)) return false;
      elem_entry_.push_back( tmp );
      ++n;
    }
  }
  {
    XmlNode n(it, "entry-ref");
    while (n)
    {
      resource::Entry_refType tmp;
      if (!tmp.FromXml(n)) return false;
      elem_entry_ref_.push_back( tmp );
      ++n;
    }
  }
  if (it.PropertyExists("name"))
    SetName(it.GetProperty("name"));
  return true;
}


bool ListType::FromXml(const std::string& it)
{
  return FromXml(it.c_str());
}


bool ListType::FromXml(const char* it)
{
  XmlDocument doc;
  bool ret = doc.Load(it, (int)strlen(it));
  if (!ret) return false;
  XmlNode n(doc);
  return FromXml(n);
}


const std::string ListType::ToString(bool needXmlHead, const std::string& name, int partid) const
{
  std::string xmlHead = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
  std::string r;

  if (!IsSet() && IsSetted() && !name.empty())
  {
    if (needXmlHead)
      r += xmlHead;
    r += "<" + (ns_prefix_.empty() ? "" : ns_prefix_ + ":") + name + (ns_declaration_.empty()?"": " " + ns_declaration_) + "/>";

    return r;
  }

  if (partid == 1 || partid > 2)
  {
    if (needXmlHead)
      r += xmlHead;
    if (!name.empty())
      r += "<" + (ns_prefix_.empty() ? "" : ns_prefix_ + ":") + name;
    if (NameIsSet())
      r += ((GetName().empty())? "" : " name=\"" + GetName() + "\"");
    r += (ns_declaration_.empty()? "": " " + ns_declaration_) + ">";
    if (Display_nameIsSet())
      r += elem_display_name_.ToString(false, "display-name");
    for (std::vector<resource::ExternalType>::const_iterator it = elem_external_.begin(); it != elem_external_.end(); it++)
    {
      const resource::ExternalType& ref = *it;
      r += ref.ToString(false, "external");
    }
    for (std::vector<resource::EntryType>::const_iterator it = elem_entry_.begin(); it != elem_entry_.end(); it++)
    {
      const resource::EntryType& ref = *it;
      r += ref.ToString(false, "entry");
    }
    for (std::vector<resource::Entry_refType>::const_iterator it = elem_entry_ref_.begin(); it != elem_entry_ref_.end(); it++)
    {
      const resource::Entry_refType& ref = *it;
      r += ref.ToString(false, "entry-ref");
    }
  }

  if (partid >= 2)
  {
    if (!name.empty())
      r += "</" + (ns_prefix_.empty() ? "" : ns_prefix_ + ":") + name + ">";
  }

  return r;
}


bool ListType::IsSet() const
{
  return Display_nameIsSet() || ExternalIsSet() || EntryIsSet() || Entry_refIsSet() || NameIsSet();
}


void ListType::SetNsPrefix(std::string& prefix) 
{
  ns_prefix_ = prefix; 
}


void ListType::SetNsDeclaration(std::string& decl) 
{
  ns_declaration_ = decl; 
}


void ListType::SetNodeState(bool state) 
{
  is_setted_ = state; 
}


const resource::Display_nameType& ListType::GetDisplay_name() const
{
  return elem_display_name_;
}


resource::Display_nameType& ListType::GetDisplay_name()
{
  return elem_display_name_;
}


void ListType::SetDisplay_name(const resource::Display_nameType& display_name)
{
  elem_display_name_ = display_name;
}


const std::vector<resource::ExternalType>& ListType::GetExternal() const
{
  return elem_external_;
}


std::vector<resource::ExternalType>& ListType::GetExternal()
{
  return elem_external_;
}


void ListType::SetExternal(const std::vector<resource::ExternalType>& external)
{
  elem_external_ = external;
}


const std::vector<resource::EntryType>& ListType::GetEntry() const
{
  return elem_entry_;
}


std::vector<resource::EntryType>& ListType::GetEntry()
{
  return elem_entry_;
}


void ListType::SetEntry(const std::vector<resource::EntryType>& entry)
{
  elem_entry_ = entry;
}


const std::vector<resource::Entry_refType>& ListType::GetEntry_ref() const
{
  return elem_entry_ref_;
}


std::vector<resource::Entry_refType>& ListType::GetEntry_ref()
{
  return elem_entry_ref_;
}


void ListType::SetEntry_ref(const std::vector<resource::Entry_refType>& entry_ref)
{
  elem_entry_ref_ = entry_ref;
}


bool ListType::Display_nameIsSet() const
{
  return elem_display_name_.IsSet();
}


bool ListType::ExternalIsSet() const
{
  return !elem_external_.empty();
}


bool ListType::EntryIsSet() const
{
  return !elem_entry_.empty();
}


bool ListType::Entry_refIsSet() const
{
  return !elem_entry_ref_.empty();
}


const std::string& ListType::GetName() const
{
  return attr_name_;
}


void ListType::SetName(const std::string& name)
{
  attr_name_ = name;
  attr_name_is_set_ = true;
}


EntryType::EntryType()
: attr_anonymize_(false)
, attr_count_(1)
, attr_uri_is_set_(false)
, attr_copyControl_is_set_(false)
, attr_anonymize_is_set_(true)
, attr_count_is_set_(true)
, ns_prefix_("")
, ns_declaration_("")
, is_setted_(false)
{
}


void EntryType::clear()
{
  elem_display_name_.clear();
  attr_uri_.clear();
  attr_copyControl_.clear();
  attr_anonymize_ = false;
  attr_count_ = 1;
  attr_uri_is_set_ = false;
  attr_copyControl_is_set_ = false;
  attr_anonymize_is_set_ = true;
  attr_count_is_set_ = true;
  ns_prefix_.clear();
  ns_declaration_.clear();
  is_setted_ = false;
}


bool EntryType::FromXml(const XmlNode& it)
{
  SetNodeState(true);
  {
    XmlNode n(it, "display-name");
    if (n) {
      bool ret = elem_display_name_.FromXml( n );
      if (!ret) return false;
    }
  }
  SetUri(it.GetProperty("uri"));
  if (it.PropertyExists("copyControl"))
    SetCopyControl(it.GetProperty("copyControl"));
  if (it.PropertyExists("anonymize"))
  {
    std::string tmp = it.GetProperty("anonymize");
    SetAnonymize( tmp == "true" || atoi(tmp.c_str()) != 0 || tmp == "yes" );
  }
  if (it.PropertyExists("count"))
    SetCount( atoi(it.GetProperty("count").c_str()) );
  return true;
}


bool EntryType::FromXml(const std::string& it)
{
  return FromXml(it.c_str());
}


bool EntryType::FromXml(const char* it)
{
  XmlDocument doc;
  bool ret = doc.Load(it, (int)strlen(it));
  if (!ret) return false;
  XmlNode n(doc);
  return FromXml(n);
}


const std::string EntryType::ToString(bool needXmlHead, const std::string& name, int partid) const
{
  std::string xmlHead = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
  std::string r;

  if (!IsSet() && IsSetted() && !name.empty())
  {
    if (needXmlHead)
      r += xmlHead;
    r += "<" + (ns_prefix_.empty() ? "" : ns_prefix_ + ":") + name + (ns_declaration_.empty()?"": " " + ns_declaration_) + "/>";

    return r;
  }

  if (partid == 1 || partid > 2)
  {
    if (needXmlHead)
      r += xmlHead;
    if (!name.empty())
      r += "<" + (ns_prefix_.empty() ? "" : ns_prefix_ + ":") + name;
    r += ((GetUri().empty())? "" : " uri=\"" + GetUri() + "\"");
    if (CopyControlIsSet())
      r += ((GetCopyControl().empty())? "" : " copyControl=\"" + GetCopyControl() + "\"");
    if (AnonymizeIsSet())
    {
      r = r + " anonymize=\"" + Utility::l2string(GetAnonymize()) + "\"";
    }
    if (CountIsSet())
    {
      r = r + " count=\"" + Utility::l2string(GetCount()) + "\"";
    }
    r += (ns_declaration_.empty()? "": " " + ns_declaration_) + ">";
    if (Display_nameIsSet())
      r += elem_display_name_.ToString(false, "display-name");
  }

  if (partid >= 2)
  {
    if (!name.empty())
      r += "</" + (ns_prefix_.empty() ? "" : ns_prefix_ + ":") + name + ">";
  }

  return r;
}


bool EntryType::IsSet() const
{
  return Display_nameIsSet() || UriIsSet() || CopyControlIsSet() || AnonymizeIsSet() || CountIsSet();
}


void EntryType::SetNsPrefix(std::string& prefix) 
{
  ns_prefix_ = prefix; 
}


void EntryType::SetNsDeclaration(std::string& decl) 
{
  ns_declaration_ = decl; 
}


void EntryType::SetNodeState(bool state) 
{
  is_setted_ = state; 
}


const resource::Display_nameType& EntryType::GetDisplay_name() const
{
  return elem_display_name_;
}


resource::Display_nameType& EntryType::GetDisplay_name()
{
  return elem_display_name_;
}


void EntryType::SetDisplay_name(const resource::Display_nameType& display_name)
{
  elem_display_name_ = display_name;
}


bool EntryType::Display_nameIsSet() const
{
  return elem_display_name_.IsSet();
}


const std::string& EntryType::GetUri() const
{
  return attr_uri_;
}


void EntryType::SetUri(const std::string& uri)
{
  attr_uri_ = uri;
  attr_uri_is_set_ = true;
}


const std::string& EntryType::GetCopyControl() const
{
  return attr_copyControl_;
}


void EntryType::SetCopyControl(const std::string& copyControl)
{
  attr_copyControl_ = copyControl;
  attr_copyControl_is_set_ = true;
}


bool EntryType::GetAnonymize() const
{
  return attr_anonymize_;
}


void EntryType::SetAnonymize(bool anonymize)
{
  attr_anonymize_ = anonymize;
  attr_anonymize_is_set_ = true;
}


long EntryType::GetCount() const
{
  return attr_count_;
}


void EntryType::SetCount(long count)
{
  attr_count_ = count;
  attr_count_is_set_ = true;
}


Entry_refType::Entry_refType()
: attr_ref_is_set_(false)
, ns_prefix_("")
, ns_declaration_("")
, is_setted_(false)
{
}


void Entry_refType::clear()
{
  elem_display_name_.clear();
  attr_ref_.clear();
  attr_ref_is_set_ = false;
  ns_prefix_.clear();
  ns_declaration_.clear();
  is_setted_ = false;
}


bool Entry_refType::FromXml(const XmlNode& it)
{
  SetNodeState(true);
  {
    XmlNode n(it, "display-name");
    if (n) {
      bool ret = elem_display_name_.FromXml( n );
      if (!ret) return false;
    }
  }
  SetRef(it.GetProperty("ref"));
  return true;
}


bool Entry_refType::FromXml(const std::string& it)
{
  return FromXml(it.c_str());
}


bool Entry_refType::FromXml(const char* it)
{
  XmlDocument doc;
  bool ret = doc.Load(it, (int)strlen(it));
  if (!ret) return false;
  XmlNode n(doc);
  return FromXml(n);
}


const std::string Entry_refType::ToString(bool needXmlHead, const std::string& name, int partid) const
{
  std::string xmlHead = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
  std::string r;

  if (!IsSet() && IsSetted() && !name.empty())
  {
    if (needXmlHead)
      r += xmlHead;
    r += "<" + (ns_prefix_.empty() ? "" : ns_prefix_ + ":") + name + (ns_declaration_.empty()?"": " " + ns_declaration_) + "/>";

    return r;
  }

  if (partid == 1 || partid > 2)
  {
    if (needXmlHead)
      r += xmlHead;
    if (!name.empty())
      r += "<" + (ns_prefix_.empty() ? "" : ns_prefix_ + ":") + name;
    r += ((GetRef().empty())? "" : " ref=\"" + GetRef() + "\"");
    r += (ns_declaration_.empty()? "": " " + ns_declaration_) + ">";
    if (Display_nameIsSet())
      r += elem_display_name_.ToString(false, "display-name");
  }

  if (partid >= 2)
  {
    if (!name.empty())
      r += "</" + (ns_prefix_.empty() ? "" : ns_prefix_ + ":") + name + ">";
  }

  return r;
}


bool Entry_refType::IsSet() const
{
  return Display_nameIsSet() || RefIsSet();
}


void Entry_refType::SetNsPrefix(std::string& prefix) 
{
  ns_prefix_ = prefix; 
}


void Entry_refType::SetNsDeclaration(std::string& decl) 
{
  ns_declaration_ = decl; 
}


void Entry_refType::SetNodeState(bool state) 
{
  is_setted_ = state; 
}


const resource::Display_nameType& Entry_refType::GetDisplay_name() const
{
  return elem_display_name_;
}


resource::Display_nameType& Entry_refType::GetDisplay_name()
{
  return elem_display_name_;
}


void Entry_refType::SetDisplay_name(const resource::Display_nameType& display_name)
{
  elem_display_name_ = display_name;
}


bool Entry_refType::Display_nameIsSet() const
{
  return elem_display_name_.IsSet();
}


const std::string& Entry_refType::GetRef() const
{
  return attr_ref_;
}


void Entry_refType::SetRef(const std::string& ref)
{
  attr_ref_ = ref;
  attr_ref_is_set_ = true;
}


ExternalType::ExternalType()
: attr_anchor_is_set_(false)
, ns_prefix_("")
, ns_declaration_("")
, is_setted_(false)
{
}


void ExternalType::clear()
{
  elem_display_name_.clear();
  attr_anchor_.clear();
  attr_anchor_is_set_ = false;
  ns_prefix_.clear();
  ns_declaration_.clear();
  is_setted_ = false;
}


bool ExternalType::FromXml(const XmlNode& it)
{
  SetNodeState(true);
  {
    XmlNode n(it, "display-name");
    if (n) {
      bool ret = elem_display_name_.FromXml( n );
      if (!ret) return false;
    }
  }
  if (it.PropertyExists("anchor"))
    SetAnchor(it.GetProperty("anchor"));
  return true;
}


bool ExternalType::FromXml(const std::string& it)
{
  return FromXml(it.c_str());
}


bool ExternalType::FromXml(const char* it)
{
  XmlDocument doc;
  bool ret = doc.Load(it, (int)strlen(it));
  if (!ret) return false;
  XmlNode n(doc);
  return FromXml(n);
}


const std::string ExternalType::ToString(bool needXmlHead, const std::string& name, int partid) const
{
  std::string xmlHead = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
  std::string r;

  if (!IsSet() && IsSetted() && !name.empty())
  {
    if (needXmlHead)
      r += xmlHead;
    r += "<" + (ns_prefix_.empty() ? "" : ns_prefix_ + ":") + name + (ns_declaration_.empty()?"": " " + ns_declaration_) + "/>";

    return r;
  }

  if (partid == 1 || partid > 2)
  {
    if (needXmlHead)
      r += xmlHead;
    if (!name.empty())
      r += "<" + (ns_prefix_.empty() ? "" : ns_prefix_ + ":") + name;
    if (AnchorIsSet())
      r += ((GetAnchor().empty())? "" : " anchor=\"" + GetAnchor() + "\"");
    r += (ns_declaration_.empty()? "": " " + ns_declaration_) + ">";
    if (Display_nameIsSet())
      r += elem_display_name_.ToString(false, "display-name");
  }

  if (partid >= 2)
  {
    if (!name.empty())
      r += "</" + (ns_prefix_.empty() ? "" : ns_prefix_ + ":") + name + ">";
  }

  return r;
}


bool ExternalType::IsSet() const
{
  return Display_nameIsSet() || AnchorIsSet();
}


void ExternalType::SetNsPrefix(std::string& prefix) 
{
  ns_prefix_ = prefix; 
}


void ExternalType::SetNsDeclaration(std::string& decl) 
{
  ns_declaration_ = decl; 
}


void ExternalType::SetNodeState(bool state) 
{
  is_setted_ = state; 
}


const resource::Display_nameType& ExternalType::GetDisplay_name() const
{
  return elem_display_name_;
}


resource::Display_nameType& ExternalType::GetDisplay_name()
{
  return elem_display_name_;
}


void ExternalType::SetDisplay_name(const resource::Display_nameType& display_name)
{
  elem_display_name_ = display_name;
}


bool ExternalType::Display_nameIsSet() const
{
  return elem_display_name_.IsSet();
}


const std::string& ExternalType::GetAnchor() const
{
  return attr_anchor_;
}


void ExternalType::SetAnchor(const std::string& anchor)
{
  attr_anchor_ = anchor;
  attr_anchor_is_set_ = true;
}


Display_nameType::Display_nameType()
: attr_lang_is_set_(false)
, ns_prefix_("")
, ns_declaration_("")
, is_setted_(false)
{
}


void Display_nameType::clear()
{
  attr_lang_.clear();
  attr_lang_is_set_ = false;
  ns_prefix_.clear();
  ns_declaration_.clear();
  is_setted_ = false;
}


bool Display_nameType::FromXml(const XmlNode& it)
{
  SetNodeState(true);
  if (it.PropertyExists("lang"))
    SetLang(it.GetProperty("lang"));
  return true;
}


bool Display_nameType::FromXml(const std::string& it)
{
  return FromXml(it.c_str());
}


bool Display_nameType::FromXml(const char* it)
{
  XmlDocument doc;
  bool ret = doc.Load(it, (int)strlen(it));
  if (!ret) return false;
  XmlNode n(doc);
  return FromXml(n);
}


const std::string Display_nameType::ToString(bool needXmlHead, const std::string& name, int partid) const
{
  std::string xmlHead = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
  std::string r;

  if (!IsSet() && IsSetted() && !name.empty())
  {
    if (needXmlHead)
      r += xmlHead;
    r += "<" + (ns_prefix_.empty() ? "" : ns_prefix_ + ":") + name + (ns_declaration_.empty()?"": " " + ns_declaration_) + "/>";

    return r;
  }

  if (partid == 1 || partid > 2)
  {
    if (needXmlHead)
      r += xmlHead;
    if (!name.empty())
      r += "<" + (ns_prefix_.empty() ? "" : ns_prefix_ + ":") + name;
    if (LangIsSet())
      r += ((GetLang().empty())? "" : " lang=\"" + GetLang() + "\"");
    r += (ns_declaration_.empty()? "": " " + ns_declaration_) + ">";
  }

  if (partid >= 2)
  {
    if (!name.empty())
      r += "</" + (ns_prefix_.empty() ? "" : ns_prefix_ + ":") + name + ">";
  }

  return r;
}


bool Display_nameType::IsSet() const
{
  return LangIsSet();
}


void Display_nameType::SetNsPrefix(std::string& prefix) 
{
  ns_prefix_ = prefix; 
}


void Display_nameType::SetNsDeclaration(std::string& decl) 
{
  ns_declaration_ = decl; 
}


void Display_nameType::SetNodeState(bool state) 
{
  is_setted_ = state; 
}


const std::string& Display_nameType::GetLang() const
{
  return attr_lang_;
}


void Display_nameType::SetLang(const std::string& lang)
{
  attr_lang_ = lang;
  attr_lang_is_set_ = true;
}


Resource_lists::Resource_lists()
: ns_prefix_("")
, ns_declaration_("")
, is_setted_(false)
{
}


void Resource_lists::clear()
{
  elem_list_.clear();
  ns_prefix_.clear();
  ns_declaration_.clear();
  is_setted_ = false;
}


bool Resource_lists::FromXml(const XmlNode& it)
{
  SetNodeState(true);
  if (!it.Exists("list"))
    return false;
  {
    XmlNode n(it, "list");
    if (n) {
      bool ret = elem_list_.FromXml( n );
      if (!ret) return false;
    }
  }
  return true;
}


bool Resource_lists::FromXml(const std::string& it)
{
  return FromXml(it.c_str());
}


bool Resource_lists::FromXml(const char* it)
{
  XmlDocument doc;
  bool ret = doc.Load(it, (int)strlen(it));
  if (!ret) return false;
  XmlNode n(doc);
  return FromXml(n);
}


const std::string Resource_lists::ToString(bool needXmlHead, const std::string& name, int partid) const
{
  std::string xmlHead = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
  std::string r;

  if (!IsSet() && IsSetted() && !name.empty())
  {
    if (needXmlHead)
      r += xmlHead;
    r += "<" + (ns_prefix_.empty() ? "" : ns_prefix_ + ":") + name + (ns_declaration_.empty()?"": " " + ns_declaration_) + "/>";

    return r;
  }

  if (partid == 1 || partid > 2)
  {
    if (needXmlHead)
      r += xmlHead;
    if (!name.empty())
      r += "<" + (ns_prefix_.empty() ? "" : ns_prefix_ + ":") + name + (ns_declaration_.empty()?"": " " + ns_declaration_) + ">";
    if (ListIsSet())
      r += elem_list_.ToString(false, "list");
  }

  if (partid >= 2)
  {
    if (!name.empty())
      r += "</" + (ns_prefix_.empty() ? "" : ns_prefix_ + ":") + name + ">";
  }

  return r;
}


bool Resource_lists::IsSet() const
{
  return ListIsSet();
}


void Resource_lists::SetNsPrefix(std::string& prefix) 
{
  ns_prefix_ = prefix; 
}


void Resource_lists::SetNsDeclaration(std::string& decl) 
{
  ns_declaration_ = decl; 
}


void Resource_lists::SetNodeState(bool state) 
{
  is_setted_ = state; 
}


const resource::ListType& Resource_lists::GetList() const
{
  return elem_list_;
}


resource::ListType& Resource_lists::GetList()
{
  return elem_list_;
}


void Resource_lists::SetList(const resource::ListType& list)
{
  elem_list_ = list;
}


bool Resource_lists::ListIsSet() const
{
  return elem_list_.IsSet();
}


} // namespace resource

