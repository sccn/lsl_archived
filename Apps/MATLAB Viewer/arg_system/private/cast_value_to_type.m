function value = cast_value_to_type(type,value,cls) %#ok<INUSD>
% Internal function for GUI parameter casting.

if isstruct(type)
    type = PropertyType(type.PrimitiveType,type.Shape,type.Domain); end

validateattributes(type, {'PropertyType'}, {'scalar'});
assert(type.CanAccept(value), ...
    'PropertyGridField:InvalidOperation', ...
    'Setting type "%s" would invalidate current property value.', char(type));
value = type.ConvertFromMatLab(value);
