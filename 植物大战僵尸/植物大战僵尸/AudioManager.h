#pragma once
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")//播放音乐需要的库
#include <string>
#include <map>
#include <filesystem>  // C++17，VS2022 支持



class AudioManager
{
public:

	void addEffect(const std::string& name, const std::string& filepath);
	void playEffect(const std::string& name);
	void playMusic(const std::string& filepath, bool loop = true);
	void stopMusic();
	void Mute(bool mute);//静音功能
	void loadAllEffects(const std::string& folderPath);
	~AudioManager();

private:

	
	std::map<std::string, std::string> m_effectPaths;
	std::vector<std::string> m_openedDevices;   // 记录打开的设备别名
	std::string m_currentMusicAlias;
	 bool  m_isMuted = false;



};