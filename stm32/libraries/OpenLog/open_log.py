'''
 * @authors Avik De <avikde@gmail.com>

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
'''
import time
from numpy import *
from pylab import *
import struct
import sys
import os, fnmatch
import argparse

def findFileByTrialNum(pattern, pathRoot):
  pattern = '*' + str(pattern) + '.TXT'
  if pathRoot is None:
    path = '.'
  else:
    path = pathRoot
  # path = '.'
  result = []
  for root, dirs, files in os.walk(path):
    for name in files:
      if fnmatch.fnmatch(name, pattern):
        result.append(os.path.join(root, name))
  return result


def readLogFile(filename, hasAlignmentWord = True, quiet = False):
  # see if user gave trial number
  try:
    trialnum = int(filename)
    filename = findFileByTrialNum(trialnum)[0]
  except ValueError:
    pass

  f = open(filename, 'rb')
  # if not quiet:
  print('Opened'),
  print(filename)

  # First two lines are special
  keys = f.readline().rstrip('\n').split(',')
  fmt = f.readline().rstrip('\n')

  sz = struct.calcsize(fmt)
  ncols = len(fmt)

  if not quiet:
    print('Keys:'),
    print(keys)
    print('Format:'),
    print(fmt)
    print('Size:'),
    print(sz)

  #data = {}#zeros((0,ncols))
  data = dict.fromkeys(keys,zeros((0)))

  # Now just data
  if not hasAlignmentWord:
    line = f.read(sz)
    while len(line) == sz:
      tup = struct.unpack(fmt, line)

      for i in range(ncols):
        data[keys[i]] = r_[data[keys[i]],tup[i]]#vstack((data,array(tup)))
      line = f.read(sz)

  else:
    wholeFile = f.read()
    # split by alignment word
    chunks = wholeFile.split('\xaa\xbb')
    for chunk in chunks:
      if len(chunk) == sz:
        # good chunk
        tup = struct.unpack(fmt, chunk)
        for i in range(ncols):
          data[keys[i]] = r_[data[keys[i]],tup[i]]

  if not quiet:
    print('Read data of length'),
    print(data[keys[0]].shape[0])
    print 'Avg data rate = '+str(1000/float(mean(diff(data['t']))))+' Hz'

  return data

#
def trimData(data,inds):
  for key in data.keys():
    data[key] = data[key][inds]
  return data

def getRegions(tCol, stateCol = None):
  if stateCol is None:
    return None

  boundaries = r_[0, where(diff(stateCol) != 0)[0]]
  regions = zeros((0,3), dtype=int)

  if size(boundaries) > 1:
    for i in range(1,size(boundaries)):
      aa = (boundaries[i-1], boundaries[i])
      regions = vstack((regions, array([stateCol[boundaries[i]], aa[0], aa[1]], dtype=int)))
    # Add last region
    regions = vstack((regions, array([stateCol[boundaries[i]+1], aa[1], tCol.shape[0]-1], dtype=int)))
  return regions

def drawRegions(tCol, stateCol = None):
  if stateCol is None:
    return

  regions = getRegions(tCol, stateCol)
  cm = get_cmap('brg')

  for r in regions:
    bb = interp(r[0], r_[min(stateCol), max(stateCol)], r_[0,1])
    axvspan(tCol[r[1]], tCol[r[2]], facecolor=cm(bb), alpha=0.2, ec='none')

  xlim(min(tCol),max(tCol))

def drawVLines(tCol, every=None):  
  if every is None:
    return
  for x in arange(0,max(tCol),every):
    if x > min(tCol):
      axvline(x=x,ymin=-1000,ymax=1000,color='black',alpha=0.2)

def plotTrial(filename, pathRoot = None, trialType = None, trimRange = None, vlineEvery = None):
  # see if user gave trial number
  try:
    trialnum = int(filename)
    filename = findFileByTrialNum(trialnum, pathRoot)[0]
  except:
    print 'Trial not found!', filename
    return

  beQuiet = (trialType == 'noplot')
  data = readLogFile(filename, hasAlignmentWord=True, quiet=beQuiet)
  if trimRange is not None:
    data = trimData(data, where((data['t'] >= trimRange[0]) & (data['t'] <= trimRange[1]))[0])

  if trialType == 'noplot':
    return data

  try:
    figure(int(filename[-9:-4]))
  except:
    figure()

  if trialType == 'hop' or trialType == 'dwhop':

    # bperp = array([0.84304993, 0.53783531])
    # bperp = array([3.7, 1])#for 722
    # bperp = array([0.9, 1])#pre 1082
    bperp = array([2, 1])
    bperp = bperp / norm(bperp)
    print(bperp)
    b = r_[-bperp[1],bperp[0]]
    # Ad-hoc fix to line up IMU data (q3) which is laggy by ~30ms
    # data['q3'][:-7] = data['q3'][7:]
    # data['q3d'][:-7] = data['q3d'][7:]
    qa = vstack((data['q3'],data['q4']))
    qad = vstack((data['q3d'],data['q4d']))

    nrows = 2 if (trialType == 'dwhop') else 9

  if (trialType == 'hop'):
    
    ''' ORIGINAL HOPPING '''
    subplot(nrows,1,1)
    data['st'][where(data['st'] > 6)] = 0
    plot(data['t'], data['st'])
    drawRegions(data['t'], data['st'])
    drawVLines(data['t'], vlineEvery)
    ylabel('states')

    subplot(nrows,1,2)
    plot(data['t'], data['q2'] - data['q3'])
    plot(data['t'], -0.3*ones(size(data['t'])),'--')
    drawRegions(data['t'], data['st'])
    drawVLines(data['t'], vlineEvery)
    ylabel('q2-q3')

    subplot(nrows,1,3)
    plot(data['t'], qa[0,:])
    plot(data['t'], qa[1,:])
    plot(data['t'], 0 * ones(size(data['t'])),'--')
    # plot(data['t'], -math.pi * ones(size(data['t'])),'--')
    # plot(data['t'], math.pi * ones(size(data['t'])),'--')
    # plot(data['t'], 0.5*math.pi * ones(size(data['t'])),'--')
    drawRegions(data['t'], data['st'])
    drawVLines(data['t'], vlineEvery)
    ylabel('q3, q4')

    subplot(nrows,1,4)
    #0.2 * data['q3'] - 0.02 * data['q4dot']
    plot(data['t'], data['az'])
    # plot(data['t'], corraz)
    plot(data['t'], 0 * ones(size(data['t'])),'--')
    plot(data['t'], -1 * ones(size(data['t'])),'--')
    drawRegions(data['t'], data['st'])
    drawVLines(data['t'], vlineEvery)
    ylabel('az')

    subplot(nrows,1,5)
    #0.2 * data['q3'] - 0.02 * data['q4dot']
    plot(data['t'], data['q3d'])
    # plot(data['t'], corraz)
    plot(data['t'], 0 * ones(size(data['t'])),'--')
    drawRegions(data['t'], data['st'])
    drawVLines(data['t'], vlineEvery)
    ylabel('q3d')

    subplot(nrows,1,6)
    plot(data['t'], data['phi'])
    # plot(data['t'], dot(bperp,qad))
    # plot(data['t'], data['phid'])
    plot(data['t'], 0 * ones(size(data['t'])),'k--')
    drawRegions(data['t'], data['st'])
    drawVLines(data['t'], vlineEvery)
    ylabel('phi,phid')
    print(var(dot(bperp,qa)))

    subplot(nrows,1,7)
    plot(data['t'], data['vl'])
    plot(data['t'], data['vt'])
    plot(data['t'], 0 * ones(size(data['t'])),'k--')
    plot(data['t'], 1 * ones(size(data['t'])),'k--')
    plot(data['t'], -1 * ones(size(data['t'])),'k--')
    drawRegions(data['t'], data['st'])
    drawVLines(data['t'], vlineEvery)
    ylabel('vl,vt')

    subplot(nrows,1,8)
    #0.2 * data['q3'] - 0.02 * data['q4dot']
    plot(data['t'], data['velf'])
    # plot(data['t'], corraz)
    plot(data['t'], 0 * ones(size(data['t'])),'--')
    # plot(data['t'], 300 * ones(size(data['t'])),'--')
    drawRegions(data['t'], data['st'])
    drawVLines(data['t'], vlineEvery)
    ylabel('velf')

    subplot(nrows,1,9)
    data['ht'][where(data['ht'] > 50)[0]] = NaN
    #0.2 * data['q3'] - 0.02 * data['q4dot']
    plot(data['t'], data['ht'])
    # plot(data['t'], corraz)
    plot(data['t'], 0 * ones(size(data['t'])),'--')
    plot(data['t'], 9.2 * ones(size(data['t'])),'--')
    drawRegions(data['t'], data['st'])
    drawVLines(data['t'], vlineEvery)
    ylabel('ht')

  elif trialType == 'dwhop':
    #DW
    figure(figsize=(5,2))

    subplot2grid((2,2),(0,0))
    plot(0.001*data['t'], dot(a,qa), lw=1.5)
    plot(0.001*data['t'], 0 * ones(size(data['t'])),'k--')
    drawRegions(0.001*data['t'], data['st'])
    drawVLines(0.001*data['t'], vlineEvery)
    ylabel(r'$\phi$ (rad)', fontsize=15)
    gca().xaxis.set_ticks([])
    gca().yaxis.set_ticks([-0.5,0,0.5])

    subplot2grid((2,2),(1,0))
    plot(0.001*data['t'], dot(b,qa), lw=1.5)
    plot(0.001*data['t'], 0 * ones(size(data['t'])),'k--')
    drawRegions(0.001*data['t'], data['st'])
    drawVLines(0.001*data['t'], vlineEvery)
    ylabel(r'$\vartheta$ (rad)', fontsize=15)
    gca().xaxis.set_ticks([22,22.5,23])
    gca().yaxis.set_ticks([0,1,2])

    subplot2grid((2,2),(0,1))
    plot(0.001*data['t'], dot(a,qad), lw=1.5)
    plot(0.001*data['t'], 0 * ones(size(data['t'])),'k--')
    drawRegions(0.001*data['t'], data['st'])
    drawVLines(0.001*data['t'], vlineEvery)
    ylabel(r'$\dot \phi$', fontsize=15)
    gca().xaxis.set_ticks([])
    gca().yaxis.set_ticks([])

    subplot2grid((2,2),(1,1))
    plot(0.001*data['t'], dot(b,qad), lw=1.5)
    plot(0.001*data['t'], 0 * ones(size(data['t'])),'k--')
    drawRegions(0.001*data['t'], data['st'])
    drawVLines(0.001*data['t'], vlineEvery)
    ylabel(r'$\dot \vartheta$', fontsize=15)
    gca().xaxis.set_ticks([22,22.5,23])
    gca().yaxis.set_ticks([])

    tight_layout()

  elif trialType == 'dwhop2':
    figure(figsize=(20,4))


    subplot(2,1,1)
    plot(0.001*data['t'], data['q2'], lw=3)
    plot(0.001*data['t'], data['q3'], lw=3)
    plot(0.001*data['t'], data['q4'], lw=3)
    plot(0.001*data['t'], 0 * ones(size(data['t'])),'--')
    # plot(data['t'], -math.pi * ones(size(data['t'])),'--')
    # plot(data['t'], math.pi * ones(size(data['t'])),'--')
    # plot(data['t'], 0.5*math.pi * ones(size(data['t'])),'--')
    drawRegions(0.001*data['t'], data['st'])
    drawVLines(0.001*data['t'], vlineEvery)
    ylabel('Angles (rad)', fontsize=15)
    gca().xaxis.set_ticks([])
    gca().yaxis.set_ticks([-0.5,0,0.5])

    subplot(2,1,2)
    data['ht'][where(data['ht'] > 50)[0]] = NaN
    #0.2 * data['q3'] - 0.02 * data['q4dot']
    plot(0.001*data['t'], data['ht'], lw=3)
    # plot(data['t'], corraz)
    plot(0.001*data['t'], 0 * ones(size(data['t'])),'--')
    plot(0.001*data['t'], 9.2 * ones(size(data['t'])),'--')
    drawRegions(0.001*data['t'], data['st'])
    drawVLines(0.001*data['t'], vlineEvery)
    ylabel('Height (cm)', fontsize=15)
    xlabel('Time (s)', fontsize=15)
    # gca().xaxis.set_ticks([27,28,29,30])
    gca().yaxis.set_ticks([5,10,15,20])
    tight_layout()

  elif trialType == 'walk':

    clf()
    nrows = 5

    subplot(nrows,1,1)
    plot(data['t'], data['st'])
    drawRegions(data['t'], data['st'])
    drawVLines(data['t'], vlineEvery)
    ylabel('states')

    subplot(nrows,1,2)
    plot(data['t'], data['q2'] - data['q3'])
    drawRegions(data['t'], data['st'])
    drawVLines(data['t'], vlineEvery)
    ylabel('q2-q3')

    subplot(nrows,1,3)
    plot(data['t'], data['q3'])
    plot(data['t'], data['q4'])
    plot(data['t'], 0 * ones(size(data['t'])),'--')
    plot(data['t'], math.pi/2.0 * ones(size(data['t'])),'--')
    drawRegions(data['t'], data['st'])
    drawVLines(data['t'], vlineEvery)
    ylabel('q3, q4')

    subplot(nrows,1,4)
    plot(data['t'], data['vl'])
    plot(data['t'], data['vt'])
    plot(data['t'], 0 * ones(size(data['t'])),'--')
    drawRegions(data['t'], data['st'])
    drawVLines(data['t'], vlineEvery)
    ylabel('voltage')

    subplot(nrows,1,5)
    plot(data['t'], data['velf'])
    plot(data['t'], 0 * ones(size(data['t'])),'--')
    drawRegions(data['t'], data['st'])
    drawVLines(data['t'], vlineEvery)
    ylabel('velf')

  elif trialType == 'freefall':
    subplot(311)
    plot(data['t'], data['q3'])
    plot(data['t'], data['q4'])
    plot(data['t'], 0 * ones(size(data['t'])),'--')

    drawVLines(data['t'], vlineEvery)
    drawRegions(data['t'], data['st'])

    ylabel('q3, q4')

    subplot(312)
    plot(data['t'], data['q4d'])

    drawVLines(data['t'], vlineEvery)

    drawRegions(data['t'], data['st'])
    ylabel('q4dot')

    subplot(313)
    plot(data['t'], data['az'])

    drawVLines(data['t'], vlineEvery)

    drawRegions(data['t'], data['st'])
    ylabel('az')

  elif trialType == 'imudebug':
    subplot(311)
    plot(data['t'], data['ax'])
    plot(data['t'], data['ay'])
    plot(data['t'], data['az'])
    subplot(312)
    plot(data['t'], data['gx'])
    plot(data['t'], data['gy'])
    plot(data['t'], data['gz'])
    subplot(313)
    # replicate complementary filter that was in the code
    q3rep = zeros(size(data['t']))
    q3acc = zeros(size(data['t']))
    q3rep[0] = data['q3'][0]
    for t in range(1,len(q3rep)):
      q3rep[t] = q3rep[t-1] + data['gy'][t] * 0.001
      force = fabs(data['ax'][t]) + fabs(data['ay'][t]) + fabs(data['az'][t])
      q3acc[t] = -arctan2(data['ax'][t], -data['az'][t])
      q3rep[t] = q3rep[t] * 0.95 + q3acc[t] * 0.05
    plot(data['t'], q3acc)
    plot(data['t'], data['q3'])
    plot(data['t'], q3rep)

    print(diff(data['t']))

  elif trialType == 'velfdebug':
    subplot(211)
    plot(data['t'], data['velf'])
    drawRegions(data['t'], data['st'])
    subplot(212)
    plot(data['t'], -data['velfIMU']*10000)

    drawRegions(data['t'], data['st'])


  elif trialType == 'leap':
    # figure(figsize=(20,4))


    subplot(5,1,1)
    plot(0.001*data['t'], data['lpos'], lw=2)
    plot(0.001*data['t'], 0 * ones(size(data['t'])),'--')
    drawRegions(0.001*data['t'], data['st'])
    drawVLines(0.001*data['t'], vlineEvery)
    ylabel('Leg pos', fontsize=15)
    gca().xaxis.set_ticks([])
    # gca().yaxis.set_ticks([-0.5,0,0.5])

    subplot(5,1,2)
    plot(0.001*data['t'], data['tpos'], lw=2)
    plot(0.001*data['t'], 0 * ones(size(data['t'])),'--')
    drawRegions(0.001*data['t'], data['st'])
    drawVLines(0.001*data['t'], vlineEvery)
    ylabel('Tail pos', fontsize=15)
    gca().xaxis.set_ticks([])

    subplot(5,1,3)
    plot(0.001*data['t'], data['ax'], lw=2)
    plot(0.001*data['t'], data['ay'], lw=2)
    plot(0.001*data['t'], data['az'], lw=2)
    plot(0.001*data['t'], 0 * ones(size(data['t'])),'--')
    plot(0.001*data['t'], -10 * ones(size(data['t'])),'--')
    drawRegions(0.001*data['t'], data['st'])
    drawVLines(0.001*data['t'], vlineEvery)
    ylabel('Accel', fontsize=15)
    gca().xaxis.set_ticks([])

    subplot(5,1,4)
    plot(0.001*data['t'], data['gx'], lw=2)
    plot(0.001*data['t'], data['gy'], lw=2)
    plot(0.001*data['t'], data['gz'], lw=2)
    plot(0.001*data['t'], 0 * ones(size(data['t'])),'--')
    drawRegions(0.001*data['t'], data['st'])
    drawVLines(0.001*data['t'], vlineEvery)
    ylabel('Gyro', fontsize=15)

    subplot(5,1,5)
    plot(0.001*data['t'], data['phi'], lw=2)
    plot(0.001*data['t'], 0 * ones(size(data['t'])),'--')
    drawRegions(0.001*data['t'], data['st'])
    drawVLines(0.001*data['t'], vlineEvery)
    ylabel('Pitch', fontsize=15)

  elif trialType == 'turn':
    # figure(figsize=(20,4))

    subplot(3,1,1)
    plot(0.001*data['t'], data['lposL'], lw=2)
    plot(0.001*data['t'], data['lposR'], lw=2)
    plot(0.001*data['t'], 0 * ones(size(data['t'])),'--')
    drawRegions(0.001*data['t'], data['st'])
    drawVLines(0.001*data['t'], vlineEvery)
    ylabel('Leg pos', fontsize=15)
    gca().xaxis.set_ticks([])
    # gca().yaxis.set_ticks([-0.5,0,0.5])

    subplot(3,1,2)
    plot(0.001*data['t'], data['tpos0'], lw=2)
    plot(0.001*data['t'], data['tpos1'], lw=2)
    plot(0.001*data['t'], 0 * ones(size(data['t'])),'--')
    drawRegions(0.001*data['t'], data['st'])
    drawVLines(0.001*data['t'], vlineEvery)
    ylabel('Tail pos', fontsize=15)
    gca().xaxis.set_ticks([])

    subplot(3,1,3)
    plot(0.001*data['t'], data['roll'], lw=2)
    plot(0.001*data['t'], data['pitch'], lw=2)
    plot(0.001*data['t'], data['yaw'], lw=2)
    plot(0.001*data['t'], 0 * ones(size(data['t'])),'--')
    drawRegions(0.001*data['t'], data['st'])
    drawVLines(0.001*data['t'], vlineEvery)
    ylabel('RPY', fontsize=15)

    # tight_layout()

  else:
    close('all')
    # Just plot all the data in rows
    # nrows = size(data.keys()) - 1
    nrows = 4
    currow = 1

    for key in data.keys():
      # For legibility open a new figure after 4
      if currow > 4:
        figure()
        currow = 1
      if key != 't' and key != 'st' and key != 'mo':
        subplot(nrows,1,currow)
        plot(0.001*data['t'], data[key])
        if 'mo' in data.keys():
          drawRegions(0.001*data['t'], data['mo'])
        if 'st' in data.keys():
          drawRegions(0.001*data['t'], data['st'])
        ylabel(key)
        currow = currow+1
        tight_layout()

  return data


#####################################################################################
''' SCRIPT '''

if __name__ == "__main__":

  parser = argparse.ArgumentParser(description='Plot some logs.')
  parser.add_argument('filename', help='filename to open (LOGxxxxx.TXT)')
  parser.add_argument('-p', dest='pathRoot', help='Path under which to search for LOG*****.TXT files')
  parser.add_argument('-e', dest='trialType', help='experiment type {hop,freefall} (default: plot all data)')
  parser.add_argument('-t', nargs=2, dest='trimt', type=int, help='trim by time (default: show all)', metavar=('tstart', 'tend'))
  parser.add_argument('-v', dest='vlineEvery', type=int, help='vertical line every')
  args = parser.parse_args()

  data = plotTrial(args.filename, args.pathRoot, args.trialType, args.trimt, args.vlineEvery)
  show()

  # A = aEstimator()

