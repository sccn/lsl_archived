function x = arg_extract(args,names,position,default)
% Extract arguments from Args; simple version, see notes.
% Value = arg_extract(Args,Names,Position,Default)
%
% In:
%   Args : the varargin of the calling function
%
%   Names : name by which the argument is referenced, or a cell array of names
%
%   Position: the 1-based position of the argument, if it can be listed positionally, or []
%             if not (default: [])
%
%   Default : optional default value, if the argument was not given 
%             (can only be specified if position is [], see notes)
%
% Out:
%   Value : The extracted value
%
% Notes:
%   This version of arg_extract assumes that the argument of interest, if it can be specified 
%   positionally, must be contained in Args (i.e., either by name, or by position). To enforce this,
%   no Default can be assigned if the listed Position is non-empty.
%
%   The second limitation is on the allowed values of all arguments in Args: No struct-typed 
%   argument may contain any of the Names as a valid field name, and no string-typed argument may 
%   allow any of the Names as its value; otherwise, these fields may be accidentally picked out by 
%   arg_extract.
%
% Examples:
%   % extract the ALLEEG argument (by name or position)
%   x = arg_extract(varargin,'ALLEEG',1) ;
%
%   % extract the sampling rate argument (by name, give an error if missing)
%   x = arg_extract(varargin,{'srate','SamplingRate'});
%
%   % extract the sapling rate argument (by name, and fall back to some default if missing)
%   x = arg_extract(varargin,{'srate','SamplingRate'},[],200);
%
% See also:
%   arg_define
%
%                                Christian Kothe, Swartz Center for Computational Neuroscience, UCSD
%                                2010-09-24

if ~iscell(names)
    names = {names}; end
if ~iscell(args)
    error('Args must be given as a cell array...'); end

% collect the indices where the argument can appear in the Args
indices = cellfun('isclass',args,'struct');
for n = 1:length(names)
    indices = indices | strcmp(args,names{n}); end
indices = find(indices);

% search the argument by name
for k = indices(end:-1:1)
    if ischar(args{k}) && k<length(args)
        % found it in the NVPs; pick it out
        x = args{k+1};
        return;
    else
        match = isfield(args{k},names);
        if any(match)
            % found it in a struct; pick it out
            x = args{k}.(names{match});
            return;
        end
    end
end

% it was either missing or specified positionally
if exist('position','var') && ~isempty(position) 
    % a position was given; do a sanity check first
    if exist('default','var')
        error('This version of arg_extract() does not support arguments that can both have a default and be specified positionally.'); end
    if position <= length(args)
        x = args{position};
    else
        error(['The argument # ' num2str(position) ' ' hlp_tostring(names) ' was not specified.']);
    end
else
    % no position was given, assign the default, if one exists
    if exist('default','var')
        x = default;
    else
        error(['The argument ' hlp_tostring(names) ' was not specified, and no default was assigned.']);
    end
end