#include "stdafx.h"
#include "JSHandlers.h"
#include "ZenDiaryApp.h"

namespace ZenDiary
{
	namespace App
	{
		JSHandlers::JSHandlers() : 
			m_settings(nullptr),
			m_zen_app(nullptr),
			m_database(nullptr)
		{

		}

		JSHandlers::~JSHandlers()
		{

		}

		ZD_STATUS JSHandlers::SetGlobalSettings(GlobalSettings *settings)
		{
			m_settings = settings;
			return ZD_NOERROR;
		}

		ZD_STATUS JSHandlers::SetZenApp(ZenDiaryApp *app)
		{
			m_zen_app = app;
			return ZD_NOERROR;
		}

		ZD_STATUS JSHandlers::SetDatabase(SQLiteDatabase *db)
		{
			m_database = db;
			return ZD_NOERROR;
		}

		Awesomium::JSValue JSHandlers::OnAlert(Awesomium::WebView *caller, const Awesomium::JSArray &args)
		{
			if (args.size() < 1)
			{
				return Awesomium::JSValue::Undefined();
			}

			uint_t size = args.size();

			std::stringstream message;
			for (uint_t i = 0; i < size; i++)
			{
				Awesomium::JSValue val = args.At(i);
				if (val.IsString())
				{
					message << Awesomium::ToString(val.ToString());
				}
				else if (val.IsInteger())
				{
					message << val.ToInteger();
				}
				else if (val.IsDouble())
				{
					message << val.ToDouble();
				}
			}

			MessageBox(nullptr, message.str().c_str(), "Zen Diary | ��� ������ �������", MB_OK);

			return Awesomium::JSValue::Undefined();
		}

		Awesomium::JSValue JSHandlers::OnGetTemplate(Awesomium::WebView *caller, const Awesomium::JSArray &args)
		{
			Awesomium::JSValue result(Awesomium::WSLit(""));

			if (args.size() == 0)
			{
				return result;
			}

			Awesomium::JSValue js_filename = args.At(0);
			if (!js_filename.IsString())
			{
				return result;
			}

			std::string filename = Awesomium::ToString(js_filename.ToString());

			if (!Helpers::Files::IsFileExist(filename))
			{
				return result;
			}

			std::string data;

			ZD_STATUS status = Helpers::Files::GetFileContent(filename, data);
			if (ZD_FAILED(status))
			{
				return result;
			}

			result = Awesomium::JSValue(Awesomium::WSLit(data.c_str()));

			return result;
		}

		Awesomium::JSValue JSHandlers::OnGetVersionString(Awesomium::WebView *caller, const Awesomium::JSArray &args)
		{
			std::stringstream stream;
			stream << ZD_VERSION_HI(ZD_VERSION) << "." << ZD_VERSION_MID(ZD_VERSION) << "." << ZD_VERSION_LOW(ZD_VERSION);
			
			return Awesomium::JSValue(Awesomium::WSLit(stream.str().c_str()));
		}

		Awesomium::JSValue JSHandlers::OnGetUsername(Awesomium::WebView *caller, const Awesomium::JSArray &args)
		{
			if (!m_settings)
			{
				return Awesomium::JSValue::Undefined();
			}

			const AuthSettings &auth = m_settings->GetAuthSettings();

			const std::string &login = auth.GetLogin();

			return Awesomium::JSValue(Awesomium::WSLit(login.c_str()));
		}

		Awesomium::JSValue JSHandlers::OnIsFirstRun(Awesomium::WebView *caller, const Awesomium::JSArray &args)
		{
			bool first_run = true;

			if (m_settings)
			{
				const AuthSettings &auth_settings = m_settings->GetAuthSettings();
				first_run = auth_settings.IsFirstRun();
			}

			return Awesomium::JSValue(first_run);
		}

		Awesomium::JSValue JSHandlers::OnRegisterUser(Awesomium::WebView *caller, const Awesomium::JSArray &args)
		{
			if (!m_settings)
			{
				return CreateAnswerObject(false, L"���������� javascript ��������� �� ���������������, ���������� � ������������.");
			}

			AuthSettings &auth = m_settings->GetAuthSettings();

			if (!auth.IsFirstRun() || auth.GetLogin().length() > 0 || auth.GetPasshash().length() > 0)
			{
				return CreateAnswerObject(false, L"��������� ������ ������������� ��������, ���������� � ������������.");
			}

			if (args.size() < 2)
			{
				return CreateAnswerObject(false, L"����������� ����������. ������� �������� ������������ ����������, ���������� � ������������.");
			}

			Awesomium::JSValue js_login = args.At(0);
			Awesomium::JSValue js_password = args.At(1);

			if (!js_login.IsString() || !js_password.IsString())
			{
				return CreateAnswerObject(false, L"������� �������� ��������� ��������� ����, ���������� � ������������.");
			}					

			std::string login(Awesomium::ToString(js_login.ToString()));
			std::string password(Awesomium::ToString(js_password.ToString()));

			std::string salt(Helpers::String::GenerateString());
			std::string passhash(Helpers::Crypto::md5(salt + password));				

			auth.SetLogin(login);
			auth.SetPasshash(passhash);
			auth.SetSalt(salt);
			auth.SetFirstRun(false);

			ZD_SAFE_CALL(m_zen_app)->SetLoggedIn(true);

			return CreateAnswerObject(true);
		}

		Awesomium::JSValue JSHandlers::OnLoginUser(Awesomium::WebView *caller, const Awesomium::JSArray &args)
		{
			if (!m_settings)
			{
				return CreateAnswerObject(false, L"���������� javascript ��������� �� ���������������, ���������� � ������������.");
			}

			AuthSettings &auth = m_settings->GetAuthSettings();

			if (args.size() < 2)
			{
				return CreateAnswerObject(false, L"����������� ����������. ������� �������� ������������ ����������, ���������� � ������������.");
			}

			Awesomium::JSValue js_login = args.At(0);
			Awesomium::JSValue js_password = args.At(1);

			if (!js_login.IsString() || !js_password.IsString())
			{
				return CreateAnswerObject(false, L"������� �������� ��������� ��������� ����, ���������� � ������������.");
			}

			std::string login(Awesomium::ToString(js_login.ToString()));
			std::string password(Awesomium::ToString(js_password.ToString()));

			std::string saved_login(auth.GetLogin());
			std::string saved_passhash(auth.GetPasshash());
			std::string salt(auth.GetSalt());

			std::string passhash(Helpers::Crypto::md5(salt + password));

			if (passhash != saved_passhash || login != saved_login)
			{
				return CreateAnswerObject(false, L"�������� ��� ������������ ��� ������.");
			}

			ZD_SAFE_CALL(m_zen_app)->SetLoggedIn(true);

			return CreateAnswerObject(true);
		}

		Awesomium::JSValue JSHandlers::OnLogoutUser(Awesomium::WebView *caller, const Awesomium::JSArray &args)
		{
			ZD_SAFE_CALL(m_zen_app)->SetLoggedIn(false);

			return CreateAnswerObject(true);
		}

		Awesomium::JSValue JSHandlers::OnPostNote(Awesomium::WebView *caller, const Awesomium::JSArray &args)
		{
			if (!m_database)
			{
				return CreateAnswerObject(false, L"���������� javascript ��������� �� ���������������, ���������� � ������������.");
			}

			if (args.size() < 2)
			{
				return CreateAnswerObject(false, L"������� �������� ������������ ����������, ���������� � ������������.");
			}

			Awesomium::JSValue js_title = args.At(0);
			Awesomium::JSValue js_text = args.At(1);

			Awesomium::JSValue js_password(Awesomium::WSLit(""));
			if (args.size() >= 3)
			{
				js_password = args.At(2);
			}

			if (!js_title.IsString() || !js_text.IsString() ||
				(!js_password.IsString() && !js_password.IsUndefined()))
			{
				return CreateAnswerObject(false, L"������� �������� ��������� ��������� ����, ���������� � ������������.");
			}

			std::string title(Awesomium::ToString(js_title.ToString()));
			std::string text(Awesomium::ToString(js_text.ToString()));

			std::string password;
			if (js_password.IsString())
			{
				password = Awesomium::ToString(js_password.ToString());
			}

			bool use_password = password.length() > 0;

			std::string hash = Helpers::Crypto::md5(text);

			if (use_password)
			{
				char *encrypted_data = nullptr;
				size_t encrypted_data_size = 0;

				Helpers::Crypto::EncryptString(text, password, &encrypted_data, encrypted_data_size);

				text = Helpers::Crypto::Base64Encode(encrypted_data, encrypted_data_size);

				delete []encrypted_data;
			}

			std::stringstream query;
			query << "INSERT INTO `notes` (`title`, `note`, `hash`, `encrypted`) VALUES('" << 
				title << "', '" << text << "', '" << hash << "', " << (use_password ? 1 : 0) << ");";

			uint_t inserted = m_database->Execute(query.str());

			if (inserted == 0)
			{
				return CreateAnswerObject(false, L"�� ������� �������� ������ � ��, �������� ������ � ���� ������, ���������� � ������������.");
			}

			Awesomium::JSObject answer(CreateAnswerObject(true));

			int last_row_id = m_database->GetLastInsertId();

			answer.SetProperty(Awesomium::WSLit("id"), Awesomium::JSValue(last_row_id));

			return answer;
		}

		Awesomium::JSValue JSHandlers::OnUpdateNote(Awesomium::WebView *caller, const Awesomium::JSArray &args)
		{
			if (!m_database)
			{
				return CreateAnswerObject(false, L"���������� javascript ��������� �� ���������������, ���������� � ������������.");
			}

			if (args.size() < 3)
			{
				return CreateAnswerObject(false, L"������� �������� ������������ ����������, ���������� � ������������.");
			}

			Awesomium::JSValue js_id = args.At(0);
			Awesomium::JSValue js_title = args.At(1);
			Awesomium::JSValue js_text = args.At(2);

			Awesomium::JSValue js_password(Awesomium::WSLit(""));
			if (args.size() >= 4)
			{
				js_password = args.At(3);
			}

			if (!js_id.IsInteger() || !js_title.IsString() || !js_text.IsString() ||
				(!js_password.IsString() && !js_password.IsUndefined()))
			{
				return CreateAnswerObject(false, L"������� �������� ��������� ��������� ����, ���������� � ������������.");
			}

			int id = js_id.ToInteger();
			std::string title(Awesomium::ToString(js_title.ToString()));
			std::string text(Awesomium::ToString(js_text.ToString()));

			std::string password;
			if (js_password.IsString())
			{
				password = Awesomium::ToString(js_password.ToString());
			}

			bool use_password = password.length() > 0;

			std::string hash = Helpers::Crypto::md5(text);

			if (use_password)
			{
				char *encrypted_data = nullptr;
				size_t encrypted_data_size = 0;

				Helpers::Crypto::EncryptString(text, password, &encrypted_data, encrypted_data_size);

				text = Helpers::Crypto::Base64Encode(encrypted_data, encrypted_data_size);

				delete[]encrypted_data;
			}

			std::stringstream query;

			// TODO: update `update` column

			query << "UPDATE `notes` SET `title` = '" << title << "', `note` = '" << text
				<< "', `hash` = '" << hash << "', `encrypted` = " << (use_password ? 1 : 0) << " WHERE `id` = " << id << ";";;

			uint_t updated = m_database->Execute(query.str());

			if (updated == 0)
			{
				return CreateAnswerObject(false, L"�� ������� �������� ������ � ��, �������� ������ � ���� ������, ���������� � ������������.");
			}

			Awesomium::JSObject answer(CreateAnswerObject(true));

			int last_row_id = m_database->GetLastInsertId();

			answer.SetProperty(Awesomium::WSLit("id"), Awesomium::JSValue(last_row_id));

			return answer;
		}

		Awesomium::JSValue JSHandlers::OnGetNote(Awesomium::WebView *caller, const Awesomium::JSArray &args)
		{
			if (args.size() < 1)
			{
				return CreateAnswerObject(false, L"������� �������� �� ��� ���������, ���������� � ������������.");
			}

			if (!m_database)
			{
				return CreateAnswerObject(false, L"���������� javascript ��������� �� ���������������, ���������� � ������������.");
			}

			Awesomium::JSValue js_id(args.At(0));

			bool use_password = false;

			Awesomium::JSValue js_password;
			if (args.size() >= 2)
			{
				js_password = args.At(1);

				use_password = true;
			}

			if ((!js_id.IsInteger() && !js_id.IsString()) ||
				(args.size() >= 2 && !js_password.IsString()))
			{
				return CreateAnswerObject(false, L"������� �������� ��������� ������������� ����, ���������� � ������������.");
			}

			int id = 0;
			if (js_id.IsInteger())
			{
				id = js_id.ToInteger();
			}
			else if (js_id.IsString())
			{
				id = atoi(Awesomium::ToString(js_id.ToString()).c_str());
			}

			std::stringstream query;
			query << "SELECT `title`, `note`, `hash`, `encrypted`, `created`, `updated` FROM `notes` WHERE `id` = " << id << " AND `deleted` = 0;";

			IDatabaseResult *stmt = m_database->ExecuteSelect(query.str());

			const wchar_t *error_not_exist = L"�� ������� �������� ������� �� ���� ������, �������� ��� ���� �������.";

			if (!stmt || !stmt->Next())
			{
				ZD_SAFE_CALL(stmt)->Release();
				return CreateAnswerObject(false, error_not_exist);
			}

			const char *ctitle = stmt->ColumnData(0);
			const char *cnote = stmt->ColumnData(1);
			const char *chash = stmt->ColumnData(2);
			const char *cencrypted = stmt->ColumnData(3);
			const char *ccreated = stmt->ColumnData(4);
			const char *cupdated = stmt->ColumnData(5);

			if (!ctitle || !cnote || !chash || !cencrypted || !ccreated || !cupdated)
			{
				ZD_SAFE_CALL(stmt)->Release();
				return CreateAnswerObject(false, error_not_exist);
			}

			std::string title(ctitle);
			std::string note(cnote);
			std::string hash(chash);
			bool encrypted = atoi(cencrypted) > 0;
			std::string created(ccreated);
			std::string updated(cupdated);
			
			ZD_SAFE_CALL(stmt)->Release();

			if (encrypted && !use_password)
			{
				return CreateAnswerObject(false, L"���������� ������� ������������� ������� ��� ������.");
			}

			Awesomium::JSObject answer = CreateAnswerObject(true);

			answer.SetProperty(Awesomium::WSLit("id"), Awesomium::JSValue(id));
			answer.SetProperty(Awesomium::WSLit("title"), Awesomium::JSValue(Awesomium::WSLit(title.c_str())));
			answer.SetProperty(Awesomium::WSLit("hash"), Awesomium::JSValue(Awesomium::WSLit(hash.c_str())));
			answer.SetProperty(Awesomium::WSLit("encrypted"), Awesomium::JSValue(encrypted));
			answer.SetProperty(Awesomium::WSLit("created"), Awesomium::JSValue(Awesomium::WSLit(created.c_str())));
			answer.SetProperty(Awesomium::WSLit("updated"), Awesomium::JSValue(Awesomium::WSLit(updated.c_str())));	

			if (encrypted)
			{
				std::string password(Awesomium::ToString(js_password.ToString()));

				char *encoded_bytes = nullptr;
				size_t encoded_size = 0;

				Helpers::Crypto::Base64Decode(note, &encoded_bytes, encoded_size);
				std::string decoded_note = Helpers::Crypto::DecryptString(encoded_bytes, encoded_size, password);

				std::string decoded_note_hash = Helpers::Crypto::md5(decoded_note);
				if (decoded_note_hash != hash)
				{
					return CreateAnswerObject(false, L"�� ������� ������������ �������, �� ����� �������� ������.");
				}

				answer.SetProperty(Awesomium::WSLit("note"), Awesomium::JSValue(Awesomium::WSLit(decoded_note.c_str())));
			}
			else
			{
				answer.SetProperty(Awesomium::WSLit("note"), Awesomium::JSValue(Awesomium::WSLit(note.c_str())));
			}

			return answer;
		}

		Awesomium::JSValue JSHandlers::OnGetNotes(Awesomium::WebView *caller, const Awesomium::JSArray &args)
		{
			if (!m_database)
			{
				return Awesomium::JSValue::Undefined();
			}			

			Awesomium::JSArray result;

			IDatabaseResult *stmt = m_database->ExecuteSelect("SELECT `id`, `title`, `encrypted`, `created`, `updated`, `note`, `hash` FROM `notes` WHERE `deleted` = 0;");
			while (stmt && stmt->Next())
			{
				const char *cid = stmt->ColumnData(0);
				const char *ctitle = stmt->ColumnData(1);
				const char *cencrypted = stmt->ColumnData(2);
				const char *ccreated = stmt->ColumnData(3);
				const char *cupdated = stmt->ColumnData(4);
				const char *cnote = stmt->ColumnData(5);
				const char *chash = stmt->ColumnData(6);

				if (!cid || !ctitle || !cencrypted || 
					!ccreated || !cupdated || !cnote || !chash)
				{
					continue;
				}

				int id = atoi(cid);
				bool encrypted = atoi(cencrypted) != 0;
				
				std::string title(ctitle);				
				std::string created(ccreated);
				std::string updated(cupdated);

				std::string note(cnote);
				std::string hash(chash);

				Awesomium::JSObject item;
				item.SetProperty(Awesomium::WSLit("id"), Awesomium::JSValue(id));
				item.SetProperty(Awesomium::WSLit("encrypted"), Awesomium::JSValue(encrypted));
				item.SetProperty(Awesomium::WSLit("title"), Awesomium::JSValue(Awesomium::WSLit(title.c_str())));
				item.SetProperty(Awesomium::WSLit("note"), Awesomium::JSValue(Awesomium::WSLit(note.c_str())));
				item.SetProperty(Awesomium::WSLit("hash"), Awesomium::JSValue(Awesomium::WSLit(hash.c_str())));
				item.SetProperty(Awesomium::WSLit("created"), Awesomium::JSValue(Awesomium::WSLit(created.c_str())));
				item.SetProperty(Awesomium::WSLit("updated"), Awesomium::JSValue(Awesomium::WSLit(updated.c_str())));

				result.Push(item);
			}

			ZD_SAFE_CALL(stmt)->Release();

			return result;
		}

		Awesomium::JSValue JSHandlers::OnIsNoteEncrypted(Awesomium::WebView *caller, const Awesomium::JSArray &args)
		{
			if (!m_database || args.size() < 1)
			{
				return Awesomium::JSValue(false);
			}

			Awesomium::JSValue js_id(args.At(0));

			if (!js_id.IsInteger() && !js_id.IsString())
			{
				return Awesomium::JSValue(false);
			}

			int id = 0;
			if (js_id.IsInteger())
			{
				id = js_id.ToInteger();
			}
			else if (js_id.IsString())
			{
				id = atoi(Awesomium::ToString(js_id.ToString()).c_str());
			}

			std::stringstream query;
			query << "SELECT `encrypted` FROM `notes` WHERE `deleted` = 0 AND `id` = " << id << ";";

			IDatabaseResult *stmt = m_database->ExecuteSelect(query.str());
			if (!stmt)
			{
				return Awesomium::JSValue::Undefined();
			}

			bool encrypted = false;

			if (stmt->Next())
			{
				const char *cencrypted = stmt->ColumnData(0);
				if (cencrypted)
				{
					int enc = atoi(cencrypted);

					encrypted = enc > 0;
				}
			}

			stmt->Release();

			return Awesomium::JSValue(encrypted);
		}

		Awesomium::JSValue JSHandlers::OnHideNote(Awesomium::WebView *caller, const Awesomium::JSArray &args)
		{
			if (args.size() < 1)
			{
				return CreateAnswerObject(false, L"������� �������� �� ��� ���������, ���������� � ������������.");
			}

			if (!m_database)
			{
				return CreateAnswerObject(false, L"���������� javascript ��������� �� ���������������, ���������� � ������������.");
			}

			Awesomium::JSValue js_id(args.At(0));

			bool use_password = false;

			Awesomium::JSValue js_password;
			if (args.size() >= 2)
			{
				js_password = args.At(1);

				use_password = true;
			}

			if (!js_id.IsInteger() && !js_id.IsString())
			{
				return CreateAnswerObject(false, L"������� �������� ��������� ������������� ����, ���������� � ������������.");
			}

			int id = 0;
			if (js_id.IsInteger())
			{
				id = js_id.ToInteger();
			}
			else if (js_id.IsString())
			{
				id = atoi(Awesomium::ToString(js_id.ToString()).c_str());
			}

			std::stringstream query;
			query << "UPDATE `notes` SET `deleted` = 1 WHERE `id` = " << id << ";";

			int updated = m_database->Execute(query.str());

			if (updated == 0)
			{
				return CreateAnswerObject(false, L"�� ������� ������ �������, �������� ID �������.");
			}

			return CreateAnswerObject(true);
		}

		Awesomium::JSObject JSHandlers::CreateAnswerObject(bool success, const std::wstring &message)
		{
			Awesomium::JSObject result;

			result.SetProperty(Awesomium::WSLit("success"), Awesomium::JSValue(success));
			result.SetProperty(Awesomium::WSLit("message"), Awesomium::JSValue(Awesomium::WSLit(Helpers::String::ToUtf8(message).c_str())));

			return result;
		}
	}
}