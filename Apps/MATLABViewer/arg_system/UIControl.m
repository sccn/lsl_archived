% Root class for composite user controls. More form than function, it seems...

% Copyright 2008-2009 Levente Hunyadi
classdef UIControl < hgsetget
    properties (Abstract, Dependent)
        % The graphics handle that represents the entire user control.
        % This handle is used in setting the parent, position, measurement
        % unit, etc. properties of the control. In order to use MatLab's Handle
        % Graphics semantics in a derived user interface control, you should
        % query the Control property.
        Control;
    end
    properties (Dependent)
        NormalizedPosition;
        % The parent control of this UI control.
        Parent;
        % Units in which the Position property is expressed.
        % Note: this property is deliberately put before the Position property
        % to ensure proper initialization when both Units and Position are set
        % in a constructor argument list. For the constructor processes
        % arguments in the order in which they occur within the class
        % definition rather than the order in which they are specified, we
        % always get the desired behavior that Position is interpreted in terms
        % of the specified Units.
        Units = 'normalized';
        Position;
    end
    methods (Abstract)
        % Forces instantiation of underlying MatLab Handle Graphics objects.
        % Some dependent properties refer to MatLab Handle Graphics object
        % properties in an early phase of object construction. These properties
        % might fail if existence of objects they depend on cannot be
        % guaranteed.
        obj = Instantiate(obj, parent);
    end
    methods
        function obj = UIControl(arg, varargin)
            if nargin > 0 && isscalar(arg) && ishandle(arg)
                obj = obj.Instantiate(arg);
            else
                obj = obj.Instantiate();
            end
            if nargin > 0
                if isscalar(arg) && ishandle(arg)
                    obj.Parent = arg;
                    obj = constructor(obj, varargin{:});
                else
                    if ~strcmp('figure', get(obj.Control, 'Type'))
                        obj.Parent = gcf;
                    end
                    obj = constructor(obj, arg, varargin{:});
                end
            end
        end

        function parent = get.Parent(obj)
            parent = get(obj.Control, 'Parent');
        end
        
        function obj = set.Parent(obj, parent)
            if ~strcmp(get(obj.Control, 'Type'), 'figure')  % figures do not require a parent
                set(obj.Control, 'Parent', parent);
            end
        end

        function units = get.Units(obj)
            units = get(obj.Control, 'Units');
        end
        
        function obj = set.Units(obj, units)
            set(obj.Control, 'Units', units);
        end
        
        function position = get.Position(obj)
            position = get(obj.Control, 'Position');
        end
        
        function obj = set.Position(obj, position)
            set(obj.Control, 'Position', position);
        end

        function obj = set.NormalizedPosition(obj, position)
            units = get(obj.Control, 'Units');
            if isempty(units)
                units = 'normalized';
            end 
            switch units
                case 'normalized'
                    set(obj.Control, 'Position', position);
                otherwise
                    set(obj.Control, 'Units', 'normalized');
                    set(obj.Control, 'Position', position);
                    set(obj.Control, 'Units', units);
            end
        end
        
        function position = get.NormalizedPosition(obj)
            units = get(obj.Control, 'Units');
            if isempty(units)
                units = 'normalized';
            end
            switch units
                case 'normalized'
                    position = get(obj.Control, 'Position');
                otherwise
                    set(obj.Control, 'Units', 'normalized');
                    position = get(obj.Control, 'Position');
                    set(obj.Control, 'Units', units);
            end
        end

        function position = getpixelposition(obj)
            position = getpixelposition(obj.Control);
        end

        function obj = setpixelposition(obj, position)
            setpixelposition(obj.Control, position);
        end
    end
end