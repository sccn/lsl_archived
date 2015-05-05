function help = parse_help(help,summary_len)
% helper function for the arg* specifiers, to parse the help into a first and second part.
% Help = parse_help(Help,SummaryLength)
%
% In:
%   Help: some help specification (as it appears in the arg* functions
%   
%   SummaryLength : the maximum length for the executive summary portion
%
% Out:
%   Help: a cell array of {executive summary, description}
%
% See also:
%   arg_define
%
%                                Christian Kothe, Swartz Center for Computational Neuroscience, UCSD
%                                2010-09-24

if ~exist('summary_len','var')
    summary_len = 60; end

if ischar(help)
    % string: split at the end of the first sentence and put into a cell array
    [a,b] = regexp(help,'\.\s+[A-Z]','once');
    if ~isempty(a)
        help = {help(1:a-1), help(b-1:end)};
    else
        help = {help};
    end
elseif ~iscellstr(help)
    error('The help text must be a string.');
end

% remove trailing dot
if length(help{1}) > 1 && help{1}(end) == '.'
    help{1} = help{1}(1:end-1); end

% check for length limit
if length(help{1}) > summary_len
    % Note: The first sentence in the description is used in some GUIs which have a size limit;
    %       to prevent text from being cut off, please use a shorter wording in the first sentence.
    %
    %       Also note that if this sentence is not followed by a capital letter, the remaining part 
    %       is not considered separate.
    error(['The executive summary for the given argument is too long (' help{1} ')']); 
end
