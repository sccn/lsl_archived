% A property editor for custom objects.
% The property editor combines a dropdown list that the contains object
% instances with a property grid that lets the user inspect and modify
% the property values of the selected instance.
%
% See also: PropertyGrid

% Copyright 2008-2010 Levente Hunyadi
classdef PropertyEditor < UIControl
    properties (Dependent)
        Control;
        % A cell array of the items selectable in the property editor.
        Items;
        % The caption displayed at the top of the propety editor.
        Title;
        % The item currently selected in the dropdown list.
        SelectedItem;
        % The index of the item currently selected in the dropdown list.
        SelectedIndex;
    end
    properties (Access = protected)
        ItemsList;
        % A handle graphics uipanel.
        Panel;
        % A handle graphics uicontrol combo box.
        ItemsListBox;
        % A PropertyGrid instance.
        Grid;
    end
    properties (Access = private)
        ItemsListIndex;
    end
    methods
        function self = PropertyEditor(varargin)
            self = self@UIControl(varargin{:});
            if ~isempty(self.ItemsList)
                self.Grid.Item = self.ItemsList{1};
            end
        end
        
        function self = Instantiate(self, parent)
            if nargin > 1
                self.Panel = uipanel(parent);
            else
                self.Panel = uipanel();
            end
            set(self.Panel, 'ResizeFcn', @self.OnResize);
            self.ItemsListBox = uicontrol(self.Panel, ...
                'Units', 'normalized', ...
                'Position', [0, 0.8, 1, 0.2], ...
                'String', {'[empty]'}, ...
                'Style', 'popupmenu', ...
                'Callback', @self.OnSelectedIndexChanged);
            self.Grid = PropertyGrid(self.Panel, ...
                'Units', 'normalized', ...
                'Position', [0, 0, 1, 0.8]);
        end

        function control = get.Control(self)
            control = self.Panel;
        end
        
        function title = get.Title(self)
            title = set(self.Panel, 'Title');
        end
        
        function self = set.Title(self, title)
            if ~isempty(title)
                validateattributes(title, {'char'}, {'row'});
            else
                title = [];
            end
            set(self.Panel, 'Title', title);
        end
        
        function item = get.Items(self)
            item = self.ItemsList;
        end
        
        function self = set.Items(self, items)
            validateattributes(items, {'cell'}, {'vector'});
            self.SelectedIndex = [];
            items = reshape(items, 1, numel(items));  % row vector
            self.ItemsList = items;
            
            if ~isempty(items)
                % explore object instances
                names = cell(1, length(items));
                for k = 1 : length(items)
                    item = items{k};
                    try
                        name = char(item);
                    catch %#ok<CTCH>
                        name = class(item);
                    end
                    names{k} = name;
                end
            else
                names = {};
            end
            set(self.ItemsListBox, 'String', [{'[empty]'} names]);
        end
        
        function item = get.SelectedItem(self)
            if isempty(self.ItemsList)
                item = [];
                return;
            end
            
            index = self.SelectedIndex;
            if ~isempty(index)
                item = self.ItemsList{index};
            else
                item = [];  % nothing is selected
            end
        end

        function self = set.SelectedItem(self, item)
            for k = 1 : length(self.ItemsList)
                if item == self.ItemsList{k}  % test for object equality
                    self.SelectedIndex = k;
                    return;
                end
            end

            % item not found in list of items
            self.SelectedIndex = [];  % no item is selected
        end
        
        function index = get.SelectedIndex(self)
            if ~isempty(self.ItemsList)
                index = self.GetItemsListIndex();
            else
                index = [];  % items list has only single placeholder item
            end
        end
        
        function self = set.SelectedIndex(self, newindex)
            if isempty(self.ItemsList)
                self.ItemsListIndex = [];
                self.Grid.Item = [];
                return;
            end

            oldindex = self.ItemsListIndex;

            % persist previously selected item if any
            if ~isempty(oldindex)
                item = self.Grid.Item;
                self.ItemsList{oldindex} = item;  % this has no effect for reference objects but necessary for value objects
            end

            if ~isempty(newindex)
                item = self.ItemsList{newindex};  % get item that corresponds to selected index
                self.Grid.Item = item;            % populate property grid
            else
                self.Grid.Item = [];
            end

            self.ItemsListIndex = newindex;
            self.SetItemsListIndex(newindex);
        end

        function AddItem(self, item, index)
        % Add a new item to the property editor item list.
        %
        % Input arguments:
        % index (optional):
        %    the index of the list item after which the new item is to be
        %    inserted, use 0 to insert as the first element; if missing,
        %    new item is appended to the end
            if nargin < 3
                index = numel(self.ItemsList);
            else
                self.CheckAddIndexBounds(index);
            end
            self.Items = [ self.Items(1:index) {item} self.Items(index+1:end) ];
        end
        
        function AddItems(self, items, index)
        % Add a cell array of new items to the property editor item list.
        %
        % Input arguments:
        % index (optional):
        %    the index of the list item after which the new item is to be
        %    inserted, use 0 to insert as the first element; if missing,
        %    new item is appended to the end
            validateattributes(items, {'cell'}, {'vector'});
            if nargin < 3
                index = numel(self.Items);
            else
                self.CheckAddIndexBounds(index);
            end
            items = reshape(items, 1, numel(items));
            self.Items = [ self.Items(1:index) items self.Items(index+1:end) ];
        end
        
        function RemoveItem(self, index)
            validateattributes(index, {'numeric'}, {'nonnegative','integer','scalar'});
            assert(index >= 0 && index <= numel(self.Items), ...
                'PropertyEditor:OutOfBounds', ...
                'Index %d is out of bounds, should be between 1 and %d.', index, numel(self.Items));
            self.Items = [ self.Items(1:index-1) self.Items(index+1:end) ];
        end
        
        function RemoveItems(self, index)
            if nargin < 2
                self.Items = {};
                return;
            end
            validateattributes(index, {'numeric'}, {'nonnegative','integer','vector'});
            assert(all(index >= 1 & index <= numel(self.Items)), ...
                'PropertyEditor:OutOfBounds', ...
                'Index is out of bounds, should be between 1 and %d.', numel(self.Items));
            f = true(1, numel(self.Items));  % elements to keep
            f(index) = false;                % elements to drop
            self.Items = self.Items(f);
        end
    end
    methods (Access = private)
        function CheckAddIndexBounds(self, index)
            validateattributes(index, {'numeric'}, {'nonnegative','integer','scalar'});
            assert(index >= 0 && index <= numel(self.ItemsList), ...
                'PropertyEditor:OutOfBounds', ...
                'Index %d is out of bounds, should be between 0 (insert as first) and %d (insert as last).', index, numel(self.ItemsList));
        end
        
        function index = GetItemsListIndex(self)
            index = get(self.ItemsListBox, 'Value') - 1;
            if index == 0
                index = [];
            end
        end
        
        function SetItemsListIndex(self, index)
            if ~isempty(index)
                set(self.ItemsListBox, 'Value', index + 1);
            else
                set(self.ItemsListBox, 'Value', 1);  % no selection
            end
        end

        function self = OnSelectedIndexChanged(self, source, event) %#ok<INUSD>
            if isempty(self.ItemsList)
                return;
            end
            
            % get index of selected item
            index = self.GetItemsListIndex();
            if ~isempty(index) && ~isempty(self.ItemsListIndex) && index == self.ItemsListIndex ...
                    || isempty(index) && isempty(self.ItemsListIndex)
                return;
            end

            if ~isempty(index)
                self.SelectedIndex = index;
            else
                self.SelectedIndex = [];
            end
        end
        
        function self = OnResize(self, source, event) %#ok<INUSD>
            pixelpos = getpixelposition(self.Panel);
            width = pixelpos(3);
            height = pixelpos(4);
            padding = 10;
            comboheight = 25;
            setpixelposition(self.ItemsListBox, [padding, height - comboheight - 2*padding, width - 2*padding, comboheight]);
            targetwidth = width - 2*padding;
            if targetwidth < 10
                targetwidth = 10;
            end
            targetheight = height - comboheight - 3*padding;
            if targetheight < 10
                targetheight = 10;
            end
            setpixelposition(self.Grid, [padding, padding, targetwidth, targetheight]);
        end
    end
end
