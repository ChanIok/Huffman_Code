#include "iostream"
#include "fstream"
#include "algorithm"
#include "string"
#include "iomanip"

using namespace std;
#define MaxSize 512

struct HuffNode//哈夫曼树的结点结构
{
	unsigned char  byte;      //结点数据
	long long weight;	  //权值
	HuffNode* parent; //双亲结点指针
	HuffNode* leftChild; //左孩子指针
	HuffNode* rightChild; //右孩子指针
	string code;  //哈夫曼编码
	int code_len;    //编码长度 
};

int all_bytes_count = 0, bytes_count = 0, nodes_count = 0;//总字节数，字节类数，结点总数
HuffNode HuffTree[MaxSize], *Treeroot;//哈夫曼树树，根结点
string Compress_filename, Compress_suffix;//压缩文件名，后缀
string Decompress_filename, Decompress_suffix;//解压文件名，后缀

void InitHuffTree();//哈夫曼树初始化
void Statistics();//各字节权值统计
void CreatHuffTree();//构造哈夫曼树
void CreatHuffCode(HuffNode *pre);//构造哈夫曼编码，递归
void Compress();//生成压缩文件
void Decompress();//解压压缩文件
void Callwindow();//对话窗口

void InitHuffTree()//哈夫曼树初始化
{
	for (int i = 0; i < MaxSize; i++)
	{
		HuffTree[i].byte = NULL;
		HuffTree[i].weight = 0;
		HuffTree[i].parent = NULL;
		HuffTree[i].rightChild = NULL;
		HuffTree[i].leftChild = NULL;
		HuffTree[i].code_len = 0;
	}
}
void Statistics() //各字节权值统计
{
	int code_weight[MaxSize] = { 0 };//定义权值数组
	char c;
	string Filename;
	Filename = Compress_filename;
	ifstream inFile(Filename, ios::in | ios::binary); //以只读打开二进制文件；
	if (!inFile)
	{
		cout << "read error" << endl;
		return;
	}
	while (inFile.read((char *)&c, 1)) //一直读到文件结束
	{
		code_weight[(unsigned char)c]++;//字节对应下标的权值++
		all_bytes_count++;
	}
	for (int i = 0; i < MaxSize; i++)//将所有出现在权值数组的值和对应下标赋值到哈夫曼数组
	{
		if (code_weight[i] > 0)
		{
			HuffTree[bytes_count].weight = code_weight[i];
			HuffTree[bytes_count].byte = i;
			bytes_count++;//读入的字节数++
		}
	}

	inFile.close();//关闭文件
	cout << "字节类型总数：" << bytes_count << endl;
	nodes_count = bytes_count;//读入的字节数赋值到结点总数
	cout << "累计输入字节：" << all_bytes_count << endl;

}
void CreatHuffTree()//构造哈夫曼树
{
	int min1, min2;//寻找最小值min1和次最小值min2
	for (int i = 0; i < bytes_count - 1; i++)//合并bytes_count-1次
	{
		min1 = nodes_count - 1;
		for (int j = nodes_count - 1; j >= 0; j--)
		{
			if (HuffTree[j].parent == NULL && HuffTree[j].weight <= HuffTree[min1].weight)
				min1 = j;
		}
		min2 = -1;
		for (int j = nodes_count - 1; j >= 0; j--)
		{
			if (min2 == -1)
			{
				if (HuffTree[j].parent == NULL && j != min1)  //初始化min2，避开min1 
					min2 = j;
			}
			else
			{
				if (j != min1 && HuffTree[j].parent == NULL && HuffTree[j].weight <= HuffTree[min2].weight)
					min2 = j;
			}
		}
		HuffTree[min1].parent = &HuffTree[nodes_count];//设置指针
		HuffTree[min2].parent = &HuffTree[nodes_count];
		HuffTree[nodes_count].leftChild = &HuffTree[min1];
		HuffTree[nodes_count].rightChild = &HuffTree[min2];
		HuffTree[nodes_count].weight = HuffTree[min1].weight + HuffTree[min2].weight;//权值累加
		nodes_count++;//结点数++
	}
	Treeroot = &HuffTree[nodes_count - 1];//构造树完成，得到根节点
}
void CreatHuffCode(HuffNode *pre)//构造哈夫曼编码
{
	if (pre->leftChild != NULL)//左走
	{
		string left = pre->code;
		left += '0';
		pre->leftChild->code += left;
		CreatHuffCode(pre->leftChild);//往下一层递归
	}
	if (pre->rightChild != NULL)//左走
	{
		string right = pre->code;
		right += '1';
		pre->rightChild->code += right;
		CreatHuffCode(pre->rightChild);//往下一层递归
	}
	return;
}
void Compress()//压缩文件
{
	InitHuffTree();//初始化哈夫曼树
	Statistics();//权值统计
	CreatHuffTree();//构建哈夫曼树
	CreatHuffCode(Treeroot);//构造哈夫曼编码
	string Block;//输出缓冲区块
	string Buffer;//输出缓冲
	string Codetable[256];//哈夫曼编码表
	int if_Finished = 0;
	string Filename, Suffix;
	int mark_Suffix;
	char c;//暂存字符
	cout << "节点数：" << nodes_count << endl;
	for (int i = Compress_filename.length() - 1; i > 0; i--)//寻找后缀名的位置
	{
		if (Compress_filename[i] == '.')
		{
			mark_Suffix = i;
			break;
		}
	}
	Suffix = Compress_filename.substr(mark_Suffix + 1);//设置文件后缀名
	Filename = Compress_filename;
	Filename.erase(mark_Suffix, Compress_filename.length());//设置文件主名
	ofstream outFile(Filename, ios::out | ios::binary); //以写入打开二进制文件；
	outFile << Suffix << endl << all_bytes_count << endl << bytes_count << endl;
	for (int i = 0; i < nodes_count; i++)
	{
		if (HuffTree[i].leftChild == NULL)
		{
			Codetable[(unsigned char)HuffTree[i].byte] = HuffTree[i].code;//导出code到对应码表
			outFile << HuffTree[i].byte << endl << HuffTree[i].weight << endl;
		}

	}
	ifstream inFile(Compress_filename, ios::in | ios::binary); //以只读打开二进制文件；
	while (Buffer.length() >= 8 || !if_Finished) //一直读到缓存小于8
	{
		if (!inFile.read((char *)&c, 1))//读入一个字节，判断是否读完
		{
			if_Finished = 1;
		}
		if (!if_Finished)//如果未读完，将字节对应哈夫曼编码存入缓存中
		{
			Buffer += Codetable[(unsigned char)c];
		}
		if (Buffer.length() >= 8)//当缓冲大于等于8位时，转化为一个字节存入输出缓冲区块
		{
			unsigned char ch = 0;
			for (int i = 0; i < 8; i++)
			{
				if (Buffer[i] == '1')
					ch = ch + pow(2, 7 - i);
			}
			Block += ch;
			Buffer.erase(0, 8);//清除缓冲前8位
		}
		if (Block.length() >= 32)//当输出缓冲区块大于32个字节时，全部输出到文件
		{
			outFile << Block;
			Block = "";
		}
	}
	outFile << Block;//输出区块中剩余字节
	if (Buffer.length() < 8)//若缓冲不够8位，补足0
	{
		int limit = 8 - Buffer.length();
		for (int i = 0; i < limit; i++)
		{
			Buffer += '0';
		}
		unsigned char ch = 0;
		for (int i = 0; i < 8; i++)
		{
			if (Buffer[i] == '1')
				ch = ch + pow(2, 7 - i);
		}
		outFile << ch;
		Buffer = "";
	}
	outFile.close();
	inFile.close();//关闭文件
	ifstream inFilesize(Filename, ios::out | ios::binary); //计算压缩后文件大小和压缩比
	inFilesize.seekg(0, ios_base::end);
	int compress_Size = inFilesize.tellg();
	cout << "压缩后文件大小:" << compress_Size << endl;
	inFilesize.close();
	double compress_Ratio = (double)compress_Size / all_bytes_count;
	cout << "压缩比：";
	printf("%.2lf%%\n", compress_Ratio * 100);
	cout << "压缩完毕";
}
void Decompress()
{
	InitHuffTree();//初始化哈夫曼树
	unsigned int i = 0;
	unsigned char c;
	char t;
	int mark = 0;
	int btye_write_count = 0;
	int Go_finished;
	int current_code, current_weight, byte_read_count = 0, if_byte = 1;
	int    Byte_count = 0, Byte_type_count = -1;//headmessage
	string Codestream, Filename, Suffix;
	int buffer[8];//缓冲
	HuffNode *Node_Point;

	Filename = Decompress_filename;
	Suffix = Decompress_suffix;
	ifstream inFilesize(Filename, ios::out | ios::binary); //计算源文件大小
	inFilesize.seekg(0, ios_base::end);
	int compress_Size = inFilesize.tellg();
	inFilesize.close();//计算完毕
	cout << "源文件大小:" << compress_Size << endl;
	ifstream inFile(Filename, ios::in | ios::binary); //以只读打开二进制文件；
	inFile  >> Suffix >> Byte_count >> Byte_type_count;//读取头部信息
	cout << "字节类型数:" << Byte_type_count << endl;
	inFile.read((char*)&t, 1);//读掉换行符
	while (inFile.read((char *)&c, 1)) //读取哈夫曼编码表
	{
		if (if_byte == 1)//如果为对应字节
		{
			current_code = c;
			if_byte = 0;
		}
		else if (if_byte == 0)//字节对应权值
		{
			inFile >> current_weight;
			HuffTree[byte_read_count].weight = current_weight;//把对应字节和权值赋值到哈夫曼树
			HuffTree[byte_read_count].byte = current_code;
			if_byte = 1;
			byte_read_count++; //字节类型统计++
			inFile.read((char*)&t, 1);//读掉换行符
		}
		if (byte_read_count == Byte_type_count)//如果读完就跳出
		{
			break;
		}
	}
	bytes_count = nodes_count = Byte_type_count;//设置字节数，结点总数
	CreatHuffTree();//构造哈夫曼树
	CreatHuffCode(Treeroot);//构造哈夫曼编码
	Node_Point = Treeroot;//当前状态指向根节点
	Filename += ".";
	Filename += Suffix;//还原文件名
	ofstream outFile(Filename, ios::out | ios::binary); //以写入打开二进制文件；
	cout << "输出文件名:" << Filename << endl;
	while (btye_write_count != Byte_count) //一直读到所有字节输出完毕
	{
		inFile.read((char *)&c, 1);
		for (int j = 7; j >= 0; j--)
		{
			buffer[j] = c % 2;
			c = c / 2;
		}
		for (i = 0; i < 8; i++)
		{
			if (btye_write_count == Byte_count)//所有字节输出完成，跳出
			{
				break;
			}
			Go_finished = 0;
			while (Go_finished == 0)
			{
				if (buffer[i] == 0)//左走
				{
					if (Node_Point->leftChild != NULL)//有左孩子
					{
						Node_Point = Node_Point->leftChild;
						Go_finished = 1;//标记为已走
					}
					else//当前为叶子
					{
						outFile << Node_Point->byte;
						Node_Point = Treeroot;//返回根
						btye_write_count++;
					}
				}
				else//右走
				{
					if (Node_Point->rightChild != NULL)//有右孩子
					{
						Node_Point = Node_Point->rightChild;
						Go_finished = 1;//标记为已走
					}
					else//当前为叶子
					{
						outFile << Node_Point->byte;
						Node_Point = Treeroot;//返回根
						btye_write_count++;
					}
				}
			}
		}
	}
	cout << "解压完毕" << endl;
	inFile.close();
	outFile.close();
}
void Callwindow()
{
	char c;
	cout << "1.压缩" << endl;
	cout << "2.解压" << endl;
	c = getchar();
	if (c == '1')
	{
		system("cls");
		cout << "请输入需要压缩的文件名：" << endl;
		cin >> Compress_filename;
		Compress();
		return;
	}
	else if (c == '2')
	{
		system("cls");
		cout << "请输入需要解压的文件名：" << endl;
		cin >> Decompress_filename;
		Decompress();
		return;
	}
	else
	{
		system("cls");
		cout << "输入有误" << endl;
		return;
	}
}
int main()
{
	Callwindow();
	getchar();
	getchar();
}