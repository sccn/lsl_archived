% Type description for a property. 
% Supports everything except for function handles, generic cell arrays, and structs of any kind.
%
% Offers the following functionality:
%   * value/type/shape assignability test (CanAccept*)
%   * value conversion (Matlab <-> Java, type-guided)
%   * auto-discovery from values (of type/shape)
%   * type queries (isnumeric, etc)
%   * setters (Type,Shape,Domain)
%
% Notes:
%   A reason to not have structures here is that they are supported by the PropertyGridField hierarchy, anyway...
%   
%
% Copyright 2010 Levente Hunyadi

classdef PropertyType    
    properties        
        PrimitiveType = 'denserealdouble';      % The underlying per-element MATLAB type; string.
                                                % supports every primitive type, and excludes function_handle, generic cell, struct
                                                
        Shape = 'scalar';                       % Allowed array dimensions (empty,scalar,row,column,matrix)
        
        Domain = [];                            % Valid domain (a subset of the type):
                                                %  * [] = unrestricted
                                                %  * {elem,elem,elem, ...} = enumeration of discrete domain
                                                %  * [lo hi] = numeric range,inclusive        
    end    
    properties (Access = protected)
        ObjectType = '';                        % this keeps the object type namespace separate from the primitive type namespace...
    end
    
    methods

        
        % --- CORE OPERATIONS ---

        % constructor        
        function self = PropertyType(type, shape, domain)
            self.PrimitiveType = type;
            self.Shape = shape;
            if nargin > 2
                self.Domain = domain; end
        end

        % whether the given native MATLAB value can be represented by the type (by default without precision loss)
        function tf = CanAccept(self, value, precisionloss)
            if ~isscalar(self)
                error('This operation can only be applied to scalar PropertyTypes'); end
            if nargin < 3
                precisionloss = false; end
            if isempty(value)
                tf = true;
            elseif isobject(value)
                tf = (strcmp(self.PrimitiveType,'object') && isempty(self.ObjectType)) || (~isempty(self.ObjectType) && metaclass(value) <= meta.class.fromName(self.ObjectType));
            elseif (isnumeric(value) || islogical(value) || ischar(value)) && any(isnan(value(:)));
                tf = false; % cko: why is NaN disallowed?
            else
                % cko: note - there may be problems with [] here, so it is safer to supply an appropriately-typed [] here
                tf = PropertyType.CanAcceptType(self.PrimitiveType, PropertyType.AutoDiscoverType(value), precisionloss) ...
                     && PropertyType.CanAcceptShape(self.Shape, PropertyType.AutoDiscoverShape(value)) ...
                     && self.CanAcceptValue(value);
            end
        end       
        
        
        % --- CORE SETTERS ---
        
        function self = set.PrimitiveType(self, type)
            if ~any(strcmp(type,{ ...
                'logical', 'cellstr', 'char', 'object',...
                'int8','uint8','int16','uint16','int32','uint32','int64','uint64', ...
                'denserealsingle','denserealdouble', ...
                'densecomplexsingle','densecomplexdouble', ...
                'sparserealsingle','sparserealdouble', ...
                'sparsecomplexsingle','sparsecomplexdouble'}))
                error(['Unsupported type: ' type]); end
            self.PrimitiveType = type;
        end
        
        function self = set.Shape(self, shape)
            if ~any(strcmp(shape,{'scalar','row','column','matrix','empty'}))
                error(['Unsupported shape: ' shape]); end
            self.Shape = shape;
        end
        
        function self = set.Domain(self, domain)
            % do a few error checks...
            if isempty(domain)
                % okay
            elseif ~isscalar(self) && ~isstring(self) && ~(islogical(self) && isvector(self))
                error('PropertyType:InvalidArgumentValue', ...
                    'Domain can be set only for scalars, strings or logical vectors.');
            elseif iscell(domain)
                if min(size(domain)) ~= 1
                    error('The domain must be a vector-shaped cell aray.'); end
                if ~(islogical(self) && isvector(self))  % interpret as exhaustive enumeration of domain elements
                    typ = struct('PrimitiveType',{self.PrimitiveType},'Shape',{self.Shape},'Domain',{self.Domain});                                    
                    for k = 1 : numel(domain)
                        hlp_microcache('propaccept',@check_accept_value,typ,domain{k},class(domain{k})); end
                end
            elseif isnumeric(domain)  % interpret as numeric range for domain elements
                if min(size(domain)) ~= 1
                    error('The domain must be vector-shaped.'); end
                assert(isnumeric(self), 'PropertyType:ArgumentTypeMismatch', ...
                    'Closed interval domain is meaningful only for numeric data.');
                assert(length(domain) == 2, 'PropertyType:InvalidArgumentValue', ...
                    'Closed interval data is expected in the form [a,b] where a and b are numbers.');
                domain = [min(domain) max(domain)];
            else
                error('PropertyType:InvalidArgumentValue', ...
                    'Closed interval specification [a,b] or explicit enumeration {a,b,c,...} is expected.');
            end
            self.Domain = domain;
        end

        
        % --- TYPE-GUIDED CONVERSIONS ---

        % convert type to the corresponding MATLAB class
        function clazz = GetPrimitiveMatLabType(self)
            if ~isscalar(self)
                error('This operation can only be applied to scalar objects.'); end
            switch (self.PrimitiveType)
                case {'logical','char','int8','uint8','int16','uint16','int32','uint32','int64','uint64'}
                    clazz = self.PrimitiveType;
                case {'denserealsingle','densecomplexsingle','sparserealsingle','sparsecomplexsingle'}
                    clazz = 'single';
                case {'denserealdouble','densecomplexdouble','sparserealdouble','sparsecomplexdouble'}
                    clazz = 'double';
                case 'cellstr'
                    clazz = 'cell';
            end
        end

        % convert type to a MATLAB class that can then be faithfully converted to a Java object of matching type
        function javatype = GetPrimitiveJavaType(self)
            if ~isscalar(self)
                error('This operation can only be applied to scalar objects.'); end
            switch self.PrimitiveType
                case {'denserealdouble','sparserealdouble'}
                    matlabtype = 'double';
                case {'denserealsingle','sparserealsingle'}
                    matlabtype = 'single';
                case {'int8','uint8','int16'}
                    matlabtype = 'int16';
                case {'uint16','int32'}
                    matlabtype = 'int32';
                case {'uint32','int64'}
                    matlabtype = 'int64';
                case 'logical'
                    matlabtype = 'logical';
                case {'densecomplexdouble','sparsecomplexdouble','densecomplexsingle','sparsecomplexsingle'}
                    matlabtype = 'cellstr';
                otherwise
                    error('MatrixEditor:ArgumentTypeMismatch', 'Type %s is not supported.', self.PrimitiveType);
            end
            javatype = javaclass(matlabtype);
        end
        
        % convert given scalar value to a corresponding Java object
        function javavalue = GetPrimitiveJavaValue(self, value)
            if isempty(value)
                javavalue = java.lang.String();
            else
                switch self.PrimitiveType
                    case {'denserealdouble','sparserealdouble'}
                        javavalue = java.lang.Double(value);
                    case {'denserealsingle','sparserealsingle'}
                        javavalue = java.lang.Float(value);
                    case {'int8','uint8','int16'}
                        javavalue = java.lang.Short(value);
                    case {'uint16','int32'}
                        javavalue = java.lang.Integer(value);
                    case {'uint32','int64'}
                        javavalue = java.lang.Long(value);
                    case 'logical'
                        javavalue = java.lang.Boolean(value);
                    case {'densecomplexdouble','sparsecomplexdouble','densecomplexsingle','sparsecomplexsingle'}
                        javavalue = java.lang.String(mat2str(value));
                    otherwise
                        if isnumeric(value) || islogical(value)
                            javavalue = java.lang.String(mat2str(value));
                        else
                            javavalue = java.lang.String(char(value));
                        end
                end
            end
        end

        % convert given array to Java Vector of Vectors
        function javamatrix = GetJavaVectorOfVectors(self, matrix)
            assert(isnumeric(self) || islogical(self), 'PropertyType:InvalidOperation', ...
                'Operation supported on numeric and logical matrices only.');
            assert(ndims(matrix) < 3, 'PropertyType:InvalidOperation', ...
                'More than 2 dimensions are not supported.');
            javamatrix = java.util.Vector(int32(size(matrix,1)));
            for m = 1 : size(matrix,1)
                javarow = java.util.Vector(int32(size(matrix,2)));
                javamatrix.add(javarow);
                for n = 1 : size(matrix,2)
                    javarow.add(self.GetPrimitiveJavaValue(matrix(m,n)));
                end
            end
        end
        
        % convert given array to native Java 2D array
        function javamatrix = GetJavaMatrix(self, matrix)
            assert(isnumeric(self), 'PropertyType:InvalidOperation', ...
                'Operation supported on numeric matrices only.');
            assert(numel(matrix) > 0, 'PropertyType:InvalidOperation', ...
                'Empty values are not supported.');
            assert(ndims(matrix) < 3, 'PropertyType:InvalidOperation', ...
                'More than 2 dimensions are not supported.');
            javatype = self.GetPrimitiveJavaType();
            javamatrix = javaArray(char(javatype.getName()), size(matrix,1), size(matrix,2));
            for m = 1 : size(matrix,1)
                for n = 1 : size(matrix,2)
                    javamatrix(m,n) = self.GetPrimitiveJavaValue(matrix(m,n));
                end
            end
        end

        % convert given array to Java native 1D array
        function javavector = GetJavaVector(self, vector)
            assert(isnumeric(self), 'PropertyType:InvalidOperation', ...
                'Operation supported on numeric matrices only.');
            assert(numel(vector) > 0, 'PropertyType:InvalidOperation', ...
                'Empty values are not supported.');
            javatype = self.GetPrimitiveJavaType();
            javavector = javaArray(char(javatype.getName()), numel(vector));
            for k = 1 : numel(vector)
                javavector(k) = self.GetPrimitiveJavaValue(vector(k));
            end
        end

        % convert given value to something that can be represented by Java (if in doubt, this will be a string)
        function javavalue = ConvertToJava(self, value)
            switch self.Shape
                case 'scalar'
                    javavalue = self.GetPrimitiveJavaValue(value);
                case {'row','column'}
                    switch self.PrimitiveType
                        case 'char'  % add a string property
                            if strcmp(self.Shape, 'row')
                                javavalue = value;
                            else
                                javavalue = mat2str(value);
                            end
                        case 'cellstr'
                            if ~isempty(value)
                                javavalue = java.lang.String(strjoin(sprintf('\n'), value));
                            else
                                javavalue = java.lang.String();
                            end
                        case 'logical'
                            if ~isempty(self.Domain)
                                if ~any(value)
                                    javavalue = [];
                                else
                                    javavalue = javaStringArray(self.Domain(value));  % value is an indicator vector
                                end
                            else
                                javavalue = mat2str(value);
                            end
                        otherwise
                            if isnumeric(value) || islogical(value)
                                javavalue = mat2str(value);
                            else
                                javavalue = char(value);
                            end
                    end
                case 'empty'
                    javavalue = [];
                otherwise
                    if isnumeric(value) || islogical(value)
                        javavalue = mat2str(value);
                    else
                        javavalue = char(value);
                    end
            end
        end

        % convert Java object into MATLAB object of appropriate type (domain is ignored)
        function [value,stringconversion] = ConvertFromJava(self, javavalue)
            stringconversion = false;
            switch self.Shape
                case 'scalar'
                    switch self.PrimitiveType
                        case 'denserealdouble'
                            value = full(double(javavalue));
                        case 'sparserealdouble'
                            value = sparse(double(javavalue));
                        case 'denserealsingle'
                            value = full(single(javavalue));
                        case 'sparserealsingle'
                            value = sparse(single(javavalue));
                        case {'int8','uint8','int16','uint16','int32','uint32','int64','logical'}
                            value = cast(javavalue, self.PrimitiveType);
                        case {'densecomplexdouble','sparsecomplexdouble','densecomplexsingle','sparsecomplexsingle'}
                            value = self.ConvertFromString(javavalue);
                            stringconversion = true;
                        otherwise
                            error('Can''t handle that');
                    end
                case {'row','column'}
                    switch self.PrimitiveType
                        case 'char'  % add a string property
                            if strcmp(self.Shape, 'row')
                                value = char(javavalue);
                            else
                                value = self.ConvertFromString(javavalue);
                                stringconversion = true;
                                return;
                            end
                        case 'cellstr'
                            value = strsplit(javavalue);
                        case 'logical'
                            if ~isempty(self.Domain)
                                value = strsetmatch(cell(javavalue), self.Domain);
                            else
                                value = self.ConvertFromString(javavalue);
                                stringconversion = true;
                                return;
                            end
                        case {...
                                'denserealdouble','sparserealdouble','denserealsingle','sparserealsingle',...
                                'densecomplexdouble','sparsecomplexdouble','densecomplexsingle','sparsecomplexsingle',...
                                'int8','uint8','int16','uint16','int32','uint32','int64','uint64'}
                            value = self.ConvertFromString(javavalue);
                            stringconversion = true;
                        otherwise
                            error('Can''t handle that');
                    end
                    switch self.Shape
                        case 'row'
                            value = reshape(value, 1, numel(value));
                        case 'column'
                            value = reshape(value, numel(value), 1);
                    end
                case 'matrix'
                    switch self.PrimitiveType
                        case {...
                                'denserealdouble','sparserealdouble','denserealsingle','sparserealsingle',...
                                'densecomplexdouble','sparsecomplexdouble','densecomplexsingle','sparsecomplexsingle',...
                                'char','int8','uint8','int16','uint16','int32','uint32','int64','uint64','logical'}
                            value = self.ConvertFromString(javavalue);
                            stringconversion = true;
                        otherwise
                            error('Can''t handle that');
                    end
            end
        end

        % convert a string to a native MATLAB value; results in NaN if there is a type conflict
        function value = ConvertFromString(self, text)
            text = char(text);
            switch self.Shape
                case 'scalar'
                    value = str2double(text);
                    status = ~isnan(value);
                otherwise
                    [value,status] = str2num(text); %#ok<ST2NM>
            end
            if all(int32(value(:)) == value(:))  % all numbers are integers
                value = int32(value);            % cast to integer
            end
            if ~status || ~self.CanAccept(value, true)
                value = NaN;
                return;
            end
            value = self.ConvertFromMatLab(value);
        end

        % convert a native MATLAB value to a value that matches the defined type
        function value = ConvertFromMatLab(self, value)
            if ~isreal(value) && isnumeric(self) && isreal(self)
                error('PropertyType:ArgumentTypeMismatch', 'Complex value cannot be coerced into a real type.');
            end
            if iscellstr(value) && strcmp(self.PrimitiveType, 'cellstr') || isobject(value) && strcmp(self.PrimitiveType, 'object')
                return
            end
            switch self.PrimitiveType
                case {'denserealdouble','densecomplexdouble'}
                    value = full(double(value));
                case {'sparserealdouble','sparsecomplexdouble'}
                    value = sparse(double(value));
                case {'denserealsingle','densecomplexsingle'}
                    value = full(single(value));
                case {'sparserealsingle','sparsecomplexsingle'}
                    value = sparse(single(value));
                case {'int8','uint8','int16','uint16','int32','uint32','int64','logical'}
                    value = cast(value, self.PrimitiveType);
                case {'char'}
                    value = char(value);
                otherwise
                    error('PropertyType:ArgumentTypeMismatch', 'Cannot coerce type %s into type %s.', class(value), self.PrimitiveType);
            end
        end
        
        
        % --- TYPE QUERIES ---

        function tf = islogical(self)
            tf = strcmp(self.PrimitiveType, 'logical');
        end
        
        function tf = isinteger(self)
            tf = any(strcmp(self.PrimitiveType,{'int8','uint8','int16','uint16','int32','uint32','int64','uint64'}));
        end
        
        function tf = isfloat(self)
            tf = any(strcmp(self.PrimitiveType,{'denserealsingle','denserealdouble','densecomplexsingle','densecomplexdouble','sparserealsingle','sparserealdouble','sparsecomplexsingle','sparsecomplexdouble'}));
        end
        
        function tf = isnumeric(self)
            tf = isinteger(self) || isfloat(self);
        end
        
        function tf = isreal(self)
            tf = any(strcmp(self.PrimitiveType,{'denserealsingle','denserealdouble','sparserealsingle','sparserealdouble'}));
        end
        
        function tf = is2d(self)
            tf = any(strcmp(self.Shape,{'row','column','matrix'}));
        end
        
        function tf = isvector(self)
            tf = any(strcmp(self.Shape,{'row','column'}));
        end
        
        function tf = isstring(self)
            tf = strcmp(self.PrimitiveType,'char') && strcmp(self.Shape,'row');
        end
        
        function tf = isset(self)
            tf = islogical(self) && isvector(self) && ~isempty(self.Domain);
        end
        
        
        % --- MISC ---
        
        % tostring()
        function s = char(self)
            if ~isempty(self.Shape)
                s = sprintf('%s/%s', self.PrimitiveType, self.Shape);
            else
                s = self.PrimitiveType;
            end
            if ~isempty(self.Domain)
                if iscellstr(self.Domain)
                    s = [s ' ' strjoin(',', self.Domain)];
                elseif isnumeric(self.Domain)
                    s = [s ' ' mat2str(self.Domain)];
                end
            end
        end        
    end
         
    methods (Static)
        
        
        % --- TYPE/SHAPE DISCOVERY ---
        
        % create a propertytype specifically matching the given value
        function obj = AutoDiscover(value,cls) %#ok<INUSD>
            if isobject(value)
                obj = PropertyType('object', PropertyType.AutoDiscoverShape(value));
                obj.ObjectType = class(value);
            else
                obj = PropertyType(PropertyType.AutoDiscoverType(value), PropertyType.AutoDiscoverShape(value));
            end
        end

        % determine the type of the given value
        function type = AutoDiscoverType(value)
            clazz = class(value);
            switch clazz
                case {'logical','char','int8','uint8','int16','uint16','int32','uint32','int64','uint64'}
                    type = clazz;
                case {'single','double'}
                    if issparse(value)
                        sparsity = 'sparse';
                    else
                        sparsity = 'dense';
                    end
                    if isreal(value)
                        complexity = 'real';
                    else
                        complexity = 'complex';
                    end
                    type = [ sparsity complexity clazz ];
                case 'cell'
                    if iscellstr(value)
                        type = 'cellstr';
                    else
                        error('PropertyType:InvalidArgumentValue', ...
                            'Cell arrays other than cell array of strings are not supported.');
                    end
                otherwise
                    error('PropertyType:InvalidArgumentValue', ...
                        'Argument type "%s" is not supported.', class(value));
            end
        end
        
        % determine the shape of the given value
        function shape = AutoDiscoverShape(value)
            if ndims(value) > 2
                error('PropertyType:InvalidArgumentValue', ...
                    'Dimensions higher than 2 are not supported.');
            end
            if size(value,1) == 1 && size(value,2) == 1
                shape = 'scalar';
            elseif size(value,1) == 1
                shape = 'row';
            elseif size(value,2) == 1
                shape = 'column';
            elseif size(value,1) == 0 && size(value,2) == 0
                shape = 'empty';  % no dimensions
            else
                shape = 'matrix';
            end
        end
    
        
        % --- TYPE / SHAPE MATCHING ---
        
        % check whether a property of some type can hold a value of some other type (with or without loss of precision)
        function tf = CanAcceptType(generaltype, specifictype, precisionloss)
            if nargin < 3
                precisionloss = false;
            end
            if any(strcmp(generaltype, specifictype))
                tf = true;
                return;
            end
            switch generaltype
                case {'densecomplexdouble','sparsecomplexdouble'}
                    type = {...
                        'int8','uint8','int16','uint16','int32','uint32','int64','uint64', ...
                        'denserealsingle','denserealdouble', ...
                        'densecomplexsingle','densecomplexdouble', ...
                        'sparserealsingle','sparserealdouble', ...
                        'sparsecomplexsingle','sparsecomplexdouble'};
                case {'denserealdouble','sparserealdouble'}
                    type = {...
                        'int8','uint8','int16','uint16','int32','uint32','int64','uint64', ...
                        'denserealsingle','denserealdouble', ...
                        'sparserealsingle','sparserealdouble'};
                case {'densecomplexsingle','sparsecomplexsingle'}
                    type = {...
                        'int8','uint8','int16','uint16','int32','uint32','int64','uint64', ...
                        'denserealsingle','densecomplexsingle','sparserealsingle','sparsecomplexsingle'};
                    if precisionloss  % permit loss of precision
                        type = [type {'denserealdouble','densecomplexdouble','sparserealdouble','sparsecomplexdouble'}];
                    end
                case {'denserealsingle','sparserealsingle'}
                    type = {...
                        'int8','uint8','int16','uint16','int32','uint32','int64','uint64', ...
                        'denserealsingle','sparserealsingle'};
                    if precisionloss  % permit loss of precision
                        type = [type {'denserealdouble','sparserealdouble'}];
                    end
                case {'int64','uint64'}
                    type = {'int8','uint8','int16','uint16','int32','uint32', ...
                        'denserealsingle','denserealdouble', ...        % cko: added these to make PropertyType less picky!
                        'densecomplexsingle','densecomplexdouble', ...
                        'sparserealsingle','sparserealdouble'}; 

                case {'int32','uint32'}
                    type = {'int8','uint8','int16','uint16', ...
                        'denserealsingle','denserealdouble', ...        % cko: added these to make PropertyType less picky!
                        'densecomplexsingle','densecomplexdouble', ...
                        'sparserealsingle','sparserealdouble'}; 
                case {'int16','uint16'}
                    type = {'int8','uint8'};
                case {'int8','uint8'}
                    type = {};
                case 'char'
                    type = {};
                case 'logical'
                    type = {'int8','uint8','int16','uint16','int32','uint32'};
                case 'cellstr'
                    type = {};
                otherwise
                    type = {};
            end
            tf = any(strcmp(specifictype,type));
        end

        % check whether a property of some shape can hold a value of some other shape (with or without loss of precision)
        function tf = CanAcceptShape(generalshape, specificshape)
            if any(strcmp(generalshape, specificshape))
                tf = true;
                return;
            end
            switch generalshape
                case 'matrix'
                    shape = {'scalar','row','column','empty'};
                case 'row'
                    shape = {'scalar','empty'};
                case 'column'
                    shape = {'scalar','empty'};
                case 'scalar'
                    shape = {'empty'}; % cko: allowing 'empty' here
                case 'empty'
                    shape = {};
                otherwise
                    shape = {};
            end
            tf = any(strcmp(specificshape, shape));
        end
        
        % check whether the given value is in the domain (assuming types match)
        function tf = IsInDomain(domain, value)
            if isempty(value)
                tf = true; % cko: added this
            elseif isempty(domain)
                tf = true;
            elseif iscellstr(domain)
                tf = any(strcmp(value,domain)); 
            elseif iscell(domain)
                tf = any(cellfun(@(v) v==value, domain)); % FIXME: should this not use isequal()?
            elseif isnumeric(domain) && length(domain) == 2
                tf = value >= min(domain) && value <= max(domain);
            end
        end
    end
        
    methods (Access = private)
        % helper: check whether the value is allowed by the domain
        function tf = CanAcceptValue(self, value)
            if islogical(self) && isvector(self)  % interpret logical vector as a set
                tf = isempty(self.Domain) || numel(value) == numel(self.Domain);  % size of logical vector corresponds to size of universe
            else  % test whether domain contains value
                tf = PropertyType.IsInDomain(self.Domain, value);
            end
        end
    end    
end
