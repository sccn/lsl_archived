% Metadata on a single property.
% This class is used to provide type and shape information, initial value,
% categorization, and context-sensitive help when a property is added as a
% field to a property grid.
%
% Notes:
%   This is pretty much a builder class. Functionality offered is:
%    * setters / getters for the usual properties
%    * auto-construction from struct, object and specification
%    * manual hierarchy creation
%   
%
% See also: PropertyGrid, PropertyType
%
% Copyright 2010 Levente Hunyadi

classdef PropertyGridField < hgsetget
    properties        
        Name;               % The short name of the property.        
        Type;               % The type constraints the property value must conform to.        
        Value;              % The initial value of the property.        
        Category;           % The name of the category which the property should be put into.       
        DisplayName;        % A longer, descriptive name for the property.        
        Description;        % A more detailed description of the property for novice users.        
        ReadOnly = false;   % Whether the property is read-only.        
        Dependent = false;  % Whether the property is linked to other properties...
        Hidden = false;     % Whether the property is hidden from user
        Children = PropertyGridField.empty(1,0);    % Direct descendants of this property.
        ChildrenSets = {};  % cell array of alternative children sets...        
        Specification = []; % the local type specification, if any
    end
    
    methods        
        
        % --- CONSTRUCTOR ---
        
        function self = PropertyGridField(name, value, varargin)
            self.Name = name;
            self.Value = value;
            self = constructor(self, varargin{:});
        end

        
        % --- SETTERS ---
        
        function self = set.Name(self, name)
            if ~ischar(name) || isempty(name) || size(name,1) ~= 1
                error('Name must be a non-empty string.'); end
            self.Name = name;
        end
        
        function self = set.Type(self, type)
            typ = struct('PrimitiveType',{type.PrimitiveType},'Shape',{type.Shape},'Domain',{type.Domain});
            tmpval = hlp_microcache('propcast',@cast_value_to_type,typ,self.Value,class(self.Value));
            self.Type = type;
            self.Value = tmpval;
        end
        
        function self = set.Value(self, value)
            if isempty(self.Type) %#ok<MCSUP>
                self.Value = value;
                self.Type = hlp_microcache('propdiscover',@PropertyType.AutoDiscover,value,class(value));
            else                
                typ = struct('PrimitiveType',{self.Type.PrimitiveType},'Shape',{self.Type.Shape},'Domain',{self.Type.Domain});
                hlp_microcache('propaccept',@check_accept_value,typ,value,class(value));
                self.Value = value;
            end
            if ~isempty(self.Specification)
                self.Specification.value = self.Value; end
        end
        
        function self = set.Category(self, category)
            if ~ischar(category) || size(category,1) ~= 1
                error('Category must be a string!'); end
            self.Category = category;
        end
        
        function self = set.DisplayName(self, name)
            if isempty(name)
                self.DisplayName = [];
            else
                if ~ischar(name) || size(name,1) ~= 1
                    error('Name must be a string!'); end
                self.DisplayName = name;
            end
        end

        function self = set.Description(self, description)
            if isempty(description)
                self.Description = [];  % clear description
            else
                if ~ischar(description) || size(description,1) ~= 1
                    error('Description must be a string!'); end
                self.Description = description;
            end
        end
        
        function self = set.ReadOnly(self, readonly)
            if ~islogical(readonly) || ~isscalar(readonly)
                error('Readonly is a scalar logical value.'); end
            self.ReadOnly = readonly;
        end
        
        function self = set.Specification(self, spec)
            self.Specification = spec;
        end
        
        
        % --- MANUAL HIERARCHY GENERATION SUPPORT ---
        
        function root = GetHierarchy(selfarray)
        % Converts a flat property list into a hierarchical structure.
        % Parent-child relationships are discovered from hierarchical
        % qualified names where components are separated by dot (.).
            root = arrayfilter(@(self) isempty(strfind(self.Name, '.')) , selfarray);  % get simple names only
            root.WireHierarchy(selfarray);
            selfarray.UnqualifyNames();
        end
        
        function UnqualifyNames(selfarray)
        % Reduces qualified names to unqualified names (without dot).
            for k = 1 : numel(selfarray)
                self = selfarray(k);
                dot = strfind(self.Name, '.');
                if ~isempty(dot)
                    self.Name = self.Name(dot(end)+1:end);
                end
            end
        end
        
        function self = FindByName(selfarray, name)
        % Looks up a property field by name.
            for k = 1 : numel(selfarray)
                self = selfarray(k);
                if strcmp(name, self.Name)
                    return
                end
            end
            self = [];  % not found
        end
        
        
        % --- ARRAY-WISE CHECKER / HELPERS ---
        
        function tf = HasCategory(selfarray)
        % True if any object in the array has a category specification.
            for k = 1 : numel(selfarray)
                self = selfarray(k);
                if ~isempty(self.Category) || self.Children.HasCategory()
                    tf = true;
                    return;
                end
            end
            tf = false;
        end

        function tf = HasDescription(selfarray)
        % True if any object in the array has a description.
            for k = 1 : numel(selfarray)
                self = selfarray(k);
                if ~isempty(self.Description) || self.Children.HasDescription()
                    tf = true;
                    return;
                end
            end
            tf = false;
        end                        
    end
    
    
    
    methods (Static)
                
        % --- FACTORY FUNCTIONS: GENERATE HIERARCHY FROM STRUCT, CLASS, SPECIFICATION ---
        
        % generate a property hierarchy from a struct, specification, or class...
        function fields = GenerateFrom(obj)        
            % +++ cko
            if all(isfield(obj,{'head','names','value','assigner','children','alternatives','range','type','shape','help','cat','displayable'}))
                fields = PropertyGridField.GenerateFromSpec(obj);                
            % --- cko
            elseif isstruct(obj)
                fields = PropertyGridField.GenerateFromStruct(obj);
            elseif isobject(obj)
                fields = PropertyGridField.GenerateFromClass(obj);
            else
                fields = PropertyGridField.empty(1,0);
            end
        end
        
        % +++ cko
        % generate a property hierarchy from a specification ---
        function fields = GenerateFromSpec(spec)
            fields = PropertyGridField.empty(1,0);
            % for each specifier
            for f=1:length(spec)
                s = spec(f);

                % convert unsupported types to expressions...                
                if isa(s.value,'function_handle') || (iscell(s.value) && ~iscellstr(s.value)) || isstruct(s.value)
                    if isa(s.value,'function_handle')
                        info = functions(s.value);
                        if strcmp(info.type,'scopedfunction')
                            % we have a scoped function; store it in a very special manner...
                            s.value = [info.parentage{2} '::@' info.parentage{1}]; 
                        elseif ~any(strcmp(info.type,{'simple','anonymous'}))
                            error('Unsupported function type.');
                        end 
                    else
                        s.value = hlp_tostring(s.value);
                    end
                    s.type = 'expression';
                    s.shape = 'row';
                end
                
                if strcmp(s.type,'expression') && ~ischar(s.value)
                    s.value = hlp_tostring(s.value); end

                if strcmp(s.type,'logical') && iscell(s.range)
                    s.shape = 'row';
                    if iscell(s.value)
                        s.value = ismember(lower(strtrim(s.range)),lower(strtrim(s.value))); 
                    elseif ~isempty(s.value)
                        error('Only cell arrays supported for the value...');
                    else
                        s.value = {};
                    end
                end
                
                gui_name = s.names{min(2,end)};
                gui_type = s.type;
                if strcmp(gui_type,'expression')
                    gui_type = 'char'; end
                if strcmp(gui_type,'object')
                    gui_type = 'denserealdouble'; end
                gui_category = s.cat;
                if isempty(gui_category)
                    gui_category = 'Miscellaneous'; end
                if iscell(s.help) && ~isempty(s.help)
                    gui_help = [s.help{1} '. ' s.help{2:end}];
                else
                    gui_help = '';
                end
                try
                    fields(end+1) = PropertyGridField(gui_name, s.value, ...
                        'Type', PropertyType(gui_type,s.shape,s.range), ...
                        'Category',gui_category, ...
                        'DisplayName',gui_name, ...
                        'Description',gui_help, ...
                        'ReadOnly',false, ...
                        'Dependent',false, ...
                        'Hidden',~s.displayable, ...
                        'Specification',s);
                catch
                    hlp_handleerror(lasterror);
                    disp('The given argument specification (shown below) could not be converted into a GUI-supported PropertyGridField. You are now in debug mode.');
                    t = PropertyType(gui_type,s.shape,s.range); x = s.value;
                    s
                    keyboard
                end
                if ~isempty(s.children)
                    fields(end).Children = PropertyGridField.GenerateFrom(s.children); end
            end
        end
        % --- cko

        % generate a property hierarchy from a struct
        function fields = GenerateFromStruct(obj)
            names = fieldnames(obj);
            n = numel(names);
            
            k = 0;
            fields = PropertyGridField.empty(1,0);
            for i = 1 : n
                name = names{i};
                value = obj.(name);
                k = k + 1;
                fields(k) = PropertyGridField(name, value);
                fields(k).Children = PropertyGridField.GenerateFrom(value);
            end
        end
        
        % generate a property hierarchy from a class
        function fields = GenerateFromClass(obj)
        % Automatically generated property fields for an object.
            assert(isobject(obj), 'PropertyGridField:ArgumentTypeMismatch', ...
                'New-style MatLab object (defined with the classdef keyword) is required.');
            try
                clazz = metaclass(obj);
            catch %#ok<CTCH>
                fields = GenerateFromStruct(obj);  % fallback to method for old-style classes (i.e. not defined with the classdef keyword)
                return;
            end

            k = 0;  % number of properties found
            n = numel(clazz.Properties);  % maximum number of properties
            fields = PropertyGridField.empty(0, 1);
            for i = 1 : n
                property = clazz.Properties{i};
                if property.Abstract || property.Hidden || ~strcmp(property.GetAccess, 'public');
                    continue;
                end
                try
                    value = obj.(property.Name);
                catch %#ok<CTCH>
                    continue;  % could not fetch property value
                end
                description = property.Description;  % not currently used in MatLab
                if isempty(description)
                    text = helptext([clazz.Name '.' property.Name]);  % fetch help text as description
                    if ~isempty(text)
                        description = text{1};  % use first line
                    end
                end
                readonly = property.Constant || ~strcmp(property.SetAccess, 'public') || property.Dependent && isempty(property.SetMethod);
                dependent = property.Dependent;
                
                k = k + 1;
                fields(k) = PropertyGridField(property.Name, value, ...
                    'DisplayName', property.Name, ...
                    'Description', description, ...
                    'ReadOnly', readonly, ...
                    'Dependent', dependent);
                fields(k).Children = PropertyGridField.GenerateFrom(value);
            end
        end
    end
    
    
    methods (Access = private)
        
        
        % --- MISC FUNCTIONALITY FOR MANUAL HIERARCHY GENERATION ---
        
        function WireHierarchy(selfarray, descendants)
        % Wires parent-child relationships between nested properties.
            for k = 1 : numel(selfarray)
                self = selfarray(k);
                self.Children = descendants.FilterChildren(self.Name);  % add direct descendants as children
                self.Children.WireHierarchy(descendants);               % recurse for children
            end
        end

        function filteredarray = FilterChildren(selfarray, filterprefix)
        % An array of direct children.
        %
        % Input arguments:
        % filterprefix:
        %    a cell array that specifies which members to select.
        %
        % Example:
        % nodes.FilterChildren('hu.bme') selects
        %    hu.bme.aut, hu.bme.cs, hu.bme.mit
        % nodes.FilterChildren('hu.bme') does not select
        %    hu.bme.aut.www (not a direct child)
            names = getclassfield(selfarray, 'Name');
            if iscell(filterprefix)
                prefix = [strjoin('.', filterprefix) '.'];
            else
                prefix = [filterprefix '.'];
            end
            ix = strmatch(prefix, names);  % get names that begin with prefix
            if isempty(ix)
                filteredarray = PropertyGridField.empty(1,0);
                return;
            end
            
            len = numel(prefix);
            names = names(ix);                                             % drop names that do not begin with prefix
            names = cellfun(@(name) name(len+1:end), names, ...            % drop leading prefix from names
                'UniformOutput', false);
            filter = cellfun(@(name) isempty(strfind(name, '.')), names);  % property names that become simple after prefix is chopped
            filteredarray = selfarray(ix(filter));
        end
    end    
end
