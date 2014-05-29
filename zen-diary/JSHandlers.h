#pragma once

#define ZD_BIND_JS_HANDLER(N, H)						m_method_handler.BindWithRetval(zen_diary, Awesomium::WSLit(N), JSDelegateWithRetval(&m_js_handlers, H))
#define ZD_BIND_JS_HANDLER_WO_RETVAL(N, H)				m_method_handler.Bind(zen_diary, Awesomium::WSLit(N), JSDelegate(&m_js_handlers, H));	\
	zen_diary.SetCustomMethod(Awesomium::WSLit(N), false);


namespace ZenDiary
{
	namespace App
	{
		class Task5App;

		class JSHandlers
		{
		public:
			JSHandlers();
			~JSHandlers();

			JSHandlers(const JSHandlers &a) = default;
			JSHandlers &operator = (const JSHandlers &a) = default;			

			/* Javascript function handlers */
			Awesomium::JSValue OnAlert(Awesomium::WebView *caller, const Awesomium::JSArray &args);			
							
		};
	}
}