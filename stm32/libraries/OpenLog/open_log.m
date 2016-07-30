%{
 * @authors Pranav Bhounsule, Avik De <avikde@gmail.com>

  This file is part of koduino <https://github.com/avikde/koduino>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
%}
function s = open_log(fname)

if ~exist(fname,'file')
  error('File not found: %s', fname)
end

fileID = fopen(fname);
colnames = fgetl(fileID);
fmt = fgetl(fileID);
fprintf('Opened %s\nKeys: %s\nFormat: %s\n', fname, colnames, fmt)

colnames = strsplit(colnames, ',');
data = zeros([0,length(colnames)]);
i = 0;

try
  while ~feof(fileID)
    alignment1 = fread(fileID,1,'uint8');
    if alignment1 ~= 170
      continue;
    end
    alignment2 = fread(fileID,1,'uint8');
    if alignment2 ~= 187
      continue;
    end

     i = i+1;
     
    for j=1:length(fmt)
      if (fmt(j)=='f')
        type = 'float';
      elseif (fmt(j)=='I')
        type = 'uint32';
      elseif (fmt(j) =='B')
        type = 'uint8';
      else
        error('unspecfied datatype in fmt');
      end
      %fmt(j)
      data(i,j)=fread(fileID,1,type);
    end
%          data(i,1) = fread(fileID,1,'uint32')/1000;
%          data(i,2:21) = fread(fileID,[1,20],'float');
%          data(i,22) = fread(fileID,1,'uint8');
  end
  fclose(fileID);
catch
  disp('Finished reading');
end

% Return data in a struct
s = struct();
for cn=1:length(colnames)
  s = setfield(s,colnames{cn},data(:,cn));
end

end
