"""neuroPrax-LSL converter

A TCP/IP client which receives data from the neuroPrax, and publishes it via
lab streaming layer. Requires Python 3.6

Documentation
-------------

Data streaming of the neuroPrax is based on TCP/IP, running five different 
protocols, and limited to a maximum of two clients. 

This software should be run  as a standalone process. It continuously attempts 
to connect to a neuroPrax stream, looking at the respective IP adress, and
receives and handles the different protocols. 

Whenever settings are changed on the neuroPrax, the LSL StreamOutlet
is automatically updated.

Example::

    python lsl_server.py "127.0.0.1"

Args
----

host:str
    ip adress where neuroPrax Software is running. defaults to localhost, i.e
    '127.0.0.1'

.. caution::

If you stop this program with ctrl-c while a stream is received, be aware that
this can result in an incomplete shutdown of the TCP-IP client. It is best
practice to start this converter before starting the first neuroprax
measurement, and stop it after measurement is finished. Considering it 
automatically reconnects and updates the stream outlets, there is usually
no reason to restart this software.
    
License
-------

Copyright (C) 2018 Robert Guggenberger

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice, the below funding statement, and this permission 
notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Except as contained in this notice, the name of the author shall not be used in
advertising or otherwise to promote the sale, use or other dealings in this 
Software without prior written authorization from the author.

Funding
-------    

This work was funded by BMBF IMONAS - FKZ 13GW0119
"""
import sys
import threading
import socket
import struct
from uuid import getnode
import pylsl
__version__ = '0.2.0'
__author__ = 'Robert Guggenberger'
__license__ = 'MIT'


# %%
class ProtocolHandler():
    
    def __init__(self, recv_foo):
        '''inits the handler of the neuroPrax protocol stream
        
        args
        ----
        recv_foo: :meth:`socket.recv`
            the low level function for byte-wise receiving    
        
        '''
        self.recv_bytes = recv_foo
        

    def flush_protocol(self):
        'reads and ignores each byte until the end of current protocol block'
        msg = ''
        while  msg[-4:] != 'end$':
            msg += self.recv_bytes(1).decode('ASCII', errors='replace')            
        print(f'Flushed protocol from {msg[:4]} to {msg[-4:]}')            


    def parse_header(self):
        'reads the header of the current block'
        header = self.recv_bytes(36).decode()
        if 'neuroConn' not in header:
            raise ConnectionError(f'Not a valid header: {header} does not ' +
                                  'contain neuroConn')
        header = header.split('$')[2].split('-')[1].strip()
        return header

    def parse_ender(self):
        'reads the next four byte and checks whether it is a valid end'
        ende = self.recv_bytes(4)
        if not b'end$' == ende:
            raise ConnectionError(f'Not a valid ender: {ende} instead of end$ ')

    # -------------------------------------------------------------------------
    # GIP PROTOCOL
    # -------------------------------------------------------------------------
    def handle_GIP(self):
        '''handle a GIP: General Information Protocol
        
        order of keys is fixed according to protocol
        we therefore go through all keys, and store them 
        in a dictionary
        
        '''
        
        settingList = ['recFileName',
                       'recPathName',
                       'DB_PatName',
                       'DB_PatFirstName',
                       'DB_PatBirthDay',
                       'DB_PatINR',
                       'elecSetup',
                       'fs',
                       'selAlgorithm',
                       'numChannels',
                       'numEXGChannels'
                       ]
        chanInfoList = ['ChannelNames',
                        'ChannelTypes',
                        'ChannelUnits',
                        'ChannelReference']


        # receive information on the generalsetting
        params = self.recv_bytes(1576)
        paramList = params.decode().split('$')

        paramDict = {}
        for key, val in zip(settingList, paramList):
            if 'Channels' in key:
                paramDict[key] = int(val)
            elif 'fs' in key:
                paramDict[key] = int(val)
            else:
                paramDict[key] = val

        # receive information on the channels. As this depends on
        # channel number, it is within a seperate block
        chanInfo = self.recv_bytes(36 * paramDict['numChannels'])

        infoList = chanInfo.decode('cp1252').split('$')
        for idx, key in enumerate(chanInfoList):
            num = paramDict['numChannels']
            paramDict[key] = infoList[(num * idx) : (num * (idx + 1)) ]
                
        # store key information in the instance, to allow transformation
        # of packets to data files (see packet2cnt)
        self.fs = paramDict['fs']

        paramDict['ChannelNames'] = [ch.strip() for
                                    ch in paramDict['ChannelNames']]
        paramDict['ChannelTypes'] = [t.strip() for
                                    t in paramDict['ChannelTypes']]
        
        # lsl xdf specification requests 'microvolts' instead of 'µV'
        units = []
        for ch in paramDict['ChannelUnits']:            
            if ch.strip() == 'µV':
                units.append('microvolts') 
            else:
                units.append(ch.strip()) 
                
        paramDict['ChannelUnits'] = units
        self.settings = paramDict
        return paramDict

    # -------------------------------------------------------------------------
    # MNP PROTOCOL
    # -------------------------------------------------------------------------
    def handle_MNP(self):
        'handle a MNP: Marker Name Protocol'
        markerCount = self.recv_bytes(4)
        markerCount = int(markerCount.decode('cp1252')[:-1])
        params = self.recv_bytes(40 * markerCount)
        paramList = params.decode('cp1252').split('$')

        markerDict = {}
        for mi, mn in zip(paramList[0::2], paramList[1::2]):
            markerDict[mi.strip()] = mn.strip()
        
        self.receivedMNP = True
        return markerDict

    # -------------------------------------------------------------------------
    # ISP PROTOCOL
    # -------------------------------------------------------------------------
    def handle_ISP(self):
        'handle a ISP: Impedance Protocol'
        params = self.recv_bytes(5)
        numChannels = int(params.decode('cp1252').split('$')[0])

        params = self.recv_bytes(12 * numChannels)
        paramList = params.decode('cp1252').split('$')
        
        impedance = {}
        for chanNames, impedanceStatus in zip(paramList[0::2], paramList[1::2]):            
            impedance[chanNames.strip()] = impedanceStatus.strip()
        return {'impedance':impedance}

    # -------------------------------------------------------------------------
    # DP PROTOCOL
    # -------------------------------------------------------------------------
    def handle_DP(self):
        'handle a DP: Data Protocol'
        params = self.recv_bytes(36)
        params = [int(p) for p in params.decode('cp1252').split('$')[0:-1]]
        # print(params)
        #timeStamp = params[0]
        buffer = self.recv_bytes(params[1] * params[2] * 4)
        buffer = [v[0] for v in struct.iter_unpack('<f', buffer)]        
        packet = []
        for i in range(0, len(buffer), params[2]):
            packet.append(buffer[i:i+params[2]])     
        return packet

    # -------------------------------------------------------------------------
            
    def handle_stream(self):
        try:
            header = self.parse_header()
            if 'GIP' in header:
                msg= self.handle_GIP() # dict                  
            elif 'MNP' in header:
                msg = self.handle_MNP() # dict   
            elif 'DP' in header:
                msg = self.handle_DP()                
            elif 'BOP' in header:
                msg = None
            elif 'ISP' in header:
                msg = self.handle_ISP() # dict            
            else:
                msg = None
                print((f'Invalid protocol: {header}'))        
            self.parse_ender()
        except UnicodeDecodeError as e:
            self.flush_protocol()
            header = str(e)
            msg = None
        return header, msg
        
        
# %%  
class StreamReceiver():
    '''Receives the TCP/IP stream from the neuroPrax PC '''
    
    _state_was_sent = 0
    _state = ''
    def __init__(self, host=None, port=8574):
        '''initialize the TCP-IP StreamReceiver
        
        args
        ----
        host:str {None}
            the ip adress of the neuroPrax device
        port: int {8574, 8575}
            the port for raw or corrected data stream
        ''' 

        if host is None:
            raise ValueError('Specify a valid host IP')
        self.host = host
        self.port = port        
        self.state = 'standby'
        self.settings = {}
                
    def get_state(self):        
        if self._state == 'timeout_recv':
            msg = f'Standby: Waiting for neuroPrax to stream'                    
        elif self._state == 'standby':
            msg = f'Standby: Waiting for neuroPrax to connect'
        elif self._state == 'connected':
            msg = f'Connected with neuroPrax software'   
        elif self._state == 'receiving':
            msg = ('Connected with neuroPrax stream \n' + 
                  'Waiting for settings (MNP, GIP)')
        else:
            msg = f'Logging Error: Undefined state'
        
        if self._state_was_sent == False:
            self._state_was_sent = True             
            return msg

    def set_state(self, state:str):        
        if state != self._state:
            self._state_was_sent = False
            self._state = state
            print('\n\r-------------------')
            print(self.state)

        
    state = property(get_state, set_state)
        
    def connect(self):
        'connect with the TCP-IP protocol stream'
        self.interface = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.interface.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.interface.settimeout(1)
        connected = False
        while not connected:
            try:
                self.interface.connect((self.host, self.port))
                connected = True                
                self.state = 'connected'
            except socket.timeout as e:
                self.state = 'standby'
            except OSError as e:
                # if neuroPrax Software was not started yet, it throws OsError
                self.state = 'standby'                
                
        return connected
    
    def recv_bytes(self, numBytes=1):
        'blocks until timeout or it has read the next numBytes bytes'
        cntBytes = 0
        dataBuffer = b''
        while cntBytes < numBytes:
            try:
                msg = self.interface.recv(numBytes - cntBytes)
                dataBuffer += msg
                cntBytes = len(dataBuffer)
                self.state = 'receiving'
            except socket.timeout as e:
                self.state = 'timeout_recv'
            except OSError as e:
                raise ConnectionResetError('Standby: Waiting for neuroPrax')
                                        
        return dataBuffer
    
    def close(self):
        'close the TCP-IP stream'
        if hasattr(self, 'interface'):
            try:
                self.interface.shutdown(2)
                self.interface.close()      
                del self.interface
            except OSError:
                self.state = 'standby'
        return False
    
    def _is_connected(self):
        return hasattr(self, 'interface')
    is_connected = property(_is_connected)


# %%
class LSL_Publisher:
    
    @classmethod
    def from_settings(cls, settings):
               
        obj = cls(chan_names=settings['ChannelNames'], 
                  units=settings['ChannelUnits'],
                  types=settings['ChannelTypes'],
                  fs=settings['fs'])
        return obj
    
    def __init__(self, 
                 chan_names=['Mock'], 
                 units='microvolts', 
                 types='EEG',
                 fs=0):
        self.fs = fs
        self.chan_names = chan_names
        self.mrk_chan_idx = []
        if 'MRK' in chan_names:
            self.mrk_chan_idx.append(chan_names.index('MRK'))
        if 'DTRIG' in chan_names:
            self.mrk_chan_idx.append(chan_names.index('DTRIG'))
        
        source_id = str(hex(getnode()))
        # ---------------------------------------------------------------------
        # create eeg info and stream
        eeg_info= pylsl.StreamInfo(name='neuroPrax',
                             type='EEG',
                             channel_count=len(chan_names),                              
                             nominal_srate=fs, 
                             channel_format='float32',
                             source_id='neuroPrax_EEG_' + source_id)
                
        acq = eeg_info.desc().append_child("acquisition")
        acq.append_child_value('manufacturer', 'neuroConn')
        acq.append_child_value('model', 'neuroPrax')        
        acq.append_child_value('precision', '24')
        acq.append_child_value('compensated_lag', '0')
        
        channels = eeg_info.desc().append_child("channels")
        for c, u, t in zip(chan_names, units, types):
                    channels.append_child("channel") \
                    .append_child_value("label", c) \
                    .append_child_value("unit", u) \
                    .append_child_value("type", t)   
                    
        self.eeg_stream = pylsl.StreamOutlet(eeg_info,
                                       chunk_size=0,
                                       max_buffered=1)
        

        print(f'EEG StreamOutlet created with source_id ' +
              f'"neuroPrax_EEG_{source_id}"')
      
        
        # ---------------------------------------------------------------------
        mrk_info= pylsl.StreamInfo(name='neuroPrax_Markers',
                                   type='Markers',
                                   channel_count=1,  
                                   nominal_srate=0, 
                                   channel_format='string',
                                   source_id='neuroPrax_MRK_' + source_id)  
        
        self.mrk_stream = pylsl.StreamOutlet(mrk_info)

        print(f'Marker StreamOutlet created with source_id ' +
              f'"neuroPrax_MRK_{source_id}"')
        # ---------------------------------------------------------------------
        self.l0 = pylsl.local_clock()                     
        
    def publish(self, item):
        '''publish data and markers with lsl
        
        data is streamed 
        
        Information about markers is contained in two digital channels. The 
        default duration of a marker is at least 5ms. Markers are parsed from 
        this data stream, and their onset is transmitted with a timestamp 
        calculated based on the streams sampling rate.
        '''
        timestamp = pylsl.local_clock()
        # publish eeg
        self.eeg_stream.push_chunk(item, timestamp)        

        # log time since the stream started
        reltime = timestamp-self.l0
        logmsg = 'Streaming for {0:4.5f}s'.format(reltime)        
        print(logmsg, end='\r')

        # publish markers        
        for idx in self.mrk_chan_idx:             
            old_mrk_val = '0'
            for i in item:
                sample = int(i[idx])
                if sample > 0:
                    ts = timestamp + idx/self.fs
                    rs = reltime + idx/self.fs
                    mrk_val = str(sample)
                    if mrk_val != old_mrk_val:
                        self.mrk_stream.push_sample([mrk_val], ts)
                        old_mrk_val = mrk_val
                        logmsg = 'Marker at {0:>10.5f} : {1:>30.27s}'.format(rs, mrk_val)
                        print(logmsg, end='\n')
                    
             
     
# %%
class neuroPraxService(threading.Thread):

    settings = {}    
   
    def __init__(self, host=None, port=8574):
        threading.Thread.__init__(self)
        self.receiver = StreamReceiver(host=host, port=port)   
        self.handler = ProtocolHandler(self.receiver.recv_bytes)                
        
    def create_item(self):
        '''return either the last packet of data as ndarray or
        settings as dictionary
        
        
        The first two protocols the neuroPrax sends are the MNP and GIP. 
        These protocols contain information on sampling rate, channels, etc.
        Because the pylsl.StreamOutlet takes settings only during init,
        whenever these protocols are received, a new StreamOutlet is created.
        This ensures that the settings of the StreamOutlet are always up to 
        date with the neuroPrax.
        '''
        header, msg = self.handler.handle_stream()
        if header == 'DP':
            # create a publisher, if settings are already available
            # otherwise return None
            if not hasattr(self, 'publisher'):
                try:
                    self.publisher = LSL_Publisher.from_settings(self.settings)                    
                except KeyError as e:
                    return None
            return msg
        elif header == 'MNP' or header == 'GIP':
            if hasattr(self, 'publisher'):
                del self.publisher
                print('Received settings from neuroPrax')
            self.settings.update(msg)
            return None
        elif header == 'ISP':
            self.settings.update(msg)
            return None
        elif header =='BOP':
            self.receiver.close()            
            self.receiver.connect()
            print(f'Neuroprax Buffer Overflow. Resetting connection')
            return None        
    
    def run(self):          
        while True:
            if not self.receiver.is_connected:
                self.receiver.connect()  
            try:
                item = self.create_item() 
            except ConnectionError or ConnectionResetError as e:
                self.receiver.close()
                print(str(e))                
                continue
                
            if item is not None:                 
                self.publisher.publish(item)
        
        
#%%
def _help():
    print(f'Showing help for neuroPrax-LSL converter Version: {__version__}')
    print(__doc__)    

            
def _start_server(host='127.0.0.1'):
    # start server
    print(f'Starting neuroPrax-LSL converter Version: {__version__}')
    client = neuroPraxService(host=host, port=8574)    
    client.start()
    

# %%
if __name__ == "__main__":
     
    # parse arguments
    
    args = sys.argv[1:]

    if '--help' in args:
          _help()         
          
    else:
        if len(args) > 0:
            host = args[0]
            print(f'I was told to search at {host}')
            _start_server(host)
        else:
            host = '127.0.0.1'  
            print(f'Attempting to find the host at {host}')
            _start_server(host)
    
