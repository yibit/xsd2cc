
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


#ifndef XSD2CC_RESOURCE_TEST_H_INCLUDED_
#define XSD2CC_RESOURCE_TEST_H_INCLUDED_


#include <string>
#include <vector>
#include <list>
#include <xsd2cc/xsd2cc.h>


using namespace xsd2cc;


/** resource */
namespace resource {


  class Display_nameType {
  public:
    Display_nameType();
    virtual ~Display_nameType() {}

    void clear();

    bool FromXml(const XmlNode& it);

    bool FromXml(const std::string& it);

    bool FromXml(const char* it);

    const std::string ToString(bool needXmlHead=false, const std::string& name="", int partid=3) const;

    bool IsSet() const;

    void SetNsPrefix(std::string& prefix);
    void SetNsDeclaration(std::string& decl);
    void SetNodeState(bool state);

    const std::string& GetLang() const;
    void SetLang(const std::string& lang);

    bool LangIsSet() const { return attr_lang_is_set_; }

    const std::string& GetNsPrefix() const { return ns_prefix_; }
    const std::string& GetNsDeclaration() const { return ns_declaration_; }
    bool IsSetted() const { return is_setted_; }

  private:
    std::string attr_lang_;
    bool attr_lang_is_set_;
    std::string ns_prefix_;
    std::string ns_declaration_;
    bool is_setted_;
  };

} // namespace resource

/** resource */
namespace resource {


  class EntryType {
  public:
    EntryType();
    virtual ~EntryType() {}

    void clear();

    bool FromXml(const XmlNode& it);

    bool FromXml(const std::string& it);

    bool FromXml(const char* it);

    const std::string ToString(bool needXmlHead=false, const std::string& name="", int partid=3) const;

    bool IsSet() const;

    void SetNsPrefix(std::string& prefix);
    void SetNsDeclaration(std::string& decl);
    void SetNodeState(bool state);

    const resource::Display_nameType& GetDisplay_name() const;
    resource::Display_nameType& GetDisplay_name();
    void SetDisplay_name(const resource::Display_nameType& display_name);

    const std::string& GetUri() const;
    void SetUri(const std::string& uri);
    const std::string& GetCopyControl() const;
    void SetCopyControl(const std::string& copyControl);
    bool GetAnonymize() const;
    void SetAnonymize(bool anonymize);
    long GetCount() const;
    void SetCount(long count);

    bool Display_nameIsSet() const;

    bool UriIsSet() const { return attr_uri_is_set_; }
    bool CopyControlIsSet() const { return attr_copyControl_is_set_; }
    bool AnonymizeIsSet() const { return attr_anonymize_is_set_; }
    bool CountIsSet() const { return attr_count_is_set_; }

    const std::string& GetNsPrefix() const { return ns_prefix_; }
    const std::string& GetNsDeclaration() const { return ns_declaration_; }
    bool IsSetted() const { return is_setted_; }

  private:
    resource::Display_nameType elem_display_name_;
    std::string attr_uri_;
    std::string attr_copyControl_;
    bool attr_anonymize_;
    long attr_count_;
    bool attr_uri_is_set_;
    bool attr_copyControl_is_set_;
    bool attr_anonymize_is_set_;
    bool attr_count_is_set_;
    std::string ns_prefix_;
    std::string ns_declaration_;
    bool is_setted_;
  };


  class Entry_refType {
  public:
    Entry_refType();
    virtual ~Entry_refType() {}

    void clear();

    bool FromXml(const XmlNode& it);

    bool FromXml(const std::string& it);

    bool FromXml(const char* it);

    const std::string ToString(bool needXmlHead=false, const std::string& name="", int partid=3) const;

    bool IsSet() const;

    void SetNsPrefix(std::string& prefix);
    void SetNsDeclaration(std::string& decl);
    void SetNodeState(bool state);

    const resource::Display_nameType& GetDisplay_name() const;
    resource::Display_nameType& GetDisplay_name();
    void SetDisplay_name(const resource::Display_nameType& display_name);

    const std::string& GetRef() const;
    void SetRef(const std::string& ref);

    bool Display_nameIsSet() const;

    bool RefIsSet() const { return attr_ref_is_set_; }

    const std::string& GetNsPrefix() const { return ns_prefix_; }
    const std::string& GetNsDeclaration() const { return ns_declaration_; }
    bool IsSetted() const { return is_setted_; }

  private:
    resource::Display_nameType elem_display_name_;
    std::string attr_ref_;
    bool attr_ref_is_set_;
    std::string ns_prefix_;
    std::string ns_declaration_;
    bool is_setted_;
  };


  class ExternalType {
  public:
    ExternalType();
    virtual ~ExternalType() {}

    void clear();

    bool FromXml(const XmlNode& it);

    bool FromXml(const std::string& it);

    bool FromXml(const char* it);

    const std::string ToString(bool needXmlHead=false, const std::string& name="", int partid=3) const;

    bool IsSet() const;

    void SetNsPrefix(std::string& prefix);
    void SetNsDeclaration(std::string& decl);
    void SetNodeState(bool state);

    const resource::Display_nameType& GetDisplay_name() const;
    resource::Display_nameType& GetDisplay_name();
    void SetDisplay_name(const resource::Display_nameType& display_name);

    const std::string& GetAnchor() const;
    void SetAnchor(const std::string& anchor);

    bool Display_nameIsSet() const;

    bool AnchorIsSet() const { return attr_anchor_is_set_; }

    const std::string& GetNsPrefix() const { return ns_prefix_; }
    const std::string& GetNsDeclaration() const { return ns_declaration_; }
    bool IsSetted() const { return is_setted_; }

  private:
    resource::Display_nameType elem_display_name_;
    std::string attr_anchor_;
    bool attr_anchor_is_set_;
    std::string ns_prefix_;
    std::string ns_declaration_;
    bool is_setted_;
  };

} // namespace resource

/** resource */
namespace resource {


  class ListType {
  public:
    ListType();
    virtual ~ListType() {}

    void clear();

    bool FromXml(const XmlNode& it);

    bool FromXml(const std::string& it);

    bool FromXml(const char* it);

    const std::string ToString(bool needXmlHead=false, const std::string& name="", int partid=3) const;

    bool IsSet() const;

    void SetNsPrefix(std::string& prefix);
    void SetNsDeclaration(std::string& decl);
    void SetNodeState(bool state);

    const resource::Display_nameType& GetDisplay_name() const;
    resource::Display_nameType& GetDisplay_name();
    void SetDisplay_name(const resource::Display_nameType& display_name);

    const std::vector<resource::ExternalType>& GetExternal() const;
    std::vector<resource::ExternalType>& GetExternal();
    void SetExternal(const std::vector<resource::ExternalType>& external);

    const std::vector<resource::EntryType>& GetEntry() const;
    std::vector<resource::EntryType>& GetEntry();
    void SetEntry(const std::vector<resource::EntryType>& entry);

    const std::vector<resource::Entry_refType>& GetEntry_ref() const;
    std::vector<resource::Entry_refType>& GetEntry_ref();
    void SetEntry_ref(const std::vector<resource::Entry_refType>& entry_ref);

    const std::string& GetName() const;
    void SetName(const std::string& name);

    bool Display_nameIsSet() const;
    bool ExternalIsSet() const;
    bool EntryIsSet() const;
    bool Entry_refIsSet() const;

    bool NameIsSet() const { return attr_name_is_set_; }

    const std::string& GetNsPrefix() const { return ns_prefix_; }
    const std::string& GetNsDeclaration() const { return ns_declaration_; }
    bool IsSetted() const { return is_setted_; }

  private:
    resource::Display_nameType elem_display_name_;
    std::vector<resource::ExternalType> elem_external_;
    std::vector<resource::EntryType> elem_entry_;
    std::vector<resource::Entry_refType> elem_entry_ref_;
    std::string attr_name_;
    bool attr_name_is_set_;
    std::string ns_prefix_;
    std::string ns_declaration_;
    bool is_setted_;
  };


  class Resource_lists {
  public:
    Resource_lists();
    virtual ~Resource_lists() {}

    void clear();

    bool FromXml(const XmlNode& it);

    bool FromXml(const std::string& it);

    bool FromXml(const char* it);

    const std::string ToString(bool needXmlHead=false, const std::string& name="", int partid=3) const;

    bool IsSet() const;

    void SetNsPrefix(std::string& prefix);
    void SetNsDeclaration(std::string& decl);
    void SetNodeState(bool state);

    const resource::ListType& GetList() const;
    resource::ListType& GetList();
    void SetList(const resource::ListType& list);

    bool ListIsSet() const;

    const std::string& GetNsPrefix() const { return ns_prefix_; }
    const std::string& GetNsDeclaration() const { return ns_declaration_; }
    bool IsSetted() const { return is_setted_; }

  private:
    resource::ListType elem_list_;
    std::string ns_prefix_;
    std::string ns_declaration_;
    bool is_setted_;
  };

} // namespace resource



#endif // XSD2CC_RESOURCE_TEST_H_INCLUDED_

