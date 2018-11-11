libpredict  [![Build Status](https://travis-ci.org/philcrump/libpredict.svg?branch=master)](https://travis-ci.org/philcrump/libpredict)
==========

This is a fork of la1k's satellite orbit prediction library ( https://github.com/la1k/libpredict/ ).


# Notable Differences

* Library header dependencies are reworked to allow easy compilation of the library inside a parent project tree.

* `predict_parse_tle()` takes a pointer to a pre-allocated `predict_orbital_elements_t` struct, and optional pointers to pre-allocated `predict_sgp4` & `predict_sdp4` structs. If the required struct is not allocated then the function will fail.

* `predict_create_observer()` takes a pointer to a pre-allocated `predict_observer_t` struct.

* TLE checksums are verified in `predict_parse_tle()` - I plan to propose this on an upstream PR at some point.