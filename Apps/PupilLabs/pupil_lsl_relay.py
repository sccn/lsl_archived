'''
(*)~----------------------------------------------------------------------------------
 Pupil LSL Relay
 Copyright (C) 2012-2016 Pupil Labs

 Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0).
 License details are in the file license.txt, distributed as part of this software.
----------------------------------------------------------------------------------~(*)
'''

from time import sleep
import logging

import zmq
import zmq_tools
from pyre import zhelper

from plugin import Plugin
from pyglui import ui

import pylsl as lsl

logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)

NOTIFY_SUB_TOPIC = 'notify.'
PUPIL_SUB_TOPIC = 'pupil.'
GAZE_SUB_TOPIC = 'gaze'


class Pupil_LSL_Relay(Plugin):
    """Plugin to relay Pupil Capture data to LSL

    All stream outlets are of type _Pupil Capture_.

    Primitive data streams consist of 5 channels (lsl.cf_double64):
        - diameter (-1.0 for gaze streams)
        - confidence
        - timestamp
        - norm_pos.x
        - norm_pos.y

    Python Representation streams consist of 1 channel containing the
    Python repr() string of the datum.

    The plugin provides following outlets:

        if self.relay_pupil:
            - Pupil Primitive Data - Eye 0
            - Pupil Primitive Data - Eye 1
            - Pupil Python Representation - Eye 0
            - Pupil Python Representation - Eye 1

        if self.relay_gaze:
            - Gaze Primitive Data
            - Gaze Python Representation

        if self.relay_notifications:
            - Notifications

    """
    def __init__(self, g_pool, relay_pupil=True, relay_gaze=True,
                 relay_notifications=True):
        """Summary

        Args:
            relay_pupil (bool, optional): Relay pupil data
            relay_gaze (bool, optional): Relay gaze data
            relay_notifications (bool, optional): Relay notifications
        """
        super(Pupil_LSL_Relay, self).__init__(g_pool)
        self.relay_pupil = relay_pupil
        self.relay_gaze = relay_gaze
        self.relay_notifications = relay_notifications
        self.context = g_pool.zmq_ctx
        self.thread_pipe = zhelper.zthread_fork(self.context, self.thread_loop)
        self.menu = None

    def init_gui(self):
        """Initializes sidebar menu"""
        self.menu = ui.Growing_Menu('Pupil LSL Relay')
        self.g_pool.sidebar.append(self.menu)

        def close():
            self.alive = False

        def make_setter(sub_topic, attribute_name):
            def set_value(value):
                setattr(self, attribute_name, value)
                cmd = 'Subscribe' if value else 'Unsubscribe'
                self.thread_pipe.send_string(cmd, flags=zmq.SNDMORE)
                self.thread_pipe.send_string(sub_topic)
            return set_value

        help_str = ('Pupil LSL Relay subscribes to the Pupil ZMQ Backbone'
                    + ' and relays the incoming data using the lab'
                    + ' streaming layer.')
        self.menu.append(ui.Button('Close', close))
        self.menu.append(ui.Info_Text(help_str))
        self.menu.append(ui.Switch('relay_pupil', self,
                                   label='Relay pupil data',
                                   setter=make_setter(PUPIL_SUB_TOPIC,
                                                      'relay_pupil')))
        self.menu.append(ui.Switch('relay_gaze', self,
                                   label='Relay gaze data',
                                   setter=make_setter(GAZE_SUB_TOPIC,
                                                      'relay_gaze')))
        self.menu.append(ui.Switch('relay_notifications', self,
                                   label='Relay notifications',
                                   setter=make_setter(NOTIFY_SUB_TOPIC,
                                                      'relay_notifications')))

    def get_init_dict(self):
        """Store settings"""
        return {'relay_pupil': self.relay_pupil, 'relay_gaze': self.relay_gaze,
                'relay_notifications': self.relay_notifications}

    def deinit_gui(self):
        if self.menu:
            self.g_pool.sidebar.remove(self.menu)
            self.menu = None

    def cleanup(self):
        """gets called when the plugin get terminated.
           This happens either voluntarily or forced.
        """
        self.shutdown_thread_loop()
        self.deinit_gui()

    def shutdown_thread_loop(self):
        if self.thread_pipe:
            self.thread_pipe.send_string('Exit')
            while self.thread_pipe:
                sleep(.1)

    def thread_loop(self, context, pipe):
        """Background Relay Thread

        1. Connects to the ZMQ backbone
        2. Creates LSL outlets according to settings
        3. Subscibes to according topics
        4. Loop
        4.1. Relay data
        4.2. Handle un/subscription events
        4.3. Handle exit event
        5. Shutdown background thread
        """
        try:
            logger.debug('Connecting to %s...' % self.g_pool.ipc_sub_url)
            inlet = zmq_tools.Msg_Receiver(context, self.g_pool.ipc_sub_url,
                                           block_until_connected=False)
            poller = zmq.Poller()
            poller.register(pipe, flags=zmq.POLLIN)
            poller.register(inlet.socket, flags=zmq.POLLIN)

            pupil_outlets = None
            gaze_outlets = None
            notification_outlet = None

            # initial subscription
            if self.relay_pupil:
                pupil_outlets = self._create_pupil_lsl_outlets()
                inlet.subscribe(PUPIL_SUB_TOPIC)
            if self.relay_gaze:
                gaze_outlets = self._create_gaze_lsl_outlets()
                inlet.subscribe(GAZE_SUB_TOPIC)
            if self.relay_notifications:
                notification_outlet = self._create_notify_lsl_outlet()
                inlet.subscribe(NOTIFY_SUB_TOPIC)

            while True:
                items = dict(poller.poll())

                if inlet.socket in items:
                    topic, payload = inlet.recv()
                    if (topic.startswith(PUPIL_SUB_TOPIC)
                            and pupil_outlets):
                        eyeid = payload['id']
                        # push primitive sample
                        outlet = pupil_outlets[eyeid*2]
                        sample = self._generate_primitive_sample(payload)
                        outlet.push_sample(sample)
                        # push python sample
                        outlet = pupil_outlets[eyeid*2+1]
                        outlet.push_sample((repr(payload),))
                        del outlet  # delete reference

                    elif (topic.startswith(GAZE_SUB_TOPIC)
                          and gaze_outlets):
                        # push primitive sample
                        outlet = gaze_outlets[0]
                        sample = self._generate_primitive_sample(payload)
                        outlet.push_sample(sample)
                        # push python sample
                        outlet = gaze_outlets[1]
                        outlet.push_sample((repr(payload),))
                        del outlet  # delete reference

                    elif (topic.startswith(NOTIFY_SUB_TOPIC)
                          and notification_outlet):
                        sample = (payload['subject'], repr(payload))
                        notification_outlet.push_sample(sample)

                    else:
                        logger.debug('Did not handle topic "%s"' % topic)

                if pipe in items:
                    cmd = pipe.recv_string()
                    if cmd == 'Exit':
                        break

                    elif cmd == 'Subscribe':
                        topic = pipe.recv_string()
                        inlet.subscribe(topic)
                        if topic == PUPIL_SUB_TOPIC and not pupil_outlets:
                            pupil_outlets = self._create_pupil_lsl_outlets()
                        elif topic == GAZE_SUB_TOPIC and not gaze_outlets:
                            gaze_outlets = self._create_gaze_lsl_outlets()
                        elif (topic == NOTIFY_SUB_TOPIC
                              and not notification_outlet):
                            notification_outlet = self._create_notify_lsl_outlet()
                        logger.debug('Subscribed to "%s"' % topic)

                    elif cmd == 'Unsubscribe':
                        topic = pipe.recv_string()
                        inlet.unsubscribe(topic)
                        if topic == PUPIL_SUB_TOPIC and pupil_outlets:
                            pupil_outlets = None
                        elif topic == GAZE_SUB_TOPIC and gaze_outlets:
                            gaze_outlets = None
                        elif topic == NOTIFY_SUB_TOPIC and notification_outlet:
                            notification_outlet = None
                        logger.debug('Unubscribed from "%s"' % topic)

        except Exception as e:
            logger.error('Error during relaying data to LSL. '
                         + 'Unloading the plugin...')
            logger.debug('Error details: %s' % e)
        finally:
            logger.debug('Shutting down background thread...')
            self.thread_pipe = None
            self.alive = False

    def _create_pupil_lsl_outlets(self):
        """Creates array of pupil outlets

        Returns:
            list: [eye0.primitive, eye0.python, eye1.primitive, eye1.python]
        """
        lsl_outlets = []
        for eye_id in range(2):
            lsl_outlets.append(self._create_primitive_lsl_outlet(
                'Pupil Primitive Data - Eye %i' % eye_id))
            lsl_outlets.append(self._create_python_repr_lsl_outlet(
                'Pupil Python Representation - Eye %i' % eye_id))
        return lsl_outlets

    def _create_gaze_lsl_outlets(self):
        """Creates array of gaze outlets

        Returns:
            tuple: [gaze.primitive, gaze.python]
        """
        return (self._create_primitive_lsl_outlet('Gaze Primitive Data'),
                self._create_python_repr_lsl_outlet(
                    'Gaze Python Representation'))

    def _create_notify_lsl_outlet(self):
        """Creates notification outlet"""
        notification_info = lsl.StreamInfo(
            name='Notifications',
            type='Pupil Capture',
            channel_count=2,
            channel_format=lsl.cf_string,
            source_id='Notifications @ %s' % self.g_pool.ipc_sub_url)
        self._append_channel_info(notification_info, ("subject",
                                                      "Python Representation"))
        self._append_acquisition_info(notification_info)
        return lsl.StreamOutlet(notification_info)

    @staticmethod
    def _append_acquisition_info(streaminfo):
        """Appends acquisition information to stream description"""
        acquisition = streaminfo.desc().append_child("acquisition")
        acquisition.append_child_value("manufacturer", "Pupil Labs")
        acquisition.append_child_value("source", "Pupil LSL Relay Plugin")

    @staticmethod
    def _append_channel_info(streaminfo, channels):
        """Appends channel information to stream description"""
        xml_channels = streaminfo.desc().append_child("channels")
        for channel in channels:
            xml_channels.append_child("channel").append_child_value("label",
                                                                    channel)

    @staticmethod
    def _generate_primitive_sample(payload):
        """Combine payload's primitive fields into sample"""
        return (payload.get('diameter', -1.0),
                payload['confidence'],
                payload['timestamp'],
                payload['norm_pos'][0],
                payload['norm_pos'][1])

    def _create_primitive_lsl_outlet(self, name):
        """Create 5 channel primitive data outlet"""
        stream_info = lsl.StreamInfo(
            name=name,
            type='Pupil Capture',
            channel_count=5,
            channel_format=lsl.cf_double64,
            source_id='%s @ %s' % (name, self.g_pool.ipc_sub_url))
        self._append_channel_info(stream_info,
                                  ("diameter", "confidence", "timestamp",
                                   "norm_pos_x", "norm_pos_y"))
        self._append_acquisition_info(stream_info)
        return lsl.StreamOutlet(stream_info)

    def _create_python_repr_lsl_outlet(self, name):
        """Create 1 channel python representation outlet"""
        stream_info = lsl.StreamInfo(
            name=name,
            type='Pupil Capture',
            channel_count=1,
            channel_format=lsl.cf_string,
            source_id='%s @ %s' % (name, self.g_pool.ipc_sub_url))
        self._append_channel_info(stream_info, ("Python Representation",))
        self._append_acquisition_info(stream_info)
        return lsl.StreamOutlet(stream_info)
