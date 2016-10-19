version_2016_10_18(修改了WT模块写法)
	求第二项干扰项好像有点错误，明天讨论一下

version_2016_10_18(模块整合，尚未完成)
	目前还差干扰矩阵，需要资源管理单元给地理拓扑单元干扰用户的信息

version_2016_10_18(修改了WT模块中cloest二分法的实现)
	

version_2016_10_18(修改了Matrix的求逆运算)
	当矩阵不满秩时，转为求伪逆


version_2016_10_18(为Matrix类库添加伪逆计算模块)


version_2016_10_18(载干比计算又改了一部分)


version_2016_10_18(载干比计算改了一部分)

version_2016_10_11(RRM_DRA模块DRATransimit函数修改)
	分解成两个函数DRATransimitStart 和 DRATransimitEnd
	目的是为了插入一个调度信息的日志

version_2016_10_11(更新干扰信道响应部分)

version_2016_10_11(更新调度信息日志部分)

version_2016_10_11(将冲突处理拆分成两部分)
	冲突处理+模拟传输(统计吞吐量)


version_2016_10_10(修改了场景部分)


version_2016_10_10(整合了SINR计算模块)


version_2016_10_10(增加了动态链接库)


version_2016_9_14(Try)


version_2016_9_14(Debug)
	纠正了无线资源管理吞吐量的统计Bug
	修改了Matlab作图部分

version_2016_9_14(Debug)
	纠正了DRA模块下，计算occupiedTTI的错误
	RR模块还未纠正
	修改了Matlab作图部分


version_2016_9_13(Debug)
	纠正了RSUId=63没有车的Bug


version_2016_9_13(Debug)
	srand导致了车辆集中在某几个RSU中
	因此不要在初始化车辆位置的函数中使用srand((unsigned)time(NULL))


version_2016_9_12(修正了TTI含义)


version_2016_9_12(将地理拓扑单元模块化)


version_2016_9_11(修改了Matlab作图部分)


version_2016_9_10(修改了Matlab作图部分)


version_2016_9_8(增强Matrix等类库)


version_2016_9_8(集成了Matrix等相关类库)


version_2016_9_7(增加了WT模块的框架)


version_2016_9_6(增加了几个注释)

version_2016_9_6(跟上一版没差，改点参数)

version_2016_9_6(增加了吞吐量的统计)
	局限：在累计吞吐量的时候，不是[TTI，Pattern]所传输的实际bit数量，而是[TTI，Pattern]所允许传输的最大bit数量
		因此结果会有一点偏差


version_2016_9_5(修改所有非地理拓扑单元的rand)


version_2016_9_5(修改TMAC中随机数)
	将rand()通过运算产生一个均匀分布改成由标准库的随机数引擎与分布产生


version_2016_9_3(新增Data数据业务)
	新增DATA数据业务
	将BMAD改为TMAC
	修改EventLog:日志添加一项简单说明的字段


version_2016_9_2(纠正RR模块)


version_2016_9_2(BMAD模块化初步完成)
	RR模块仍未纠正

version_2016_9_2(RR模块化初步完成)
	仍有错误，当前只是排队调度，并没有轮询
	删除了FileOutput??.cpp文件，以及toString.cpp文件，Schedule.h,Schedule.cpp
	删除了未完成的PF调度模块

version_2016_9_1(减少了DRA模块不必要的链表之间的转换)
	例如从事件表到接纳表，以及切换表到接纳表

version_2016_9_1(日志部分新增枚举类型代替12345...)

version_2016_9_1(DRA模块化BUG纠正)

version_2016_9_1(DRA模块化初步完成)

version_2016_9_1(调度部分初稿完成)

version_2016_9_1(编写轮询调度部分)

version_2016_8_31(修改了日志部分的错误)

version_2016_8_31(增加了轮询调度部分，尚未完成)

version_2016_8_6(取消了对txt的追踪)


version_2016_7_25(增加了冲突的统计)
	*sEvent中增加数据成员conflictNum

version_2016_7_25(修改了泊松分布的生成)
	*修改了紧急事件泊松分布的生成
	*增加了用于泊松分布验证的数据成员m_VeUEEmergencyNum


version_2016_7_25(更换了远程仓库)
	*远程仓库信息具体见群公告

version_2016_7_24(微调)
	*将Event以及Message写入了Traffic.h头文件
	*在运行过程中（运行无线资源管理模块时），内存占用量缓慢上升，这是由于记录Event的日志所引起的，属正常

version_2016_7_21(修复BUG，进行调整)
	*在channelGeneration调用前后，对于RSU内的VeUEIdList进行维护，在位置更新进行前进行清空
	*RSU.VeUEIdList是在freshLoc调用过程中写入的
	*根据RSU.VeUEIdList，将其写入eNB.VeUEIdList
	*目前eNB.RSUIdList尚未正确初始化，因此eNB.VeUEIdList也是错误的，需要场景模块在撒点时，初始化eNB.RSUIdList
	*删除了Message类中占用TTI时隙的数据成员，将byteNum改为bitNum，并在Global中设置每种消息的bit数量
	*每个消息占用的TTI时间，需要根据消息的bit数量，以及一个RB在单位TTI内传输的bit数量，以及该消息对应的Pattern所占用的FB数量进行计算


version_2016_7_21(解决内存泄露)
	*解决地理拓扑单元大部分内存泄露问题
	*地理单元增加相应的注释


version_2016_7_21(合并后的调整)
	*排除了所有warning
	*将Definition删除，数据类型移入Enumeration.h
	*将Road.h中的eNB类移入eNB.h
	*将Const.h以及Function.h移入Global.h
	*将System类中的命名进行更正，其他类型尚未修改


version_2016_7_21(与场景模块合并)
	*剩余问题：地理位置更新，分簇
	*内存泄漏


version_2016_7_20(规范了几个参数设置)
	*总带宽20M
	*一个RB带宽180k
	*共有111个RB，因此分为6*1+60*1+5*9：6个紧急pattern，每个占用1个RB，60个周期信号的pattern，每个占用1个RB，9个数据pattern，每个占用5个RB


version_2016_7_20(微调)
	*改动了日志输出的错误
	*在VeUE类中新增了成员，表示调度的相对区间范围


version_2016_7_20(更改了调度信息的输出)
	*其中紧急事件的Pattern单独编号(从0开始)
	*其他事件的Pattern一起编号(从0开始)


version_2016_7_20(增加了紧急事件的处理)
	*删除RSU的静态变量，将其转为全局变量写入global.h
	*Pattern的编号还需改动，每个种类的Pattern单独编号

version_2016_7_18(恢复了git对某些文件的追踪)
	*不然会导致克隆无法运行

version_2016_7_18(删除了git对Debug的追踪)


version_2016_7_18(增加了时延统计数据成员的定义)
	*规定了未实现部分用"<<"some explanation">>"在注释中进行说明，便于后期查找
	*增加了EventLog中的时延统计部分，包括发送时延（目前的含义是占用资源块的TTI数量）以及等待时延
	*增加了EventLog中正在传输时刻的信息输出



version_2016_7_17(删除了对部分文件的追踪)
	*.db
	*.suo


version_2016_7_17(删除了Log类，调整了日志记录)
	*将EventLog以及TTILog以函数的形式进行记录


version_2016_7_14(删除了Process的TXT输出)


version_2016_7_14(增加了基于Pattern的资源强占模式)
	*将基于FB的资源抢占改为基于Pattern的资源抢占
	*若要用以前的基于FB的强占模式，只需要将PatternTypeNum改为1即可
	*改变了ScheduleTable向WaitEventIdList以及SwitchEventIdList转换的方法
		->之前，已经出RSU的车辆没有及时出去（只有到该簇进行传输时，才会对其所属RSU进行判断)
		->改为，进行分簇后就得调用这个函数，并且遍历所有的簇以及Pattern，来查找切换RSU的车辆）
	*解决了头文件互相包含引发的大量未声明的错误


version_2016_7_12(删除了ATTI，增加了事件的日志史)
	*将ATTI改为TTI，即从0开始的TTI时刻（之前是起始时刻随机）
	*增加了事件日志历史，用list维护，可以在程序运行完毕后将日志写入对应的txt


version_2016_7_12(修复了BUG)
	*修复了事件链表序号不按时间顺序出现的错误（由于错把DRA_NTTI当成了了周期事件的周期了）


version_2016_7_12(修改了RB_IsAvailable的属性)
	*内层由int变为bool，int指示的是资源释放的绝对TTI时刻，现在改为标识是否可用


version_2016_7_12(完备了链表间的传递)
	*支持信息传输过程中发生了簇切换，RSU切换的链表传递的行为
	*输出了所有事件切换\成功传输的Log日志


version_2016_7_12(修改了格式化输出)
	*修复了，可选资源块为0的时候的错误
	*尚未处理的问题：出现过仍有资源没有分配时，车辆数量已经为负数


version_2016_7_12(更改了传递链表存储的含义)
	*原先存的是车辆的ID
	*现在改为存事件的ID
	*将cVeUE类中的sMessage成员除去，消息类型不应该作为车辆的成员，应该属于事件的成员
	*尚未处理的问题：可选资源块为0的时候会抛出异常
	*将要处理的问题：修改各类toString()方法，接收一个参数，控制其缩进


version_2016_7_11(更改了呼叫链表的更新方式)
	*进一步更新格式化输出
	*尚未处理的问题：可选资源块为0的时候会抛出异常



version_2016_7_10(格式化输出)
	*System级别的输出放在System.cpp中
	*定义了几个类型的toString方法，用到了ostringstream (在头文件sstream中)


version_2016_7_10(修改了调度信息输出格式)


version_2016_7_11(更新了数据结构的类型，更改了部分机制)
	*更新的表：如冲突表，RSU内的车辆表，以及RSU内簇内的车辆表
	*将所有的运行时信息都记录到文件中
	*更改了冲突处理机制

version_2016_7_10(更新了车辆的随机分布，以及生成了呼叫事件链表)
	*目前呼叫事件链表只包含周期性的事件
	
	
version_2016_7_10(排除了Warning)
	*到目前为止，不涉及车辆重新分簇（不涉及车辆为止的更新导致的RSU覆盖范围内的车辆更新）
	*信息正常传输完毕后，会释放其资源，（调度链表的清空）

	
version_2016_7_10(完善了冲突处理)
	*到目前为止，不涉及车辆重新分簇（不涉及车辆为止的更新导致的RSU覆盖范围内的车辆更新）
	*信息正常传输完毕后，会释放其资源，（调度链表的清空）
	
	
version_2016_7_10(添加了冲突处理)
	*到目前为止，不涉及车辆重新分簇（不涉及车辆为止的更新导致的RSU覆盖范围内的车辆更新）
	*信息正常传输完毕相应数据的更新也尚未完成
	
	
version_2016_7_9(增加了冲突退避机制)
	*调度信息的更新仍需要完善
	*当数据传输完毕，或者监听到冲突后，应该将其从m_DRAScheduleInfo表中删除，并相应地m_DRARB_IsAvailable
	
	
	
WRONG



ONDONE



ATTENTION