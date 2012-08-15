function arg_issuereport(payload)
% Internal function to issue a report to a requesting function. 
% This is implemented by throwing an exception. Used by arg_define() and declare_properties()
%
% In:
%   Payload : the payload to report
%
% See also:
%   arg_define, arg_report

global tracking;

% first obtain a report ticket
if ~isfield(tracking,'arg_sys')
    tracking.arg_sys = struct(); end
if ~isfield(tracking.arg_sys,'tickets')
    tracking.arg_sys.tickets = java.util.concurrent.LinkedBlockingDeque();
    for k=5000:-1:1
        tracking.arg_sys.tickets.addLast(k); end
end
ticket = tracking.arg_sys.tickets.removeLast();

% ... and store the payload
tracking.arg_sys.reports{ticket} = payload;

% now throw the exception
error('arg_system:report_args','This (internal) exception is destined to be caught by arg_report(); please do not interfere with it. Ticket=%.0f',ticket);
