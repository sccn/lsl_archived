srcpath = '.';
srates = [50,100]; %100,200,500,1000,2000]; %,2000,3000,4000,5000];

for k=1:length(srates)
    rate = srates(k);
    % load data for test case
    datapath = [srcpath filesep 'NR' num2str(rate)];
    truedata = {}; jitdata = {}; cppdata = {};
    figure; hold;
    for j=1:100
        fp = fopen([datapath '_YMatrix_' num2str(j) '.bin'],'rb'); truedata{j} = fread(fp,inf,'*double'); fclose(fp);
        fp = fopen([datapath '_ZMatrix_' num2str(j) '.bin'],'rb'); jitdata{j} = fread(fp,inf,'*double'); fclose(fp);
        fp = fopen([datapath '_WMatrix_' num2str(j) '.bin'],'rb'); cppdata{j} = fread(fp,inf,'*double'); fclose(fp);
        r = 1:10:length(truedata{j});
        plot((r-1)/rate,truedata{j}(r)-cppdata{j}(r)); title(num2str(rate));
    end
    ml=min(cellfun(@length,truedata));
    truedata = cellfun(@(x){x(1:ml)},truedata);
    cppdata = cellfun(@(x){x(1:ml)},cppdata);
    errs = abs([truedata{:}] - [cppdata{:}]);
    errs = sort(errs,2);
    qt95 = errs(:,round(size(errs,2)*0.95));
    qt99 = errs(:,round(size(errs,2)*0.99));
    figure; hold;
    plot((0:length(qt99)-1)/rate,qt99,'r');
    plot((0:length(qt95)-1)/rate,qt95,'b');
    drawnow;
    truedata = vertcat(truedata{:}); jitdata = vertcat(jitdata{:}); cppdata = vertcat(cppdata{:});
end
