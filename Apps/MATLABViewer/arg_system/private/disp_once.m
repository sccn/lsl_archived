function disp_once(varargin)
% Display a specific message only once (per MATLAB session).
% disp_once(Message,Arguments...)
%
% This function displays a message like fprintf (though with implicit newline), except that a 
% message with the given content is only emitted once per MATLAB session.
%
% In:
%   Message : the message content (as in fprintf)
%             special case: if this is 'clear', the memory of displayed messages will be discarded
%
%   Arguments... : list of arguments that may be substituted into Message (as in fprintf)
%
% Examples:
%   % display a message, but just once
%   disp_once('Note: Trying to read file now...');
%
% See also:
%   disp
%
%                                Christian Kothe, Swartz Center for Computational Neuroscience, UCSD
%                                2011-02-13

persistent displayed_messages;

if strcmp(varargin{1},'clear')
    % clear the stored messages
    displayed_messages = [];
    return;
end

message_content = sprintf(varargin{1:end});

% generate a hash of it
str = java.lang.String(message_content);
message_id = sprintf('x%.0f',str.hashCode()+2^31);

% and check if it had been displayed before
if ~isfield(displayed_messages,message_id)
    % emit the message
    disp(message_content);
    % remember to not display the message again
    displayed_messages.(message_id) = true;
end
