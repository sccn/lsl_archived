% A single JIDE property field in a JIDE property grid.
% This is an internal class and is not intended to be used directly.
%
% Notes:
%   This contains JideDefaultProperty instances, and creates / maintains them on the basis
%   of PropertyGridField objects.
%   Functionality offered:
%    * set/get value
%    * create recursively, using custom initializers, also registering cell editors, etc...
%    * misc accessors
%   Basically does only local JIDE-related management, handles no inter-field wiring, etc., by itself.
%
% References:
% http://www.jidesoft.com/products/JIDE_Grids_Developer_Guide.pdf
%
% See also: PropertyGrid

% Copyright 2010 Levente Hunyadi
classdef JidePropertyGridField < handle
    properties        
        PropertyData;       % A PropertyGridField instance.
        Children = JidePropertyGridField.empty(0,1); % Children the property field might have.        
    end
    properties (Dependent)
        Value;              % value of the property (via getValue())
    end
    properties
        ChildrenSets = {};                           % cko: sets of alternative Children the property field might have...
        Control;                                     % A JIDE DefaultProperty instance.        
        Context;                                     % JIDE editor context (if any).        
        ContextType;                                 % JIDE editor context type (if any).
    end
    methods
        
        % --- CONSTRUCTOR ---
        
        function self = JidePropertyGridField(data)
            if nargin > 0
                self.Initialize(data);
            end
        end
        
        % Initializes a JIDE property based on a property grid field.
        % May register editor contexts with the global CellEditorManager.
        function self = Initialize(self, data)
            self.PropertyData = data;
            
            % create a new DefaultProperty & assign fields...
            field = objectEDT('com.jidesoft.grid.DefaultProperty'); 
            field = com.jidesoft.grid.DefaultProperty();            
            field.setName(data.Name);  % JIDE automatically uses a hierarchical naming scheme
            field.setCategory(data.Category);
            field.setDisplayName(data.DisplayName);
            field.setDescription(data.Description);
            field.setEditable(~data.ReadOnly && ~data.Hidden);
            switch data.Type.Shape
                case 'scalar'
                    % remap some types so that they can be properly edited
                    switch data.Type.PrimitiveType
                        case {'denserealdouble','sparserealdouble'}  % add a double-precision floating point property
                            matlabtype = 'double';  % MatLab type that is marshalled to Java
                        case {'denserealsingle','sparserealsingle'}  % add a single-precision floating point property
                            matlabtype = 'single';
                        case {'int8','uint8','int16'}
                            matlabtype = 'int16';
                        case {'uint16','int32'}  % add an integer property
                            matlabtype = 'int32';
                        case {'uint32','int64'}
                            matlabtype = 'int64';
                        case 'logical'  % add a logical property
                            matlabtype = 'logical';
                            % if its there, why not use it! ...
                            field.setEditorContext(com.jidesoft.grid.BooleanCheckBoxCellEditor.CONTEXT);
                        case {'densecomplexdouble','sparsecomplexdouble','densecomplexsingle','sparsecomplexsingle'}
                            matlabtype = [];
                            % string-typed...
                            field.setType(javaclass('char',1));  % edit as string and convert with eval
                        % cko/TODO: here we can add fancy things such as expression types...
                        otherwise
                            % other things cannot be edited... e.g., objects!
                            matlabtype = [];
                            field.setEditable(false);
                    end
                    if ~isempty(matlabtype)
                        % set the appropriate Java type
                        javatype = javaclass(matlabtype);
                        field.setType(javatype);
                        if ~isempty(data.Type.Domain)
                            % add domain support!!
                            if iscell(data.Type.Domain)  % explicit enumeration of domain elements
                                self.AddComboBoxEditor(field, javatype, data.Type.Domain);
                            elseif isnumeric(data.Type.Domain) && isinteger(data.Type)  % domain expressed as interval
                                self.AddSpinnerEditor(field, javatype, min(data.Type.Domain), max(data.Type.Domain));
                            end
                        end
                    end
                case {'row','column'}
                    % init the field with a vector value
                    InitializeVector(self, field);
                case 'matrix'
                    % init the field with a matrix value
                    InitializeMatrix(self, field);
                case 'empty'
                    % empty properties are non-editable (great...)
                    field.setEditable(false);
                otherwise
                    error('PropertyGrid:ArgumentTypeMismatch', 'Data shape %s is not supported.', data.Type.Shape);
            end
            % set the field and its value (note: this goes through a Java conversion...)
            self.Control = field;
            self.Value = data.Value;
            
            % pre-create the children recursively
            for k = 1 : numel(data.Children)
                self.Children(k) = JidePropertyGridField(data.Children(k));
                if ~self.Children(k).PropertyData.Hidden
                    self.Control.addChild(self.Children(k).Control);  end
            end
        end
        
        
        % --- VALUE GET/SET ---        
              
        % get the native MATLAB value for a property
        function value = get.Value(self)        
            javavalue = self.Control.getValue();
            [value,stringconversion] = self.PropertyData.Type.ConvertFromJava(javavalue);
            if stringconversion  % display pretty-formatted value
                javavalue = self.PropertyData.Type.ConvertToJava(value);
                self.Control.setValue(javavalue);
            end
        end
        
        % assign a native MATLAB value to a property
        function set.Value(self, value)
            try
                % this is a workaround as there is no efficient isfield() equivalent for objects at the moment
                has_alternatives = ~isempty(self.PropertyData.Specification.alternatives);
            catch
                has_alternatives = false;
            end
            if has_alternatives
                if ~isequal(value,self.PropertyData.Value)                
                    % get the old & new index into ChildrenSets
                    if ischar(value) && ischar(self.PropertyData.Value)
                        % string-based selection (switch)
                        oldidx = find(strcmp(self.PropertyData.Value,self.PropertyData.Specification.range));
                        newidx = find(strcmp(value,self.PropertyData.Specification.range));
                    elseif islogical(value) && islogical(self.PropertyData.Value)
                        % boolean selection (toggle)
                        oldidx = self.PropertyData.Value+1;
                        newidx = value+1;
                    elseif int(value) && isscalar(value) && isnumeric(self.PropertyData.Value) && isscalar(self.PropertyData.Value)
                        % 1-based indexed selection (cell)
                        oldidx = self.PropertyData.Value;
                        newidx = value;
                    else
                        error('Unsupported selector class.');
                    end
                    % generate PropertyData.ChildrenSets{newidx} on the fly if necessary....
                    if oldidx > length(self.PropertyData.ChildrenSets) || isempty(self.PropertyData.ChildrenSets{oldidx})
                        self.PropertyData.ChildrenSets{oldidx} = PropertyGridField.GenerateFrom(self.PropertyData.Specification.alternatives{oldidx}); end
                    if newidx > length(self.PropertyData.ChildrenSets) || isempty(self.PropertyData.ChildrenSets{newidx})
                        self.PropertyData.ChildrenSets{newidx} = PropertyGridField.GenerateFrom(self.PropertyData.Specification.alternatives{newidx}); end
                    
                    % replace ChildrenSets{oldidx} by children
                    self.ChildrenSets{oldidx} = self.Children;
                    if newidx > length(self.ChildrenSets)
                        self.ChildrenSets{newidx} = []; end
                    if isempty(self.ChildrenSets{newidx})
                        % generate them on the fly...
                        if isnumeric(self.ChildrenSets{newidx})
                            self.ChildrenSets{newidx} = JidePropertyGridField.empty(0,1); end
                        for k = 1:length(self.PropertyData.ChildrenSets{newidx})
                            self.ChildrenSets{newidx}(k) = JidePropertyGridField(self.PropertyData.ChildrenSets{newidx}(k)); end
                    end
                    self.Children = self.ChildrenSets{newidx};
                    % do the same in our data model
                    self.PropertyData.ChildrenSets{oldidx} = self.PropertyData.Children; 
                    self.PropertyData.Children = self.PropertyData.ChildrenSets{newidx};
                    % now do the JIDE relinking
                    if self.Control.getAllVisibleChildrenCount()
                        self.Control.removeAllChildren(); end
                    for k = 1:length(self.Children)
                        if ~self.Children(k).PropertyData.Hidden
                            self.Control.addChild(self.Children(k).Control); end
                    end
                end
            end
            
            % and change the actual value
            javavalue = self.PropertyData.Type.ConvertToJava(value);
            self.Control.setValue(javavalue);
        end

        
        % assignment test
        function tf = CanAccept(self, value)            
            validateattributes(self, {'JidePropertyGridField'}, {'scalar'});
            tf = self.PropertyData.Type.CanAccept(value);
        end
        
        % getting the specification
        function s  = GetPropertySpecification(selfarray)
            propdata = [selfarray.PropertyData];
            s = [propdata.Specification];
            for k = 1:numel(selfarray)
                if strcmp(s(k).type,'expression') && ~isempty(strfind(s(k).value,'::@'))
                    s(k).value = ['resolve_function(''' s(k).value ''')']; end
                if strcmp(s(k).type,'logical') && iscell(s(k).range)
                    s(k).value = s(k).range(s(k).value); end
                if ~isempty(selfarray(k).Children)
                    s(k).children = selfarray(k).Children.GetPropertySpecification(); 
                else
                    s(k).children = [];
                end
            end
        end
                
        
        % --- MISC ---
        
        % get properties as a PropertyGridField array
        function properties = GetProperties(selfarray)
            properties = PropertyGridField.empty(0,1);
            for k = 1 : numel(selfarray)
                properties(k) = selfarray(k).PropertyData;
            end
        end

        % get properties as Java native DefaultProperty array
        function list = GetTableModel(selfarray)
            ctrls = {};
            for k = 1:numel(selfarray)
                if ~selfarray(k).PropertyData.Hidden
                    ctrls{end+1} = selfarray(k).Control; end
            end
            list = javaArrayList(ctrls);
        end
        
        % find the JidePropertyGridField with the given dot-notated name in an array
        function self = FindByName(selfarray, name)        
            self = selfarray.FindByNameRecurse(strsplit(name, '.'));
        end

        % deallocate registered editor context guts
        function delete(self)
            if ~isempty(self.Context)
                com.jidesoft.grid.CellEditorManager.unregisterEditor(self.ContextType, self.Context);
            end
        end        
    end
    
    
    methods (Access = private)
                
        % --- ADDITIONAL JIDE PROPERTYGRIDFIELD INITIALIZERS ---
        
        % initialize the field for a vector-shaped property
        function InitializeVector(self, field)
            data = self.PropertyData;
            switch data.Type.PrimitiveType
                case 'char'  % add a string property
                    switch data.Type.Shape
                        case 'row'
                            field.setType(javaclass('char',1));
                            if ~isempty(data.Type.Domain)
                                self.AddComboBoxEditor(field, javaclass('char',1), javaStringArray(data.Type.Domain));
                            end
                        otherwise
                            field.setType(javaclass('char',1));  % edit as string and convert with eval
                    end
                case 'cellstr'
                    field.setType(javaclass('char',1));
                    field.setEditorContext(com.jidesoft.grid.MultilineStringCellEditor.CONTEXT);
                case 'logical'
                    if ~isempty(data.Type.Domain)
                        field.setType(javaclass('cellstr',1));  % java.lang.String array
                        self.AddCheckBoxListEditor(field, data.Type.Domain);
                    else
                        field.setType(javaclass('char',1));  % edit as string and convert with eval
                    end
                case {...
                        'denserealdouble','sparserealdouble','denserealsingle','sparserealsingle',...
                        'densecomplexdouble','sparsecomplexdouble','densecomplexsingle','sparsecomplexsingle',...
                        'int8','uint8','int16','uint16','int32','uint32','int64','uint64'}
                    field.setType(javaclass('char',1));  % edit as string and convert with eval
                otherwise
                    field.setEditable(false);
                    %error('PropertyGrid:ArgumentTypeMismatch', 'Unsupported type: %s %s.', data.Type.Shape, data.Type.PrimitiveType);
            end
        end
        
        % initialize the field for a matrix-shaped property
        function InitializeMatrix(self, field)
            data = self.PropertyData;
            switch data.Type.PrimitiveType
                case {...
                        'denserealdouble','sparserealdouble','denserealsingle','sparserealsingle',...
                        'densecomplexdouble','sparsecomplexdouble','densecomplexsingle','sparsecomplexsingle',...
                        'char','int8','uint8','int16','uint16','int32','uint32','int64','uint64','logical'}
                    field.setType(javaclass('char',1));  % edit as string and convert with eval
                otherwise
                    field.setEditable(false);
                    %error('PropertyGrid:ArgumentTypeMismatch', 'Matrix %s is not supported.', data.Type.PrimitiveType);
            end
        end
        
        
        % --- EDITOR CONTEXT REGISTRATION ---
        
        function ApplyContext(self, field, javatype, editor, editortype)
        % Registers a context for an editor.
        %
        % Input arguments:
        % field:
        %    a com.jidesoft.grid.Property instance
        % javatype:
        %    the Java type to be associated with the context
        % editor:
        %    a cell editor instance
        % editortype:
        %    the key used for generating the name of the context
            self.Context = com.jidesoft.grid.EditorContext(['JIDE_' editortype '_' int2str(self.GetContextCounterValue())]);
            self.ContextType = javatype;
            com.jidesoft.grid.CellEditorManager.registerEditor(javatype, editor, self.Context);  % register new context
            field.setEditorContext(self.Context);  % apply context to property
        end
        
        function AddCheckBoxListEditor(self, field, labels)
        % Registers a new check box list context.
        %
        % Input arguments:
        % field:
        %    a com.jidesoft.grid.Property instance
        % labels:
        %    a cell array of strings to label elements in the set shown
            editor = com.jidesoft.grid.CheckBoxListComboBoxCellEditor(javaStringArray(labels), javaclass('cellstr',1));
            self.ApplyContext(field, javaclass('cellstr',1), editor, 'checkboxlist');
        end
        
        function AddComboBoxEditor(self, field, javatype, javadomain)
        % Registers a new list selection context.
        % A drop-down combo box allows the user to select a single value
        % from a predefined set of values.
        %
        % Input arguments:
        % field:
        %    a com.jidesoft.grid.Property instance
        % javatype:
        %    a java.lang.Class instance
        % javadomain:
        %    an array of Java objects whose type corresponds to javatype
            editor = com.jidesoft.grid.ListComboBoxCellEditor(javadomain);
            self.ApplyContext(field, javatype, editor, 'combobox');
        end
        
        function AddSliderEditor(self, field, javatype, lower, upper)
        % Registers a new slider context.
        % The slider has a limited range.
        %
        % field:
        %    a com.jidesoft.grid.Property instance
        % javatype:
        %    a java.lang.Class instance
        % lower:
        %    an integer representing the upper lower of the interval
        % upper:
        %    an integer representing the upper bound of the interval
            editor = com.jidesoft.grid.SliderCellEditor(int32(lower), int32(upper));
            self.ApplyContext(field, javatype, editor, 'slider');
        end
        
        function AddSpinnerEditor(self, field, javatype, lower, upper)
        % Registers a new spinner context.
        % The spinner has a limited range and a fixed step.
        %
        % field:
        %    a com.jidesoft.grid.Property instance
        % javatype:
        %    a java.lang.Class instance
        % lower:
        %    an integer representing the upper lower of the interval
        % upper:
        %    an integer representing the upper bound of the interval
            spinner = javax.swing.SpinnerNumberModel(int32(lower), int32(lower), int32(upper), int32(1));
            editor = com.jidesoft.grid.SpinnerCellEditor(spinner);
            self.ApplyContext(field, javatype, editor, 'spinner');
        end
        
        
        % --- IRRELEVANT STUFF ---
        
        % recursively finds the property JidePropertyGrid field for a given cell array path into the name tree
        function self = FindByNameRecurse(selfarray, nameparts)
            names = getclassfield(getclassfield(selfarray, 'PropertyData'), 'Name');
            ix = find(strcmp(nameparts{1},names),1);
            if isempty(ix)
                self = JidePropertyGridField.empty(0,1);
                return;
            end
            namerest = nameparts(2:end);
            if isempty(namerest)
                self = selfarray(ix);
            else
                self = selfarray(ix).Children.FindByNameRecurse(namerest);
            end
        end        
    end
    
    methods (Static, Access = private)
        % whatever. The current counter value for the JIDE editor context.
        function counter = GetContextCounterValue()
            persistent jide_counter;
            if isempty(jide_counter)  % if not yet assigned
                jide_counter = 0;
            end
            jide_counter = jide_counter + 1;  % increment global field counter
            counter = jide_counter;           % return current value
        end
    end
end
