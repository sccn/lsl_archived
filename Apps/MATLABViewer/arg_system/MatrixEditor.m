% Displays a matrix in a pop-up window for convenient editing.
% This control presents an m-by-n table of matrix entries.

% Copyright 2008-2010 Levente Hunyadi
classdef MatrixEditor < UIControl
    properties
        Type = PropertyType.empty(1,0);
    end
    properties (Dependent)
        Control;
        Item;
        % Whether to show transpose of the matrix instead of actual matrix.
        Transpose;
        % Whether matrix is read-only.
        ReadOnly;
    end
    properties (Access = protected)
        MatrixTranspose = false;
        Matrix;
        Table;
        JideTableModel;
        JideTable;
    end
    properties (Access = private)
        Menu;
    end
    methods
        function self = MatrixEditor(varargin)
            self = self@UIControl(varargin{:});
        end
        
        function self = Instantiate(self, parent)
            if nargin < 2
                parent = figure;
            end

            % container panel
            panel = uipanel(parent, ...
                'Units', 'normalized', ...
                'Position', [0 0 1 1], ...
                'Tag', '__MatrixEditor__', ...
                'UserData', self);
            
            % initialization
            com.mathworks.mwswing.MJUtilities.initJIDE;
            com.jidesoft.grid.CellRendererManager.initDefaultRenderer();
            com.jidesoft.grid.CellEditorManager.initDefaultEditor();
            
            % table model
            self.JideTableModel = handle(javax.swing.table.DefaultTableModel(), 'CallbackProperties');
            set(self.JideTableModel, 'TableChangedCallback', @MatrixEditor.OnTableChanged);
            
            % JIDE table grid control
            self.JideTable = objectEDT('com.jidesoft.grid.ContextSensitiveTable', self.JideTableModel);
            self.JideTable.setCellSelectionEnabled(true);

            pixelpos = getpixelposition(panel);
            [control,self.Table] = javacomponent(self.JideTable, [10 10 pixelpos(3)-20 pixelpos(4)-20], panel); %#ok<ASGLU>
            set(self.Table, ...
                'Units', 'normalized');
            
            % menu
            menu = uimenu(ancestor(parent, 'figure'), ...
                'Label', 'Matrix', ...
                'Callback', @self.OnMatrixMenuClicked);
            self.Menu = struct( ...
                'ShowTranspose', uimenu(menu, 'Label', 'Show transpose', 'Callback', @self.OnShowTranspose), ...
                'AddColumnBefore', uimenu(menu, 'Label', 'Add column before', 'Callback', @self.OnAddColumnBefore), ...
                'AddColumnAfter', uimenu(menu, 'Label', 'Add column after', 'Callback', @self.OnAddColumnAfter), ...
                'AddRowBefore', uimenu(menu, 'Label', 'Add row before', 'Callback', @self.OnAddRowBefore), ...
                'AddRowAfter', uimenu(menu, 'Label', 'Add row after', 'Callback', @self.OnAddRowAfter), ...
                'RemoveColumn', uimenu(menu, 'Label', 'Remove column', 'Callback', @self.OnRemoveColumn), ...
                'RemoveRow', uimenu(menu, 'Label', 'Remove row', 'Callback', @self.OnRemoveRow));
            self.OnMatrixMenuClicked();
        end
        
        function control = get.Control(self)
            control = self.Table;
        end
        
        function t = get.Transpose(self)
            t = self.MatrixTranspose;
        end
        
        function self = set.Transpose(self, t)
            validateattributes(t, {'logical'}, {'scalar'});
            item = self.Item;
            self.MatrixTranspose = t;
            self.Item = item;
        end
        
        function r = get.ReadOnly(self)
            r = ~self.JideTable.isEnabled();
        end
        
        function self = set.ReadOnly(self, r)
            validateattributes(r, {'logical'}, {'scalar'});
            self.JideTable.setEnabled(~r);
        end
        
        function matrix = get.Item(self)
            matrix = self.Matrix;
            if self.MatrixTranspose
                matrix = transpose(matrix);
            end
        end
        
        function self = set.Item(self, matrix)
            validateattributes(matrix, {'numeric','logical'}, {'2d'});
            if self.MatrixTranspose
                matrix = transpose(matrix);
            end
            self.Matrix = matrix;
            if isempty(self.Type)
                self.Type = PropertyType.AutoDiscover(matrix);
            end
            data = self.Type.GetJavaVectorOfVectors(matrix);
            if size(matrix,2) > 0
                list = java.util.Arrays.asList(javaArray('java.lang.String', size(matrix,2)));  % no labels
                labels = java.util.Vector(list);
            else
                labels = java.util.Vector();
            end
            self.JideTableModel.setDataVector(data, labels);
            self.JideTable.doLayout();
        end

        function self = set.Type(self, type)
            validateattributes(type, {'PropertyType'}, {'scalar'});
            self.Type = type;
        end
        
        function item = GetItemAsString(self)
            validateattributes(self, {'MatrixEditor'}, {'scalar'});
            matrix = self.Matrix;
            if self.MatrixTranspose
                matrix = transpose(matrix);
            end
            item = mat2str(matrix);
        end
    end
    methods (Static, Access = private)
        function self = FindEditor(obj)
        % Finds the editor object to which a table model object belongs.
        %
        % Input arguments:
        % obj:
        %    a javax.swing.table.DefaultTableModel instance
            h = findobjuser(@(userdata) userdata.JideTableModel == obj, '__MatrixEditor__');  % find which HGO contains the object for which the callback is executing
            self = get(h, 'UserData');
        end
    end
    methods (Static)
        function OnTableChanged(source, event)
        % Fired when a matrix entry has been changed.
            javarow = event.getFirstRow();
            javacol = event.getColumn();
            if javarow < 0 || javacol < 0 || event.getType() ~= javax.swing.event.TableModelEvent.UPDATE
                return;  % no selection
            end
            self = MatrixEditor.FindEditor(source);
            row = javarow + 1;
            col = javacol + 1;
            
            entryvalue = source.getValueAt(javarow, javacol);
            value = entryvalue;  % represented as-is by default
            if ischar(entryvalue)  % represented as text
                if isnumeric(self.Type)
                    value = str2double(entryvalue);
                    if isnan(value)
                        value = [];
                    end
                elseif islogical(self.Type)
                    switch entryvalue
                        case 'true'
                            value = true;
                        case 'false'
                            value = false;
                        otherwise
                            value = str2double(entryvalue);
                            if isnan(value)
                                value = [];
                            end
                    end
                end
            end
            if ~isempty(value)
                try
                    value = self.Type.ConvertFromMatLab(value);
                    self.Matrix(row,col) = value;  % persist changes
                catch %#ok<CTCH>
                    % no action
                end
            end
            if ischar(entryvalue)
                value = self.Matrix(row,col);
                javavalue = self.Type.GetPrimitiveJavaValue(value);
                if javavalue ~= source.getValueAt(javarow, javacol)  % do not trigger change event if value to set is equivalent to value in cell
                    source.setValueAt(javavalue, javarow, javacol);
                end
            end
        end
    end        
    methods (Access = private)
        function self = OnMatrixMenuClicked(self, source, event) %#ok<INUSD>
        % Fired when the Matrix menu (or the context menu) is to be shown.
            if ~isempty(self.Type) && ~strcmp(self.Type.Shape, 'column')
                addcolstate = 'on';
            else
                addcolstate = 'off';
            end
            if ~isempty(self.Type) && ~strcmp(self.Type.Shape, 'row')
                addrowstate = 'on';
            else
                addrowstate = 'off';
            end
            if ~isempty(self.Type) && ~strcmp(self.Type.Shape, 'column') ...  % cannot remove single column for column vector
                    && size(self.Matrix,2) > 0 ...     % check if there is anything to remove
                    && ~isempty(self.GetSelectedColumn())
                remcolstate = 'on';
            else
                remcolstate = 'off';
            end
            if ~isempty(self.Type) && ~strcmp(self.Type.Shape, 'row') ...  % cannot remove single row for row vector
                    && size(self.Matrix,1) > 0 ...  % check if there is anything to remove
                    && ~isempty(self.GetSelectedRow())
                remrowstate = 'on';
            else
                remrowstate = 'off';
            end
            set(self.Menu.AddColumnBefore, 'Enable', addcolstate);
            set(self.Menu.AddColumnAfter, 'Enable', addcolstate);
            set(self.Menu.AddRowBefore, 'Enable', addrowstate);
            set(self.Menu.AddRowAfter, 'Enable', addrowstate);
            set(self.Menu.RemoveColumn, 'Enable', remcolstate);
            set(self.Menu.RemoveRow, 'Enable', remrowstate);
        end
        
        function self = OnShowTranspose(self, source, event) %#ok<INUSD>
            self.Transpose = ~self.Transpose;  % call setter method
        end
        
        function self = OnAddColumnBefore(self, source, event) %#ok<INUSD>
            index = self.GetSelectedColumn();
            if isempty(index)
                index = 0;
            end
            self.AddColumnAt(index);  % index ranges from 0 to column count-1
        end

        function self = OnAddColumnAfter(self, source, event) %#ok<INUSD>
            index = self.GetSelectedColumn();
            if ~isempty(index)
                self.AddColumnAt(index+1);  % index+1 ranges from 1 to column count
            else
                self.AddColumnAt(size(self.Matrix,2));
            end
        end

        function self = OnAddRowBefore(self, source, event) %#ok<INUSD>
            index = self.GetSelectedRow();
            if isempty(index)
                index = 0;
            end
            self.AddRowAt(index);  % index ranges from 0 to row count-1
        end
        
        function self = OnAddRowAfter(self, source, event) %#ok<INUSD>
            index = self.GetSelectedRow();
            if ~isempty(index)
                self.AddRowAt(index+1);  % index+1 ranges from 1 to row count
            else
                self.AddRowAt(size(self.Matrix,1));
            end
        end
        
        function self = OnRemoveColumn(self, source, event) %#ok<INUSD>
            index = self.GetSelectedColumn();  % index ranges from 0 to column count-1
            if isempty(index)
                return;
            end
            matrix = self.Matrix;
            matrix(:,index+1) = [];
            self.Matrix = matrix;
            data = self.JideTableModel.getDataVector();
            for k = 0 : size(matrix,1)-1
                row = data.elementAt(k);
                row.removeElementAt(index);
            end
            self.JideTableModel.setColumnCount(size(matrix,2));
            self.JideTable.repaint();
        end

        function self = OnRemoveRow(self, source, event) %#ok<INUSD>
            index = self.GetSelectedRow();  % index ranges from 0 to row count-1
            if isempty(index)
                return;
            end
            matrix = self.Matrix;
            matrix(index+1,:) = [];
            self.Matrix = matrix;
            self.JideTableModel.removeRow(index);
            self.JideTable.repaint();
        end
        
        function index = GetSelectedColumn(self)
        % Returns the currently selected column.
        %
        % Output arguments:
        % index:
        %    a zero-based index
            index = self.JideTable.getSelectedColumn();
            if index < 0  % no row is selected
                index = [];
            end
        end
        
        function index = GetSelectedRow(self)
        % Returns the currently selected row.
        %
        % Output arguments:
        % index:
        %    a zero-based index
            index = self.JideTable.getSelectedRow();
            if index < 0  % no row is selected
                index = [];
            end
        end

        function self = AddColumnAt(self, index)
        % Add new column at the specified index.
        % Items whose index is greater than the specified index are shifted
        % to the right.
        %
        % Input argument:
        % index:
        %    an insertion index between 0 (to insert as first column) and
        %    size(self.Matrix,2) (to insert as last column)
            matrix = self.Matrix;
            if isempty(matrix)
                ix1 = [];
                ix2 = [];
            else
                ix1 = 1 : index;
                ix2 = index+1 : size(matrix,2);
            end
            matrix = [ matrix(:,ix1) , zeros(size(matrix,1),1) , matrix(:,ix2) ];
            self.Matrix = matrix;
            data = self.JideTableModel.getDataVector();
            for k = 0 : size(matrix,1)-1
                row = data.elementAt(k);
                row.insertElementAt(self.Type.GetPrimitiveJavaValue(0), index);
            end
            self.JideTableModel.setColumnCount(size(matrix,2));
            self.JideTable.repaint();
        end
        
        function self = AddRowAt(self, index)
        % Add new row at the specified index.
        % Items whose index is greater than the specified index are shifted
        % downwards.
        %
        % Input argument:
        % index:
        %    an insertion index between 0 (to insert as first row) and
        %    size(self.Matrix,1) (to insert as last row)
            matrix = self.Matrix;
            if isempty(matrix)
                ix1 = [];
                ix2 = [];
            else
                ix1 = 1 : index;
                ix2 = index+1 : size(matrix,1);
            end
            matrix = [ matrix(ix1,:) ; zeros(1,size(matrix,2)) ; matrix(ix2,:) ];
            self.Matrix = matrix;
            data = self.Type.GetJavaVector(zeros(1,size(matrix,2)));
            self.JideTableModel.insertRow(index, data);
            self.JideTable.repaint();
        end
    end
end