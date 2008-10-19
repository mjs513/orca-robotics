function [timestamps, features] = parsepolarfeaturedata( file )
%
% function [times, features] = parsepolarfeaturedata( file )
%
% Parses Polar2dFeature data from an ASCII-file generated by the Orca logrecorder
%
% Author: Tobias Kaupp
%
% Output:
%    timestamps = Each row is a Unix timestamp
%    features = A cell array containing the feature data. 
%               Each cell corresponds to a timestamp and is of size (1xnumFeatures).
%               Each cell contains another cell array with the individual features.
%               The first 4 entries of the individual features are:
%                   type (see orca/src/interfaces/slice/orca/featuremap2d.ice)
%                   P(falsePositive) (see orca/src/interfaces/slice/orca/polarfeature2d.ice)
%                   P(truePositive)
%                   classType (0: PointFeature, 1:PoseFeature, 2:LineFeature)
%               Depending on the classType, there are a different number of entries in the rest of the array:
%                   PointFeature (4 numbers)
%                   PoseFeature (6 numbers)
%                   LineFeature (8 numbers)
%               The numbers are defined in orca/src/interfaces/slice/orca/polarfeature2d.ice
%     

fid = fopen( file );
if ( fid == -1 )
    error(sprintf('Could not open file: %s\n',file));
end

% first line format
firstLineFormat = '%f %f %d';

% point feature format
formatPointFeature = '%f %f %f %f';

% pose feature format
formatPoseFeature = '%f %f %f %f %f %f';

% line feature format
formatLineFeature = '%f %f %f %f %f %f %f %f';

i=0;
while true
    i=i+1;

    % read the first line
    firstLine = fscanf(fid, firstLineFormat, 3);

    % check if we have reached the end of the file
    if size(firstLine,1) ~= 3
        break;
    end
    
    timestamps(i,:) = firstLine(1:2)';
    
    numFeatures = firstLine(3);
    if (numFeatures==0) 
        features{i} = [];
        continue;
    end
    
    for j=1:numFeatures
        
        type = fscanf(fid, '%d', 1);
        pFalsePos = fscanf(fid, '%f', 1);
        pTruePos = fscanf(fid, '%f', 1);
        classType = fscanf(fid, '%d', 1);
        
        if classType==0
            featureData = fscanf(fid, formatPointFeature, 4);
        elseif classType==1
            featureData = fscanf(fid, formatPoseFeature, 6);
        elseif classType==2
            featureData = fscanf(fid, formatLineFeature, 8);
        else
            error(sprintf('Did not recognize class type: %d\n',classType));
        end
        
        fTmp{j} = [type; pFalsePos; pTruePos; classType; featureData];
    
    end
    
    features{i} = fTmp;
    clear fTmp;
    
end

fclose(fid);