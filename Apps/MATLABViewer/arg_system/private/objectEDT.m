function result = objectEDT(constructor, varargin)
% Auto-delegate Java object method calls to Event Dispatch Thread (EDT).

% Copyright 2010 Levente Hunyadi

error(nargchk(1, 256, nargin));
validateattributes(constructor, {'char'}, {'nonempty','row'});

v = sscanf(version, '%u.%u');  % major and minor version number
if v(1) > 7 || v(1) == 7 && v(2) > 6  % > 7.6, i.e. > MatLab 2008a
    result = javaObjectEDT(constructor, varargin{:});
else
    result = feval(constructor, varargin{:});
end
