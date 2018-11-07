libpredict
==========

This is a fork of la1k's satellite orbit prediction library ( https://github.com/la1k/libpredict/ ).


# Notable Differences

* Library header dependencies are reworked to allow easy compilation of the library inside a parent project tree.

* `predict_parse_tle()` takes a pointer to a pre-allocated `predict_orbital_elements_t` struct.

* `predict_create_observer()` takes a pointer to a pre-allocated `predict_observer_t` struct.

* TLE checksums are verified in `predict_parse_tle()` - I plan to propose this on an upstream PR at some point.


# TODO

* Remove dynamic allocation of SGP/SDP elements in orbit.c L118/L138. However these element sets are large and ideally should not both be pre-allocated.