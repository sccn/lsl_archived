#include "../include/lsl_cpp.h"
#include "pugixml/pugixml.hpp"

//
// === implementation of the xml_element class ===
//

using namespace lsl;
using namespace pugi;

// boilerplate code calling the private implementation
xml_element::xml_element(): node_(0) {}
xml_element::xml_element(const xml_node &node): node_(node.internal_object()) {}
xml_element xml_element::first_child() const { return xml_node(node_).first_child(); }
xml_element xml_element::last_child() const { return xml_node(node_).last_child(); }
xml_element xml_element::next_sibling() const { return xml_node(node_).next_sibling(); }
xml_element xml_element::previous_sibling() const { return xml_node(node_).previous_sibling(); }
xml_element xml_element::parent() const { return xml_node(node_).parent(); }
xml_element xml_element::child(const char *name) const { return xml_node(node_).child(name); }
xml_element xml_element::next_sibling(const char *name) const { return xml_node(node_).next_sibling(name); }
xml_element xml_element::previous_sibling(const char *name) const { return xml_node(node_).previous_sibling(name); }
bool xml_element::empty() const { return xml_node(node_).empty(); }
bool xml_element::is_text() const { return xml_node(node_).type() != node_element; }
const char *xml_element::name() const { return xml_node(node_).name(); }
const char *xml_element::value() const  { return xml_node(node_).value(); }
const char_t* xml_element::child_value() const { return xml_node(node_).child_value(); }
const char_t* xml_element::child_value(const char *name) const { return xml_node(node_).child_value(name); }
bool xml_element::set_name(const char *rhs) { return xml_node(node_).set_name(rhs); }
bool xml_element::set_value(const char *rhs) { return xml_node(node_).set_value(rhs); }
bool xml_element::set_child_value(const char *name,const char *rhs) {
	xml_node n = xml_node(node_).child(name);
	if (xml_node n2 = n.first_child())
		return n2.set_value(rhs);
	else
		return n.append_child(node_pcdata).set_value(rhs);
}
xml_element xml_element::append_child(const char *name) { return xml_node(node_).append_child(name); }
xml_element xml_element::prepend_child(const char *name) { return xml_node(node_).prepend_child(name); }
xml_element xml_element::append_copy(const xml_element &e) { return xml_node(node_).append_copy(xml_node(e.node_)); }
xml_element xml_element::prepend_copy(const xml_element &e) { return xml_node(node_).prepend_copy(xml_node(e.node_)); }
void xml_element::remove_child(const char *name) { xml_node(node_).remove_child(name); }
void xml_element::remove_child(const xml_element &e) { xml_node(node_).remove_child(xml_node(e.node_)); }
pugi::xml_node_struct *xml_element::ptr() { return node_; }

/// convenience function for document construction
xml_element xml_element::append_child_value(const char *name, const char *value) { 
	xml_node result = xml_node(node_).append_child(name);
	result.append_child(node_pcdata).set_value(value);
	return xml_node(node_);
}

/// convenience function for document construction
xml_element xml_element::prepend_child_value(const char *name, const char *value) { 
	xml_node result = xml_node(node_).prepend_child(name);
	result.append_child(node_pcdata).set_value(value);
	return xml_node(node_);
}

