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

		Awesomium::JSObject JSHandlers::CreateAnswerObject(bool success, const std::wstring &message)
		{
			Awesomium::JSObject result;

			result.SetProperty(Awesomium::WSLit("success"), Awesomium::JSValue(success));
			result.SetProperty(Awesomium::WSLit("message"), Awesomium::JSValue(Awesomium::WSLit(Helpers::String::ToUtf8(message).c_str())));

			return result;
		}
	}
}