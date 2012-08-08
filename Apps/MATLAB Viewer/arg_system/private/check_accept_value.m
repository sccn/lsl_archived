function check_accept_value(type,value,cls) %#ok<INUSD>
% Internal function for GUI parameter assignment checking.

if isstruct(type)
    type = PropertyType(type.PrimitiveType,type.Shape,type.Domain); end

assert(type.CanAccept(value), ...
    'PropertyGridField:InvalidArgumentValue', ...
    'This value does not conform to property type restrictions.');
