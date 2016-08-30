% A property grid based on the JIDE grid implementation.
% A graphical user control based on the integrated JIDE PropertyGrid that
% mimics MatLab's property inspector. Unlike the inspector, it supports
% structures, new-style MatLab objects, both with value and handle
% semantics.
% 
% Notes:
%   This is fairly simple, actually. Pretty much creates a JIDE Table / Pane from the book,
%   assigns to it a TableModel which is a transliteration of the specified PropertyGridField's,
%   and wires the appropriate update events, then adds some bells & whistles (F1/F2 keys, matrix editor, struct binding).
%
%
% The property grid displays a list of (object) properties with values
% editable in-place. Each property has an associated semantics (or type)
% that restricts the possible values the property can take and helps
% visualize the property value in a natural manner, in particular:
% * a character array is displayed as a string and can be edited in-place
% * a scalar logical is mapped to a checkbox
% * an integer value that has a limited range is manipulated with a spinner
% * a selection from a set of values is presented as a drop-down list
% * a cell array of strings (either row or column vector) can be edited as
%   multi-line text in a pop-up text box
% * a logical vector that is an indicator for a set (e.g. [false false
%   true] for 'C' from the universe {'A','B','C'}]) is visualized as a
%   checkbox list
% * numeric vectors and matrices can be edited elementwise in a pop-up
%   window
% * parent-child relationships are presented with the help of expandable
%   properties
%
% Supported types include all shapes (scalar, vector and matrix) of all
% primitive types (logicals, integers, real/complex double/single) as well
% as cell arrays of strings, structures, and both value and handle MatLab
% objects with arbitrary level of nesting.
%
% If a property is selected use F1 to get help (a dialog is displayed with
% the help text of the property) or F2 to edit a numeric matrix in a pop-up
% dialog.
%
% References:
% The com.jidesoft.grid package by JIDE Software,
%    http://www.jidesoft.com/products/JIDE_Grids_Developer_Guide.pdf
%
% See also: inspect

% Copyright 2010 Levente Hunyadi
classdef PropertyGrid < UIControl
    properties (Dependent)        
        Control;    % The handle graphics control that wraps the property grid. This is redirected to the Container...
        Properties; % Properties listed in the property grid. This is redirected to Fields.GetProperties()
        Item;       % The MatLab structure or object bound to the property grid. This is redirected to BoundItem
    end
    properties (Access = private)        
        Container;  % A uipanel that wraps the property grid.        
        Pane;       % A com.jidesoft.grid.PropertyPane instance.
                    % Encapsulates a property table and decorates it with icons to
                    % choose sorting order, expand and collapse categories, and a 
                    % description panel.        
        Table;      % A com.jidesoft.grid.PropertyTable instance.
                    % Lets the user view and edit property name--value pairs.        
        Model;      % A com.jidesoft.grid.PropertyTableModel instance.
                    % Contains the properties enlisted in the property grid.
        Fields = JidePropertyGridField.empty(1,0);  % the fields of the JIDE PropertyTableModel as references...
        BoundItem = []; % The MatLab structure or object bound to the property grid.
    end
    methods
        
        
        % --- CONSTRUCTOR(S) ---
        
        function self = PropertyGrid(varargin)
            self = self@UIControl(varargin{:});
        end
        
        function self = Instantiate(self, parent)
            % create new figure if necessary
            if nargin < 2
                parent = figure; end
 
            % create a uipanel below the parent, filling it completely
            self.Container = uipanel(parent, ...
                'Units', 'normalized', ...
                'Position', [0 0 1 1], ...
                'Tag', '__PropertyGrid__', ...
                'UserData', self);

            % initialize JIDE (note: we should call these only once...)
            com.mathworks.mwswing.MJUtilities.initJIDE;
            % register a few special editors and renderers, depending on property type... (there are a billion other editors, too)
            com.jidesoft.grid.CellEditorManager.registerEditor(javaclass('cellstr',1), com.jidesoft.grid.StringArrayCellEditor);
            com.jidesoft.grid.CellEditorManager.registerEditor(javaclass('char',1), com.jidesoft.grid.MultilineStringCellEditor, com.jidesoft.grid.MultilineStringCellEditor.CONTEXT);
            com.jidesoft.grid.CellRendererManager.registerRenderer(javaclass('char',1), com.jidesoft.grid.MultilineStringCellRenderer, com.jidesoft.grid.MultilineStringCellEditor.CONTEXT);

            % create JIDE property grid (running on the event dispatch thread...)
            self.Table = handle(objectEDT('com.jidesoft.grid.PropertyTable'), 'CallbackProperties');  % property grid (without table model)
            % and create a JIDE property pane with the grid as its grid...
            self.Pane = objectEDT('com.jidesoft.grid.PropertyPane', self.Table);  % property pane (with icons at top and help panel at bottom)

            % put the JIDE pane into the uipanel
            pixelpos = getpixelposition(self.Container);
            [control,container] = javacomponent(self.Pane, [0 0 pixelpos(3) pixelpos(4)], self.Container); %#ok<ASGLU>
            set(container, 'Units', 'normalized');            
            % register a callback on the table (for F1,F2)
            set(self.Table, 'KeyPressedCallback', @PropertyGrid.OnKeyPressed);
        end

        
        % --- SET PROPERTIES ---

        % set the properties to be displayed in the grid directly...
        function self = set.Properties(self, properties)
            validateattributes(properties, {'PropertyGridField'}, {'vector'});
            % clear bound item
            self.BoundItem = [];
            % and clear callbacks...
            if ~isempty(self.Model)
                set(self.Model, 'PropertyChangeCallback', []);  % clear callback
            end
                        
            % set up the table properties in JIDE
            
            % if none of the properties has a category, we don't need a toolbar
            toolbar = properties.HasCategory();
            % likewise, if none of the properties has a description, we don't need a panel for that
            description = properties.HasDescription();
            
            % first we transform the property model object into a JidePropertyGridField object
            % this field basically creates a list of JIDE DefaultPropertyObjects
            self.Fields = JidePropertyGridField.empty(0,1);
            for k = 1 : numel(properties)
                self.Fields(k) = JidePropertyGridField(properties(k)); end

            % get a custom table model for them (which is basically a java array of those JIDE DefaultProperty objects)
            list = self.Fields.GetTableModel();
            % create a new PropertyTableModel (refcounted) from these properties
            model = handle(com.jidesoft.grid.PropertyTableModel(list), 'CallbackProperties');            
            model.setMiscCategoryName('Miscellaneous');  % caption for uncategorized properties
            % make sure that all are expanded - WE PROBABLY DON'T WANT THAT!
            model.expandAll();
            % assign
            self.Model = model;

            % assign the model to the table
            self.Table.setModel(model);
            % turn in the toolbar if necessary
            self.Pane.setShowToolBar(toolbar);
            % set the appropriate ordering
            if toolbar
                self.Pane.setOrder(0);
            else
                self.Pane.setOrder(1);
            end
            % and turn on the description if necessary...
            self.Pane.setShowDescription(description);

            % set a reasonable sizing for the columns
            pixelpos = getpixelposition(self.Container);
            col0 = self.Table.getColumnModel().getColumn(0);
            col0.setPreferredWidth(pixelpos(3)*0.65);
            col1 = self.Table.getColumnModel().getColumn(1);
            col1.setPreferredWidth(pixelpos(3)*0.35);
            
            % wire property change event hook
            set(model, 'PropertyChangeCallback', @PropertyGrid.OnPropertyChange);
            
            % annoying hack to "repaint" the PropertyGrid for some OS'
            if ~mod(self.Parent,1)
                set(self.Parent,'position',get(self.Parent,'Position')+1);
                set(self.Parent,'position',get(self.Parent,'Position')-1);
            end
        end
        
        
        % --- GETTERS (redirecting) ---
        
        % get property hierarchy as a struct() - cko: this works only for manually constructed fields (actually, it needs to be hierarchical)
        function s = GetPropertyValues(self)
            % issue an editing-finished event
            self.Table.editingStopped(javax.swing.event.ChangeEvent(javax.swing.event.ChangeEvent(self.Model)));
            % make sure that it ripples through the threads...
            drawnow;
            % get the contents...
            s = struct;
            for k = 1 : numel(self.Fields)
                field = self.Fields(k);
                s = nestedassign(s, field.PropertyData.Name, field.PropertyData.Value);
            end
        end
                
        % get property hierarchy as a specification
        function s = GetPropertySpecification(self)
            % issue an editing-finished event
            self.Table.editingStopped(javax.swing.event.ChangeEvent(javax.swing.event.ChangeEvent(self.Model)));
            % make sure that it ripples through the threads...
            drawnow;
            % obtain the specification
            s = self.Fields.GetPropertySpecification();
        end
                
        function ctrl = get.Control(self)
            ctrl = self.Container;
        end
        
        function properties = get.Properties(self)
            properties = self.Fields.GetProperties();
        end

        function item = get.Item(self)
            item = self.BoundItem;
        end
        
        
        % --- ITEM BINDING ---
        
        function self = set.Item(self, item)
        % Binds an object to the property grid.
        % Any changes made in the property grid are automatically reflected
        % in the bound object. Only handle objects (i.e. those that derive
        % from the handle superclass) are supported for on-line binding
        % (i.e. changes are immediately available in the original object).
        % In order to simplify usage, this property may even be set with
        % value objects (i.e. those not derived from handle) and structures
        % but the Item property has to be queried explicitly to fetch
        % changes (off-line binding).
            if ~isempty(item)
                assert(isstruct(item) || isobject(item), 'PropertyGrid:ArgumentTypeMismatch', ...
                    'Only structures and MatLab objects are bindable.');
            end
            if ~isempty(item)
                % generate the properties
                properties = PropertyGridField.GenerateFrom(item);
            else
                properties = PropertyGridField.empty(1,0);
            end
            % bind!
            self.Bind(item, properties);
        end               
        
        % bind an object to the property grid with a custom property list.
        function self = Bind(self, item, properties)
            self.Properties = properties;
            self.BoundItem = item;
        end
    end
    
    
    methods (Access = private)
        % called via OnPropertyChange events; updates all related fields (e.g. PropertyGridField values, etc.)
        % cko/TODO: this is where we can implement the replaceChildren action! :-)
        function UpdateField(self, name)
            field = self.Fields.FindByName(name);
            value = field.Value;
            if field.CanAccept(value)
                try
                    if ~isempty(self.BoundItem)
                        self.BoundItem = nestedassign(self.BoundItem, name, value); end                    

                    % +++ cko: support for alternatives
                    try
                        has_alternatives = ~isempty(field.PropertyData.Specification.alternatives);
                    catch
                        has_alternatives = false;
                    end
                        
                    if has_alternatives && ~isequal(field.PropertyData.Value,value)
                        curRow = self.Table.getSelectedRow();
                        javaMethodEDT('expandRow',self.Table,curRow,false);
                        % value changed for a property with alternative chilren sets: relink!
                        field.Value = value;
                        javaMethodEDT('expandRow',self.Table,curRow,true);
                    end
                    field.PropertyData.Value = value;                    
                    % --- cko: support for alternatives
                    
                    self.UpdateDependentProperties(field);
                catch me
                    % revert changes
                    field.Value = field.PropertyData.Value;
                    self.Table.repaint();
                    rethrow(me);
                end
            else
                % the user has entered illegal things: revert changes
                field.Value = field.PropertyData.Value;
                self.Table.repaint();
            end
        end
        
        % dependent properties have setters that may have significant side affects throughout other fields of a structure/object, etc.
        function UpdateDependentProperties(self, field)
            if isempty(self.BoundItem)
                return; end
            % dependent property set; requery all properties as the dependent property might have changed the value of any of them
            if field.PropertyData.Dependent  
                for k = 1 : numel(self.Fields)
                    f = self.Fields(k);
                    if f ~= field
                        value = nestedfetch(self.BoundItem, f.PropertyData.Name);  % query dependent property value
                        f.Value = value;
                        f.PropertyData.Value = value;
                    end
                end
                self.Table.repaint();
            else  % requery dependent properties only
                dependent = getdependentproperties(self.BoundItem);  % a cell array of dependent property names
                if ~isempty(dependent)
                    for k = 1 : numel(dependent)
                        name = dependent{k};
                        value = nestedfetch(self.BoundItem, name);  % query dependent property value
                        field = self.Fields.FindByName(name);
                        field.Value = value;               % update value displayed in grid
                        field.PropertyData.Value = value;  % update value stored internally
                    end
                    self.Table.repaint();
                end
            end
        end
        
        % helper: bring up the custom matrix editor for a field (in .notation)
        function EditMatrix(self, name)
            field = self.Fields.FindByName(name);
            if ~isnumeric(field.PropertyData.Type) && ~islogical(field.PropertyData.Type) || ~is2d(field.PropertyData.Type)
                return; end
            self.Table.editingStopped(javax.swing.event.ChangeEvent(self.Table));  % commit value to edited cell
            fig = figure( ...
                'DockControls', 'off', ...
                'MenuBar', 'none', ...
                'Name', sprintf('Edit matrix "%s"', name), ...
                'NumberTitle', 'off', ...
                'Toolbar', 'none');
            editor = MatrixEditor(fig, ...
                'ReadOnly', field.PropertyData.ReadOnly, ...
                'Type', field.PropertyData.Type, ...
                'Item', field.PropertyData.Value);
            uiwait(fig);
            field.Value = editor.Item;
            self.UpdateField(name);
        end
    end
    
    methods (Access = private, Static)
        % find the object property grid (com.jidesoft.grid.DefaultProperty) that contains the given field.
        function self = FindPropertyGrid(obj, member)
            validateattributes(member, {'char'}, {'nonempty','row'});
            % find which PropertyGrid contains the object for which the callback is executing
            h = findobjuser(@(userdata) userdata.(member) == obj, '__PropertyGrid__');
            self = get(h, 'UserData');
        end

        % get the name of the selected property (of the given object...) - for key events
        function name = GetSelectedProperty(obj)            
            selectedfield = obj.getSelectedProperty();
            if isempty(selectedfield)
                name = [];
            else
                name = char(selectedfield.getFullName());
            end
        end
    end
    
    
    methods (Static)  % methods (Access = private, Static) for MatLab 2010a and up
        % property change event handler -- redirects to UpdateField
        function OnPropertyChange(obj, event)
            self = PropertyGrid.FindPropertyGrid(obj, 'Model');
            name = get(event, 'PropertyName');  % JIDE automatically uses a hierarchical naming scheme
            self.UpdateField(name);

            if 0  % debug mode
                oldvalue = var2str(get(event, 'OldValue'));  % old value as a string
                newvalue = var2str(get(event, 'NewValue'));  % new value as a string
                fprintf('Property value of "%s" has changed.\n', name);
                fprintf('Old value: %s\n', oldvalue);
                fprintf('New value: %s\n', newvalue);
            end
        end
        
        % key press handler; used to implement F1 and F2...
        function OnKeyPressed(obj, event)
            key = char(event.getKeyText(event.getKeyCode()));
            switch key
                case 'F1'
                    name = PropertyGrid.GetSelectedProperty(obj);
                    self = PropertyGrid.FindPropertyGrid(obj, 'Table');
                    if ~isempty(name) && ~isempty(self.BoundItem)  % help
                        nameparts = strsplit(name, '.');
                        if numel(nameparts) > 1
                            helpobject = nestedfetch(self.BoundItem, strjoin('.', nameparts(1:end-1)));
                        else
                            helpobject = self.BoundItem;
                        end
                        helpdialog([class(helpobject) '.' nameparts{end}]);
                    end
                case 'F2'
                    name = PropertyGrid.GetSelectedProperty(obj);
                    if ~isempty(name)  % edit property value
                        self = PropertyGrid.FindPropertyGrid(obj, 'Table');
                        self.EditMatrix(name);
                    end
            end
        end
    end
end
