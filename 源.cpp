#include "iostream"
#include "fstream"
#include "algorithm"
#include "string"
#include "iomanip"

using namespace std;
#define MaxSize 512

struct HuffNode//���������Ľ��ṹ
{
	unsigned char  byte;      //�������
	long long weight;	  //Ȩֵ
	HuffNode* parent; //˫�׽��ָ��
	HuffNode* leftChild; //����ָ��
	HuffNode* rightChild; //�Һ���ָ��
	string code;  //����������
	int code_len;    //���볤�� 
};

int all_bytes_count = 0, bytes_count = 0, nodes_count = 0;//���ֽ������ֽ��������������
HuffNode HuffTree[MaxSize], *Treeroot;//�����������������
string Compress_filename, Compress_suffix;//ѹ���ļ�������׺
string Decompress_filename, Decompress_suffix;//��ѹ�ļ�������׺

void InitHuffTree();//����������ʼ��
void Statistics();//���ֽ�Ȩֵͳ��
void CreatHuffTree();//�����������
void CreatHuffCode(HuffNode *pre);//������������룬�ݹ�
void Compress();//����ѹ���ļ�
void Decompress();//��ѹѹ���ļ�
void Callwindow();//�Ի�����

void InitHuffTree()//����������ʼ��
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
void Statistics() //���ֽ�Ȩֵͳ��
{
	int code_weight[MaxSize] = { 0 };//����Ȩֵ����
	char c;
	string Filename;
	Filename = Compress_filename;
	ifstream inFile(Filename, ios::in | ios::binary); //��ֻ���򿪶������ļ���
	if (!inFile)
	{
		cout << "read error" << endl;
		return;
	}
	while (inFile.read((char *)&c, 1)) //һֱ�����ļ�����
	{
		code_weight[(unsigned char)c]++;//�ֽڶ�Ӧ�±��Ȩֵ++
		all_bytes_count++;
	}
	for (int i = 0; i < MaxSize; i++)//�����г�����Ȩֵ�����ֵ�Ͷ�Ӧ�±긳ֵ������������
	{
		if (code_weight[i] > 0)
		{
			HuffTree[bytes_count].weight = code_weight[i];
			HuffTree[bytes_count].byte = i;
			bytes_count++;//������ֽ���++
		}
	}

	inFile.close();//�ر��ļ�
	cout << "�ֽ�����������" << bytes_count << endl;
	nodes_count = bytes_count;//������ֽ�����ֵ���������
	cout << "�ۼ������ֽڣ�" << all_bytes_count << endl;

}
void CreatHuffTree()//�����������
{
	int min1, min2;//Ѱ����Сֵmin1�ʹ���Сֵmin2
	for (int i = 0; i < bytes_count - 1; i++)//�ϲ�bytes_count-1��
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
				if (HuffTree[j].parent == NULL && j != min1)  //��ʼ��min2���ܿ�min1 
					min2 = j;
			}
			else
			{
				if (j != min1 && HuffTree[j].parent == NULL && HuffTree[j].weight <= HuffTree[min2].weight)
					min2 = j;
			}
		}
		HuffTree[min1].parent = &HuffTree[nodes_count];//����ָ��
		HuffTree[min2].parent = &HuffTree[nodes_count];
		HuffTree[nodes_count].leftChild = &HuffTree[min1];
		HuffTree[nodes_count].rightChild = &HuffTree[min2];
		HuffTree[nodes_count].weight = HuffTree[min1].weight + HuffTree[min2].weight;//Ȩֵ�ۼ�
		nodes_count++;//�����++
	}
	Treeroot = &HuffTree[nodes_count - 1];//��������ɣ��õ����ڵ�
}
void CreatHuffCode(HuffNode *pre)//�������������
{
	if (pre->leftChild != NULL)//����
	{
		string left = pre->code;
		left += '0';
		pre->leftChild->code += left;
		CreatHuffCode(pre->leftChild);//����һ��ݹ�
	}
	if (pre->rightChild != NULL)//����
	{
		string right = pre->code;
		right += '1';
		pre->rightChild->code += right;
		CreatHuffCode(pre->rightChild);//����һ��ݹ�
	}
	return;
}
void Compress()//ѹ���ļ�
{
	InitHuffTree();//��ʼ����������
	Statistics();//Ȩֵͳ��
	CreatHuffTree();//������������
	CreatHuffCode(Treeroot);//�������������
	string Block;//�����������
	string Buffer;//�������
	string Codetable[256];//�����������
	int if_Finished = 0;
	string Filename, Suffix;
	int mark_Suffix;
	char c;//�ݴ��ַ�
	cout << "�ڵ�����" << nodes_count << endl;
	for (int i = Compress_filename.length() - 1; i > 0; i--)//Ѱ�Һ�׺����λ��
	{
		if (Compress_filename[i] == '.')
		{
			mark_Suffix = i;
			break;
		}
	}
	Suffix = Compress_filename.substr(mark_Suffix + 1);//�����ļ���׺��
	Filename = Compress_filename;
	Filename.erase(mark_Suffix, Compress_filename.length());//�����ļ�����
	ofstream outFile(Filename, ios::out | ios::binary); //��д��򿪶������ļ���
	outFile << Suffix << endl << all_bytes_count << endl << bytes_count << endl;
	for (int i = 0; i < nodes_count; i++)
	{
		if (HuffTree[i].leftChild == NULL)
		{
			Codetable[(unsigned char)HuffTree[i].byte] = HuffTree[i].code;//����code����Ӧ���
			outFile << HuffTree[i].byte << endl << HuffTree[i].weight << endl;
		}

	}
	ifstream inFile(Compress_filename, ios::in | ios::binary); //��ֻ���򿪶������ļ���
	while (Buffer.length() >= 8 || !if_Finished) //һֱ��������С��8
	{
		if (!inFile.read((char *)&c, 1))//����һ���ֽڣ��ж��Ƿ����
		{
			if_Finished = 1;
		}
		if (!if_Finished)//���δ���꣬���ֽڶ�Ӧ������������뻺����
		{
			Buffer += Codetable[(unsigned char)c];
		}
		if (Buffer.length() >= 8)//��������ڵ���8λʱ��ת��Ϊһ���ֽڴ��������������
		{
			unsigned char ch = 0;
			for (int i = 0; i < 8; i++)
			{
				if (Buffer[i] == '1')
					ch = ch + pow(2, 7 - i);
			}
			Block += ch;
			Buffer.erase(0, 8);//�������ǰ8λ
		}
		if (Block.length() >= 32)//����������������32���ֽ�ʱ��ȫ��������ļ�
		{
			outFile << Block;
			Block = "";
		}
	}
	outFile << Block;//���������ʣ���ֽ�
	if (Buffer.length() < 8)//�����岻��8λ������0
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
	inFile.close();//�ر��ļ�
	ifstream inFilesize(Filename, ios::out | ios::binary); //����ѹ�����ļ���С��ѹ����
	inFilesize.seekg(0, ios_base::end);
	int compress_Size = inFilesize.tellg();
	cout << "ѹ�����ļ���С:" << compress_Size << endl;
	inFilesize.close();
	double compress_Ratio = (double)compress_Size / all_bytes_count;
	cout << "ѹ���ȣ�";
	printf("%.2lf%%\n", compress_Ratio * 100);
	cout << "ѹ�����";
}
void Decompress()
{
	InitHuffTree();//��ʼ����������
	unsigned int i = 0;
	unsigned char c;
	char t;
	int mark = 0;
	int btye_write_count = 0;
	int Go_finished;
	int current_code, current_weight, byte_read_count = 0, if_byte = 1;
	int    Byte_count = 0, Byte_type_count = -1;//headmessage
	string Codestream, Filename, Suffix;
	int buffer[8];//����
	HuffNode *Node_Point;

	Filename = Decompress_filename;
	Suffix = Decompress_suffix;
	ifstream inFilesize(Filename, ios::out | ios::binary); //����Դ�ļ���С
	inFilesize.seekg(0, ios_base::end);
	int compress_Size = inFilesize.tellg();
	inFilesize.close();//�������
	cout << "Դ�ļ���С:" << compress_Size << endl;
	ifstream inFile(Filename, ios::in | ios::binary); //��ֻ���򿪶������ļ���
	inFile  >> Suffix >> Byte_count >> Byte_type_count;//��ȡͷ����Ϣ
	cout << "�ֽ�������:" << Byte_type_count << endl;
	inFile.read((char*)&t, 1);//�������з�
	while (inFile.read((char *)&c, 1)) //��ȡ�����������
	{
		if (if_byte == 1)//���Ϊ��Ӧ�ֽ�
		{
			current_code = c;
			if_byte = 0;
		}
		else if (if_byte == 0)//�ֽڶ�ӦȨֵ
		{
			inFile >> current_weight;
			HuffTree[byte_read_count].weight = current_weight;//�Ѷ�Ӧ�ֽں�Ȩֵ��ֵ����������
			HuffTree[byte_read_count].byte = current_code;
			if_byte = 1;
			byte_read_count++; //�ֽ�����ͳ��++
			inFile.read((char*)&t, 1);//�������з�
		}
		if (byte_read_count == Byte_type_count)//������������
		{
			break;
		}
	}
	bytes_count = nodes_count = Byte_type_count;//�����ֽ������������
	CreatHuffTree();//�����������
	CreatHuffCode(Treeroot);//�������������
	Node_Point = Treeroot;//��ǰ״ָ̬����ڵ�
	Filename += ".";
	Filename += Suffix;//��ԭ�ļ���
	ofstream outFile(Filename, ios::out | ios::binary); //��д��򿪶������ļ���
	cout << "����ļ���:" << Filename << endl;
	while (btye_write_count != Byte_count) //һֱ���������ֽ�������
	{
		inFile.read((char *)&c, 1);
		for (int j = 7; j >= 0; j--)
		{
			buffer[j] = c % 2;
			c = c / 2;
		}
		for (i = 0; i < 8; i++)
		{
			if (btye_write_count == Byte_count)//�����ֽ������ɣ�����
			{
				break;
			}
			Go_finished = 0;
			while (Go_finished == 0)
			{
				if (buffer[i] == 0)//����
				{
					if (Node_Point->leftChild != NULL)//������
					{
						Node_Point = Node_Point->leftChild;
						Go_finished = 1;//���Ϊ����
					}
					else//��ǰΪҶ��
					{
						outFile << Node_Point->byte;
						Node_Point = Treeroot;//���ظ�
						btye_write_count++;
					}
				}
				else//����
				{
					if (Node_Point->rightChild != NULL)//���Һ���
					{
						Node_Point = Node_Point->rightChild;
						Go_finished = 1;//���Ϊ����
					}
					else//��ǰΪҶ��
					{
						outFile << Node_Point->byte;
						Node_Point = Treeroot;//���ظ�
						btye_write_count++;
					}
				}
			}
		}
	}
	cout << "��ѹ���" << endl;
	inFile.close();
	outFile.close();
}
void Callwindow()
{
	char c;
	cout << "1.ѹ��" << endl;
	cout << "2.��ѹ" << endl;
	c = getchar();
	if (c == '1')
	{
		system("cls");
		cout << "��������Ҫѹ�����ļ�����" << endl;
		cin >> Compress_filename;
		Compress();
		return;
	}
	else if (c == '2')
	{
		system("cls");
		cout << "��������Ҫ��ѹ���ļ�����" << endl;
		cin >> Decompress_filename;
		Decompress();
		return;
	}
	else
	{
		system("cls");
		cout << "��������" << endl;
		return;
	}
}
int main()
{
	Callwindow();
	getchar();
	getchar();
}