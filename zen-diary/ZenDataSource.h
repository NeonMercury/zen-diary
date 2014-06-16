#pragma once
#include "Localization.h"

namespace ZenDiary
{
	namespace App
	{
		class ZenDataSource : public Awesomium::DataSource
		{
		public:
			ZenDataSource();
			virtual ~ZenDataSource();

			ZenDataSource(const ZenDataSource &a) = delete;
			ZenDataSource &operator = (const ZenDataSource &a) = delete;

			virtual void OnRequest(int request_id, const Awesomium::ResourceRequest& request, const Awesomium::WebString& path) override final;

			ZD_STATUS AddMimeType(const std::string &key, const std::string &value);
			ZD_STATUS SetLocalization(Localization *locale);

		private:
			const std::string &GetMimeType(const std::string &key);			

			static void ThreadProc(void *pParam);			

		private:
			ZD_PROPERTY(std::string, m_home_path, HomePath);
			ZD_PROPERTY(std::string, m_current_directory, CurrentDirectory);

			std::map<std::string, std::string> m_mime_types;

			Localization *m_locale;
		
			struct ZenThreadParam
			{
				ZenDataSource *object;
				std::string fullname;
				int request_id;
			};
		};
	};
};