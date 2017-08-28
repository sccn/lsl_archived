% Help text associated with a function, class, property or method.
% Spaces are removed as necessary.
%
% See also: helpdialog

% Copyright 2008-2010 Levente Hunyadi
function text = helptext(obj)

if ischar(obj)
    text = gethelptext(obj);
else
    text = gethelptext(class(obj));
end
text = texttrim(text);

function text = gethelptext(key)

persistent dict;
if isempty(dict) && usejava('jvm')
    dict = java.util.Properties();
end

if ~isempty(dict)
    text = char(dict.getProperty(key));  % look up key in cache
    if ~isempty(text)  % help text found in cache
        return;
    end
    text = help(key);
    if ~isempty(text)  % help text returned by help call, save it into cache
        dict.setProperty(key, text);
    end
else
    text = help(key);
end

function lines = texttrim(text)
% Trims leading and trailing whitespace characters from lines of text.
% The number of leading whitespace characters to trim is determined by
% inspecting all lines of text.

loc = strfind(text, sprintf('\n'));
n = numel(loc);
loc = [ 0 loc ];
lines = cell(n,1);
if ~isempty(loc)
    for k = 1 : n
        lines{k} = text(loc(k)+1 : loc(k+1));
    end
end
lines = deblank(lines);

% determine maximum leading whitespace count
f = ~cellfun(@isempty, lines);  % filter for non-empty lines
firstchar = cellfun(@(line) find(~isspace(line), 1), lines(f));  % index of first non-whitespace character
if isempty(firstchar)
    indent = 1;
else
    indent = min(firstchar);
end

% trim leading whitespace
lines(f) = cellfun(@(line) line(min(indent,numel(line)):end), lines(f), 'UniformOutput', false);