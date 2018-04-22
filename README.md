What is Keydb?
--------------

I want to use a key-value database like redis,use easly and run fast,but capacity limited by memory.

I want to use a big capacity databse like mysql,but it not designed to key-value storage.

Keydb is a fast key-value-store disk based storage service.

Keydb core performance? 
--------------

    value-length | value-count | gen-time | memory | disk | random-read | random-write 
    2K           | 209715200   | 32Min    | 27G    | 400G |	4975QPS     | 4361QPS
    20K          | 20971520    | 9Min     | 4G     | 400G |	3211QPS     | 3336QPS
    200K         | 2097152     | 7Min     | 0.4G   | 400G |	1123QPS     | 1333QPS

Building Keydb? 
--------------

    # cd src
    # make
