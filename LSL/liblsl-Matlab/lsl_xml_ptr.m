classdef lsl_xml_ptr
	% A lightweight XML element tree; models the .desc() field of lsl_streaminfo.
	% Has a name and can have multiple named children or have text content as value; attributes are omitted.
	% Insider note: The interface is modeled after a subset of pugixml's node type and is compatible with it.
	% See also http://pugixml.googlecode.com/svn/tags/latest/docs/manual/access.html for additional documentation.

    properties
        LibHandle = 0;  % this is a handle to the liblsl library (to call its functions)
        PtrHandle = 0;  % this is a ptr into some XML document (the .desc() field of a streaminfo)
    end
    
    methods
        function self = lsl_xml_ptr(LibHandle,PtrHandle)
            % Internal constructor.
            self.PtrHandle = PtrHandle;
            self.LibHandle = LibHandle;
        end        
        
        
        % === Tree Navigation ===
        
        function result = child(self,name)
            % Get the first child of the element with a given name 
            result = lsl_xml_ptr(self.LibHandle,lsl_child(self.LibHandle,self.PtrHandle,name));
        end
        
        function result = first_child(self)
            % Get the first child of the element
            result = lsl_xml_ptr(self.LibHandle,lsl_first_child(self.LibHandle,self.PtrHandle));
        end
        
        
        function result = last_child(self)
            % Get the last child of the element
            result = lsl_xml_ptr(self.LibHandle,lsl_last_child(self.LibHandle,self.PtrHandle));
        end
        
        
        function result = next_sibling(self,name)
            % Get the next sibling of the element, optionally of a given name            
            if nargin>1
                result = lsl_xml_ptr(self.LibHandle,lsl_next_sibling_n(self.LibHandle,self.PtrHandle,name));
            else
                result = lsl_xml_ptr(self.LibHandle,lsl_next_sibling(self.LibHandle,self.PtrHandle));
            end
        end
        
        
        function result = previous_sibling(self)
            % Get the previous sibling of the element, optionally of a given name
            if nargin>1
                result = lsl_xml_ptr(self.LibHandle,lsl_previous_sibling_n(self.LibHandle,self.PtrHandle,name));
            else
                result = lsl_xml_ptr(self.LibHandle,lsl_previous_sibling(self.LibHandle,self.PtrHandle));
            end
        end
        
        
        function result = parent(self)
            % Get the parent of the element
            result = lsl_xml_ptr(self.LibHandle,lsl_parent(self.LibHandle,self.PtrHandle));
        end
        
        
        % === Tree Navigation By Name (legacy functions, now also without _n) ===
        
        function result = next_sibling_n(self,name)
            % Get the next sibling of the element that has the given name 
            result = lsl_xml_ptr(self.LibHandle,lsl_next_sibling_n(self.LibHandle,self.PtrHandle,name));
        end
        
        
        function result = previous_sibling_n(self,name)
            % Get the previous sibling of the element that has the given name 
            result = lsl_xml_ptr(self.LibHandle,lsl_previous_sibling_n(self.LibHandle,self.PtrHandle,name));
        end
        
        
        % === Content Queries ===
        
        function result = empty(self)
            % Determine whether the given node is empty
            result = lsl_empty(self.LibHandle,self.PtrHandle);
        end
        
        
        function result = is_text(self)
            % Determine whether this is a text body (instead of an XML element). True both for plain
            % char data and CData.
            result = lsl_is_text(self.LibHandle,self.PtrHandle);
        end
        
        
        function result = name(self)
            % Name of the element.
            result = lsl_name(self.LibHandle,self.PtrHandle);
        end
        
        
        function result = value(self)
            % String value of the element.
            result = lsl_value(self.LibHandle,self.PtrHandle);
        end
        
        
        function result = child_value(self,name)
            % Get child value (value of the first child that is text), optionally of a given name.
            if nargin>1
                result = lsl_child_value_n(self.LibHandle,self.PtrHandle,name);
            else
                result = lsl_child_value(self.LibHandle,self.PtrHandle);
            end
        end
        
        
        function result = child_value_n(self,name)
            % Get the value of a named child
            result = lsl_child_value_n(self.LibHandle,self.PtrHandle,name);
        end
        
        
        % === Modification ===
        
        function result = append_child_value(self,name,value)
            % Append a child node with a given name, which has a (nameless) plain-text child with
            % the given text value (i.e., this is what you most likely want to use)
            result = lsl_xml_ptr(self.LibHandle,lsl_append_child_value(self.LibHandle,self.PtrHandle,name,value));
        end
        
        
        function result = prepend_child_value(self,name,value)
            % Prepend a child node with a given name, which has a (nameless) plain-text child with
            % the given text value.
            result = lsl_xml_ptr(self.LibHandle,prepend_child_value(self.LibHandle,self.PtrHandle,name,value));
        end
        
        
        function result = set_child_value(self,name,value)
            % Set the text value of the (nameless) plain-text child of a named child node.
            result = lsl_set_child_value(self.LibHandle,self.PtrHandle,name,value);
        end
        
        
        function result = set_name(self,name)
            % Set the element's name.
            result = lsl_set_name(self.LibHandle,self.PtrHandle,name);
        end

        
        function result = set_value(self,value)
            % Set the element's value.
            result = lsl_set_value(self.LibHandle,self.PtrHandle,value);
        end
        
        
        function result = append_child(self,name)
            % Append a child element with the specified name.
            result = lsl_xml_ptr(self.LibHandle,lsl_append_child(self.LibHandle,self.PtrHandle,name));
        end
        
        
        function result = prepend_child(self,name)
            % Prepend a child element with the specified name.
            result = lsl_xml_ptr(self.LibHandle,lsl_prepend_child(self.LibHandle,self.PtrHandle,name));
        end
        
        
        function result = append_copy(self,node)
            % Append a copy of the specified element as a child.
            result = lsl_xml_ptr(self.LibHandle,lsl_append_copy(self.LibHandle,self.PtrHandle,node.PtrHandle));
        end
        
        
        function result = prepend_copy(self,node)
            % Prepend a copy of the specified element as a child.
            result = lsl_xml_ptr(self.LibHandle,lsl_prepend_copy(self.LibHandle,self.PtrHandle,node.PtrHandle));
        end
        
        
        function remove_child(self,node)
            % Prepend a copy of the specified element as a child.
            lsl_remove_child(self.LibHandle,self.PtrHandle,node.PtrHandle);
        end
        
        
        function remove_child_n(self,name)
            % Prepend a copy of the specified element as a child.
            lsl_remove_child_n(self.LibHandle,self.PtrHandle,name);
        end
        
    end
end
