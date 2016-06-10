W7500 test-pin not driven
=========================

To work properly, the W7500 test-pin must be pulled to ground (see reference
manual [1] Table 5 "operation of mode selection") It seem that when pin is
not connected everything is ok; but this issue should be fixed in next version.

[1] http://wizwiki.net/wiki/lib/exe/fetch.php?media=products:w7500:documents:w7500_rm_v102.pdf
