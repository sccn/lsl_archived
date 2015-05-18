pylsl
=====

This is the Python interface to the Lab Streaming Layer (LSL). LSL is an
overlay network for real-time exchange of time series between applications, most
often used in research environments. LSL has clients for many other languages
and platforms that are compatible with each other.

Pylsl should work with any recent version of the ``liblsl`` library (which
is also included with this package), on any operating system and with any recent
Python version, including 2.7+ and 3.x.

Let us know if you encounter any bugs (ideally using the issue tracker on
the GitHub project).

Known Issues
------------
* On Linux one currently cannot call ``pylsl`` functions from a thread that is
  not the main thread.

Acknowledgments
---------------
Pylsl was primarily written by Christian Kothe while at Swartz Center for
Computational Neuroscience, UCSD. The LSL project was funded by the Army
Research Laboratory under Cooperative Agreement Number W911NF-10-2-0022 as
well as through NINDS grant 3R01NS047293-06S1. Thanks for contributions,
bug reports, and suggestions go to Bastian Venthur, Chadwick Boulay,
David Medine, Clemens Brunner, and Matthew Grivich.
