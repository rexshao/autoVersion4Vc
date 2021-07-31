#include<shlwapi.h>
#include <sstream>
#include <fstream>
#include<string>
#include<iomanip>
#include<list>
#include<vector>
#include<regex>
#include<time.h>
using namespace std;
#pragma comment(lib,"Shlwapi.lib")
void splitString(const std::wstring& str, std::vector<std::wstring>& list, const std::wstring& splitor, int times = 0) {
	list.clear();
	if (str.length() > 0) {
		size_t pos = 0;
		size_t end = 0;
		size_t start_pos = 0;
		size_t splitor_lenth = splitor.length();
		while (-1 != pos)
		{
			pos = str.find(splitor, start_pos);
			if (pos == -1)
			{
				end = str.length();
			}
			else {
				end = pos;
			}
			list.push_back(str.substr(start_pos, end - start_pos));
			start_pos = pos + splitor_lenth;
			times--;
			if (times == 0)
			{
				if (str.length() > start_pos)
				{//���ƴ������˺���ȫ���ӽ�ȥ
					list.push_back(str.substr(start_pos));
				}
				break;
			}
		}
	}
}
int tm2week(tm* lt) {

	//
	mktime(lt);
	int current_week = lt->tm_yday / 7;
	int rest_day = lt->tm_yday % 7;
	int first_wday = lt->tm_wday - rest_day;
	if (first_wday < 0)
	{
		first_wday += 7;
	}
	if (rest_day > 0)
	{
		current_week++;
	}
	if (lt->tm_wday < first_wday)
	{//ǰ�������
		current_week++;
	}
	return current_week;

}
int main(int argc, char** argv)
{
	const char* fname;
	if (argc < 2)
	{
		return 1;
	}
	else {
		fname = argv[1];
	}
	if (!PathFileExistsA(fname))
	{
		return 1;
	}
	auto ifh = fopen(fname, "rb");
	if (!ifh) return 1;

	list<wstring> lines;

	auto t = time(0);
	tm lt;
	localtime_s(&lt, &t);
	int ver2 = (lt.tm_year + 1900) % 100;
	int ver3 = tm2week(&lt);
	wstringstream ss;
	ss << "$1$02" << ver2 << "$04" << ver3 << "$06";
	wstring fmt;
	wregex numVersion(L"(FILEVERSION|PRODUCTVERSION)([^,]*,)(\\d*)(,)(\\d*)(,)(\\d*)", regex::ECMAScript);
	wregex strVersion(L"(FileVersion|ProductVersion)([^\\.]*\\.)(\\d*)(\\.)(\\d*)(\\.)(\\d*)", regex::ECMAScript);
	wstring allstr;
	char head[2];
	fread_s(head, 2, 1, 2, ifh);//��Byte Order Mark������
	fseek(ifh, 0, SEEK_END);
	size_t fsize = ftell(ifh);
	void* buf = malloc(fsize);//���ﲻ��Ҫ�������ֽ� ��Ϊ�ļ�ͷ�����ֽ�Byte Order Mark�۲������ַ�������;
	ZeroMemory(buf, fsize);
	fseek(ifh, 2, SEEK_SET);
	fread(buf, 1, fsize - 2, ifh);
	fclose(ifh);
	allstr = (WCHAR*)buf;
	vector<wstring> inlines;
	splitString(allstr, inlines, L"\n");
	for (auto& line : inlines)
	{
		if (fmt.length() == 0)
		{
			if (line.find(L"FILEVERSION") != -1 || line.find(L"PRODUCTVERSION") != -1)
			{
				auto pos = line.rfind(',');
				wstringstream lastv3;
				int ver4 = 1;
				int lastVer3;
				wstring s1 = line.substr(0, pos);
				auto pos2 = s1.rfind(',');
				lastv3 << s1.substr(pos2 + 1);
				lastv3 >> lastVer3;
				if (ver3 == lastVer3)
				{//�ж�v3��û�б仯���û�б仯��ô��ԭ�ȵ�v4���� ����v4ȡ1
					wstringstream lastv4;
					lastv4 << line.substr(pos + 1);
					lastv4 >> ver4;
					ver4++;
				}
				ss << ver4;
				fmt = ss.str();
			}
		}
		if (fmt.length())
		{
			wstring s1 = regex_replace(line, numVersion, fmt);
			line = regex_replace(s1, strVersion, fmt);
		}
		lines.push_back(line);
	}
	auto f = fopen(fname, "wb");
	if (f)
	{
		fwrite(head, 1, 2, f);//д��ȥByte Order Mark
		for (auto& l : lines)
		{
			fwrite(l.c_str(), sizeof(WCHAR), l.length(), f);
			if (l.back() == L'\r')
			{//��ֹĩ��һֱ��\n
				fwrite("\n\0", 1, 2, f); //���ﲹ��һ��\n 
			}
		}
		fclose(f);
		return 0;
	}
	return 1;
}
