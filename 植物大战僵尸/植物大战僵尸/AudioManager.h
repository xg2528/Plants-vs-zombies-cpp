#pragma once
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")//播放音乐需要的库
#include <string>
#include <map>
#include <vector>
#include <chrono>
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
	// 音效限流：同一音效名 -> 最近播放时间戳列表
	std::map<std::string, std::vector<std::chrono::steady_clock::time_point>> m_recentPlays;
	static constexpr int MAX_SAME_EFFECT = 3;       // 同一音效最多同时存在数
	static constexpr float EFFECT_WINDOW = 0.5f;    // 限流时间窗口（秒）


};