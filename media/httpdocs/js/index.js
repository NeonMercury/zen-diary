var new_post = {

	current_note_id : 0,
	editor : {},

	in_preview : false,

	setSavedButtonState : function()
	{
		$("#on-btn-post").attr("class", "btn btn-success");	
		$("#on-btn-post-icon").attr("class", "icon i i-checkmark2");
		$("#on-btn-post-text").html("Сохранено");
	},

	setUnsavedButtonState : function()
	{
		$("#on-btn-post").attr("class", "btn btn-default");		
		$("#on-btn-post-icon").attr("class", "icon fa fa-save");
		$("#on-btn-post-text").html("Сохранить заметку");	
	},

	changePreviewButtonIcon : function()
	{
		if (new_post.in_preview)
		{
			var span = $("<span>").addClass("fa fa-code");
			$("#on-btn-preview").html(span);		
		}
		else
		{			
			var span = $("<span>").addClass("fa fa-desktop");
			$("#on-btn-preview").html(span);
		}
	},

	switchToPreview : function()
	{
		new_post.in_preview = true;

		new_post.changePreviewButtonIcon();

		var md_text = new_post.editor.getValue();
		var conv = new Markdown.Converter();
		var html_text = conv.makeHtml(md_text); 

		var textarea_height = $(".CodeMirror").height();
		var textarea_width = $(".CodeMirror").width();

		$("#text-preview").css("height", textarea_height + "px");
		$("#text-preview").css("width", textarea_width + "px");

		$("#text-preview").html(html_text);

		$(".CodeMirror").fadeOut(zenapi.animation_duration, function()
		{
			$("#text-preview").fadeIn(zenapi.animation_duration);
		});
	},

	switchToEdit : function()
	{
		new_post.in_preview = false;

		new_post.changePreviewButtonIcon();

		$("#text-preview").fadeOut(zenapi.animation_duration, function()
		{
			$(".CodeMirror").fadeIn(zenapi.animation_duration);
		});		
	},

	surroundSelection : function(object, open_tag, close_tag)
	{
		if (close_tag == undefined)
		{
			close_tag = "";
		}

		var start = $(object)[0].selectionStart;
		var end = $(object)[0].selectionEnd;	

		var old_value = $(object).val();

		var left = old_value.substring(0, start);
		var middle = old_value.substring(start, end);
		var right = old_value.substring(end);
		
		var value = left + open_tag + middle + close_tag + right;

		$(object).val(value);

		$(object).focus();

		var selection_index = start + open_tag.length;

		$(object)[0].setSelectionRange(start + open_tag.length, end + open_tag.length);
	},

	insertToEditor : function(what)
	{
		new_post.editor.replaceSelection(what);
	}
};

$(window).resize(function(e)
{
	var height = $(this).height();
	$(".CodeMirror").css("height", (height - 351) + "px");	
	$(".CodeMirror-gutters").css("height", (height - 351) + "px");	

	var textarea_height = $(".CodeMirror").height();
	var textarea_width = $(".CodeMirror").width();

	$("#text-preview").css("height", textarea_height + "px");
	$("#text-preview").css("width", textarea_width + "px");
});


$(document).ready(function()
{
	new_post.editor = CodeMirror.fromTextArea(document.getElementById("text"),
	{
		mode : "markdown",
		lineNumbers : zen.getShowLineNumbers(),
		theme : "default",
		extraKeys : {
			"Enter" : "newlineAndIndentContinueMarkdownList"
		},
		autoCloseBrackets: false			// TODO: make checkbox in settings
	});

	$(window).resize();

	var search = window.location.search;

	if (search.length > 0 && (search = search.substr(1)).length > 0)
	{
		var encrypted = zen.isNoteEncrypted(search);
		if (encrypted)
		{
			var dialog_message = zen.getTemplate("../media/httpdocs/templates/input-password.html");

			var dialog = new BootstrapDialog({
				title : "Заметка зашифрована",
				message : dialog_message,
				type : BootstrapDialog.TYPE_DEFAULT,
				buttons : [
					{
						id : "on-btn-enter-password",
						label : "OK"
					}
				],

				onshown : function(dlg)
				{
					$("#input-note-password").focus();
				}			
			});

			dialog.realize();

			var btn_enter_pasword = dialog.getButton("on-btn-enter-password");
			btn_enter_pasword.click({}, function(e)
			{			
				var password = $("#input-note-password").val();		
				var result = zen.getNote(search, password);			
				if (result.success)
				{
					$("#title").val(result.title);						
					
					new_post.editor.setValue(result.note);


					$("#password").val(password);
					$("#password-confirm").val(password);
					new_post.current_note_id = result.id;

					$("#page-title").html("Редактирование заметки");

					$.notify("Заметка расшифрована успешно.", 
					{
						position: "top right",
						className : "success"
					});
				}
				else
				{
					$.notify(result.message, 
					{
						position: "top right"
					});
				}

				dialog.close();
			});

			dialog.open();
		}
		else
		{
			var result = zen.getNote(search);			
			if (result.success)
			{
				$("#title").val(result.title);
				
				new_post.editor.setValue(result.note);

				new_post.current_note_id = result.id;

				$("#page-title").html("Редактирование заметки");
			}
			else
			{
				$.notify(result.message, 
				{
					position: "top right"
				});
			}
		}
	}	
});

$(document).keydown(function(e)
{
	if (e.keyCode == 80 && e.altKey)
	{
		$("#on-btn-preview").click();
	}	
});

$("#on-btn-post").click(function(e)
{
	e.preventDefault();	

	var title = $("#title").val();
	var text = new_post.editor.getValue();
	var pass = $("#password").val();
	var passconfirm = $("#password-confirm").val();		

	if (pass.length > 0 && pass != passconfirm)
	{
		$.notify("Введённые вами пароли не совпадают.", 
		{
			position : "top right"
		});
		$("password-confirm").focus();
		return;
	}

	var result = {};

	if (new_post.current_note_id == 0)
	{
		result = zen.postNote(title, text, pass);		
	}
	else
	{
		result = zen.updateNote(new_post.current_note_id, title, text, pass);		
	}

	if (result.success)
	{
		new_post.setSavedButtonState();

		if (new_post.current_note_id == 0)
		{
			new_post.current_note_id = result.id;
		}

		$.notify("Заметка сохранена.", 
		{
			position : "top right",
			className : "success"
		});
	}
	else
	{
		$.notify(result.message, 
		{
			position : "top right"
		});
	}
});

$("#title").keydown(function(e)
{
	new_post.setUnsavedButtonState();
	zenapi.clearAlerts();
});

$("#password, #password-confirm").keydown(function(e)
{
	zenapi.clearAlerts();
});

$(document).on("keydown", "#input-link", function(e)
{
	if (e.keyCode == 13)
	{
		e.preventDefault();
		$("#on-btn-enter-link").click();
	}
});

$("#on-btn-preview").click(function(e)
{
	if (new_post.in_preview)
	{
		new_post.switchToEdit();
	}
	else
	{
		new_post.switchToPreview();
	}
});

$("#on-btn-markdown-help").click(function(e)
{
	e.preventDefault();

	var dialog_message = zen.getTemplate("../media/httpdocs/modal/markdown-help.html");

	var dialog = new BootstrapDialog({
		title : "Синтаксис markdown",
		message : dialog_message,
		type : BootstrapDialog.TYPE_DEFAULT,		
	});

	dialog.realize();	
	dialog.open();
});

$("#on-btn-qsave").click(function(e)
{
	e.preventDefault();
	$("#on-btn-post").click();
});

$("#on-btn-image").click(function(e)
{
	e.preventDefault();

	var fname = zen.openFileDialog("Файлы изображений", "*.art; *.bm; *.bmp; *.dwg; *.dxf; *.fif; *.flo; *.fpx; *.g3; *.gif; *.ico; *.ief; *.iefs; *.jfif; *.jfif-tbnl; *.jpe; *.jpeg; *.jpg; *.jps; *.jut; *.nap; *.naplps; *.nif; *.niff; *.pbm; *.pct; *.pcx; *.pgm; *.pic; *.pict; *.png; *.pnm; *.ppm; *.qif; *.qti; *.qtif; *.ras; *.rast; *.rf; *.rgb; *.rp; *.svf; *.tif; *.tiff; *.turbot; *.wbmp; *.x-png; *.xbm; *.xif; *.xpm; *.xwd", "Все файлы (*.*)", "*.*");
	if (fname != undefined)
	{
		fname = "asset://zen-diary/" + zen.uriEncode(fname);

		var md_image = "![](" + fname + ")";

		new_post.insertToEditor(md_image);
	}
});

$("#on-btn-youtube").click(function(e)
{
	e.preventDefault();

	var dialog_message = zen.getTemplate("../media/httpdocs/templates/input-link.html");	

	var dialog = new BootstrapDialog({
		title : "Вставка видеоклипа",
		message : dialog_message,
		type : BootstrapDialog.TYPE_DEFAULT,
		buttons : [
			{
				id : "on-btn-enter-link",
				label : "OK"
			}
		],

		onshown : function(dlg)
		{
			$("#input-link").focus();
		}			
	});

	dialog.realize();

	var btn_enter_link = dialog.getButton("on-btn-enter-link");
	btn_enter_link.click({}, function(e)
	{			
		var link = $("#input-link").val();		
		dialog.close();

		if (link != undefined && link.length > 0)
		{
			var youtube_url = "http://www.youtube.com/watch?v=";
			var youtube_short_url = "http://youtu.be/";

			var index = link.lastIndexOf(youtube_url);
			if (index == -1)
			{
				index = link.lastIndexOf(youtube_short_url);
				if (index >= 0)
				{
					index = index + youtube_short_url.length;
				}
			}
			else
			{
				index = index + youtube_url.length;
			}

			if (index >= 0)			
			{
				var code = link.substr(index);

				var youtube_iframe = "<iframe width=\"420\" height=\"315\" src=\"http://www.youtube.com/embed/";

				youtube_iframe = youtube_iframe + code;

				youtube_iframe = youtube_iframe + "\" frameborder=\"0\" allowfullscreen></iframe>";

				new_post.insertToEditor(youtube_iframe);
			}
		}
	});

	dialog.open();
});

$("#on-btn-export-to-html").click(function(e)
{
	e.preventDefault();
	var text = new_post.editor.getValue();
	zenapi.exportToHtml(text);
});