% Sample object to test data persistence functions and user controls.

% Copyright 2008-2009 Levente Hunyadi
classdef SampleObject
    properties
        % A scalar of the standard MatLab type.
        RealDoubleScalar = pi;
        % A matrix of the standard MatLab type.
        RealDoubleMatrix = [1.5,2.5;3.5,4.5];
        RealSingleMatrix = single([1.5,2.5;3.5,4.5]);
        ComplexDoubleMatrix = [1.5,2.5i;3.5i,4.5];
        ComplexDoubleScalar = 1.5 + 2.5i;
        IntegerScalar = int32(32);
        IntegerMatrix = int32([1,2;3,4]);
        Logical = true;
        LogicalMatrix = [true,false;false,true];
        String = 'this is a sample string';
        CellArrayOfStrings = {'this','is','a','cell','array','of','strings'};
        Caption = 'SampleObject';
        % An unchangeable property.
        % Whenever the setter method of this property is used to modify the
        % value, an exception is thrown.
        Unchangeable = 'unchangeable';
        NestedObject = SampleNestedObject;
    end
    properties (Constant)
        Re = 6.378e6;  % Earth radius (m)
        RealDoubleConstantMatrix = [1.5,2.5;3.5,4.5];
    end
    properties (Dependent)
        DependentProperty;
        ReadOnlyDependentProperty;
    end
    properties (Hidden)
        HiddenProperty = 'hidden';
    end
    properties (Transient)
        TransientProperty = 'transient';
    end
    properties (Access = private)
        PrivateProperty = 'private';
    end
    properties (Access = protected)
        ProtectedProperty = 'protected';
    end
    methods
        function Method(self)
            disp(self.PrivateProperty);
        end
        
        function self = set.Caption(self, caption)
            caption = deblank(caption);
            fprintf('Caption has been changed from %s to %s.\n', self.Caption, caption);
            self.Caption = caption;
        end
        
        function value = get.DependentProperty(self)
            value = self.Caption;
        end
        
        function self = set.DependentProperty(self, value)
            self.Caption = value;
        end
        
        function value = get.ReadOnlyDependentProperty(self)
            value = [ self.Caption ' - ' self.Caption ];
        end
        
        function self = set.Unchangeable(self, value) %#ok<INUSL>
            error('SampleObject:InvalidOperation', 'Cannot change property value to %s', value);
        end
    end
end
