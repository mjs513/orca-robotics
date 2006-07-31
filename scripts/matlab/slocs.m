% keep track of orca project size using sloccount
clear all

% include/* except: include/{Freeze IceSSL IcePatch2}
% src/{Ice Slice IceGrid icecpp IceStorm slice2cpp IceUtil IceBox IceXML}
Sice212 = [
(7842-1000) + (22093+18419+11499+10568+4392+4200+4252+567+316)
];
Sice301 = [
(8164-1000) + (23018+19384+19169+10596+4479+4343+4308+622+319)
];
% now also add Java:
% src/{IceInternal Ice IceUtil IceBox}
Sice310 = [
(8810-1200) + (24275+20494+22689+10587+4569+4877+4770+667+319) + (13190+6915+1262+493)
];

% for comparison, ASN
Sasn = 90433;

% indeces
iyy=1;
imm=2;
idd=3;
iver=4;
isub=5;
ibase=6;
icomp=7;

Sorca = [ ...
% YY    MM  DD  ?   SUB     BASE     COMP             VERSION
% 0.8.6: a single distro, divide in halves arbitrarily
2003    4   14  1   0       18930/2 18930/2;          % 0.8.6 (last change in doc/ChangeLog
% BASE: orca-base
% COMP: orca-components
2004    9   13  1   0       42543   21726;            % 0.11.0
2004    11  16  1   0       8963    27310;            % 0.12.0
2004    12  23  0   0       10596   31845;            % 0.12.1
2005    3  16   1   0       13767   43576;            % 0.13.0
2005    3  21   0   0       13620   45874;            % 0.13.1
2005    4   4   0   0       15630   45944;            % 0.13.2
2005    4   16  0   0       12283   46666;            % 0.13.3
2005    5   9   1   0       14639   46837;            % 0.14.0
2005    7   2   1   0       19751   49854;            % 0.15.0
2005    10  11  1   0       20818   52408;            % 1.0.0
% BASE: just orcaice; should include interfaces/slice but can't
% COMP: comp + util - orcaice 
2005    11  20  1   Sice212 2320    9226+8781-2320;   % 2.0.0-rc1
2006    1   22  0   Sice301 2511    9801+9695-2511;   % 2.0.0-rc2
2006    2   20  0   Sice301 2998    9867+14734-2998;  % 2.0.0-rc3
2006    3   29  0   Sice301 5250    14625+18131-5250; % 2.0.0-rc4
2006    6   11  0   Sice310 4985    19999+24867-4985; % 2.0.0-rc5
2006    8   9   1   Sice310 5086    19979+21711-5086  % 2.0.0
];
Vorca ={'0.8.6', '0.11.0', '0.12.0', '0.12.1', '0.13.0', '0.13.1', '0.13.2', '0.13.3', '0.14.0', '0.15.0', '1.0.0', ...
    '2.0.0-rc1', '2.0.0-rc2', '2.0.0-rc3', '2.0.0-rc4', '2.0.0-rc5', '2.0.0' }';

% add a dummy release just for display
Sorca(end+1,:) = Sorca(end,:);
Sorca(end,imm) = Sorca(end,imm)+1;
Sorca(end,iver) = 0;
Vorca{end+1} = 'dummy';

% convert dates
Torca = datenum(Sorca(:,iyy),Sorca(:,imm),Sorca(:,idd));

% 2nd digit releases
i2 = (Sorca(:,iver)==1);
Torca2 = Torca(i2);
Vorca2 = Vorca(i2);

Splayer = [ 
% YY    MM  DD  ?   SUB     BASE                COMP           VERSION
% BASE: (server-drivers)+client_libs
% COMP: drivers + utils
2002    12 04   1  0    (17331-12037)+9188      12037+2685;    % 1.3.1
2003    7  15   1  0    (30452-24334)+12458      24334+3064;    % 1.4rc1
2003    12 07   1  0    (37378-30947)+13110     30947+9633;    % 1.4rc2
2004    5  31   1  0    (49225-42643)+13664     42643+3550;    % 1.5
2004    11 12   1  0    (50758-43190)+13545     43190+4479;    % 1.6
2005    1  31   0  0    (52069-44517)+13576     44517+4662;    % 1.6.2
2005    7  28   0  0    (53348-45686)+14300     45686+5188;    % 1.6.5
% BASE: (libplayercore-./bindings/java/net) + client_libs + (libplayertcp-./bindings/java)
% note: libplayerxdr is autogenerated
% COMP: server + utils
2005    10 25   0  0    (4866-0)+7560+(1391-0)  46474+5725      % 2.0pre7
2006    2  28   1  0    (52739-20157)+8513+(2740-1291)  48764+5940 % 2.0.0
2006    3  25   1  0    (53957-20592)+8909+(2740-1291)  47069+5964 % 2.0.1
2006    6  9    1  0    (56741-21756)+9575+(2767-1319)  51983+9438 % 2.0.2
];
Tplayer = datenum(Splayer(:,1),Splayer(:,2),Splayer(:,3));

%  Player v. 2.0.2
%  56741   libplayercore   cpp=33113,java=21756,ansic=1692,python=180
%  51983   server          cpp=42391,ansic=9592
%  9575    client_libs     ansic=5053,cpp=4300,python=222
%  9438    utils           ansic=5310,cpp=3971,python=157
%  8357    top_dir         sh=8357
%  7053    libplayerxdr    ansic=6840,python=213
%  2849    rtk2            ansic=2849
%  2767    libplayertcp    cpp=2383,java=284,ansic=100
%  1352    examples        cpp=1277,ansic=75
%  237     replace         ansic=237
%  205     libplayerjpeg   ansic=205
%  22      doc             sh=19,php=3
%  0       config          (none)

Smax = 130;
d0 = [Torca Torca]'; d0=d0(:); d0(1)=[];
d1 = Smax-Sorca(:,isub)-Sorca(:,ibase); d1 = [d1 d1]'; d1=d1(:); d1(end)=[];
d2 = Sorca(:,isub); d2 = [d2 d2]'; d2=d2(:); d2(end)=[];
d3 = Sorca(:,ibase); d3 = [d3 d3]'; d3=d3(:); d3(end)=[];
d4 = Sorca(:,icomp); d4 = [d4 d4]'; d4=d4(:); d4(end)=[];

figure(1), clf
ha=area( d0, [d1 d2 d3 d4]/1e3, -Smax ); hold on
plot( Torca, 0*Torca, 'k-o' )
h2=plot( Torca2, 0*Torca2, 'ko' );
set(h2,'markerfacecolor','k')
ylabel('Source Code Size (kSLOC)')

set(ha(1),'facecolor',1*[1 1 1]);
set(ha(2),'facecolor',.85*[1 1 1]);
set(ha(3),'facecolor',min(255,[255 153 0]+60)/255);
set(ha(4),'facecolor',min(255,[51 102 255]+60)/255);
ha(1)=[];
legend(ha,'Ice middleware','libOrcaIce (orca-base)','components+utils (orca-components)', 'Location','NorthWest')
%  xlim(datenum([2004;2005],[7;7],[1;1]) )
ylim([-Smax 100])
ax=axis;
xlim( [ax(1) datenum([2006 12 31])] )
ax=axis;
% label releases
mark('x', Torca2 )
i2odd = (1:2:length(Torca2));
ht = text(Torca2(i2odd)+5,.04*ax(4)*ones(length(i2odd),1),Vorca2(i2odd));
set(ht,'fontsize',8)
i2even = (2:2:length(Torca2));
ht = text(Torca2(i2even)+5,.08*ax(4)*ones(length(i2even),1),Vorca2(i2even));
set(ht,'fontsize',8)
datetick( 'x', 12 )
hp = plot( Tplayer, -Splayer(:,ibase)/1e3, 'k--o', Tplayer, Splayer(:,icomp)/1e3, 'k--o' );
set(hp, 'color', .5*[1 1 1] )
hpt = text( Tplayer(2), -Splayer(2,ibase)/1e3-8, 'Player "middleware"');
set(hpt, 'color', .5*[1 1 1] )
hpt = text( Tplayer(2), Splayer(2,icomp)/1e3-5, 'Player "components+utils"');
set(hpt, 'color', .5*[1 1 1] )
%mark('y', Dasn(1,1)/1e3 );
%text( datenum([2003 5 1]), Dasn(1,1)/1e3, 'ASN')
xlabel('Releases')
title('orca-robotics.sf.net')

ht=text(datenum(2003,7,1),-Smax+10, '* generated using David A. Wheeler`s SLOCCount' );
set(ht,'fontsize',8)

return
wpos = get(gcf,'position')
ppos = get(gcf,'paperposition')
ppos(4) = wpos(4) * ppos(3)/wpos(3);
ppos
set(gcf,'paperposition', ppos );
print -dpng 'release_sloc_hist'

set(gca,'xtick',Torca2, 'xticklabel', v2)
mark('x',2005)
text(2005,0,'2005')
