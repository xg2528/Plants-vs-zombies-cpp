#include"pch.h"
#include"AudioManager.h"
namespace fs = std::filesystem;
void AudioManager::addEffect(const std::string& name, const std::string& filepath)
{
	m_effectPaths[name] = filepath;
}
void AudioManager::playEffect(const std::string& name)
{
	if (m_isMuted) return;
	auto it = m_effectPaths.find(name);
	if (it == m_effectPaths.end())	return;
	static int id = 0;
	// 打开设备
	std::string alias = "Effect_" + std::to_string(id++);
	std::string openCmd = "open \"" + it->second + "\" alias " + alias;//得到路径并生成别名，前面的alias用来告诉系统这是一个别名，后面是别名的名字，""表示路径
	mciSendStringA(openCmd.c_str(), NULL, 0, NULL);
	// 播放（异步，不循环）
	std::string playCmd = "play " + alias;
	mciSendStringA(playCmd.c_str(), NULL, 0, NULL);

	// 记录已打开的设备
	m_openedDevices.push_back(alias);
}
void AudioManager::playMusic(const std::string& filepath, bool loop)
{
	if (m_isMuted) return;          // 静音时不播放

	// 1. 先停止当前正在播放的音乐（如果有）
	stopMusic();

	// 2. 打开新音乐文件，使用固定别名 "bgm"
	std::string openCmd = "open \"" + filepath + "\" alias bgm";
	if (mciSendStringA(openCmd.c_str(), NULL, 0, NULL) != 0) {
		// 打开失败（文件不存在或格式不支持），直接返回
		return;
	}

	// 3. 播放，可选循环
	std::string playCmd = "play bgm";
	if (loop) playCmd += " repeat";
	mciSendStringA(playCmd.c_str(), NULL, 0, NULL);

	// 4. 记录当前音乐别名（用于 stopMusic）
	m_currentMusicAlias = "bgm";
}
void AudioManager::stopMusic()
{
	if (m_currentMusicAlias.empty()) return;  // 没有正在播放的音乐

	// 停止并关闭设备
	std::string stopCmd = "stop " + m_currentMusicAlias;
	std::string closeCmd = "close " + m_currentMusicAlias;
	mciSendStringA(stopCmd.c_str(), NULL, 0, NULL);
	mciSendStringA(closeCmd.c_str(), NULL, 0, NULL);

	// 清空记录
	m_currentMusicAlias.clear();
}
void AudioManager::Mute(bool mute)
{
	m_isMuted = mute;
	if (m_isMuted) {
		stopMusic();   // 立即停止背景音乐
	}
}
AudioManager::~AudioManager() {
	// 关闭所有音效设备
	for (const auto& alias : m_openedDevices) {
		std::string closeCmd = "close " + alias;
		mciSendStringA(closeCmd.c_str(), NULL, 0, NULL);
	}
	m_openedDevices.clear();
	// 停止并关闭音乐
	stopMusic();
}
void AudioManager::loadAllEffects(const std::string& folderPath) {
	for (const auto& entry : fs::directory_iterator(folderPath)) {
		if (entry.is_regular_file()) {
			std::string fullPath = entry.path().string();                // 完整路径
			std::string fileName = entry.path().filename().string();    // 带扩展名的文件名，例如 "bleep.mp3"
			// 直接用文件名（含扩展名）作为音效名称
			addEffect(fileName, fullPath);
		}
	}
}