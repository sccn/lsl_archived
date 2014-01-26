function vis_stream(varargin)
% Display an LSL stream.
%
% Keyboard shortcuts:
%   [up arrow]   : increase the y scale of the time series
%   [down arrow] : decrease the y scale of the time series
%   [right arrow]: increase the displayed time range
%   [left arrow] : decrease the displayed time range
%   [page up]    : go up by one page of channels
%   [page down]  : go down by one page of channels
%
% In:
%   StreamName : Stream to display. The name of the stream that you would like to display.
%
%   TimeScale : Initial time scale in seconds. The time range of the display window;
%               can be changed with keyboard shortcuts (see help). Default=5
%
%   DataScale : Initial scale of the data. The scale of the data, in units between horizontal lines;
%               can be changed with keyboard shortcuts (see help). Default=150
%
%   ChannelRange : Channels to display. The channel range to display. Default=[1:32]
%
%   SamplingRate : Sampling rate for display. This is the sampling rate that is used for plotting, in Hz;
%                  for faster drawing. Default=100
%
%   RefreshRate : Refresh rate for display. This is the rate at which the graphics are updated, in Hz.
%                 Default=10
%
%   Rereference : Apply common-average re-referencing to the data. Useful for noisy EEG streams.
%                 Default=false
%
%   PageOffset : Channel page offset. Allows to flip forward or backward pagewise through the displayed channels.
%                Default=0
%
%   Position : Figure position. Allows to script the position at which the figures should appear.
%              This is a 4-element vector of the form [X-offset,Y-offset,Width,Height]
%              with all values in pixes.
%              Default=[]
%
%                                Christian Kothe, Swartz Center for Computational Neuroscience, UCSD
%                                2012-07-10
%
%                                uses portions of vis_dataStreamViewer
%                                (c) 2012 by Tim Mullen


% make sure that everything is on the path and LSL is loaded
if ~exist('arg_define','file')
    addpath(genpath(fileparts(mfilename('fullpath')))); end
if ~exist('env_translatepath','file')
    % standalone case
    lib = lsl_loadlib();
else
    % if we're within BCILAB we want to make sure that the library is also found if the toolbox is compiled
    lib = lsl_loadlib(env_translatepath('dependencies:/liblsl-Matlab/bin'));
end

% handle input arguments
streamnames = unique(find_streams);
opts = arg_define(varargin, ...
    arg({'streamname','StreamName'},streamnames{1},streamnames,'LSL stream that should be displayed. The name of the stream that you would like to display.'), ...
    arg({'bufferrange','BufferRange'},10,[],'Maximum time range to buffer. Imposes an upper limit on what can be displayed.'), ...
    arg({'timerange','TimeRange'},5,[],'Initial time range in seconds. The time range of the display window; can be changed with keyboard shortcuts (see help).'), ...
    arg({'datascale','DataScale'},150,[],'Initial scale of the data. The scale of the data, in units between horizontal lines; can be changed with keyboard shortcuts (see help).'), ...
    arg({'channelrange','ChannelRange'},1:32,[],'Channels to display. The channel range to display.'), ...
    arg({'samplingrate','SamplingRate'},100,[],'Sampling rate for display. This is the sampling rate that is used for plotting; for faster drawing.'), ...
    arg({'refreshrate','RefreshRate'},10,[],'Refresh rate for display. This is the rate at which the graphics are updated.'), ...
    arg({'reref','Rereference'},false,[],'Common average reference. Enable this to view the data with a common average reference filter applied.'), ...
    arg({'standardize','Standardize'},false,[],'Standardize data.'), ...
    arg_nogui({'subplot','Subplot'},0,[],'Subplot #, if any.'), ...
    arg_nogui({'pageoffset','PageOffset'},0,[],'Channel page offset. Allows to flip forward or backward pagewise through the displayed channels.'), ...
    arg_nogui({'position','Position'},[],[],'Figure position. Allows to script the position at which the figures should appear.'));


if isempty(varargin)
    % bring up GUI dialog if no arguments were passed
    arg_guidialog;
else
    % fix up some arguments
    opts.bufferrange = max(opts.bufferrange,opts.timerange);    

    % init shared handles
    [fig,ax,lines] = deal([]);
        
    % choose variable names to hold the stream's data (in the base workspace)
    taken = evalin('base','whos(''lsl*'')');
    chunkname = genvarname(['lsl_' opts.streamname '_chunk'],{taken.name});
    buffername = genvarname(['lsl_' opts.streamname '_stream'],{taken.name});
        
    % create a stream inlet
    inlet = create_inlet(opts);
    
    % create the stream data structure in the base workspace
    assignin('base', buffername, create_streambuffer(opts,inlet.info()));
    
    % create the figure
    create_figure(opts);
    
    % set up a timer that reads from LSL    
    th = timer('Period', 1.0/opts.refreshrate,'ExecutionMode','fixedRate','TimerFcn',@on_timer,'StartDelay',0.2,'Tag',['lsl_' genvarname(opts.streamname) '_timer']);
    start(th);
end


    % === nested functions (sharing some handles with each other) ===

    % create a new figure and axes
    function create_figure(opts)
        if opts.subplot
            fig = gcf;
            ax = subplot(2,1,opts.subplot);
        else
            fig = figure('Tag',['Fig' buffername],'Name',['LSL:Stream''' opts.streamname ''''],'CloseRequestFcn','delete(gcbf)', ...
                'KeyPressFcn',@(varargin)on_key(varargin{2}.Key));
            if ~isempty(opts.position)
                set(fig,'Position',opts.position); end
            ax = axes('Parent',fig, 'Tag','LSLViewer', 'YDir','reverse');
        end
    end

    function on_timer(varargin)
        try 
            % check if the buffer is still there
            if evalin('base',['exist(''' buffername ''',''var'')'])
                
                
                % === update buffer contents (happens in the base workspace) ===
                
                % pull a new chunk from LSL
                [chunkdata,timestamps] = inlet.pull_chunk();
                assignin('base',chunkname,chunkdata);
                
                % append it to the stream buffer
                evalin('base',['[' buffername '.smax,' buffername '.data(:,1+mod(' buffername '.smax:' buffername '.smax+size(' chunkname ',2)-1,' buffername '.pnts))] = deal(' buffername '.smax + size(' chunkname ',2),' chunkname ');']);
                
                % get the updated stream buffer
                stream = evalin('base',buffername);
                
                % reformat the stream buffer to contain only the current block that should be displayed
                samples_to_get = min(stream.pnts, round(stream.srate*stream.opts.timerange));
                stream.data = stream.data(:, 1+mod(stream.smax-samples_to_get:stream.smax-1,stream.pnts));
                [stream.nbchan,stream.pnts,stream.trials] = size(stream.data);
                if ~isempty(timestamps)
                    stream.xmax = max(timestamps)-lsl_local_clock(lib);
                elseif ~isfield(stream,'xmax')
                    stream.xmax = 0;
                end
                stream.xmin = stream.xmax - (stream.pnts-1)/stream.srate;
                
                
                % === data post-processing for plotting ===
                
                % determine channels and samples to display
                plotchans = stream.opts.channelrange + stream.opts.pageoffset*length(stream.opts.channelrange);
                if isempty(plotchans)
                    plotchans = 1:stream.nbchan;
                else
                    plotchans = intersect(1:stream.nbchan,plotchans);
                end
                plotdata = stream.data(plotchans, round(1 : stream.srate/stream.opts.samplingrate : end));
                plottime = linspace(stream.xmin,stream.xmax,size(plotdata,2));
                
                % re-reference
                if stream.opts.reref
                    plotdata = bsxfun(@minus,plotdata,mean(plotdata)); end
                if stream.opts.standardize
                    plotdata = bsxfun(@times,plotdata,1./std(plotdata')'); end
                
                % zero-mean
				tmpdata = plotdata; tmpdata(isnan(tmpdata(:))) = 0;
                plotdata = bsxfun(@minus, plotdata, mean(tmpdata,2));
                
                % arrange for plotting
                plotoffsets = (0:size(plotdata,1)-1)*stream.opts.datascale;
                plotdata = bsxfun(@plus, plotdata', plotoffsets);
                
                
                % === actual drawing ===
                
                % draw the block contents...
                if ~isempty(plotdata)
                    if ~exist('lines','var') || isempty(lines)                        
                        lines = plot(ax,plottime,plotdata);
                        title(ax,opts.streamname);
                        xlabel(ax,'Time (sec)','FontSize',12);
                        ylabel(ax,'Activation','FontSize',12);
                    else
                        for k=1:length(lines)
                            set(lines(k),'Ydata',plotdata(:,k));
                            set(lines(k),'Xdata',plottime);
                        end
                    end
                
                    % update the axis limit and tickmarks
                    axis(ax,[stream.xmin stream.xmax -stream.opts.datascale size(plotdata,2)*stream.opts.datascale + stream.opts.datascale]);
                    set(ax, 'YTick',plotoffsets, 'YTickLabel',{stream.chanlocs(plotchans).labels});
                end
                
                drawnow;
            else
                try 
                    disp(['Deleting timer ' get(th,'Tag') '.']);
                catch e
                    disp('Deleting timer.');
                end
                % delete the timer
                warning off MATLAB:timer:deleterunning
                delete(th);
            end
        catch e
            if isempty(findobj('Tag',['Fig' buffername]))
                disp('Figure was closed.');
            else
                disp('An error occurred during the stream viewer update: ');
                try
                    hlp_handleerror(e);
                catch
                    disp(e.message);
                end
            end
            warning off MATLAB:timer:deleterunning
            delete(th);
        end
    end

    function on_key(key)
        stream = evalin('base',buffername);
        switch lower(key)
            case 'uparrow'
                % decrease datascale
                stream.opts.datascale = stream.opts.datascale*0.9;
            case 'downarrow'
                % increase datascale
                stream.opts.datascale = stream.opts.datascale*1.1;
            case 'rightarrow'
                % increase timerange
                stream.opts.timerange = stream.opts.timerange*1.1;                
            case 'leftarrow'
                % decrease timerange
                stream.opts.timerange = stream.opts.timerange*0.9;                
            case 'pagedown'
                % shift display page offset down
                stream.opts.pageoffset = stream.opts.pageoffset+1;                
            case 'pageup'
                % shift display page offset up
                stream.opts.pageoffset = stream.opts.pageoffset-1;
        end
        assignin('base',buffername,stream);
    end


    % === utility functions ===
    
    % find names of streams on the lab network...
    function names = find_streams
        streams = lsl_resolve_all(lib,0.3);
        names = cellfun(@(s)s.name(),streams ,'UniformOutput',false);
        if isempty(names)
            error('There is no stream visible on the network.'); end
    end


    % create an inlet to read from the stream with the given name
    function inlet = create_inlet(opts)
        % look for the desired device
        result = {};
        disp(['Looking for a stream with name=' opts.streamname ' ...']);
        while isempty(result)
            result = lsl_resolve_byprop(lib,'name',opts.streamname); end
        % create a new inlet
        disp('Opening an inlet...');
        inlet = lsl_inlet(result{1},opts.bufferrange);
    end


    % create a new stream buffer in the base workspace
    function stream = create_streambuffer(opts,info)
        stream.srate = info.nominal_srate();                                % sampling rate in Hz
        stream.chanlocs = struct('labels',derive_channel_labels(info));     % struct with per-channel meta-data
        stream.pnts = max(opts.bufferrange*stream.srate,100);               % number of data points in the buffer
        stream.nbchan = info.channel_count();                               % number of channels in the buffer
        stream.trials = 1;                                                  % number of segments in the buffer (always 1)
        stream.data = zeros(stream.nbchan,stream.pnts,stream.trials);       % the circular buffer storage
        stream.smax = 0;                                                    % number of samples that have been written into the buffer so far (wrapping around)
        stream.opts = opts;                                                 % current display options for this stream
    end


    % derive a list of channel labels for the given stream info
    function channels = derive_channel_labels(info)
        channels = {};
        ch = info.desc().child('channels').child('channel');
        while ~ch.empty()
            name = ch.child_value_n('label');
            if name
                channels{end+1} = name; end
            ch = ch.next_sibling_n('channel');
        end
        if length(channels) ~= info.channel_count()
            disp('The number of channels in the steam does not match the number of labeled channel records. Using numbered labels.');
            channels = cellfun(@(k)['Ch' num2str(k)],num2cell(1:info.channel_count(),1),'UniformOutput',false);
        end
    end
    
end

