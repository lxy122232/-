//此头文件包含所需使用的结构体和全局变量

#pragma once
#include<iostream>
#include <cstdlib>
#include <string>
#include <ctime>
#define  BLOCKSIZE  1024      //磁盘块大小
#define  SIZE         1024000   //虚拟磁盘空间大小
#define  END         65535     //文件结束标志
#define  FREE        0         //盘块空闲标志
#define  ROOTBLOCKNUM  2   //根目录区所占盘块总数
#define  MAXOPENFILE    10   //最多同时打开文件个数

//（1）文件控制块FCB
//用于记录文件的描述和控制信息，每个文件设置一个FCB，它也是文件的目录项的内容。
typedef struct FCB //仿照FAT16设置的 
{
	char filename[8]; //文件名
	char exname[3];//文件扩展名
	unsigned char attribute;//文件属性字段：为简单起见，我们只为文件设置了两种属性：
	//值为0时表示目录文件，值为1时表示数据文件
	unsigned short time;//文件创建时间
	unsigned short data;//文件创建日期
	unsigned short first;//文件起始盘块号
	unsigned long length;//文件长度（字节数）
	char free;//表示目录项是否为空，若值为0，表示空，值为1，表示已分配
}fcb;

//（2）文件分配表FAT
//在本实例中，文件分配表有两个作用：一是记录磁盘上每个文件所占据的磁盘块的块号；二是记录磁盘上哪些块已经分配出去了，哪些块是空闲的，即起到了位示图的作用。若FAT中某个表项的值为FREE，则表示该表项所对应的磁盘块是空闲的；若某个表项的值为END，则表示所对应的磁盘块是某文件的最后一个磁盘块；若某个表项的值是其他值，则该值表示某文件的下一个磁盘块的块号。为了提高系统的可靠性，本实例中设置了两张FAT表，它们互为备份，每个FAT占据两个磁盘块。
typedef struct FAT
{
	unsigned short id;
}fat;

//（3）用户打开文件表USEROPEN
//当打开一个文件时，必须将文件的目录项中的所有内容全部复制到内存中，同时还要记录有关文件操作的动态信息，如读写指针的值等。在本实例中实现的是一个用于单用户单任务系统的文件系统，为简单起见，我们把用户文件描述符表和内存FCB表合在一起，称为用户打开文件表，表项数目为10，即一个用户最多可同时打开10个文件。然后用一个数组来描述，则数组下标即某个打开文件的描述符。另外，我们在用户打开文件表中还设置了一个字段“char dir[80]”，用来记录每个打开文件所在的目录名，以方便用户打开不同目录下具有相同文件名的不同文件。
typedef struct USEROPEN
{
	char filename[8]; //文件名
	char exname[3];//文件扩展名
	unsigned char attribute;//文件属性：值为0时表示目录文件，值为1时表示数据文件
	unsigned short time;//文件创建时间
	unsigned short data;//文件创建日期
	unsigned short first;//文件起始盘块号
	unsigned long length;//文件长度（对数据文件是字节数，对目录文件可以是目录项个数）
	char free;//表示目录项是否为空，若值为0，表示空，值为1，表示已分配
		//前面内容是文件的FCB中的内容。
		// 下面设置的dirno和diroff记录了相应打开文件的目录项在父目录文件中的位置，//这样如果该文件的fcb被修改了，则要写回父目录文件时比较方便
	int dirno;  //相应打开文件的目录项在父目录文件中的盘块号 
	int diroff;// 相应打开文件的目录项在父目录文件的dirno盘块中的目录项序号
	char dir[MAXOPENFILE][80]; //相应打开文件所在的目录名，这样方便快速检查出
	//指定文件是否已经打开
	int count; //读写指针在文件中的位置
	char fcbstate; //是否修改了文件的FCB的内容，如果修改了置为1，否则为0
	char topenfile; //表示该用户打开表项是否为空，若值为0，表示为空，否则表示已
	//被某打开文件占据
}useropen;

//（4）引导块BLOCK0
//在引导块中主要存放逻辑磁盘的相关描述信息，比如磁盘块大小、磁盘块数量、文件分配表、根目录区、数据区在磁盘上的起始位置等。如果是引导盘，还要存放操作系统的引导信息。本实例是在内存的虚拟磁盘中创建一个文件系统，因此所包含的内容比较少，只有磁盘块大小、磁盘块数量、数据区开始位置、根目录文件开始位置等。
typedef struct BLOCK0  //引导块内容
{
	//存储一些描述信息，如磁盘块大小、磁盘块数量、最多打开文件数等、
	char information[200];
	unsigned short root; //根目录文件的起始盘块号
	unsigned char* startblock; //虚拟磁盘上数据区开始位置
}block0;

//全局变量定义
unsigned char* myvhar; //指向虚拟磁盘的起始地址
useropen openfilelist[MAXOPENFILE]; //用户打开文件表数组
useropen* ptrcurdir;//指向用户打开文件表中的当前目录所在打开文件表项的位置；
char currentdir[80];     //记录当前目录的目录名（包括目录的路径）
unsigned char* startp;   //记录虚拟磁盘上数据区开始位置

/*在头文件.h中定义变量，但是不初始化
int x;
相当于定义了一个全局变量x，同时在所有引用该头文件的.cpp文件中声明。作用域为整个项目（所有源文件);
在引用该头文件的源文件中可以直接使用变量x，在没有引用该头文件的源文件中，则需要 extern int x；*/