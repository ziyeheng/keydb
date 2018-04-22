What is Keydb?
--------------

I want to use a key-value database like redis,use easly and run fast,but capacity limited by memory.

I want to use a big capacity databse like mysql,but it not designed to key-value storage.

Keydb is a fast key-value-store disk based storage service.



Keydb core performance? 
--------------

	数量	生成耗时	内存使用	硬盘使用	rand read	rand write
2k	209715200	32M	27G	400G	4975	4361
20k	20971520	9M	4G	400G	3211	3336
200k	2097152	7M	0.4G	400G	1123	1333

