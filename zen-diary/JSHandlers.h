#pragma once
#include "GlobalSettings.h"
#include "DatabaseList.h"
#include "Localization.h"
#include "SpellChecker.h"
#include "WebWindow.h"
#include "resource.h"
#include "Updater.h"
#include "SQLite.h"

#define ZD_BIND_JS_HANDLER(N, H)						m_method_handler.BindWithRetval(zen_diary, Awesomium::WSLit(N), JSDelegateWithRetval(&m_js_handlers, H))
#define ZD_BIND_JS_HANDLER_WO_RETVAL(N, H)				m_method_handler.Bind(zen_diary, Awesomium::WSLit(N), JSDelegate(&m_js_handlers, H));	\
	zen_diary.SetCustomMethod(Awesomium::WSLit(N), false);


namespace ZenDiary
{
	namespace App
	{
		class ZenDiaryApp;

		class JSHandlers
		{
		public:
			JSHandlers();
			~JSHandlers();

			JSHandlers(const JSHandlers &a) = default;
			JSHandlers &operator = (const JSHandlers &a) = default;	

			ZD_STATUS SetGlobalSettings(GlobalSettings *settings);
			ZD_STATUS SetZenApp(ZenDiaryApp *app);
			ZD_STATUS SetDatabase(SQLiteDatabase *db);
			ZD_STATUS SetUpdater(Updater *updater);
			ZD_STATUS SetWebWindow(WebWindow *window);
			ZD_STATUS SetSpellChecker(SpellChecker *spell_checker);
			ZD_STATUS SetLocalization(Localization *localization);
			ZD_STATUS SetDatabaseList(DatabaseList *database_list);

			ZD_STATUS AddMimeType(const std::string &key, const std::string &value);

			/* Javascript function handlers */
			Awesomium::JSValue OnToInt(Awesomium::WebView *caller, const Awesomium::JSArray &args);

			Awesomium::JSValue OnSetFileContent(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnGetFileContent(Awesomium::WebView *caller, const Awesomium::JSArray &args);

			Awesomium::JSValue OnDeleteFile(Awesomium::WebView *caller, const Awesomium::JSArray &args);

			Awesomium::JSValue OnToUpper(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnToLower(Awesomium::WebView *caller, const Awesomium::JSArray &args);

			Awesomium::JSValue OnUriEncode(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnUriDecode(Awesomium::WebView *caller, const Awesomium::JSArray &args);

			Awesomium::JSValue OnAlert(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnTerminate(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnShellExecute(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnGetTemplate(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnGetVersionString(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnGetUsername(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnIsFirstRun(Awesomium::WebView *caller, const Awesomium::JSArray &args);

			Awesomium::JSValue OnRegisterUser(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnLoginUser(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnChangeCredits(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnLogoutUser(Awesomium::WebView *caller, const Awesomium::JSArray &args);

			Awesomium::JSValue OnGetDatabasePath(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnSetDatabasePath(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnGetUseJavascript(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnSetUseJavascript(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnGetShowLineNumbers(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnSetShowLineNumbers(Awesomium::WebView *caller, const Awesomium::JSArray &args);

			Awesomium::JSValue OnPostNote(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnUpdateNote(Awesomium::WebView *caller, const Awesomium::JSArray &args);

			Awesomium::JSValue OnGetNote(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnGetNotes(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnIsNoteEncrypted(Awesomium::WebView *caller, const Awesomium::JSArray &args);

			Awesomium::JSValue OnHideNote(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnShowNote(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnDeleteNote(Awesomium::WebView *caller, const Awesomium::JSArray &args);

			Awesomium::JSValue OnOpenFileDialog(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnSaveFileDialog(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnSetClipboard(Awesomium::WebView *caller, const Awesomium::JSArray &args);

			Awesomium::JSValue OnGetUsersCount(Awesomium::WebView *caller, const Awesomium::JSArray &args);

			Awesomium::JSValue OnToggleFullscreen(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnIsFullscreenMode(Awesomium::WebView *caller, const Awesomium::JSArray &args);

			Awesomium::JSValue OnSpellCheck(Awesomium::WebView *caller, const Awesomium::JSArray &args);

			Awesomium::JSValue OnGetPreviewWidth(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnSetPreviewWidth(Awesomium::WebView *caller, const Awesomium::JSArray &args);

			Awesomium::JSValue OnIsAutocloseBrackets(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnSetAutocloseBrackets(Awesomium::WebView *caller, const Awesomium::JSArray &args);

			Awesomium::JSValue OnGetMimeType(Awesomium::WebView *caller, const Awesomium::JSArray &args);

			Awesomium::JSValue OnGetDatabaseList(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnGetDatabaseListSize(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnAddItemToDatabaseList(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnRemoveItemFromDatabaseList(Awesomium::WebView *caller, const Awesomium::JSArray &args);

			Awesomium::JSValue OnHideWindow(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnShowWindow(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnSetForegroundWindow(Awesomium::WebView *caller, const Awesomium::JSArray &args);

			Awesomium::JSValue OnShowNotifyIcon(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnDeleteNotifyIcon(Awesomium::WebView *caller, const Awesomium::JSArray &args);

			Awesomium::JSValue OnIsSignoutWhenHideToTray(Awesomium::WebView *caller, const Awesomium::JSArray &args);
			Awesomium::JSValue OnSetSignoutWhenHideToTray(Awesomium::WebView *caller, const Awesomium::JSArray &args);

		private:			
			static Awesomium::JSObject CreateAnswerObject(bool success, const std::string &message = std::string());

		private:
			GlobalSettings *m_settings;
			ZenDiaryApp *m_zen_app;
			SQLiteDatabase *m_database;
			Updater *m_updater;
			WebWindow *m_web_window;
			SpellChecker *m_spell_checker;
			Localization *m_localization;
			DatabaseList *m_db_list;

			std::map<std::string, std::string> m_mime_types;
		};
	}
}