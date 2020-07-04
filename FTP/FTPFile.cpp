
#include <winsock2.h>

#include <stdio.h>

#include <sys/stat.h>


#define _WINSOCK_DEPRECATED_NO_WARNINGS
// for windows mkdir
#ifdef _WIN32
#include <direct.h>
#endif

#pragma comment(lib, "ws2_32")
#pragma warning(disable : 4996)

SOCKET controlSocket;
bool login(char*, char*);
void printWorkingDirectory();
bool changeWorkingDirectory(char*);
void changeWorkingDirectory();
void listWorkingDirectory();
void getListWorkingDirectory(char*);
void makeDirectory();
void makeDirectory(char*);
void removeDirectory();

void downloadFile(char*, char*);
void downloadFile(char*);
void downloadFile();
void uploadFile(char*, char*);
void uploadFile();

void deleteFile();
void renameFile();

void uploadFolder(char*, char*);
void uploadFolder();
void downloadFolder(char*, char*);
void downloadFolder();

void quit();
int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	controlSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(21);

	connect(controlSocket, (SOCKADDR*)&addr, sizeof(addr));
	Sleep(100);
	// Truyen nhan du lieu
	char buf[256];
	int ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;

	printf("%s\n", buf);

	char user[32], pass[32];
	while (1) {
		printf("User: ");
		scanf("%s", user);
		printf("Password: ");
		scanf("%s", pass);
		if (login(user, pass)) break;
	}
	int select;
	bool isquit = false;
	while (!isquit) {
		printf("1. Print working directory\n");
		printf("2. Change working directory\n");
		printf("3. List working directory\n");
		printf("4. Make directory\n");
		printf("5. Remove directory\n");
		printf("6. Upload file\n");
		printf("7. Download file\n");
		printf("8. Upload folder\n");
		printf("9. Download folder\n");
		printf("10. Delete file\n");
		printf("11. Rename file\n");
		printf("99. Quit\n");
		scanf("%d", &select);
		switch (select)
		{
		case 1: {
			printWorkingDirectory();
			break;
		}
		case 2: {
			changeWorkingDirectory();
			break;
		}
		case 3: {
			listWorkingDirectory();
			break;
		}
		case 4: {
			makeDirectory();
			break;
		}
		case 5: {
			removeDirectory();
			break;
		}
		case 6: {
			uploadFile();
			break;
		}
		case 7: {
			downloadFile();
			break;
		}
		case 8: {
			uploadFolder();
			break;
		}
		case 9: {
			downloadFolder();
			break;
		}
		case 10: {
			deleteFile();
			break;
		}
		case 11: {
			renameFile();
			break;
		}
		case 99: {
			isquit = true;
			quit();
			break;
		}
		default:
			printf("Command not support\n");
			isquit = true;
			quit();
			break;
		}
	}

	closesocket(controlSocket);
	WSACleanup();
}

bool login(char* user, char* pass) {
	char buf[256];
	int cmd;
	sprintf(buf, "USER %s\n", user);
	send(controlSocket, buf, strlen(buf), 0);
	recv(controlSocket, buf, sizeof(buf), 0);
	sprintf(buf, "PASS %s\n", pass);
	send(controlSocket, buf, strlen(buf), 0);
	int ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);

	sscanf(buf, "%d", &cmd);
	if (cmd == 230) return true;
	return false;
}
void printWorkingDirectory() {
	char buf[256];
	sprintf(buf, "PWD\n");
	send(controlSocket, buf, strlen(buf), 0);
	int ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);
}
bool changeWorkingDirectory(char* path) {
	char buf[256];
	sprintf(buf, "CWD %s\n", path);
	send(controlSocket, buf, strlen(buf), 0);
	int ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);

	int cmd;
	sscanf(buf, "%d", &cmd);
	if (cmd == 250) return true;
	return false;
}
void changeWorkingDirectory() {
	char buf[256], path[256];
	printf("Enter folder: ");
	scanf("%s", path);
	changeWorkingDirectory(path);
}

void listWorkingDirectory() {
	char buf[2048];

	sprintf(buf, "Type A\n");
	send(controlSocket, buf, strlen(buf), 0);
	int ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);

	sprintf(buf, "PASV\n");
	send(controlSocket, buf, strlen(buf), 0);
	ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);
	char* p = strstr(buf, "(");
	int i1 = atoi(strtok(p + 1, ","));
	int i2 = atoi(strtok(NULL, ","));
	int i3 = atoi(strtok(NULL, ","));
	int i4 = atoi(strtok(NULL, ","));
	int p1 = atoi(strtok(NULL, ","));
	int p2 = atoi(strtok(NULL, ","));

	SOCKADDR_IN dataAddr;
	dataAddr.sin_family = AF_INET;
	char ip[16];
	sprintf(ip, "%d.%d.%d.%d", i1, i2, i3, i4);
	dataAddr.sin_addr.s_addr = inet_addr(ip);
	dataAddr.sin_port = htons(p1 * 256 + p2);

	SOCKET data = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	connect(data, (SOCKADDR*)&dataAddr, sizeof(dataAddr));

	strcpy(buf, "LIST\n");
	send(controlSocket, buf, strlen(buf), 0);

	ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);

	ret = recv(data, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);

	ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);
}
void getListWorkingDirectory(char* list) {
	char buf[2048];

	sprintf(buf, "Type A\n");
	send(controlSocket, buf, strlen(buf), 0);
	int ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);

	sprintf(buf, "PASV\n");
	send(controlSocket, buf, strlen(buf), 0);
	ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);
	char* p = strstr(buf, "(");
	int i1 = atoi(strtok(p + 1, ","));
	int i2 = atoi(strtok(NULL, ","));
	int i3 = atoi(strtok(NULL, ","));
	int i4 = atoi(strtok(NULL, ","));
	int p1 = atoi(strtok(NULL, ","));
	int p2 = atoi(strtok(NULL, ","));

	SOCKADDR_IN dataAddr;
	dataAddr.sin_family = AF_INET;
	char ip[16];
	sprintf(ip, "%d.%d.%d.%d", i1, i2, i3, i4);
	dataAddr.sin_addr.s_addr = inet_addr(ip);
	dataAddr.sin_port = htons(p1 * 256 + p2);

	SOCKET data = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	connect(data, (SOCKADDR*)&dataAddr, sizeof(dataAddr));

	strcpy(buf, "LIST\n");
	send(controlSocket, buf, strlen(buf), 0);

	ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);

	//Get data from server
	ret = recv(data, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);
	memcpy(list, buf, strlen(buf));
	ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);
}
void makeDirectory(char* folder) {
	char buf[256];
	sprintf(buf, "MKD %s\0", folder);
	send(controlSocket, buf, strlen(folder) + 5, 0);
	int ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);
}
void makeDirectory() {
	char buf[256], path[256];
	printf("Enter folder: ");
	scanf("%s", path);
	makeDirectory(path);
}
void removeDirectory() {
	char buf[256], path[256];
	printf("Enter folder: ");
	scanf("%s", path);
	sprintf(buf, "RMD %s\0", path);
	send(controlSocket, buf, strlen(path) + 5, 0);
	int ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);
}
void uploadFile() {
	char buf[1024];

	sprintf(buf, "Type I\n");
	send(controlSocket, buf, strlen(buf), 0);
	int ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);

	sprintf(buf, "PASV\n");
	send(controlSocket, buf, strlen(buf), 0);
	ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);

	char* p = strstr(buf, "(");
	int i1 = atoi(strtok(p + 1, ","));
	int i2 = atoi(strtok(NULL, ","));
	int i3 = atoi(strtok(NULL, ","));
	int i4 = atoi(strtok(NULL, ","));
	int p1 = atoi(strtok(NULL, ","));
	int p2 = atoi(strtok(NULL, ","));

	SOCKADDR_IN dataAddr;
	dataAddr.sin_family = AF_INET;
	char ip[16];
	sprintf(ip, "%d.%d.%d.%d", i1, i2, i3, i4);
	dataAddr.sin_addr.s_addr = inet_addr(ip);
	dataAddr.sin_port = htons(p1 * 256 + p2);

	SOCKET data = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	connect(data, (SOCKADDR*)&dataAddr, sizeof(dataAddr));

	char file[256];
	printf("Enter file: ");
	scanf("%s", file);

	sprintf(buf, "STOR %s\n", file);
	send(controlSocket, buf, strlen(buf), 0);

	ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);

	//Lấy dữ liệu
	char path[256];
	sprintf(path, "P:\\Code4Life\\NetworkProgramming\\FTP\\%s", file);
	FILE* f = fopen(path, "rb");
	while (true) {
		ret = fread(buf, 1, sizeof(buf), f);
		if (ret <= 0) break;
		send(data, buf, ret, 0);
	}
	fclose(f);
	closesocket(data);
	ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);
}
void downloadFile(char* absPath, char* file) {
	char buf[1024];

	sprintf(buf, "Type I\n");
	send(controlSocket, buf, strlen(buf), 0);
	int ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);

	sprintf(buf, "PASV\n");
	send(controlSocket, buf, strlen(buf), 0);
	ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);

	char* p = strstr(buf, "(");
	int i1 = atoi(strtok(p + 1, ","));
	int i2 = atoi(strtok(NULL, ","));
	int i3 = atoi(strtok(NULL, ","));
	int i4 = atoi(strtok(NULL, ","));
	int p1 = atoi(strtok(NULL, ","));
	int p2 = atoi(strtok(NULL, ","));

	SOCKADDR_IN dataAddr;
	dataAddr.sin_family = AF_INET;
	char ip[16];
	sprintf(ip, "%d.%d.%d.%d", i1, i2, i3, i4);
	dataAddr.sin_addr.s_addr = inet_addr(ip);
	dataAddr.sin_port = htons(p1 * 256 + p2);

	SOCKET data = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	connect(data, (SOCKADDR*)&dataAddr, sizeof(dataAddr));

	sprintf(buf, "RETR %s\n", file);
	send(controlSocket, buf, strlen(buf), 0);
	ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);

	int cmd;
	sscanf(buf, "%d", &cmd);
	if (cmd == 550) return;

	//Nhận dữ liệu
	char path[256];
	sprintf(path, "%s\\%s", absPath, file);
	FILE* f = fopen(path, "wb");
	while (true) {
		ret = recv(data, buf, sizeof(buf), 0);
		if (ret < sizeof(buf)) buf[ret] = 0;
		if (ret <= 0) break;
		fwrite(buf, ret, 1, f);
	}
	fclose(f);

	ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);
}
void downloadFile(char* file) {
	downloadFile((char*)"P:\\Code4Life\\NetworkProgramming\\FTP", file);
}
void downloadFile() {
	char file[256];
	printf("Enter file: ");
	scanf("%s", file);

	downloadFile(file);
}
void downloadFolder(char* clientPath, char* serverPath) {
	//Nếu là file
	if (!changeWorkingDirectory(serverPath)) {
		downloadFile(clientPath, serverPath);
	}
	//Nếu là folder
	else {
		char buf[256];
		sprintf(buf, "%s\\%s", clientPath, serverPath);
		//Tạo thư mục phía client
		mkdir(buf);
		char list[1024];
		//Lấy danh sách file và thư mục con
		getListWorkingDirectory(list);
		char fileName[128];
		char t1[20], t2[5], t3[5], t4[5], t5[20], t6[5], t7[5], t8[10];

		//char* line = strtok(list, "\r\n");
		char* line = list;
		while (line[2] != -52) {
			sscanf(line, "%s %s %s %s %s %s %s %s %s", t1, t2, t3, t4, t5, t6, t7, t8, fileName);
			downloadFolder(buf, fileName);
			line = strstr(line + 2, "\r\n");
		}
		//Duyệt hết, Quay lại thư mục cha
		changeWorkingDirectory((char*)"..");
	}
}
//tải folder
void downloadFolder() {
	char folder[256];
	printf("Enter folder: ");
	scanf("%s", folder);
	downloadFolder((char*)"P:\\Code4Life\\NetworkProgramming\\FTP", folder);
}
void uploadFile(char* path, char* file) {
	char buf[1024];

	sprintf(buf, "Type I\n");
	send(controlSocket, buf, strlen(buf), 0);
	int ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);

	sprintf(buf, "PASV\n");
	send(controlSocket, buf, strlen(buf), 0);
	ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);

	char* p = strstr(buf, "(");
	int i1 = atoi(strtok(p + 1, ","));
	int i2 = atoi(strtok(NULL, ","));
	int i3 = atoi(strtok(NULL, ","));
	int i4 = atoi(strtok(NULL, ","));
	int p1 = atoi(strtok(NULL, ","));
	int p2 = atoi(strtok(NULL, ","));

	SOCKADDR_IN dataAddr;
	dataAddr.sin_family = AF_INET;
	char ip[16];
	sprintf(ip, "%d.%d.%d.%d", i1, i2, i3, i4);
	dataAddr.sin_addr.s_addr = inet_addr(ip);
	dataAddr.sin_port = htons(p1 * 256 + p2);

	SOCKET data = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	connect(data, (SOCKADDR*)&dataAddr, sizeof(dataAddr));

	sprintf(buf, "STOR %s\n", file);
	send(controlSocket, buf, strlen(buf), 0);

	ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);

	//Lấy dữ liệu
	FILE* f = fopen(path, "rb");
	while (true) {
		ret = fread(buf, 1, sizeof(buf), f);
		if (ret <= 0) break;
		send(data, buf, ret, 0);
	}
	fclose(f);
	closesocket(data);
	ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);
}
void uploadFolder(char* pbuf, char* folderName) {
	makeDirectory(folderName);
	changeWorkingDirectory(folderName);
	char path[256];
	sprintf(path, "%s\\*.*", pbuf);
	WIN32_FIND_DATAA DATA;
	HANDLE h = FindFirstFileA(path, &DATA);
	FindNextFileA(h, &DATA);
	while (FindNextFileA(h, &DATA)) {
		char buf[256];
		sprintf(buf, "%s\\%s", pbuf, DATA.cFileName);
		if (DATA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			// Đây là một thư mục, đệ quy
			uploadFolder(buf, DATA.cFileName);
		}
		else {
			//Là file thục hiện tải
			uploadFile(buf, DATA.cFileName);
		}
	}
	//Quay lại thư mục cha
	changeWorkingDirectory((char*)"..");
}
void uploadFolder() {
	char folder[256];
	printf("Enter folder: ");
	scanf("%s", folder);
	char path[256];
	sprintf(path, "P:\\Code4Life\\NetworkProgramming\\FTP\\%s", folder);
	uploadFolder(path, folder);
}

void deleteFile() {
	char buf[256], file[256];
	printf("Enter file name: ");
	scanf("%s", file);
	sprintf(buf, "DELE %s\0", file);
	send(controlSocket, buf, strlen(file) + 6, 0);
	int ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);
}
void renameFile() {
	char buf[256], file[256], newFile[256];
	printf("Enter file name: ");
	scanf("%s", file);
	printf("Enter new name for file: ");
	scanf("%s", newFile);
	sprintf(buf, "RNFR %s\0", file);
	send(controlSocket, buf, strlen(file) + 6, 0);
	int ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);
	sprintf(buf, "RNTO %s\0", newFile);
	send(controlSocket, buf, strlen(newFile) + 6, 0);
	ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s", buf);
}

void quit() {
	char buf[256];
	strcpy(buf, "QUIT\n");
	send(controlSocket, buf, strlen(buf), 0);

	int ret = recv(controlSocket, buf, sizeof(buf), 0);
	buf[ret] = 0;
	printf("%s\n", buf);

	closesocket(controlSocket);
}