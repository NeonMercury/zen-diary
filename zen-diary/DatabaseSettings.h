#pragma once
#include "DatabaseListItem.h"

namespace ZenDiary
{
	namespace App
	{
		class DatabaseSettings : public Interfaces::ISerializable
		{
		public:
			DatabaseSettings();
			~DatabaseSettings();

			DatabaseSettings(const DatabaseSettings &a) = delete;
			DatabaseSettings &operator = (const DatabaseSettings &a) = delete;

			virtual ZD_STATUS Serialize(JsonBox::Value &root) const override final;
			virtual ZD_STATUS Deserialize(JsonBox::Value &root) override final;

			ZD_STATUS ClearDbItems();
			ZD_STATUS AddDbItem(const DatabaseListItem &item);
			std::vector<DatabaseListItem> &GetDatabaseListItems();

		private:
			ZD_PROPERTY(std::string, m_path, Path);

			std::vector<DatabaseListItem> m_db_items;
		};
	};
};