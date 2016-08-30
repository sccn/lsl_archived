#include "../include/lsl_c.h"
#include "pugixml/pugixml.hpp"


// === implementation of the lsl_xml_ptr functions of lsl_c.h ===

using namespace pugi;

LIBLSL_C_API lsl_xml_ptr lsl_first_child(lsl_xml_ptr e) { return (lsl_xml_ptr)xml_node((xml_node_struct*)e).first_child().internal_object(); }
LIBLSL_C_API lsl_xml_ptr lsl_last_child(lsl_xml_ptr e) { return (lsl_xml_ptr)xml_node((xml_node_struct*)e).last_child().internal_object(); }
LIBLSL_C_API lsl_xml_ptr lsl_next_sibling(lsl_xml_ptr e) { return (lsl_xml_ptr)xml_node((xml_node_struct*)e).next_sibling().internal_object(); }
LIBLSL_C_API lsl_xml_ptr lsl_previous_sibling(lsl_xml_ptr e) { return (lsl_xml_ptr)xml_node((xml_node_struct*)e).previous_sibling().internal_object(); }
LIBLSL_C_API lsl_xml_ptr lsl_parent(lsl_xml_ptr e) { return (lsl_xml_ptr)xml_node((xml_node_struct*)e).parent().internal_object(); }
LIBLSL_C_API lsl_xml_ptr lsl_child(lsl_xml_ptr e, char *name) { return (lsl_xml_ptr)xml_node((xml_node_struct*)e).child(name).internal_object(); }
LIBLSL_C_API lsl_xml_ptr lsl_next_sibling_n(lsl_xml_ptr e, char *name) { return (lsl_xml_ptr)xml_node((xml_node_struct*)e).next_sibling(name).internal_object(); }
LIBLSL_C_API lsl_xml_ptr lsl_previous_sibling_n(lsl_xml_ptr e, char *name) { return (lsl_xml_ptr)xml_node((xml_node_struct*)e).previous_sibling(name).internal_object(); }

LIBLSL_C_API int lsl_empty(lsl_xml_ptr e) { return xml_node((xml_node_struct*)e).empty(); }
LIBLSL_C_API int  lsl_is_text(lsl_xml_ptr e) { return xml_node((xml_node_struct*)e).type() != node_element; }
LIBLSL_C_API char *lsl_name(lsl_xml_ptr e) { return const_cast<char*>(xml_node((xml_node_struct*)e).name()); }
LIBLSL_C_API char *lsl_value(lsl_xml_ptr e) { return const_cast<char*>(xml_node((xml_node_struct*)e).value()); }
LIBLSL_C_API char *lsl_child_value(lsl_xml_ptr e) { return const_cast<char*>(xml_node((xml_node_struct*)e).child_value()); }
LIBLSL_C_API char *lsl_child_value_n(lsl_xml_ptr e, char *name) { return const_cast<char*>(xml_node((xml_node_struct*)e).child_value(name)); }

LIBLSL_C_API int lsl_set_name(lsl_xml_ptr e, char *rhs) { return xml_node((xml_node_struct*)e).set_name(rhs); }
LIBLSL_C_API int lsl_set_value(lsl_xml_ptr e, char *rhs) { return xml_node((xml_node_struct*)e).set_value(rhs); }
LIBLSL_C_API lsl_xml_ptr lsl_append_child(lsl_xml_ptr e, char *name) { return (lsl_xml_ptr)xml_node((xml_node_struct*)e).append_child(name).internal_object(); }
LIBLSL_C_API lsl_xml_ptr lsl_prepend_child(lsl_xml_ptr e, char *name) { return (lsl_xml_ptr)xml_node((xml_node_struct*)e).prepend_child(name).internal_object(); }
LIBLSL_C_API lsl_xml_ptr lsl_append_copy(lsl_xml_ptr e, lsl_xml_ptr e2) { return (lsl_xml_ptr)xml_node((xml_node_struct*)e).append_copy(xml_node((xml_node_struct*)e2)).internal_object(); }
LIBLSL_C_API lsl_xml_ptr lsl_prepend_copy(lsl_xml_ptr e, lsl_xml_ptr e2) { return (lsl_xml_ptr)xml_node((xml_node_struct*)e).prepend_copy(xml_node((xml_node_struct*)e2)).internal_object(); }
LIBLSL_C_API void lsl_remove_child_n(lsl_xml_ptr e, char *name) { xml_node((xml_node_struct*)e).remove_child(name); }
LIBLSL_C_API void lsl_remove_child(lsl_xml_ptr e, lsl_xml_ptr e2) { xml_node((xml_node_struct*)e).remove_child(xml_node((xml_node_struct*)e2)); }

LIBLSL_C_API int lsl_set_child_value(lsl_xml_ptr e, char *name, char *value) { return xml_node((xml_node_struct*)e).child(name).first_child().set_value(value); }

LIBLSL_C_API lsl_xml_ptr lsl_append_child_value(lsl_xml_ptr e, char *name, char *value) {
	xml_node result = xml_node((xml_node_struct*)e).append_child(name);
	result.append_child(node_pcdata).set_value(value);
	return (lsl_xml_ptr)e;
}

LIBLSL_C_API lsl_xml_ptr lsl_prepend_child_value(lsl_xml_ptr e, char *name, char *value) {
	xml_node result = xml_node((xml_node_struct*)e).prepend_child(name);
	result.append_child(node_pcdata).set_value(value);
	return (lsl_xml_ptr)e;
}

