
=== Table of Content Types ===

In the following the standardized labels for content types (for streams or channels) are specified. These are case sensitive.
If your data conforms to one of these types, it is highly recommended that you use stick to these default labels to maximize compatibility between various client applications.

There is an effort related to LSL that aims at providing a set of recommendations for stream meta-data. This is the XDF project (http://code.google.com/p/xdf/); the meta-data section
is at: http://code.google.com/p/xdf/wiki/MetaData

Content Type Description																		Recommended Unit		Recommended Channel Format
EEG			 Electroencephalography (electrical activity of the scalp/brain)					Microvolts				float32 (for Amp resolution up to 24bit, otherwise possibly double64)
MEG			 Magnetoencephalograpy (magnetic activity near the scalp/brain)						Femtotesla				double64
ECoG		 Electrocorticography (electrical activity measured on the cortex)					Microvolts				float32 (for Amp resolution up to 24bit, otherwise possibly double64)
EMG			 Electromyography (electrical activity of the muscles)								Microvolts				float32
EOG			 Electrooculography (electrical activity of the eyes)								Microvolts				float32
ECG			 Electrocardiography (electrical activity of the heart)								Microvolts				float32
GSR			 Galvanic Skin Response (skin conductance)											Microsiemens			float32
Audio		 Audio signals.																		(-1..+1)				float32
Gaze		 Gaze information (screen intersection, pupil diameter, world ray/intersections)	Relative screen coords  float32
																								pupil diameter in mm
																								3d positions in Meters
MoCap		 Data from motion-capture systems (positions, rarely also orientations)				Meters					float32
Trigger		 Trigger channel data (regularly sampled).											Positive integers		float32 (or int32 to support event markers greater than 1677216)
Markers		 Application-generated event marker data (irregularly sampled )						Positive integers		int32
MentalState  Mental state estimate (as produced by Brain-Computer Interfaces, etc.).            Probabilities preferred float32
                                                                                                if state space is
                                                                                                discrete.

If you have a content type that is missing from this list and would like to propose it for addition please send an email to mgrievich@ucsd.edu.


=== Configuration File ===

Under most circumstances the LSL API needs no configuration. However, for highly specialized usage scenarios (such as internet-wide operation, hiding
concurrent recording sessions from each other on the same network, or dealing with extremely restrictive network configurations) it is possible to
create a file in a special location to have clients automatically adopt custom settings without any need for customizations in the recording programs.

The locations where the a config file can be placed are:
lsl_api.cfg			      (in the same folder as the recording program, highest precedence)
~/lsl_api/lsl_api.cfg	  (in the user's home directory, second-highest precedence)
/etc/lsl_api/lsl_api.cfg  (in a global directory of the machine - possibly mounted over the network, lowest precedence)

Files in locations with higher precedence override those in locations with lower precedence.

The configuration file is structured as in the following example (the example describes the full default settings -- a typical file would contain only a subset of settings).


[ports]

; This port is used by machines to advertise and request streams.
MulticastPort = 16571

; This is where the range of ports to serve data and service information begins (growing upwards according to the PortRange).
BasePort = 16572

; Ports from the BasePort to BasePort+Portrange-1 are assigned to both TCP data ports (on the even ports, if the BasePort is odd)
; and UDP service ports (on odd ports, if BasePort is even); since these ports are occupied in pairs, there can effectively be
; PortRange/2 stream outlets coexisting on a single machine. A new outlet will occupy a successively higher pair of ports when
; lower ones are occupied. The number of coexistant outlets can be increased by increasing this number. However, note that if
; multicast and broadcast or all UDP transmission are disabled on some router, the peers will need to "manually" scan this range,
; which can be slow on such a network. Also note that, to communicate with external parties, the port range needs to be open in the
; respective firewall configurations.
PortRange = 32

; How to treat IPv6: can be "disable" (then only v4 is used), or "allow" (then both are used side by side) or "force" (then only v6 is used).
IPv6 = allow


[multicast]

; The scope within which one's outlets and inlets are visible to each other. This can be machine (local to the machine),
; link (local to the subnet), site (local to the site as defined by local policy), organization (e.g., campus), or global.
; Always use only the smallest scope that works for your goals. This setting effectively merges the contents of
; MachineAdresses, LinkAddresses, SiteAddresses, OrganizationAddresses, and GlobalAddresses, and sets the packet
; TTL to one of the settings: 0, 1, 24, 32, or 255. If you share streams with remote collaborators, consider using the
; KnownPeers setting under [lab] (thus listing their machines directly, which is more likely to work than internet-scale
; multi-casting). Another possibility is to use the AddressesOverride and TTLOverride settings to avoid pulling in every
; site at intermediate scopes.
ResolveScope = site

; These are the default address pools for VisibilityScope. The following lists of addresses are merged according
; to the VisibilityScope setting to yield the set of addresses considered for communication.
; Note that making an uninformed/unfortunate address choice can interfere with your site's operations.
MachineAddresses = {FF31:113D:6FDD:2C17:A643:FFE2:1BD1:3CD2}
LinkAddresses = {255.255.255.255, 224.0.0.183, FF02:113D:6FDD:2C17:A643:FFE2:1BD1:3CD2}
SiteAddresses = {239.255.172.215, FF05:113D:6FDD:2C17:A643:FFE2:1BD1:3CD2}
OrganizationAddresses = {239.192.172.215, FF08:113D:6FDD:2C17:A643:FFE2:1BD1:3CD2}
GlobalAddresses = {}

; This allows to override the addresses calculated by VisibilityScope. To communicate conveniently wth a remote party without negotiating
; the involved hostnames, you may choose a privately agreed-on multicast address of the appropriate scope here.
AddressesOverride = {}

; This setting allows to override the packet time-to-live setting. If you intend to use multicast with a custom address to conveniently
; communicate with a specific remote party, you may set this to a sufficiently high level (255 for international collaboration).
TTLOverride = -1


[lab]

; This setting mainly serves as a fallback in case that your network configuration does not permit multicast/broadcast communciation.
; By listing the names or IP addresses of your lab's machines here (both stream providers and stream users) and make the file available
; on all involved machines, you can bypass the need for multicasting. This setting can also be used to link a small collection of machines
; across the internet, provided that the firewall settings of each party permit communication (forward the BasePort to BasePort+PortRange ports).
KnownPeers = {}

; This is the default "vanilla" session id; modify it to logically isolate your recording acitities from others within the scope.
; The session id should not be relied on as a "password" to hide one's data from unpriviledged users; use operating-system and
; network settings for this purpose. Note that you machine still gets to see some traffic from other activities if within the scope.
SessionID = default



=== Time Synchronicity ===

When a sample is received from another machine whose clock is not synchronized with the receiver machine, the sample's time
stamp may require a significant correction to be accurate in the local time on the receiver. Such an estimated correction
factor can be queried from the inlet that returned the sample (by calling its .time_correction() method). When recording data to a

file it is recommended to simply store both the time stamps of the received samples as-is as well as the a periodic measure of the
correction offset (e.g. once per 10s) without pre-adding them. This way, corrections can be made based on averaged offsets rather than
comparatively noisy short-time values. Empirically, the estimation accuracy is within +/- 0.2ms (median absolute error ~0.1ms).
The latency on a single machine has been found to be 0.2-0.4ms (depending on machine configuration). You can also supply your own
time stamps (e.g., from synchronized GPS clocks) rather than relying on lsl_local_clock() when pushing samples or chunks and
ignore the off-the-shelf correction offered by the LSL.


=== Meta-Data Recommendations ===

A stream can be associated with fixed meta-data (which must be known before an outlet can be created). It is highly recommended to specify all
relevant setup information that might be useful for later analysis of the data, such as amplifier settings, units, conversion factors, or information
about the subject if collected via an entry mask (age, gender, handedness, etc.). To do this in a manner that allows for later programmatic analysis,
please adhere to the existing guidelines. While the description can be arbitrarily large, it is recommended to keep the size from exceeding 5MB,
if possible, to reduce the chance of system slowdowns; instead consider referring to separate files to supply the necessary descriptions. Adding meta-data
is very easy -- please see the SendData or SendDataC example programs for how to do this.

See also http://code.google.com/p/xdf/wiki/MetaData for an effort to grow a body of recommendations for interoperable meta-data.

