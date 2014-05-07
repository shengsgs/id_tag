一.基本情况说明：
（1）编译器及其版本：
Apple LLVM version 5.1 (clang-503.0.40) (based on LLVM 3.4svn)
Target: x86_64-apple-darwin13.1.0
Thread model: posix
（2）文件编码格式：统一为Unicode(UTF_8)
（3）直接运行a.out完成测试（或编译test.cc，输入文件：input.txt）
（4）函数及变量说明在doc中

二.思路：
（1）读入数据，用标签建立关于用户id的倒排索引
tag1:	uid_a1,	uid_b1,	uid_c1……
tag2:	uid_a2,	uid_b2,	uid_c2……
……
（2）然后对每两个标签对后的用户id集合求交集，
（3）对交集集合去除长度小于1的，然后对结果集合取出两两组合，得到一些uid_pair
（4）以uid_pair（uid_a1,uid_a2）为key建立hash表，然后将标签加入uid_pair对应的value结果集中，最后得到的hash表即所需求的结果

三.实现细节：
1.类型使用说明：
（1）在微博api中定义的uid时int64类型的，所以在代码中使用的关于uid的类型都是int64（__int64_t类型）；
（2）定义的标签tags类型是string，所以也使用string来存标签信息（微博中的限制为每个用户最多10个标签，每个标签长度不超过7个汉字）；	

2.变量定义说明：
（1）indexform_:用来存倒排表的中间结果，是以STL的hash_map为核心的数据结构，以标签（string：tag）为索引,映射的用户id（set：uid1， uid2，……）
（2）outputform_:是一个用来存最终的输出结果的hash表，key是相关的两个uid的pair，value是自定义的struct类型，存有两个uid，标签列表和一个访问标记
（3）为了程序运行速度测试时使用的输出hash表大小（ MAX_COMBINATION）是17，要进行大数据测试要改为一个大的质数

四.测试结果：
input:
140	体育 新闻	清华 百年校庆
251	娱乐 八卦 清华 新闻	
362	体育 娱乐 新闻
473	八卦 新闻 娱乐
output:

八卦	251	473	
娱乐	251	362	473	
百年校庆	140	
清华	140	251	
新闻	140	251	362	473	
体育	140	362	

Now is the result for GetIntersection:

251,140:	新闻	清华	
362,251:	娱乐	新闻	
473,362:	娱乐	新闻	
362,140:	体育	新闻	
473,251:	八卦	娱乐	新闻	
Input anything and tap enter for ending
quit

五.优化思路：
（1）现在通用的比较好的方法应该是Lucene建立索引。使用java的话可以直接引入lucene的jar包，建立倒排索引，lucene对文件建立倒排索引非常方便且速度比较快；
(2)也能用Sphinx建立索引，Sphinx对中文支持很好，同时实现了分布式的索引；可以使用Sphinx建立索引，利用Sphinx支持的分布式索引结合MapReduce可以在多服务器和CPU下有更好的表现。
（3）为了检索效率,在内存足够的情况下，可对标签建立词典，这样避免直接操作string速度可能更理想
