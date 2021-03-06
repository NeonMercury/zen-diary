#pragma once

namespace ZenDiary
{
	namespace App
	{
		class Updater
		{
		public:
			Updater();
			~Updater();

			Updater(const Updater &a) = delete;
			Updater &operator = (const Updater &a) = delete;

			ZD_STATUS DownloadUsersCount();

			ZD_STATUS CheckForUpdates();
			bool IsNeedToUpdate() const;

			ZD_STATUS OpenUpdateLink() const;
			int GetUsersCount() const;

			const std::string &GetNewVersion() const;
			const std::string &GetChangeLog() const;

		private:
			struct VersionInfo
			{
				uchar_t major;
				uchar_t middle;
				uchar_t minor;

				std::string link;
				std::string changelog;

				bool operator < (const VersionInfo &info) const;
			};

		private:
#ifdef _DEBUG
			const std::string m_update_version_file_path = std::string("http://zendiary.dev/api/get-last-version.json");						
#else
			const std::string m_update_version_file_path = std::string("http://zendiary.org/api/get-last-version.json");
#endif			

			bool m_need_to_update;
			int m_users_count;
			std::string m_new_verson;			

			std::string m_update_link;
			std::string m_change_log;
		};
	};
};