% Find handle graphics object with user data check.
% Retrieves those handle graphics objects (HGOs) that have the specified
% Tag property and whose UserData property satisfies the given predicate.
%
% Input arguments:
% fcn:
%    a predicate (a function that returns a logical value) to test against
%    the HGO's UserData property
% tag (optional):
%    a string tag to restrict the set of controls to investigate
%
% See also: findobj

% Copyright 2010 Levente Hunyadi
function h = findobjuser(fcn, tag)

validateattributes(fcn, {'function_handle'}, {'scalar'});
if nargin < 2 || isempty(tag)
    tag = '';
else
    validateattributes(tag, {'char'}, {'row'});
end

%hh = get(0, 'ShowHiddenHandles');
%cleanup = onCleanup(@() set(0, 'ShowHiddenHandles', hh));  % restore visibility on exit or exception

if ~isempty(tag)
    % look among all handles (incl. hidden handles) to help findobj locate the object it seeks
    h = findobj(findall(0), '-property', 'UserData', '-and', 'Tag', tag);  % more results if multiple matching HGOs exist
else
    h = findobj(findall(0), '-property', 'UserData');
end
h = unique(h);
try
    for k=1:length(h)
        pred = fcn(get(h(k), 'UserData'));
        if isempty(pred)
            pred = false; end
        f(k) = pred;
    end        
    %f = arrayfun(@(handle) fcn(get(handle, 'UserData')), h, 'UniformOutput',false);
catch
    1
end
h = h(f);