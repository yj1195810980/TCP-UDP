#pragma once
enum PackageCommand
{
	C2R_CMD,			//执行cmd命令
	C2R_SCREEN,			//获取屏幕
	C2R_FILETRAVELS,	//获取指定文件夹下的文件
	C2R_FILEUPLOAD,		//文件上传

	R2C_SCREEN,			//被控端到控制端 被控端返回屏幕数据
};

#pragma pack(push)
#pragma pack(1)
struct PackageHead
{
	PackageCommand m_command;	//包命令
	DWORD m_dwDataLen;			//包的数据长度

};
#pragma pack(pop)