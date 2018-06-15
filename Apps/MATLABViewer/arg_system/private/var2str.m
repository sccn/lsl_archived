% Textual representation of any MatLab value.

% Copyright 2009 Levente Hunyadi
function s = var2str(value)

if islogical(value) || isnumeric(value)
    s = num2str(value);
elseif ischar(value) && isvector(value)
    s = reshape(value, 1, numel(value));
elseif isjava(value)
    s = char(value);  % calls java.lang.Object.toString()
else
    try
        s = char(value);
    catch %#ok<CTCH>
        s = '[no preview available]';
    end
end