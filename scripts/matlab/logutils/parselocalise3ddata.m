function [times, hypotheses] = parselocalise3ddata( file )
%PARSELOCALISE3DDATA
% [TIMES, HYPOTHESES] = PARSELOCALISE3DDATA( file )
%
% parse laser data from orca logger (ASCII)
% output args
% TIMES      Each row is a Unix timestamp [sec usec]
% HYPOTHESES Each cell represents hypotheses corresponding to
%            one timestamp.
%            Each cell is a matrix, where each each row
%            describes a hypothesis in the form:
%   [meanX,meanY,meanZ,meanR,meanP,meanA,
%    Pxx,Pxy,Pxz,Pxr,Pxp,Pxa,
%        Pyy,Pyz,Pyr,Pyp,Pya,
%            Pzz,Pzr,Pzp,Pza,
%                Prr,Prp,Pra,
%                    Ppp,Ppa,
%                        Paa,weight]
%   where X=x, Y=y, Z=z, R=roll, P=pitch, A=yaw
%
% Example:
% [t,H]=parselocalise3ddata( 'filename.log' );
% for i=(1:length(t))
%    X(i,1:6) = H{i}(1:6);
% end
% figure(1),clf
% plot(X(:,1),X(:,2),'b-', X(1,1),X(1,2),'bo')
% axis equal
% title( 'XY plot' )
%

fid = fopen( file );
if ( fid == -1 )
    error(sprintf('Couldnt open file: %s\n',file));
end

% time stamp format
timeStampFormat = '%f %f';

% hypothesis format
formatHypothesis = '%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f';

i=1;
while true
    % read time stamp
    timeTmp = fscanf(fid, timeStampFormat, 2);

    % check if we have reached then of the file
    if size(timeTmp,1) ~= 2
        break;
    end
    
    times(i,:) = timeTmp';

    % read num hypotheses
    numHypotheses = fscanf(fid, '%d', 1);
    
    for j=1:numHypotheses
        hypothesisTmp = fscanf(fid, formatHypothesis, 28);
        if ( length(hypothesisTmp) ~= 28 )
            error('Malformed hypothesis.');
        end
    
        hypotheses{i}(j,:) = hypothesisTmp;
    end

    i=i+1;
    
end

fclose(fid);
